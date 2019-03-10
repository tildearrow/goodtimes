// specs2 soundchip sequence interpreter...
#include "soundchip.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <jack/jack.h>

jack_client_t* ac;
jack_port_t* ao[2];
jack_status_t as;

int sr;

soundchip sc;

double targetSR;

double noProc;

double procPos;
int ticks, speed;

FILE* f;
int c;
int curChan;

float resa0[2], resa1[2];

#define resaf 0.33631372025095791864295318996109

int decHex(int ch) {
  switch (ch) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return ch-'0';
      break;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
      return 10+ch-'A';
      break;
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
      return 10+ch-'a';
      break;
  }
  return 0;
}

void runSSOps() {
  char temp;
  if (feof(f)) {
    exit(0);
    return;
  }
  while (!feof(f)) {
    c=fgetc(f);
    switch (c) {
      case '$':
        curChan=fgetc(f)-'0';
        break;
      case 'V':
        sc.chan[curChan].vol=(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        break;
      case 'Y':
        sc.chan[curChan].duty=(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        break;
      case 'f':
        sc.chan[curChan].freq=(decHex(fgetc(f))<<12)+(decHex(fgetc(f))<<8)+(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        break;
      case 'S':
        sc.chan[curChan].flags.shape=fgetc(f)-'0';
        break;
      case 'I':
        sc.chan[curChan].flags.fmode=fgetc(f)-'0';
        break;
      case 'c':
        sc.chan[curChan].cutoff=(decHex(fgetc(f))<<12)+(decHex(fgetc(f))<<8)+(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        break;
      case 'r':
        sc.chan[curChan].reson=(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        break;
      case 'M':
        temp=fgetc(f)-'0';
        sc.chan[curChan].flags.swvol=!!(temp&1);
        sc.chan[curChan].flags.swfreq=!!(temp&2);
        sc.chan[curChan].flags.swcut=!!(temp&4);
        break;
      case 'v':
        sc.chan[curChan].swvol.speed=(decHex(fgetc(f))<<12)+(decHex(fgetc(f))<<8)+(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        temp=(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        sc.chan[curChan].swvol.amt=temp&0x1f;
        sc.chan[curChan].swvol.dir=!!(temp&0x20);
        sc.chan[curChan].swvol.loop=!!(temp&0x40);
        sc.chan[curChan].swvol.loopi=!!(temp&0x80);
        sc.chan[curChan].swvol.bound=(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        break;
      case 'k':
        sc.chan[curChan].swfreq.speed=(decHex(fgetc(f))<<12)+(decHex(fgetc(f))<<8)+(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        temp=(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        sc.chan[curChan].swfreq.amt=temp&0x7f;
        sc.chan[curChan].swfreq.dir=!!(temp&0x80);
        sc.chan[curChan].swfreq.bound=(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        break;
      case 'l':
        sc.chan[curChan].swcut.speed=(decHex(fgetc(f))<<12)+(decHex(fgetc(f))<<8)+(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        temp=(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        sc.chan[curChan].swcut.amt=temp&0x7f;
        sc.chan[curChan].swcut.dir=!!(temp&0x80);
        sc.chan[curChan].swcut.bound=(decHex(fgetc(f))<<4)+decHex(fgetc(f));
        break;
    }
    if (c=='R') break;
  }
};

int process(jack_nframes_t nframes, void* arg) {
  float* buf[2];
  float temp[2];
  for (int i=0; i<2; i++) {
    buf[i]=(float*)jack_port_get_buffer(ao[i],nframes);
  }
  
  for (size_t i=0; i<nframes;) {
    ticks-=20; // 20 CPU cycles per sound output cycle
    if (ticks<=0) {
      runSSOps();
      printf("-----------------------------------\n");
      for (int i=0; i<256; i++) {
        printf("%.2x",((unsigned char*)sc.chan)[i]);
        if ((i&31)==31) printf("\n");
      }
      ticks+=speed;
    }
    sc.NextSample(&temp[0],&temp[1]);
    resa0[0]=resa0[0]+resaf*(temp[0]-resa0[0]);
    resa1[0]=resa1[0]+resaf*(resa0[0]-resa1[0]);
    resa1[0]=resa1[0]+resaf*(resa0[0]-resa1[0]);
    resa1[0]=resa1[0]+resaf*(resa0[0]-resa1[0]);
    
    resa0[1]=resa0[1]+resaf*(temp[1]-resa0[1]);
    resa1[1]=resa1[1]+resaf*(resa0[1]-resa1[1]);
    resa1[1]=resa1[1]+resaf*(resa0[1]-resa1[1]);
    resa1[1]=resa1[1]+resaf*(resa0[1]-resa1[1]);
    
    buf[0][i]=0.25*resa1[0];
    buf[1][i]=0.25*resa1[1];
    procPos+=noProc;
    if (procPos>=1) {
      procPos-=1;
      i++;
    }
  }
  return 0;
}

int main(int argc, char** argv) {
  printf("ssinter.\n");
  sc.Init();
  procPos=0;
  ticks=0;
  resa0[0]=0; resa0[1]=0;
  resa1[0]=0; resa1[1]=0;
  
  if (argc<2) {
    printf("provide a file\n");
    return 1;
  }
  
  if ((f=fopen(argv[1],"r"))==NULL) {
    printf("not exist\n");
    return 1;
  }
  
  ac=jack_client_open("ssinter",JackNullOption,&as);
  if (ac==NULL) return 1;
  
  sr=jack_get_sample_rate(ac);
  
  targetSR=297500; // PAL.
  speed=119000; // PAL.
  //targetSR=309000; // NTSC.
  //speed=103103; // NTSC. 103000 for no colorburst compensation
  
  noProc=sr/targetSR;
  
  jack_set_process_callback(ac,process,NULL);
  
  ao[0]=jack_port_register(ac,"outL",JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);
  ao[1]=jack_port_register(ac,"outR",JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);
  
  jack_activate(ac);
  
  jack_connect(ac,"ssinter:outL","system:playback_1");
  jack_connect(ac,"ssinter:outR","system:playback_2");
  
  sc.chan[0].vol=127;
  sc.chan[0].duty=63;
  sc.chan[0].freq=4096;
  
  sleep(-1);
  return 0;
}

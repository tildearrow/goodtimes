// specs2 soundchip sequence interpreter...
#include "soundchip.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
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
int curChan;
int frame;

int curOctave;

size_t fsize;

float resa0[2], resa1[2];

#define resaf 0.33631372025095791864295318996109

std::string str;

// maximum permissible notes
unsigned short noteFreqs[12]={
  0x7344,
  0x7a1e,
  0x8161,
  0x8913,
  0x913a,
  0x99dc,
  0xa302,
  0xacb4,
  0xb6f9,
  0xc1da,
  0xcd61,
  0xd998
};

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

int bufchar(const char* buf, size_t tell, size_t bound) {
  if (tell>=bound) return EOF;
  return buf[tell];
}

#define _NEXT_ bufchar(buf,set++,size)

// returns false if end of stream
bool runSSOps(const char* buf, size_t set, size_t size) {
  char temp;
  int c;
  if (set>=size) {
    printf("\x1b[9B\n");
    return false;
  }
  while (set<size) {
    c=_NEXT_;
    switch (c) {
      case '$':
        curChan=(_NEXT_-'0')&7;
        break;
      case 'V':
        sc.chan[curChan].vol=(decHex(_NEXT_)<<4);
        sc.chan[curChan].vol+=decHex(_NEXT_);
        break;
      case 'Y':
        sc.chan[curChan].duty=(decHex(_NEXT_)<<4);
        sc.chan[curChan].duty+=decHex(_NEXT_);
        break;
      case 'f':
        sc.chan[curChan].freq=(decHex(_NEXT_)<<12);
        sc.chan[curChan].freq+=(decHex(_NEXT_)<<8);
        sc.chan[curChan].freq+=(decHex(_NEXT_)<<4);
        sc.chan[curChan].freq+=decHex(_NEXT_);
        break;
      case 'S':
        sc.chan[curChan].flags.shape=_NEXT_-'0';
        break;
      case 'I':
        sc.chan[curChan].flags.fmode=_NEXT_-'0';
        break;
      case 'c':
        sc.chan[curChan].cutoff=(decHex(_NEXT_)<<12);
        sc.chan[curChan].cutoff+=(decHex(_NEXT_)<<8);
        sc.chan[curChan].cutoff+=(decHex(_NEXT_)<<4);
        sc.chan[curChan].cutoff+=decHex(_NEXT_);
        break;
      case 'r':
        sc.chan[curChan].reson=(decHex(_NEXT_)<<4);
        sc.chan[curChan].reson+=decHex(_NEXT_);
        break;
      case 'M':
        temp=(_NEXT_-'0')&7;
        sc.chan[curChan].flags.swvol=!!(temp&1);
        sc.chan[curChan].flags.swfreq=!!(temp&2);
        sc.chan[curChan].flags.swcut=!!(temp&4);
        break;
      case 'v':
        sc.chan[curChan].swvol.speed=(decHex(_NEXT_)<<12);
        sc.chan[curChan].swvol.speed+=(decHex(_NEXT_)<<8);
        sc.chan[curChan].swvol.speed+=(decHex(_NEXT_)<<4);
        sc.chan[curChan].swvol.speed+=decHex(_NEXT_);
        temp=(decHex(_NEXT_)<<4);
        temp+=decHex(_NEXT_);
        sc.chan[curChan].swvol.amt=temp&0x1f;
        sc.chan[curChan].swvol.dir=!!(temp&0x20);
        sc.chan[curChan].swvol.loop=!!(temp&0x40);
        sc.chan[curChan].swvol.loopi=!!(temp&0x80);
        sc.chan[curChan].swvol.bound=(decHex(_NEXT_)<<4);
        sc.chan[curChan].swvol.bound+=decHex(_NEXT_);
        break;
      case 'k':
        sc.chan[curChan].swfreq.speed=(decHex(_NEXT_)<<12);
        sc.chan[curChan].swfreq.speed+=(decHex(_NEXT_)<<8);
        sc.chan[curChan].swfreq.speed+=(decHex(_NEXT_)<<4);
        sc.chan[curChan].swfreq.speed+=decHex(_NEXT_);
        temp=(decHex(_NEXT_)<<4);
        temp+=decHex(_NEXT_);
        sc.chan[curChan].swfreq.amt=temp&0x7f;
        sc.chan[curChan].swfreq.dir=!!(temp&0x80);
        sc.chan[curChan].swfreq.bound=(decHex(_NEXT_)<<4);
        sc.chan[curChan].swfreq.bound+=decHex(_NEXT_);
        break;
      case 'l':
        sc.chan[curChan].swcut.speed=(decHex(_NEXT_)<<12);
        sc.chan[curChan].swcut.speed+=(decHex(_NEXT_)<<8);
        sc.chan[curChan].swcut.speed+=(decHex(_NEXT_)<<4);
        sc.chan[curChan].swcut.speed+=decHex(_NEXT_);
        temp=(decHex(_NEXT_)<<4);
        temp+=decHex(_NEXT_);
        sc.chan[curChan].swcut.amt=temp&0x7f;
        sc.chan[curChan].swcut.dir=!!(temp&0x80);
        sc.chan[curChan].swcut.bound=(decHex(_NEXT_)<<4);
        sc.chan[curChan].swcut.bound+=decHex(_NEXT_);
        break;
      case 'O':
        curOctave=_NEXT_-'0';
        if (curOctave<0) curOctave=0;
        if (curOctave>7) curOctave=7;
        break;
      case 'C':
        sc.chan[curChan].freq=noteFreqs[0]>>(7-curOctave);
        break;
      case 'D':
        sc.chan[curChan].freq=noteFreqs[2]>>(7-curOctave);
        break;
      case 'E':
        sc.chan[curChan].freq=noteFreqs[4]>>(7-curOctave);
        break;
      case 'F':
        sc.chan[curChan].freq=noteFreqs[5]>>(7-curOctave);
        break;
      case 'G':
        sc.chan[curChan].freq=noteFreqs[7]>>(7-curOctave);
        break;
      case 'A':
        sc.chan[curChan].freq=noteFreqs[9]>>(7-curOctave);
        break;
      case 'B':
        sc.chan[curChan].freq=noteFreqs[11]>>(7-curOctave);
        break;
      case '#':
        c=_NEXT_;
        switch (c) {
          case 'C':
            sc.chan[curChan].freq=noteFreqs[1]>>(7-curOctave);
            break;
          case 'D':
            sc.chan[curChan].freq=noteFreqs[3]>>(7-curOctave);
            break;
          case 'F':
            sc.chan[curChan].freq=noteFreqs[6]>>(7-curOctave);
            break;
          case 'G':
            sc.chan[curChan].freq=noteFreqs[8]>>(7-curOctave);
            break;
          case 'A':
            sc.chan[curChan].freq=noteFreqs[10]>>(7-curOctave);
            break;
        }
        break;
    }
    if (c=='R') break;
  }
  return true;
};

int process(jack_nframes_t nframes, void* arg) {
  float* buf[2];
  float temp[2];
  int wc;
  for (int i=0; i<2; i++) {
    buf[i]=(float*)jack_port_get_buffer(ao[i],nframes);
  }
  
  for (size_t i=0; i<nframes;) {
    ticks-=20; // 20 CPU cycles per sound output cycle
    if (ticks<=0) {
      str="";
      while ((wc=fgetc(f))!=EOF) {
        str+=wc;
        if (wc=='R') break;
      }
      runSSOps(str.c_str(),0,str.size());
      printf("ssinter: filename                      % 8ld/%ld  % 8d\n",ftell(f),fsize,frame);
      printf("\x1b[1;33m----\x1b[32m--\x1b[36m--\x1b[m----\x1b[1;34m----\x1b[31m--\x1b[35m--\x1b[30m------------\x1b[33m--------\x1b[32m--------\x1b[34m--------\x1b[m----\x1b[33m----\x1b[m\n");
      for (int i=0; i<256; i++) {
        printf("%.2x",((unsigned char*)sc.chan)[i]);
        if ((i&31)==31) printf("\n");
      }
      printf("\x1b[10A");
      ticks+=speed;
      frame++;
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
  int which;
  sc.Init();
  procPos=0;
  curOctave=4;
  frame=0;
  ticks=0;
  resa0[0]=0; resa0[1]=0;
  resa1[0]=0; resa1[1]=0;
  
  if (argc<2) {
    printf("usage: %s [-n] file\n",argv[0]);
    return 1;
  }
  targetSR=297500; // PAL.
  speed=119000; // PAL.
  which=1;

  if (strcmp(argv[1],"-n")==0) {
    which=2;
    targetSR=309000; // NTSC.
    speed=103103; // NTSC. 103000 for no colorburst compensation
  }
  
  if ((f=fopen(argv[which],"r"))==NULL) {
    printf("not exist\n");
    return 1;
  }

  fseek(f,0,SEEK_END);
  fsize=ftell(f);
  fseek(f,0,SEEK_SET);
  
  ac=jack_client_open("ssinter",JackNullOption,&as);
  if (ac==NULL) return 1;
  
  sr=jack_get_sample_rate(ac);
  
  noProc=sr/targetSR;
  
  jack_set_process_callback(ac,process,NULL);
  
  ao[0]=jack_port_register(ac,"outL",JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);
  ao[1]=jack_port_register(ac,"outR",JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);
  
  jack_activate(ac);
  
  jack_connect(ac,"ssinter:outL","system:playback_1");
  jack_connect(ac,"ssinter:outR","system:playback_2");
  
  for (int i=0; i<8; i++) {
    sc.chan[i].pan=0;
    sc.chan[i].duty=0x3f;
  }
  
  sleep(-1);
  return 0;
}

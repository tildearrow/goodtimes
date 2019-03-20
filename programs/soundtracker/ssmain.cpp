// specs2 soundchip sequence interpreter...
#include "soundchip.h"
#include "ssinter.h"
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
int frame;

SSInter s;

size_t fsize;

float resa0[2], resa1[2];

#define resaf 0.33631372025095791864295318996109

std::string str;

int process(jack_nframes_t nframes, void* arg) {
  float* buf[2];
  float temp[2];
  int wc;
  int writable;
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
      writable=0;
      s.next(str.c_str(),writable,str.size());
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

  s.init(&sc);

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
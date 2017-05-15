#include "soundchip.h"
#include <string.h>

char soundchip::Saw(int theduty, float value) {
  return 1;
}

char soundchip::Pulse(int theduty, float value) {
  return 2*round(value+((float)(theduty-63)/128.0f))-1;
}

char soundchip::Sine(int theduty, float value) {
  return sin(value*pi*2);
}

char soundchip::Triangle(int theduty, float value) {
  return (fabs(0.5-value)*2)-0.5;
}

char soundchip::Noise(int theduty, float value) {
  return 1; // should not happen
}

void soundchip::NextSample(float* l, float* r) {
  for (int i=0; i<8; i++) {
    if (chan[i].vol==0) {fns[i]=0; continue;}
    if (chan[i].flags.pcm) {
      ns[i]=pcm[chan[i].pcmpos];
    } else if ((chan[i].flags.shape)==6) {
      ns[i]=randmem[i][(cycle[i]*5)/chan[i].freq]*127;
    } else if ((chan[i].flags.shape)==5) {
      ns[i]=randmem[i][(cycle[i]*chan[i].duty)/chan[i].freq]*127;
    } else if ((chan[i].flags.shape)==4) {
      if (cycle[i]==0 || cycle[i]==((chan[i].freq*(chan[i].duty+1))>>7)) {
      bool feed=((lfsr) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
      ns[i]=(lfsr&1)*127;
      lfsr=(lfsr>>1|feed<<31);
      } else {
        ns[i]=(lfsr&1)*127;
      }
    } else if ((chan[i].flags.shape)==0) {
      ns[i]=(cycle[i]>((chan[i].freq*(chan[i].duty+1))>>7))*127;
    } else {
      ns[i]=(short)ShapeFunctions[(chan[i].flags.shape)][(cycle[i]<<8)/chan[i].freq];
    }
    
    if (chan[i].flags.pcm) {
      pcmdec[i]+=((128)*32768)/chan[i].freq;
      if (pcmdec[i]<0) {
        pcmdec[i]&=0x7fff;
        if (chan[i].pcmpos<chan[i].pcmbnd) {
          chan[i].pcmpos++;
          if (chan[i].pcmpos==chan[i].pcmbnd) {
            if (chan[i].flags.pcmloop) {
              chan[i].pcmpos=chan[i].pcmrst;
            }
          }
        } else if (chan[i].flags.pcmloop) {
          chan[i].pcmpos=chan[i].pcmrst;
        }
      }
    } else {
      if (cycle[i]++>=chan[i].freq) {
        cycle[i]=0;
        chan[i].wc++;
      }
      if (chan[i].flags.restim) {
        if (rcycle[i]++>=resetfreq[i]) {
          cycle[i]=0;
          rcycle[i]=0;
        }
      }
    }
    //ns[i]=(char)((short)ns[i]*(short)vol[i]/256);
    fns[i]=(float)ns[i]/128;
    if (chan[i].flags.fmode!=0) {
      float f=2*sin(3.141592653589*(((float)chan[i].cutoff)/2.5)/297500);
      nslow[i]=nslow[i]+(f)*nsband[i];
      nshigh[i]=fns[i]-nslow[i]-(1.0-(float)chan[i].reson/256.0)*nsband[i];
      nsband[i]=(f)*nshigh[i]+nsband[i];
      fns[i]=(((chan[i].flags.fmode&1)?(nslow[i]):(0))+((chan[i].flags.fmode&2)?(nshigh[i]):(0))+((chan[i].flags.fmode&4)?(nsband[i]):(0)));
    }
    fns[i]*=(float)chan[i].vol/256;
    nsL[i]=fns[i]*((127-(fmax(0,(float)chan[i].pan)))/127);
    nsR[i]=fns[i]*((128+(fmin(0,(float)chan[i].pan)))/128);
    if ((chan[i].freq>>8)!=(oldfreq[i]>>8) || oldflags[i]!=chan[i].flags.flags) {
      bool feed=((lfsr) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
      for (int j=0; j<7; j++) {
        randmem[i][(unsigned char)(randpos[i]++)]=(lfsr&(1<<j));
        randpos[i]%=chan[i].duty+1;
      }
      lfsr=(lfsr>>1|feed<<31);
    }
    oldfreq[i]=chan[i].freq;
    oldflags[i]=chan[i].flags.flags;
  }
  *l=((nsL[0]+nsL[1]+nsL[2]+nsL[3]+nsL[4]+nsL[5]+nsL[6]+nsL[7]));///256;
  *r=((nsR[0]+nsR[1]+nsR[2]+nsR[3]+nsR[4]+nsR[5]+nsR[6]+nsR[7]));///256;
}

void soundchip::Init() {
  Reset();
  for (int i=0; i<65280; i++) {
    pcm[i]=0;
  }
  ShapeFunctions[0]=SCsaw;
  ShapeFunctions[1]=SCsaw;
  ShapeFunctions[2]=SCsine;
  ShapeFunctions[3]=SCtriangle;
  ShapeFunctions[4]=SCsaw;
  ShapeFunctions[5]=SCsaw;
  ShapeFunctions[6]=SCsaw;
  ShapeFunctions[7]=SCsaw;
  for (int i=0; i<256; i++) {
    SCsaw[i]=i;
    SCsine[i]=sin((float)i/128*pi)*127;
    SCtriangle[i]=(i>127)?(255-i):(i);
  }
  for (int i=0; i<1024; i++) {
    randmem[i>>7][i&127]=rand();
  }
}

void soundchip::Reset() {
  for (int i=0; i<8; i++) {
    cycle[i]=0;
    resetfreq[i]=0;
    //duty[i]=64;
    //pan[i]=0;
    //cut[i]=0;
    //res[i]=0;
    //flags[i]=0;
    //postvol[i]=0;
    voldcycles[i]=0;
    volicycles[i]=0;
    fscycles[i]=0;
    sweep[i]=0;
    ns[i]=0;
  }
  memset(chan,0,sizeof(channel)*8);
  lfsr=11111;
}

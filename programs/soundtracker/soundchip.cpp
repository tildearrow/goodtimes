#include "soundchip.h"

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
    if (vol[i]==0) {fns[i]=0; continue;}
    if (flags[i]&8) {
      ns[i]=pcm[pcmpos[i]];
    } else if ((flags[i]&7)==6) {
      ns[i]=randmem[i][(cycle[i]*5)/freq[i]]*127;
    } else if ((flags[i]&7)==5) {
      ns[i]=randmem[i][(cycle[i]*duty[i])/freq[i]]*127;
    } else if ((flags[i]&7)==4) {
      if (cycle[i]==0 || cycle[i]==((freq[i]*(duty[i]+1))>>7)) {
      bool feed=((lfsr) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
      ns[i]=(lfsr&1)*127;
      lfsr=(lfsr>>1|feed<<31);
      } else {
        ns[i]=(lfsr&1)*127;
      }
    } else if ((flags[i]&7)==0) {
      ns[i]=(cycle[i]>((freq[i]*(duty[i]+1))>>7))*127;
    } else {
      ns[i]=(short)ShapeFunctions[(flags[i]&7)][(cycle[i]<<8)/freq[i]];
    }
    
    if (flags[i]&8) {
      pcmdec[i]+=(((pcmmult[i]&127)+1)*32768)/freq[i];
      if (pcmdec[i]<0) {
        pcmdec[i]&=0x7fff;
        if (pcmpos[i]<pcmend[i]) {
          pcmpos[i]++;
          if (pcmpos[i]==pcmend[i]) {
            if (pcmmult[i]&128) {
              pcmpos[i]=pcmreset[i];
            }
          }
        } else if (pcmmult[i]&128) {
          pcmpos[i]=pcmreset[i];
        }
      }
    } else {
      if (cycle[i]++>=freq[i]) {
        cycle[i]=0;
      }
    }
    //ns[i]=(char)((short)ns[i]*(short)vol[i]/256);
    fns[i]=(float)ns[i]/128;
    if ((((flags[i]>>5)&7)!=0)) {
      float f=2*sin(3.141592653589*(((float)cut[i])/10.0)/297500);
      nslow[i]=nslow[i]+(f)*nsband[i];
      nshigh[i]=fns[i]-nslow[i]-(1.0-(float)res[i]/256.0)*nsband[i];
      nsband[i]=(f)*nshigh[i]+nsband[i];
      fns[i]=(((flags[i]&32)?(nslow[i]):(0))+((flags[i]&64)?(nshigh[i]):(0))+((flags[i]&128)?(nsband[i]):(0)));
    }
    fns[i]*=(float)vol[i]/256;
    nsL[i]=fns[i]*((127-(fmax(0,(float)pan[i])))/127);
    nsR[i]=fns[i]*((128+(fmin(0,(float)pan[i])))/128);
    if ((freq[i]>>8)!=(oldfreq[i]>>8) || oldflags[i]!=flags[i]) {
      bool feed=((lfsr) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
      for (int j=0; j<7; j++) {
        randmem[i][(unsigned char)(randpos[i]++)]=(lfsr&(1<<j));
        randpos[i]%=duty[i]+1;
      }
      lfsr=(lfsr>>1|feed<<31);
    }
    oldfreq[i]=freq[i];
    oldflags[i]=flags[i];
  }
  *l=((nsL[0]+nsL[1]+nsL[2]+nsL[3]+nsL[4]+nsL[5]+nsL[6]+nsL[7]));///256;
  *r=((nsR[0]+nsR[1]+nsR[2]+nsR[3]+nsR[4]+nsR[5]+nsR[6]+nsR[7]));///256;
}

void soundchip::Init() {
  Reset();
  for (int i=0; i<131072; i++) {
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
    freq[i]=1;
    resetfreq[i]=0;
    duty[i]=64;
    vol[i]=0;
    pan[i]=0;
    cut[i]=0;
    res[i]=0;
    flags[i]=0;
    postvol[i]=0;
    voldcycles[i]=0;
    volicycles[i]=0;
    fscycles[i]=0;
    sweep[i]=0;
    ns[i]=0;
  }
  lfsr=11111;
}

#include "soundchip.h"

float soundchip::Saw(int theduty, float value) {
  return 2*value-1;
}

float soundchip::Pulse(int theduty, float value) {
  return 2*round(value+((float)(theduty-63)/128.0f))-1;
}

float soundchip::Sine(int theduty, float value) {
  return sin(value*pi*2);
}

float soundchip::Triangle(int theduty, float value) {
  return (fabs(0.5-value)*2)-0.5;
}

float soundchip::Noise(int theduty, float value) {
  return 1; // should not happen
}

float soundchip::NextSample() {
  for (int i=0; i<8; i++) {
    if ((flags[i]&7)==4) {
      if (cycle[i]==0) {
      bool feed=((lfsr) ^ (lfsr >> 4) ^ (lfsr >> 7) ^ (lfsr >> 5) ) & 1;
      ns[i]=lfsr&1;
      lfsr=(lfsr>>1|feed<<31);
      } else {
	ns[i]=lfsr&1;
      }
      cycle[i]++;
    } else {
    ns[i]=ShapeFunctions[(flags[i]&7)](duty[i],((float)(cycle[i]++)/(float)freq[i]));
    }
    if (cycle[i]>freq[i]) {
      cycle[i]=0;
    }
    ns[i]*=(float)vol[i]/127;
    if ((flags[i]>>5!=0)) {
      float f=2*sin(3.141592653589*(((float)cut[i])/10.0)/297500);
      nslow[i]=nslow[i]+(f)*nsband[i];
      nshigh[i]=ns[i]-nslow[i]-(1.0-(float)res[i]/256.0)*nsband[i];
      nsband[i]=(f)*nshigh[i]+nsband[i];
      ns[i]=(flags[i]>>5)?(nslow[i]):(0)+(flags[i]>>6)?(nshigh[i]):(0)+(flags[i]>>7)?(nsband[i]):(0);
    }
  }
  return (ns[0]+ns[1]+ns[2]+ns[3]+ns[4]+ns[5]+ns[6]+ns[7]);
}

void soundchip::Init() {
  Reset();
  for (int i=0; i<131072; i++) {
    pcm[i]=0;
  }
  ShapeFunctions[0]=&Pulse;
  ShapeFunctions[1]=&Saw;
  ShapeFunctions[2]=&Sine;
  ShapeFunctions[3]=&Triangle;
  ShapeFunctions[4]=&Noise;
  ShapeFunctions[5]=&Pulse;
  ShapeFunctions[6]=&Pulse;
  ShapeFunctions[7]=&Pulse;
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
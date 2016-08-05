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
    if ((flags[i]&7)==4) {
      if (cycle[i]==0) {
      bool feed=((lfsr) ^ (lfsr >> 4) ^ (lfsr >> 7) ^ (lfsr >> 5) ) & 1;
      ns[i]=(lfsr&1)+127;
      lfsr=(lfsr>>1|feed<<31);
      } else {
	ns[i]=(lfsr&1)+127;
      }
      
    } else if ((flags[i]&7)==0) {
      ns[i]=((duty[i]+1)>(int)(((float)(cycle[i])/(float)freq[i])*128))+127;
    } else {
      ns[i]=(short)ShapeFunctions[(flags[i]&7)][(int)(((float)(cycle[i])/(float)freq[i])*256)];
    }
    
    if (cycle[i]++>freq[i]) {
      cycle[i]=0;
    }
    //ns[i]=(char)((short)ns[i]*(short)vol[i]/256);
    fns[i]=(float)ns[i]/128;
    if ((flags[i]>>5!=0)) {
      float f=2*sin(3.141592653589*(((float)cut[i])/10.0)/297500);
      nslow[i]=nslow[i]+(f)*nsband[i];
      nshigh[i]=fns[i]-nslow[i]-(1.0-(float)res[i]/256.0)*nsband[i];
      nsband[i]=(f)*nshigh[i]+nsband[i];
      fns[i]=(((flags[i]>>5)?(nslow[i]):(0))+((flags[i]>>6)?(nshigh[i]):(0))+((flags[i]>>7)?(nsband[i]):(0)));
    }
    fns[i]*=(float)vol[i]/256;
  }
  *l=((fns[0]+fns[1]+fns[2]+fns[3]+fns[4]+fns[5]+fns[6]+fns[7]));///256;
  *r=((fns[0]+fns[1]+fns[2]+fns[3]+fns[4]+fns[5]+fns[6]+fns[7]));///256;
}

void soundchip::Init() {
  Reset();
  for (int i=0; i<131072; i++) {
    pcm[i]=0;
  }
  ShapeFunctions[0]=SCsaw;
  ShapeFunctions[1]=SCsaw;
  ShapeFunctions[2]=SCsaw;
  ShapeFunctions[3]=SCsaw;
  ShapeFunctions[4]=SCsaw;
  ShapeFunctions[5]=SCsaw;
  ShapeFunctions[6]=SCsaw;
  ShapeFunctions[7]=SCsaw;
  /*
  ShapeFunctions[0]=&Pulse;
  ShapeFunctions[1]=&Saw;
  ShapeFunctions[2]=&Sine;
  ShapeFunctions[3]=&Triangle;
  ShapeFunctions[4]=&Noise;
  ShapeFunctions[5]=&Pulse;
  ShapeFunctions[6]=&Pulse;
  ShapeFunctions[7]=&Pulse;*/
  for (int i=0; i<256; i++) {
    SCsaw[i]=i;
    SCsine[i]=i;
    SCtriangle[i]=i;
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
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
    if (chan[i].vol==0 && !chan[i].flags.swvol) {fns[i]=0; continue;}
    if (chan[i].flags.pcm) {
      ns[i]=pcm[chan[i].pcmpos];
    } else if ((chan[i].flags.shape)==6) {
      ns[i]=randmem[i][(cycle[i]>>15)&127]*127;
    } else if ((chan[i].flags.shape)==5) {
      ns[i]=(lfsr[i]&1)*127;
    } else if ((chan[i].flags.shape)==4) {
      ns[i]=(lfsr[i]&1)*127;
    } else if ((chan[i].flags.shape)==0) {
      ns[i]=(((cycle[i]>>15)&127)>chan[i].duty)*127;
    } else {
      ns[i]=(short)ShapeFunctions[(chan[i].flags.shape)][(cycle[i]>>14)&255];
    }
    
    if (chan[i].flags.pcm) {
      pcmdec[i]+=chan[i].freq;
      if (pcmdec[i]>32767) {
        pcmdec[i]-=32767;
        if (chan[i].pcmpos<chan[i].pcmbnd) {
          chan[i].pcmpos++;
          chan[i].wc++;
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
      ocycle[i]=cycle[i];
      if (chan[i].flags.shape==5) {
        switch ((chan[i].duty>>4)&3) {
          case 0:
            cycle[i]+=chan[i].freq*1-(chan[i].freq>>3);
            break;
          case 1:
            cycle[i]+=chan[i].freq*2-(chan[i].freq>>3);
            break;
          case 2:
            cycle[i]+=chan[i].freq*4-(chan[i].freq>>3);
            break;
          case 3:
            cycle[i]+=chan[i].freq*8-(chan[i].freq>>2)-(chan[i].freq>>3);
            break;
        }
      } else {
        cycle[i]+=chan[i].freq;
      }
      if ((cycle[i]&0xf80000)!=(ocycle[i]&0xf80000)) {
        if (chan[i].flags.shape==4) {
          lfsr[i]=(lfsr[i]>>1|(((lfsr[i]) ^ (lfsr[i] >> 2) ^ (lfsr[i] >> 3) ^ (lfsr[i] >> 5) ) & 1)<<31);
        } else {
          switch ((chan[i].duty>>4)&3) {
            case 0:
              lfsr[i]=(lfsr[i]>>1|(((lfsr[i] >> 3) ^ (lfsr[i] >> 4) ) & 1)<<5);
              break;
            case 1:
              lfsr[i]=(lfsr[i]>>1|(((lfsr[i] >> 2) ^ (lfsr[i] >> 3) ) & 1)<<5);
              break;
            case 2:
              lfsr[i]=(lfsr[i]>>1|(((lfsr[i]) ^ (lfsr[i] >> 2) ^ (lfsr[i] >> 3) ) & 1)<<5);
              break;
            case 3:
              lfsr[i]=(lfsr[i]>>1|(((lfsr[i]) ^ (lfsr[i] >> 2) ^ (lfsr[i] >> 3) ^ (lfsr[i] >> 5) ) & 1)<<6);
              break;
          }
          if ((lfsr[i]&63)==0) {
            lfsr[i]=11111;
          }
        }
      }
      if (chan[i].flags.restim) {
        if (--rcycle[i]<=0) {
          cycle[i]=0;
          rcycle[i]=chan[i].restimer;
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
      bool feed=((lfsr[i]) ^ (lfsr[i] >> 2) ^ (lfsr[i] >> 3) ^ (lfsr[i] >> 5) ) & 1;
      for (int j=0; j<127; j++) {
        randmem[i][j]=randmem[i][j+1];
      }
      randmem[i][127]=lfsr[i]&1;
      lfsr[i]=(lfsr[i]>>1|feed<<31);
    }
    oldfreq[i]=chan[i].freq;
    oldflags[i]=chan[i].flags.flags;
    if (chan[i].flags.swvol) {
      if (--swvolt[i]<=0) {
        swvolt[i]=chan[i].swvol.speed;
        if (chan[i].swvol.dir) {
          chan[i].vol+=chan[i].swvol.amt;
          if (chan[i].vol>chan[i].swvol.bound && !chan[i].swvol.loop) {
            chan[i].vol=chan[i].swvol.bound;
          }
          if (chan[i].vol&0x80) {
            if (chan[i].swvol.loop) {
              if (chan[i].swvol.loopi) {
                chan[i].swvol.dir=!chan[i].swvol.dir;
                chan[i].vol=0xff-chan[i].vol;
                assert(chan[i].vol>=0);
              } else {
                chan[i].vol&=~0x80;
              }
            } else {
              chan[i].vol=0x7f;
            }
          }
        } else {
          chan[i].vol-=chan[i].swvol.amt;
          if (chan[i].vol&0x80) {
            if (chan[i].swvol.loop) {
              if (chan[i].swvol.loopi) {
                chan[i].swvol.dir=!chan[i].swvol.dir;
                chan[i].vol=-chan[i].vol;
                assert(chan[i].vol>=0);
              } else {
                chan[i].vol&=~0x80;
              }
            } else {
              chan[i].vol=0x0;
            }
          }
          if (chan[i].vol<chan[i].swvol.bound && !chan[i].swvol.loop) {
            chan[i].vol=chan[i].swvol.bound;
          }
        }
      }
    }
    if (chan[i].flags.swfreq) {
      if (--swfreqt[i]<=0) {
        swfreqt[i]=chan[i].swfreq.speed;
        if (chan[i].swfreq.dir) {
          if (chan[i].freq>(0xffff-chan[i].swfreq.amt)) {
            chan[i].freq=0xffff;
          } else {
            chan[i].freq+=chan[i].swfreq.amt;
            if ((chan[i].freq>>8)>chan[i].swfreq.bound) {
              chan[i].freq=chan[i].swfreq.bound<<8;
            }
          }
        } else {
          if (chan[i].freq<chan[i].swfreq.amt) {
            chan[i].freq=0;
          } else {
            chan[i].freq-=chan[i].swfreq.amt;
            if ((chan[i].freq>>8)<chan[i].swfreq.bound) {
              chan[i].freq=chan[i].swfreq.bound<<8;
            }
          }
        }
      }
    }
    if (chan[i].flags.swcut) {
      if (--swcutt[i]<=0) {
        swcutt[i]=chan[i].swcut.speed;
        if (chan[i].swcut.dir) {
          if (chan[i].cut>(0xffff-chan[i].swcut.amt)) {
            chan[i].cut=0xffff;
          } else {
            chan[i].cut+=chan[i].swcut.amt;
            if ((chan[i].cut>>8)>chan[i].swcut.bound) {
              chan[i].cut=chan[i].swcut.bound<<8;
            }
          }
        } else {
          if (chan[i].cut<chan[i].swcut.amt) {
            chan[i].cut=0;
          } else {
            chan[i].cut-=chan[i].swcut.amt;
            if ((chan[i].cut>>8)<chan[i].swcut.bound) {
              chan[i].cut=chan[i].swcut.bound<<8;
            }
          }
        }
      }
    }
  }
  tnsL=((nsL[0]+nsL[1]+nsL[2]+nsL[3]+nsL[4]+nsL[5]+nsL[6]+nsL[7]));///256;
  tnsR=((nsR[0]+nsR[1]+nsR[2]+nsR[3]+nsR[4]+nsR[5]+nsR[6]+nsR[7]));///256;
  *l=0.9997*(pnsL+tnsL-ppsL);
  *r=0.9997*(pnsR+tnsR-ppsR);
  pnsL=*l;
  pnsR=*r;
  ppsL=tnsL;
  ppsR=tnsR;
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
    randmem[i>>7][i&127]=rand()&1;
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
    swvolt[i]=1;
    swfreqt[i]=1;
    swcutt[i]=1;
  }
  memset(chan,0,sizeof(channel)*8);
  lfsr[0]=11111;
  lfsr[1]=11111;
  lfsr[2]=11111;
  lfsr[3]=11111;
  lfsr[4]=11111;
  lfsr[5]=11111;
  lfsr[6]=11111;
  lfsr[7]=11111;
}

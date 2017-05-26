#include <stdio.h>
#include <stdlib.h>
#include <math.h>
const float pi=3.1415926535;

class soundchip {
  char SCsaw[257];
  char SCsine[257];
  char SCtriangle[257];
  unsigned short cycle[8];
  unsigned short rcycle[8];
  unsigned int lfsr;
  char ns[8];
  bool randmem[8][128];
  char randpos[8];
  float fns[8];
  float nsL[8];
  float nsR[8];
  float nslow[8];
  float nshigh[8];
  float nsband[8];
  static char Saw(int theduty, float value);
  static char Pulse(int theduty, float value);
  static char Sine(int theduty, float value);
  static char Triangle(int theduty, float value);
  static char Noise(int theduty, float value);
  //char (*ShapeFunctions[8])(int theduty, float value);
  char* ShapeFunctions[8];
  unsigned short oldfreq[8];
  unsigned short oldflags[8];
  public:
    unsigned short resetfreq[8];
    //char pan[8];
    //unsigned char res[8];
    //char postvol[8];
    unsigned short voldcycles[8];
    unsigned short volicycles[8];
    unsigned short fscycles[8];
    unsigned char sweep[8];
    //int pcmpos[8];
    int pcmdec[8];
    //int pcmend[8];
    //int pcmreset[8];
    //unsigned char pcmmult[8];
    struct channel {
      unsigned short freq;
      char vol;
      char pan;
      union {
        unsigned short flags;
        struct {
          unsigned char shape: 3;
          unsigned char pcm: 1;
          unsigned char ring: 1;
          unsigned char fmode: 3;
          unsigned char resosc: 1;
          unsigned char resfilt: 1;
          unsigned char pcmloop: 1;
          unsigned char restim: 1;
          unsigned char swfreq: 1;
          unsigned char swvol: 1;
          unsigned char swcut: 1;
          unsigned char padding: 1;
        };
      } flags;
      unsigned short cutoff;
      unsigned char duty;
      unsigned char reson;
      unsigned short pcmpos;
      unsigned short pcmbnd;
      unsigned short pcmrst;
      struct {
        unsigned short speed;
        char amt;
        unsigned char bound;
      } swfreq;
      struct {
        unsigned short speed;
        char amt: 6;
        unsigned char loop: 1;
        unsigned char loopi: 1;
        unsigned char bound;
      } swvol;
      struct {
        unsigned short speed;
        char amt;
        unsigned char bound;
      } swcut;
      unsigned short wc;
      unsigned short restimer;
    } chan[8];
    char pcm[65280];
    void NextSample(float* l, float* r);
    void Init();
    void Reset();
};

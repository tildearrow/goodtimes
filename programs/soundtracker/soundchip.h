#include <stdio.h>
#include <stdlib.h>
#include <math.h>
const float pi=3.1415926535;

class soundchip {
  char SCsaw[257];
  char SCsine[257];
  char SCtriangle[257];
  unsigned short cycle[8];
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
    unsigned short freq[8];
    unsigned short resetfreq[8];
    char duty[8];
    char vol[8];
    char pan[8];
    int cut[8];
    unsigned char res[8];
    unsigned short flags[8];
    char postvol[8];
    unsigned short voldcycles[8];
    unsigned short volicycles[8];
    unsigned short fscycles[8];
    unsigned char sweep[8];
    int pcmpos[8];
    short pcmdec[8];
    int pcmend[8];
    int pcmreset[8];
    unsigned char pcmmult[8];
    char pcm[131072];
    void NextSample(float* l, float* r);
    void Init();
    void Reset();
};

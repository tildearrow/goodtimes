#include <math.h>
const float pi=3.1415926535;

class soundchip {
  unsigned short cycle[8];
  unsigned int lfsr;
  float ns[8];
  float nsL[8];
  float nsR[8];
  float nslow[8];
  float nshigh[8];
  float nsband[8];
  static float Saw(int theduty, float value);
  static float Pulse(int theduty, float value);
  static float Sine(int theduty, float value);
  static float Triangle(int theduty, float value);
  static float Noise(int theduty, float value);
  float (*ShapeFunctions[8])(int theduty, float value);
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
    char pcm[131072];
    float NextSample();
    void Init();
    void Reset();
};

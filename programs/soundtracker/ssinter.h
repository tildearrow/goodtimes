#include <stdio.h>

class SSInter {
  soundchip* out;
  int octave;
  int curChan;
  public:
    bool next(const char* buf, size_t set, size_t size);
    void init(soundchip* where);
    SSInter(): out(NULL), octave(4), curChan(0) {}
};

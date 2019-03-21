#include "fextra.h"

size_t fsize(FILE* f) {
  size_t tell, ret;
  tell=ftell(f);
  fseek(f,0,SEEK_END);
  ret=ftell(f);
  fseek(f,tell,SEEK_SET);
  return ret;
}

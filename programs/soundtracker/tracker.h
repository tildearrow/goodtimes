// new tracker code
#include <stdio.h> 
#include <math.h> 
#include <allegro5/allegro.h> 
#include <allegro5/allegro_audio.h> 
#include <allegro5/allegro_acodec.h> 
#include <allegro5/allegro_font.h> 
#include <allegro5/allegro_image.h> 
#include <allegro5/allegro_ttf.h> 
#include <allegro5/allegro_primitives.h> 
#include <allegro5/allegro_native_dialog.h> 
#ifdef _WIN32 
#include <windows.h> 
#else 
#ifndef __APPLE__ 
#include <X11/Xlib.h> 
#endif 
#endif 
#include <stdint.h> 
#include <string> 
#include <vector>

struct Point {
  float x, y;
};

struct Color {
  float r, g, b;
};

class Graphics {
  Point textPos;
  Color textCol;
  char putBuf[4096];
  bool inited;
  float nlPos;
  ALLEGRO_FONT* allegFont;
  ALLEGRO_COLOR alCol;
  public:
    Point getTPos();
    Point getWSize();
    void tPos(float x, float y);
    void tNLPos(float x);
    void tColor(unsigned char color);
    int printf(const char* format, ...);
    bool init(ALLEGRO_FONT* f);
    bool quit();
    Graphics(): inited(false), nlPos(0) {}
};

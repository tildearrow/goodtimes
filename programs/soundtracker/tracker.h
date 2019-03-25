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
#ifdef __APPLE__
extern "C" {
  #include "nsstub.h"
}
#endif
#include <stdint.h> 
#include <string> 
#include <vector>

#include "fextra.h"

struct Point {
  float x, y;
};

struct Color {
  float r, g, b;
};

class Graphics {
  Point textPos;
  Point scrSize;
  Color textCol;
  char putBuf[4096];
  bool inited;
  float nlPos;
  float align;
  float dpiScale;
  ALLEGRO_FONT* allegFont;
  ALLEGRO_DISPLAY* display;
  ALLEGRO_COLOR alCol;
  public:
    Point getTPos();
    Point getWSize();
    // HACK BEGIN //
    ALLEGRO_DISPLAY* _getDisplay();
    float _getScale();
    // HACK END //
    
    void tPos(float x, float y);
    void tPos(float y);
    void tNLPos(float x);
    void tAlign(float x);
    void tColor(unsigned char color);
    void setTarget(ALLEGRO_BITMAP* where);
    void trigResize();
    int printf(const char* format, ...);
    bool init(int width, int height);
    bool quit();
    Graphics(): inited(false), nlPos(0), align(0) {}
};

#include "tracker.h"

float intens[6]={
  0, 0.37, 0.53, 0.68, 0.84, 1
};

Point Graphics::getTPos() {
  return textPos;
}

void Graphics::tPos(float x, float y) {
  textPos.x=x; textPos.y=y;
  //::fprintf(stderr,"\x1b[%d;%dH",(int)y+1,(int)x+1);
}

void Graphics::tPos(float y) {
  textPos.x=nlPos; textPos.y=y;
  //::fprintf(stderr,"\x1b[%d;%dH",(int)y+1,(int)x+1);
}

void Graphics::tNLPos(float x) {
  nlPos=x;
}

void Graphics::tColor(unsigned char color) {
  textCol.r=0;
  textCol.g=0;
  textCol.b=0;
  if (color<16) {
    if (color==8) {
      textCol.r=0.5;
      textCol.g=0.5;
      textCol.b=0.5;
    } else {
      if (color<8) {
        textCol.r=(color&1)?0.75:0;
        textCol.g=(color&2)?0.75:0;
        textCol.b=(color&4)?0.75:0;
      } else {
        textCol.r=(color&1)?1:0;
        textCol.g=(color&2)?1:0;
        textCol.b=(color&4)?1:0;
      }
    }
  } else {
    if (color>231) {
      // shade of gray/grey
      textCol.r=(float)(color-232)/24.0;
      textCol.g=(float)(color-232)/24.0;
      textCol.b=(float)(color-232)/24.0;
    } else {
      textCol.r=intens[(color-16)/36];
      textCol.g=intens[((color-16)/6)%6];
      textCol.b=intens[(color-16)%6];
    }
  }
  //::fprintf(stderr,"\x1b[38;5;%dm",color);
  alCol=al_map_rgb_f(textCol.r,textCol.g,textCol.b);
}

int Graphics::printf(const char* format, ...) {
  va_list va;
  int ret;
  va_start(va,format);
  ret=vsnprintf(putBuf,4095,format,va);
  //write(2,putBuf,ret);
  
  al_hold_bitmap_drawing(true);
  for (int i=0; i<ret; i++) {
    if (putBuf[i]=='\n') {
      textPos.x=nlPos;
      textPos.y++;
    } else {
      al_draw_glyph(allegFont,alCol,8*textPos.x,12*textPos.y,putBuf[i]);
      textPos.x++;
    }
  }
  al_hold_bitmap_drawing(false);
  va_end(va);
  return ret;
}

bool Graphics::init(ALLEGRO_FONT* f) {
  tPos(0,0);
  tColor(15);
  allegFont=f;
  //::fprintf(stderr,"\x1b[2J");
  return true;
}

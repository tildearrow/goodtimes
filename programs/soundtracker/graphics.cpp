#include "tracker.h"

float intens[6]={
  0, 0.37, 0.53, 0.68, 0.84, 1
};

double getScale() {
  char* env;
  // try with environment variable
  env=getenv("TRACKER_SCALE");
  if (env!=NULL) {
    return atof(env);
  }
#if defined(__linux__)
  // linux (wayland) code here
#elif defined(_WIN32)
  // windows code here
  HDC disp;
  int dpi;
  disp=GetDC(NULL);
  if (disp==NULL) {
    return 1;
  }
  dpi=GetDeviceCaps(disp,LOGPIXELSX);
  ReleaseDC(NULL,disp);
  return (double)dpi/96.0;
#elif defined(__APPLE__)
  // macOS code here
  double dpi;
  if ((dpi=nsStubDPI())>0) {
    return dpi;
  }
#elif defined(__ANDROID__)
  // android code here
#endif
#if defined(__unix__)
  // X11
  Display* disp;
  int dpi;
  disp=XOpenDisplay(NULL);
  if (disp!=NULL) {
    dpi=(int)(0.5+(25.4*(double)XDisplayWidth(disp,XDefaultScreen(disp))/(double)XDisplayWidthMM(disp,XDefaultScreen(disp))));
    XCloseDisplay(disp);
    return (double)dpi/96.0;
  }
#endif
  // assume 1
  return 1;
}

Point Graphics::getTPos() {
  return textPos;
}

void Graphics::tPos(float x, float y) {
  textPos.x=x; textPos.y=y;
  //fprintf(stderr,"\x1b[%d;%dH",(int)y+1,(int)x+1);
}

void Graphics::tPos(float y) {
  textPos.x=nlPos; textPos.y=y;
  //fprintf(stderr,"\x1b[%d;%dH",(int)y+1,(int)nlPos+1);
}

void Graphics::tNLPos(float x) {
  nlPos=x;
}

void Graphics::tAlign(float x) {
  align=x;
}

void Graphics::tColor(unsigned char color) {
  textCol.a=1;
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
  //fprintf(stderr,"\x1b[38;5;%dm",color);
}

// we blittin' baby!
// later
int Graphics::printf(const char* format, ...) {
  va_list va;
  int ret;
  va_start(va,format);
  ret=vsnprintf(putBuf,4095,format,va);
  
  if (align!=0) {
    tPos(textPos.x-(float)ret*align,textPos.y);
  }
  
  for (int i=0; i<ret; i++) {
    fputc(putBuf[i],stderr);
    if (putBuf[i]=='\n' || putBuf[i]=='\r') {
      textPos.x=nlPos;
      textPos.y++;
      if (nlPos!=0) {
        fprintf(stderr,"\x1b[%d;%dH",(int)textPos.y+1,(int)textPos.x+1);
      }
    } else {
      //al_draw_glyph(allegFont,alCol,8*textPos.x,12*textPos.y,putBuf[i]);
      textPos.x++;
    }
  }
  va_end(va);
  return ret;
}

void Graphics::setTarget(SDL_Texture* where) {
  /*
  if (where==NULL) {
    SDL_SetRenderTarget(sdlRend,NULL);
  } else {
    SDL_SetRenderTarget(sdlRend,where);
  }*/
}

void Graphics::trigResize() {
  return; // TODO
  //scrSize.x=al_get_display_width(display)/dpiScale;
  //scrSize.y=al_get_display_height(display)/dpiScale;
}

Point Graphics::getWSize() {
  return scrSize;
}

bool Graphics::quit() {
  return true; // TODO
}

bool Graphics::preinit() {
  /*
  if (!al_init()) return false;
  
  al_init_font_addon();
  al_init_ttf_addon();
  al_init_primitives_addon();
  al_init_image_addon();
  */
  
  if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_EVENTS)==-1) return false;
  if (TTF_Init()==-1) return false;
  
  return true;
}

bool Graphics::init(int width, int height) {
  tPos(0,0);
  tColor(15);
  
  dpiScale=getScale();
  
  sdlWin=SDL_CreateWindow("soundtracker (SDL)",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width*dpiScale,height*dpiScale,SDL_WINDOW_RESIZABLE);
  if (!sdlWin) return false;
  
  sdlRend=SDL_CreateRenderer(sdlWin,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_TARGETTEXTURE);
  if (!sdlRend) return false;
  
  sdlFont=TTF_OpenFont("unifont.ttf",16);
  if (sdlFont==NULL) {
    printf("unifont.ttf wasn't found...");
    return false;
  }
  scrSize.x=width;
  scrSize.y=height;
  
  //fprintf(stderr,"\x1b[2J");
  return true;
}

SDL_Renderer* Graphics::_getDisplay() {
  return sdlRend;
}

float Graphics::_getScale() {
  return dpiScale;
}

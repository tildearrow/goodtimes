// new tracker code
#ifndef _TRACKER_H
#define _TRACKER_H
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
#include <dirent.h>
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
    
    // HACK: Allegro methods wrapped for eventual SDL transition
    ALLEGRO_COLOR _WRAP_map_rgb(unsigned char r, unsigned char g, unsigned char b) {
      return al_map_rgb(r,g,b);
    }
    ALLEGRO_COLOR _WRAP_map_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
      return al_map_rgba(r,g,b,a);
    }
    ALLEGRO_COLOR _WRAP_map_rgb_f(float r, float g, float b) {
      return al_map_rgb_f(r,g,b);
    }
    ALLEGRO_COLOR _WRAP_map_rgba_f(float r, float g, float b, float a) {
      return al_map_rgba_f(r,g,b,a);
    }
    bool _WRAP_install_mouse() {
      return al_install_mouse();
    }
    bool _WRAP_install_keyboard() {
      return al_install_keyboard();
    }
    double _WRAP_get_time() {
      return al_get_time();
    }
    bool _WRAP_wait_for_vsync() {
      return al_wait_for_vsync();
    }
    void _WRAP_flip_display() {
      al_flip_display();
    }
    void _WRAP_draw_filled_rectangle(float x1, float y1, float x2, float y2, ALLEGRO_COLOR color) {
      al_draw_filled_rectangle(x1,y1,x2,y2,color);
    }
    void _WRAP_clear_to_color(ALLEGRO_COLOR color) {
      al_clear_to_color(color);
    }
    ALLEGRO_BITMAP* _WRAP_create_bitmap(int w, int h) {
      return al_create_bitmap(w,h);
    }
    void _WRAP_destroy_bitmap(ALLEGRO_BITMAP* bitmap) {
      al_destroy_bitmap(bitmap);
    }
    bool _WRAP_get_next_event(ALLEGRO_EVENT_QUEUE* q, ALLEGRO_EVENT* e) {
      return al_get_next_event(q,e);
    }
    void _WRAP_rest(double t) {
      al_rest(t);
    }
    void _WRAP_use_transform(const ALLEGRO_TRANSFORM* t) {
      al_use_transform(t);
    }
    void _WRAP_scale_transform(ALLEGRO_TRANSFORM* t, float x, float y) {
      al_scale_transform(t,x,y);
    }
    void _WRAP_identity_transform(ALLEGRO_TRANSFORM* t) {
      al_identity_transform(t);
    }
    void _WRAP_draw_pixel(float x, float y, ALLEGRO_COLOR color) {
      al_draw_pixel(x,y,color);
    }
    void _WRAP_register_event_source(ALLEGRO_EVENT_QUEUE* q, ALLEGRO_EVENT_SOURCE* s) {
      al_register_event_source(q,s);
    }
    ALLEGRO_EVENT_SOURCE* _WRAP_get_keyboard_event_source() {
      return al_get_keyboard_event_source();
    }
    ALLEGRO_EVENT_SOURCE* _WRAP_get_display_event_source(ALLEGRO_DISPLAY* d) {
      return al_get_display_event_source(d);
    }
    ALLEGRO_EVENT_QUEUE* _WRAP_create_event_queue() {
      return al_create_event_queue();
    }
    ALLEGRO_BITMAP* _WRAP_load_bitmap(const char* fn) {
      return al_load_bitmap(fn);
    }
    void _WRAP_draw_bitmap(ALLEGRO_BITMAP* bitmap, float x, float y, int flags) {
      al_draw_bitmap(bitmap,x,y,flags);
    }
    void _WRAP_set_blender(int op, int source, int dest) {
      al_set_blender(op,source,dest);
    }
    void _WRAP_draw_line(float x1, float y1, float x2, float y2, ALLEGRO_COLOR color, float thick) {
      al_draw_line(x1,y1,x2,y2,color,thick);
    }
    void _WRAP_draw_rectangle(float x1, float y1, float x2, float y2, ALLEGRO_COLOR color, float thick) {
      al_draw_rectangle(x1,y1,x2,y2,color,thick);
    }
    void _WRAP_reset_clipping_rectangle() {
      al_reset_clipping_rectangle();
    }
    void _WRAP_set_clipping_rectangle(int x, int y, int w, int h) {
      al_set_clipping_rectangle(x,y,w,h);
    }
    int _WRAP_get_bitmap_width(ALLEGRO_BITMAP* bitmap) {
      return al_get_bitmap_width(bitmap);
    }
    void _WRAP_draw_bitmap_region(ALLEGRO_BITMAP* bitmap, float x, float y, float w, float h, float dx, float dy, int flags) {
      al_draw_bitmap_region(bitmap,x,y,w,h,dx,dy,flags);
    }
    bool _WRAP_key_down(ALLEGRO_KEYBOARD_STATE* ks, int kc) {
      return al_key_down(ks,kc);
    }
    void _WRAP_get_keyboard_state(ALLEGRO_KEYBOARD_STATE* ks) {
      al_get_keyboard_state(ks);
    }
    void _WRAP_get_mouse_state(ALLEGRO_MOUSE_STATE* ms) {
      al_get_mouse_state(ms);
    }
    void _WRAP_draw_scaled_bitmap(ALLEGRO_BITMAP* bitmap, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags) {
      al_draw_scaled_bitmap(bitmap,sx,sy,sw,sh,dx,dy,dw,dh,flags);
    }
    void _WRAP_draw_rotated_bitmap(ALLEGRO_BITMAP* bitmap, float cx, float cy, float x, float y, float r, int flags) {
      al_draw_rotated_bitmap(bitmap,cx,cy,x,y,r,flags);
    }
    void _WRAP_draw_circle(float x, float y, float r, ALLEGRO_COLOR color, float thick) {
      al_draw_circle(x,y,r,color,thick);
    }
    
    void tPos(float x, float y);
    void tPos(float y);
    void tNLPos(float x);
    void tAlign(float x);
    void tColor(unsigned char color);
    void setTarget(ALLEGRO_BITMAP* where);
    void trigResize();
    int printf(const char* format, ...);
    bool preinit();
    bool init(int width, int height);
    bool quit();
    Graphics(): inited(false), nlPos(0), align(0) {}
};

#endif

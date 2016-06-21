// tildearrow's game engine!
#include <math.h>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include "engine.h"
#define PointInRect(x1,y1,x2,y2,checkx,checky) ((checkx>x1&&checkx<x2&&checky>y1&&checky<y2)?(1):(0))
ALLEGRO_MOUSE_STATE* mousestate;
ALLEGRO_KEYBOARD_STATE* keystate;
int mousebutton;
int mousebutton1;

void setinputpointers(ALLEGRO_MOUSE_STATE* m,ALLEGRO_KEYBOARD_STATE* k){
  mousestate=m;
  keystate=k;
}

bool boxcollisioncheck(float x1, float y1, float x2, float y2, float w1, float h1, float w2, float h2) {
  return ((w1/2)+(w2/2)<fabs(x1-x2) || (h1/2)+(h2/2)<fabs(y1-y2));
}

bool radiuscollisioncheck(float x1, float y1, float x2, float y2, float r1, float r2) {
  return false; // anti-error C<insertnumberhere>: will NOT return a value!
}

void entity::calculateevents(void) {
  events[evMouseIn]=false;
  events[evMouseLeftClick]=false;
  events[evMouseLeftPressed]=false;
  if (PointInRect(x-((al_get_bitmap_width(spr)*xscale)/2),y-((al_get_bitmap_height(spr)*yscale)/2),x+((al_get_bitmap_width(spr)*xscale)/2),y+((al_get_bitmap_height(spr)*yscale)/2),mousestate->x,mousestate->y)) {
    if (mousebutton) {
      if (mousebutton!=mousebutton1) {
	events[evMouseLeftClick]=true;
      }
      events[evMouseLeftPressed]=true;
    }
    events[evMouseIn]=true;
  }
}

void entity::destroy() {
  exists=false;
}

void entity::draw() {
  if (spr!=NULL) {
    al_draw_scaled_rotated_bitmap(spr,al_get_bitmap_width(spr)/2,al_get_bitmap_height(spr)/2,x,y,xscale,yscale,-angle*ALLEGRO_PI/1800,0);
    al_draw_rectangle(x-((al_get_bitmap_width(spr)*xscale)/2),y-((al_get_bitmap_height(spr)*yscale)/2),x+((al_get_bitmap_width(spr)*xscale)/2),y+((al_get_bitmap_height(spr)*yscale)/2),(events[4])?(al_map_rgb(255,0,0)):(al_map_rgb(255,255,255)),1);
  } else {
    al_draw_filled_rectangle(x,y,x+16,y+16,(events[4])?(al_map_rgb(255,0,0)):(al_map_rgb(255,255,255)));
  }
}

void entity::update() {
  calculateevents();
  this->x+=cos((this->direction)*ALLEGRO_PI/180)*this->speed;
  this->y-=sin((this->direction)*ALLEGRO_PI/180)*this->speed;
}

void scene::update() {
  mousebutton1=mousebutton;
  mousebutton=mousestate->buttons&1;
  for (ei=entities.begin();ei!=entities.end();ei++){
    if (ei->exists) {
      ei->update();
    }
  }
  // clean dead entities (at a rate of 1 per second)
  bool CleanEntity=false;
  for (ei=entities.begin();ei!=entities.end();ei++){
    if (!(ei->exists)) {
      CleanEntity=true; break;
    }
  }
  if (CleanEntity) {
    entities.erase(ei);
  }
}

void scene::draw() {
  for (ei=entities.begin();ei!=entities.end();ei++){
    if (ei->exists) {
      ei->draw();
    }
  }
}

int scene::spawn(float xpos, float ypos) {
  // first resize the entity vector
    entities.resize(entities.size()+1);
    // then set attributes for new entity
    entities[entities.size()-1].x=xpos;
    entities[entities.size()-1].y=ypos;
    entities[entities.size()-1].xscale=1;
    entities[entities.size()-1].yscale=1;
    entities[entities.size()-1].exists=true;
    return (entities.size()-1);
}

int add(int x, int y)
{
    return x + y;
}

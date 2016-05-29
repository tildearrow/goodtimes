#include <stdio.h>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <math.h>

#include "engine.h"

#define DEFAULT_FPS 60.0

// graphic stuff
ALLEGRO_DISPLAY* display; // main display
int dw, dh;
bool doredraw=false;
ALLEGRO_FONT* fText;
ALLEGRO_BITMAP* bitmap;

// sound stuff

// controller stuff
ALLEGRO_MOUSE_STATE ms;
ALLEGRO_KEYBOARD_STATE ks;

// game logic stuff
ALLEGRO_TIMER* frame;
ALLEGRO_EVENT_QUEUE* eq;

// game variables
float musicvol=0;
float soundvol=1;
float gamespeed=1;

// the first thing
int ver=9;
int lost=0;
int totalspawned=0;
bool endgame=false;
int score=0;

// some stuff
scene gameScene;
int counter=8;

void gamelogic(){
  // gather mouse and keyboard status
  al_get_mouse_state(&ms);
  al_get_keyboard_state(&ks);
  counter--;
  if (counter<=0) {
    counter=8;
    // spawn entity
    // first resize the entity vector
    int eid;
    eid=gameScene.spawn(-128,rand()%dh);
    gameScene.entities[eid].speed=fmax(1,totalspawned/10);
    gameScene.entities[eid].spr=bitmap;
    totalspawned++;
  }
  for (gameScene.ei=gameScene.entities.begin();gameScene.ei!=gameScene.entities.end();gameScene.ei++){
    if (gameScene.ei->exists) {
      //gameScene.ei->direction++;
      //gameScene.ei->angle++;
      if (gameScene.ei->x>dw) {gameScene.ei->destroy(); lost++; endgame=true;}
      if (gameScene.ei->events[evMouseLeftClick]) {gameScene.ei->destroy(); score++;}
    }
  }
  gameScene.update();
}

int main(int argc, char **argv) {
    al_init();
    
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    
    // create stuff
    //al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    dw=1280; dh=720; // default sizes
    if (!(display=al_create_display(dw,dh))) {printf("error\n");return 1;}
    dw=al_get_display_width(display); dh=al_get_display_height(display); // default sizes
    if (!(frame=al_create_timer(1.0/DEFAULT_FPS))) {printf("error\n");return 1;}
    if (!(eq=al_create_event_queue())) {printf("error\n");return 1;}
    if (!(bitmap=al_load_bitmap("../alexlogo.png"))) {printf("error\n");return 1;}
    
    // load fonts
    fText=al_load_ttf_font("text.ttf",20,0);
    
    // register event sources
    al_register_event_source(eq,al_get_display_event_source(display));
    al_register_event_source(eq,al_get_timer_event_source(frame));
    
    // clear the display
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(0,0,0));
    al_wait_for_vsync();
    al_flip_display();
    
    // wait a second
    //al_rest(1);
    
    setinputpointers(&ms,&ks);
    
    // begin the game
    al_start_timer(frame);
    
    while(1){
      // get next event
      ALLEGRO_EVENT event;
      al_wait_for_event(eq,&event);
      if (event.type==ALLEGRO_EVENT_TIMER){
	
	doredraw=true;
      } else if (event.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
	endgame=true;
      }
      if (doredraw && al_is_event_queue_empty(eq)){
	//doredraw=false;
	gamelogic();
	al_set_target_backbuffer(display);
	al_clear_to_color(al_map_rgb(0,0,0));
	gameScene.draw();
	al_draw_textf(fText,al_map_rgb(255,255,255),0,0,ALLEGRO_ALIGN_LEFT,"game engine dev%d",ver);
	al_draw_textf(fText,al_map_rgb(255,255,255),0,20,ALLEGRO_ALIGN_LEFT,"entities: %d",(int)gameScene.entities.size());
	al_draw_textf(fText,al_map_rgb(255,255,255),0,40,ALLEGRO_ALIGN_LEFT,"score: %d",score);
	al_flip_display();
	if (endgame) {break;}
      }
    }
    
    // end the game
    printf("game over... score: %d\n",score);
    al_stop_timer(frame);
    return 0;
}

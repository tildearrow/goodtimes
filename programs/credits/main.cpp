// play once!
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>

#define DEFAULT_FPS 60.0

// graphic stuff
ALLEGRO_DISPLAY* display; // main display
int dw, dh;
bool doredraw=false;
ALLEGRO_FONT* fText;
const char* credits[52]={"umm,","this thing should","sync perfectly",
  "","because I will NOT tolerate","any jitter...",
  "","(not yet!)","(not yet!)",
  "","(not yet!)","(not yet!)!",
  "","","(not yet!)",
  "","","*(not yet!)*",
  "(not yet!)","","*(not yet!)*",
  "(not yet!)","","*(not yet!)*",
  "(not yet!)","","*(not yet!)*",
  "(not yet!)","","*(not yet!)*",
  "(not yet!)","","*(not yet!)*","-","-","-","-","-","-","-","-","-","-",
  "","*(not yet!)*","-","-","-","-","-","",
  ""
};

// sound stuff

// controller stuff

// game logic stuff
ALLEGRO_TIMER* frame;
ALLEGRO_EVENT_QUEUE* eq;

// game variables
float musicvol=0;
float soundvol=1;
float gamespeed=1;

// the first thing
int ver=1;

int main(int argc, char **argv) {
    al_init();
    
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    
    // create stuff
    dw=1600; dh=900; // default sizes
    //al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    if (!(display=al_create_display(dw,dh))) {printf("error\n");return 1;}
    if (!(frame=al_create_timer(1.0/DEFAULT_FPS))) {printf("error\n");return 1;}
    if (!(eq=al_create_event_queue())) {printf("error\n");return 1;}
    
    // load fonts
    fText=al_load_ttf_font("text.ttf",48,0);
    
    // register event sources
    al_register_event_source(eq,al_get_display_event_source(display));
    al_register_event_source(eq,al_get_timer_event_source(frame));
    
    // clear the display
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(0,0,0));
    al_wait_for_vsync();
    al_flip_display();
    
    // wait a second
    al_rest(5);
    
    // begin the game
    double thing=0;
    int creditspos=0;
    al_start_timer(frame);
    printf("remember, run just once!\n");
    
    while(1){
      // get next event
      ALLEGRO_EVENT event;
      al_wait_for_event(eq,&event);
      if (event.type==ALLEGRO_EVENT_TIMER){
	thing+=0.05;
	creditspos++;
	doredraw=true;
      } else if (event.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
	break;
      }
      if (doredraw && al_is_event_queue_empty(eq)){
	doredraw=false;
	al_set_target_backbuffer(display);
	al_clear_to_color(al_map_rgb(0,0,0));
	// some sort of efficient credits display
	for (int ii=fmax((creditspos/64)-24,0);ii<fmin((creditspos/64)+8,52);ii++){
	al_draw_text(fText,al_map_rgb(255,255,255),dw/2,dh-creditspos+(ii*64),ALLEGRO_ALIGN_CENTER,credits[ii]);
        }
	//al_draw_filled_rectangle((dw/2)+sin(thing)*(dw/2),0,(dw/2)+sin(thing)*(dw/2)+16,dh,al_map_rgb(255,255,255));
	al_flip_display();
      }
    }
    
    // end the game
    al_stop_timer(frame);
    return 0;
}

// counter
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_color.h>
#include <math.h>

#define DEFAULT_FPS 100.0

// graphic stuff
ALLEGRO_DISPLAY* display; // main display
int dw, dh;
bool doredraw=false;
ALLEGRO_FONT* fText;

// sound stuff

// controller stuff
ALLEGRO_MOUSE_STATE ms;

// game logic stuff
ALLEGRO_TIMER* frame;
ALLEGRO_EVENT_QUEUE* eq;

// game variables
long long int ts;
float time1, time2;

// the first thing
int ver=1;

float lengthdir_x(float len,float dir){
	return len*cos(dir*(ALLEGRO_PI/180));
}
float lengthdir_y(float len,float dir){
	return len*sin(dir*(ALLEGRO_PI/180));
}

int main(int argc, char **argv) {
    al_init();
    
    al_install_keyboard();
    al_install_mouse();
    al_init_font_addon();
    al_init_ttf_addon();
    
    // create stuff
    dw=160; dh=32; // default sizes
    //al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    if (!(display=al_create_display(dw,dh))) {printf("error\n");return 1;}
    if (!(frame=al_create_timer(1.0/DEFAULT_FPS))) {printf("error\n");return 1;}
    if (!(eq=al_create_event_queue())) {printf("error\n");return 1;}
    
    // load fonts
    fText=al_load_ttf_font("text.ttf",18,0);
    
    // register event sources
    al_register_event_source(eq,al_get_display_event_source(display));
    al_register_event_source(eq,al_get_timer_event_source(frame));
    
    // clear the display
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(0,0,0));
    al_wait_for_vsync();
    al_flip_display();
    
    // begin the game
    double thing=0;
    al_start_timer(frame);
    
    while(1){
      // get next event
      ALLEGRO_EVENT event;
      al_wait_for_event(eq,&event);
      if (event.type==ALLEGRO_EVENT_TIMER){
	doredraw=true;
	time1+=1/DEFAULT_FPS;
	al_get_mouse_state(&ms);
	if (ms.buttons&1){
	  time1=0;
	}
      } else if (event.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
	break;
      }
      if (doredraw && al_is_event_queue_empty(eq)){
	doredraw=false;
	al_set_target_backbuffer(display);
	al_clear_to_color(al_map_rgb(0,0,0));
	// drawing code
	al_draw_textf(fText,al_map_rgb(255,255,255),8,8,0,"%d:%.2d:%.2d:%.2d.%.2d",(int)(time1/86400),(int)(time1/3600)%24,(int)(time1/60)%60,(int)(time1)%60,(int)(fmod(time1,1)*50));
	al_flip_display();
      }
    }
    
    // end the game
    al_stop_timer(frame);
    return 0;
}

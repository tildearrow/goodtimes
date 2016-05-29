// metronome!
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>

#define DEFAULT_FPS 50.0

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
float musicvol=0;
float soundvol=1;
float gamespeed=1;
double tempo=125;
double counter=0;
double frames=0;

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
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    
    // create stuff
    dw=400; dh=400; // default sizes
    if (argc>1) {
    tempo=atof(argv[1]);
    }
    //al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    if (!(display=al_create_display(dw,dh))) {printf("error\n");return 1;}
    if (!(frame=al_create_timer(1.0/DEFAULT_FPS))) {printf("error\n");return 1;}
    if (!(eq=al_create_event_queue())) {printf("error\n");return 1;}
    
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
    
    // begin the game
    double thing=0;
    al_start_timer(frame);
    
    while(1){
      // get next event
      ALLEGRO_EVENT event;
      al_wait_for_event(eq,&event);
      if (event.type==ALLEGRO_EVENT_TIMER){
	doredraw=true;
	counter+=(tempo/DEFAULT_FPS)/15;
	frames++;
	al_get_mouse_state(&ms);
	if (ms.buttons&1){
	  counter=0;
	  frames=0;
	}
      } else if (event.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
	break;
      }
      if (doredraw && al_is_event_queue_empty(eq)){
	doredraw=false;
	al_set_target_backbuffer(display);
	al_clear_to_color(al_map_rgb(0,0,0));
	// drawing code
	al_draw_textf(fText,al_map_rgb(255,255,255),8,8,0,"%.3f BPM, %d:%d:%.3d",tempo,(int)counter/16+1,(int)(counter/4)%4+1,(int)(fmod(counter,16)*48));
	al_draw_textf(fText,al_map_rgb(255,255,255),8,24,0,"NTSC: %.2d/30 - %.2d/60",(int)(frames*(30/DEFAULT_FPS))%30,(int)(frames*(60/DEFAULT_FPS))%60);
	al_draw_textf(fText,al_map_rgb(255,255,255),8,40,0,"PAL/SECAM: %.2d/25 - %.2d/50",(int)(frames*(25/DEFAULT_FPS))%25,(int)(frames*(50/DEFAULT_FPS))%50);
	al_draw_textf(fText,al_map_rgb(255,255,255),8,56,0,"NTSC: %d PAL: %d",(int)(frames*(60/DEFAULT_FPS)),(int)(frames*(50/DEFAULT_FPS)));
	al_draw_textf(fText,al_map_rgb(255,255,255),8,72,0,"rt: %d:%.2d:%.2d.%.2d",(int)frames/(60*3600),((int)frames/3600)%60,((int)frames/(int)DEFAULT_FPS)%60,(int)(frames*(100/DEFAULT_FPS))%100);
	al_draw_line(dw/2,dh/2,dw/2+lengthdir_x(96,fmod(counter,16)*22.5-90),dh/2+lengthdir_y(96,fmod(counter,16)*22.5-90),al_map_rgb(255,255,255),2);
	al_flip_display();
      }
    }
    
    // end the game
    al_stop_timer(frame);
    return 0;
}

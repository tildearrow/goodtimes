// video test suite
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>

#define DEFAULT_FPS 60

// graphic stuff
ALLEGRO_DISPLAY* display; // main display
int dw, dh;
bool doredraw=false;
ALLEGRO_FONT* fText;
bool space=false;
ALLEGRO_KEYBOARD_STATE ks;

// sound stuff

// controller stuff
ALLEGRO_MOUSE_STATE ms;

// game logic stuff
ALLEGRO_TIMER* frame;
ALLEGRO_EVENT_QUEUE* eq;

int testnumber=0;
int posX=0;
int posY=0;
#define CIRCLES 16

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
    dw=1280; dh=800; // default sizes
    //al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    //al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    if (!(display=al_create_display(dw,dh))) {printf("error\n");return 1;}
    dw=al_get_display_width(display);
    dh=al_get_display_height(display);
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
	al_get_keyboard_state(&ks);
	if (al_key_down(&ks,ALLEGRO_KEY_SPACE)) {
	  if (!space) {testnumber++; posX=0; posY=0;}
	  space=true;
	} else {space=false;}
	if (al_key_down(&ks,ALLEGRO_KEY_ESCAPE)) {break;}

      } else if (event.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
	break;
      }
      if (doredraw && al_is_event_queue_empty(eq)){
	doredraw=false;
      switch (testnumber) {
	 // case 0: posX++; if (posX>47) {testnumber++;posX=0;}; break; // black test
	 // case 1: posX++; if (posX>47) {testnumber++;posX=0;};break; // red test
	 // case 2:posX++; if (posX>47) {testnumber++;posX=0;}; break; // green test
	 // case 3: posX++; if (posX>47) {testnumber++;posX=0;};break; // blue test
	 // case 4:posX++; if (posX>47) {testnumber=1;posX=0;} break; // white test
	  case 5: posX+=4; if ((posX-128)>dw) posX=-128; break; // vertical line test
	  case 6: posX+=4; if ((posX-128)>dh) posX=-128; break; // horizontal line test
	  case 7: posX=!posX; break; // frame stutter test
	  case 8: posX++; if (posX>2) posX=0; break; // color test
	  case 9: posX=!posX; break; // interlace test
	  case 10: posX=!posX; break; // interlace test 2
	  case 11: posX=!posX; break; // horizontal interlace test
	  case 12: posX+=4; if (posX>256) {testnumber=13; posX=0;}; break; // bw fade test
	  case 13: posX+=4; if (posX>256) {testnumber=14; posX=0;}; break; // red fade test
	  case 14: posX+=4; if (posX>256) {testnumber=15; posX=0;}; break; // green fade test
	  case 15: posX+=4; if (posX>256) {testnumber=16; posX=0;}; break; // blue fade test
	  case 16: posX=!posX; break; // phase test
          case 17: posX++; break; // sine hortest
          case 18: posX++; break; // circle test
          case 19: posX++; break; // hue test
          case 20: posX++; break; // circle test 2
	}
	al_set_target_backbuffer(display);
	al_clear_to_color(al_map_rgb(0,0,0));
	// drawing code
	switch (testnumber) {
	  case 0: al_clear_to_color(al_map_rgb(0,0,0)); break;
	  case 1: al_clear_to_color(al_map_rgb(255,0,0)); break;
	  case 2: al_clear_to_color(al_map_rgb(0,255,0)); break;
	  case 3: al_clear_to_color(al_map_rgb(0,0,255)); break;
	  case 4: al_clear_to_color(al_map_rgb(255,255,255)); break;
	  case 5: al_draw_line(posX,0,posX,dh,al_map_rgb(255,255,255),128); break;
	  case 6: al_draw_line(0,posX,dw,posX,al_map_rgb(255,255,255),128); break;
	  case 7: al_clear_to_color(al_map_rgb(posX*255,posX*255,posX*255)); break;
	  case 8: for (int i=0;i<dw+posX;i++) {al_draw_line(i-posX,0,i-posX,dh,al_map_rgb((i%3==0)*255,(i%3==1)*255,(i%3==2)*255),1);}; break;
	  case 9: al_draw_filled_rectangle(posX*(dw/2),0,dw/2+(posX*dw/2),dh,al_map_rgb(255,255,255)); break;
	  case 10: for (int i=0;i<dh/2;i++) {al_draw_line(0,posX+(i*2),dw,posX+(i*2),al_map_rgb(255,255,255),1);}; break;
	  case 11: for (int i=0;i<dw/2;i++) {al_draw_line(posX+(i*2),0,posX+(i*2),dh,al_map_rgb(255,255,255),1);}; break;
	  case 12: al_clear_to_color(al_map_rgb(posX,posX,posX)); break;
	  case 13: al_clear_to_color(al_map_rgb(posX,0,0)); break;
	  case 14: al_clear_to_color(al_map_rgb(0,posX,0)); break;
	  case 15: al_clear_to_color(al_map_rgb(0,0,posX)); break;
	  case 16: for (int i=0;i<dh;i++) {al_draw_line(0,i,dw,i,((posX+i)&1)?(al_map_rgb(0,255,0)):(al_map_rgb(255,0,255)),1);}; break;
          case 17: al_draw_line((dw/2)+sin((float)posX/32.0)*(float)dw/2.0,0,(dw/2)+sin(ALLEGRO_PI+((float)posX/32.0))*(float)dw/2.0,dh,al_map_rgb(255,255,255),128); break;
          case 18: 
            for (int i=0;i<CIRCLES;i++) {al_draw_circle((dw/2)+cos(((float)posX/32.0)+(((float)i/CIRCLES)*2*ALLEGRO_PI))*256,(dh/2)+sin(((float)posX/32.0)+(((float)i/CIRCLES)*2*ALLEGRO_PI))*256,32,al_color_hsv((float)i*360/(float)CIRCLES,1,1),8);} break;
          case 19: al_clear_to_color(al_color_hsv(posX,1,1)); break;
          case 20: for (int i=0;i<CIRCLES;i++) {al_draw_circle((dw/2)+cos(((float)posX/32.0)+(((float)i/CIRCLES)*2*ALLEGRO_PI))*(256+sin((float)posX/512)*64),(dh/2)+sin(((float)posX/32.0)+(((float)i/CIRCLES)*2*ALLEGRO_PI))*(256+sin((float)posX/512)*64),33+sin(((float)posX+(float)(i*4))/31)*32,al_color_hsv((float)i*360/(float)CIRCLES,1,1),8);} break;
          case 21: for (int i=0;i<dw+posX;i++) {al_draw_line(i-posX,0,i-posX,dh,al_map_rgb((i%3==0)*255,(i%3==1)*255,(i%3==2)*255),1);}; break;
	}
	al_flip_display();
      }
    }
    
    // end the game
    al_stop_timer(frame);
    return 0;
}

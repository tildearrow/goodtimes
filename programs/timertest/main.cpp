#include <stdio.h>
#include <allegro5/allegro.h>
ALLEGRO_TIMER* timer;
ALLEGRO_EVENT_QUEUE* eq;
double time1;
double time2;
double time3;
double time4;

int main(int argc, char **argv) {
    time1=0;
    time2=0;
    time3=0;
    time4=0;
    al_init();
    eq=al_create_event_queue();
    if (argc<2) {fprintf(stderr,"sorry, i'm crashing now...\n");}
    timer=al_create_timer(1.0/(double)atoi(argv[1]));
    al_register_event_source(eq,al_get_timer_event_source(timer));
    al_start_timer(timer);
    while (1) {
      ALLEGRO_EVENT ev;
      al_wait_for_event(eq,&ev);
      switch (ev.type) {
	case ALLEGRO_EVENT_TIMER:
	  time2=time1;
	  time1=al_get_time();
	  time4=time3;
	  time3=time1-time2;
	  printf("timings: %f, timerdrift: %f\n",time1-time2,time3-time4);
	  
      }
    }
}

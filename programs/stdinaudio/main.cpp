#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <queue>
//#include <jack/jack.h>
#define PERIODS 1024

ALLEGRO_AUDIO_STREAM* as;
ALLEGRO_EVENT_QUEUE* eq;
ALLEGRO_EVENT* ev;
ALLEGRO_THREAD* input;
short* buf;
int period=0;
int symspeed=48000;
bool bit=0;
short data=0;
int pos=0;
int warns=0;
unsigned char cs;
double timing1=0;
double timing2=0;
std::queue<char> bufqueue;

static void *thread_input(ALLEGRO_THREAD *thread, void *arg){
    while (1) {
      if (bufqueue.size()<131072) {
      bufqueue.push(getchar());
}
    }
}

char popfront() {
   if (bufqueue.size()<1) {
    printf("what the\n");
} else {
    char chartoreturn;
    chartoreturn=bufqueue.front();
    bufqueue.pop();
    return chartoreturn;
 }
}

int main(int argc, char **argv) {
    printf("ok\n");
    if (argc<2) {
    printf("warning: rate not provided, using default of %d\n",symspeed);
} else {
    symspeed=atoi(argv[1]);
}
    al_init();
    al_install_audio();
    al_reserve_samples(2);
    ev=new ALLEGRO_EVENT;
    as=al_create_audio_stream(4,PERIODS,symspeed,ALLEGRO_AUDIO_DEPTH_INT16,ALLEGRO_CHANNEL_CONF_2);
    eq=al_create_event_queue();
    al_attach_audio_stream_to_mixer(as,al_get_default_mixer());
    al_register_event_source(eq,al_get_audio_stream_event_source(as));
    input=al_create_thread(thread_input,NULL);
    al_start_thread(input);
    while (1) {
      timing1=al_get_time();
      al_wait_for_event(eq,ev);
      if (ev->type==ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT){
	// put the buffer
	buf=(short*)al_get_audio_stream_fragment(as);
        if (buf!=NULL) {
        //printf("%d %s\n",bufqueue.size(),(buf==NULL)?("NO"):("ok"));
        if (!(bufqueue.size()>PERIODS*4)) {
         printf("warning: poor or no signal\n\n");
        }
	for (int i=0;i<PERIODS*2;i++) {
	  data=0;
	  data+=(bufqueue.size()>PERIODS*4)?(popfront()):(rand());
	  data+=((bufqueue.size()>PERIODS*4)?(popfront()):(rand()))<<8;
	  buf[i]=data;
	}
	al_set_audio_stream_fragment(as,buf);
       timing2=al_get_time();
       printf("signal quality: %d%%, timing: %f\033[1A\n",bufqueue.size()/1310,timing2-timing1);
       if (timing2-timing1<0.001) {
         warns++;
         if (warns>16) {
         printf("output looks dead, quitting\n"); return 1;
  }
       }
} else {
    //        printf("warning: output gone? %d\n",warns);
           warns--;
            //if (warns>100) {
            //  printf("error: receiver output is gone, quitting now\n");
            //  return 1;
           // }
      }
    }
}    
    return 0;
}

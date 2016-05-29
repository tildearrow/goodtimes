#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <string.h>
#include <jack/jack.h>
#include <jack/midiport.h>

#define DEFAULT_FPS 60.0

// graphic stuff
ALLEGRO_DISPLAY* display; // main display
int dw, dh;
bool doredraw=false;
ALLEGRO_FONT* font;

// sound stuff
jack_port_t *input_port;
jack_port_t *output_port;
jack_client_t *client;

// controller stuff
ALLEGRO_MOUSE_STATE ms;

// game logic stuff
ALLEGRO_TIMER* frame;
ALLEGRO_EVENT_QUEUE* eq;
ALLEGRO_THREAD* athread;

// game variables
float musicvol=0;
float soundvol=1;
float gamespeed=1;

// the first thing
int ver=1;

// sound variables
#define bufsize 1024
#define buffers 2
//#define cpuclock 1108404
//#define cpuclock 1022727
#define muffleconstant 0.15
float muffles1, muffles2;
//unsigned char vol;
int nppos=0;
bool con[4]={0,0,0,0};
float buf[bufsize];
float* sbuf;
float bufinc;
float prevbufinc;
float nextbv;
int curcontrol=0;
bool leftpress=false;
bool leftpressold=false;
int thesr;
int acycle=0;
int shape=0; // 0=sine, 1=saw, 2=square
float a=0;
float d=4096;
float s=0.4;
float r=0;
float fa=0;
float fd=4096;
float fs=0.4;
float fr=0;
float cut=0.1;
float res=0.3;
float low[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float high[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float band[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float envstatus[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float fenvstatus[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float vol[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float duty=0.5;
//ALLEGRO_AUDIO_STREAM* str;
int note[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char program[4]={0,0,0,0};
bool noteon[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float pos[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float ns[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

ALLEGRO_EVENT_QUEUE* strq;

float makeADSR(float envpos) {
  if (envpos<a) {return (envpos/a);}
  return fmax(((d-(envpos-a))/d),s);
}

float makefADSR(float envpos) {
  if (envpos<fa) {return (envpos/fa);}
  return pow(fmax(((fd-(envpos-fa))/fd),fs),3);
}

void fillbuffer(int howmanysamples){
  //float factor=((float)thesr/((float)cpuclock/4));
  for (int i=0; i<howmanysamples; i++) {
    buf[i]=0;
    for (int j=0; j<16; j++) {
    if (noteon[j]) {
      pos[j]+=(220*(pow(2.0f,(float)(((float)note[j]-57)/12))));
      pos[j]=fmod(pos[j],(float)thesr);
      envstatus[j]++; fenvstatus[j]++;
      switch (shape) {
	case 0: ns[j]=(((pos[j]/(float)thesr)-0.5)*makeADSR(envstatus[j]))/2; break;
	default: ns[j]=((round((pos[j])/(float)thesr+(duty-0.5))-0.5)*makeADSR(envstatus[j]))/2; break;
      }
    } else {ns[j]=0;}
      low[j]=low[j]+(makefADSR(envstatus[j])*cut)*band[j];
      high[j]=ns[j]-low[j]-(1.0-(float)res)*band[j];
      band[j]=(makefADSR(envstatus[j])*cut)*high[j]+band[j];
      buf[i]+=(low[j]*vol[j]);
    }
  }
}

//int transpose[4]={-1,-1,-5,0};
//int transpose[4]={-1,11,23,-36};
int transpose[4]={0,12,24,0};
//int transpose[4]={-27,-15,-9,3};

/*int noteperiod(float note) {
	return (int)round(((cpuclock/32))/(440*(pow(2.0f,(float)(((float)note-58)/12))))); //+1000
}*/

//const int midimap[16]={0,1,2,3,0,1,2,0,0,0,0,0,0,0,0,0};
//const int midimap[16]={0,1,0,2,0,1,0,1,0,1,0,2,0,1,0,1};
//const int midimap[16]={2,0,3,1,0,0,0,0,0,0,0,0,0,0,0,0};


int process (jack_nframes_t nframes, void *arg)
{
	jack_default_audio_sample_t *in, *out;
	
	in = (jack_default_audio_sample_t*)jack_port_get_buffer (input_port, nframes);	
	out = (jack_default_audio_sample_t*)jack_port_get_buffer (output_port, nframes);
	
	jack_midi_event_t in_event;
	//jack_nframes_t event_index = 0;
	jack_nframes_t event_count = jack_midi_get_event_count(in);
	jack_nframes_t total_event_count = event_count;
	while (event_count) {
	  jack_midi_event_get(&in_event, in, total_event_count-event_count);
	  if (((*(in_event.buffer)&0xf0))==0x80) { // note off
	    noteon[((*(in_event.buffer)&15))]=false;
	  }
	  else if (((*(in_event.buffer)&0xf0))==0x90) { // note on
	    note[((*(in_event.buffer)&15))]=(*(in_event.buffer+1));
	    printf("%d\n",note[((*(in_event.buffer)&15))]);
	    noteon[((*(in_event.buffer)&15))]=true;
	    vol[((*(in_event.buffer)&15))]=(float)(*(in_event.buffer+2))/127.0;
	    envstatus[((*(in_event.buffer)&15))]=0;
	    //shift[midimap[((*(in_event.buffer)&15))]]=program[midimap[((*(in_event.buffer)&15))]];
	  }
	  /*else if (((*(in_event.buffer)&0xf0))==0xc0) { // program change
	    program[midimap[((*(in_event.buffer)&15))]]=*(in_event.buffer + 1);
	    shift[midimap[((*(in_event.buffer)&15))]]=program[midimap[((*(in_event.buffer)&15))]];
	  }*/
	  /*else if (((*(in_event.buffer)&0xf0))==0xe0) { // pitch bend
	    freq[midimap[((*(in_event.buffer)&15))]]=127-noteperiod(note[midimap[((*(in_event.buffer)&15))]]+1+transpose[midimap[((*(in_event.buffer)&15))]]+
	    ((float)(((*(in_event.buffer+2)<<7)+(*(in_event.buffer+1)))-0x2000)/8192.0f)
	    );
	  }*/
	  event_count--;
	}
	
	 /*
    //freq[0]=(unsigned char)((float)ms.x*256.0/(float)dw);
    freq[0]=127-noteperiod(48+(ms.x/32));
    if (!con[0]){
      shift[0]=0;
      con[0]=true;
      //freq[0]=127-((127-freq[0])/2);
    }
  }
  else {con[0]=false;}*/
	
	fillbuffer(nframes);
	//printf("done\n");
	memcpy (out, buf,
		sizeof (jack_default_audio_sample_t) * nframes);

	return 0;      
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void
jack_shutdown (void *arg)
{
	exit (1);
}


int main(int argc, char **argv) {
    al_init();
    bufinc=0;
    
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    
    font=al_load_ttf_font("text.ttf",20,0);
    
    // create stuff
    dw=400; dh=300; // default sizes
    if (!(display=al_create_display(dw,dh))) {printf("error\n");return 1;}
    if (!(frame=al_create_timer(1.0/DEFAULT_FPS))) {printf("error\n");return 1;}
    if (!(eq=al_create_event_queue())) {printf("error\n");return 1;}
    
    // load audio
    	const char **ports;
	const char *client_name = "synth1";
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;
	
	/* open a client connection to the JACK server */

	client = jack_client_open (client_name, options, &status, server_name);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}

	/* tell the JACK server to call `process()' whenever
	   there is work to be done.
	*/
	thesr=jack_get_sample_rate(client);

	jack_set_process_callback (client, process, 0);

	/* tell the JACK server to call `jack_shutdown()' if
	   it ever shuts down, either entirely, or if it
	   just decides to stop calling us.
	*/

	jack_on_shutdown (client, jack_shutdown, 0);

	/* display the current sample rate. 
	 */

	printf ("engine sample rate: %" PRIu32 "\n",
		jack_get_sample_rate (client));

	/* create two ports */

	input_port = jack_port_register (client, "input",
					 JACK_DEFAULT_MIDI_TYPE,
					 JackPortIsInput, 0);
	output_port = jack_port_register (client, "output",
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsOutput, 0);

	if ((input_port == NULL) || (output_port == NULL)) {
		fprintf(stderr, "no more JACK ports available\n");
		exit (1);
	}

	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */

	if (jack_activate (client)) {
		fprintf (stderr, "cannot activate client");
		exit (1);
	}

	/* Connect the ports.  You can't do this before the client is
	 * activated, because we can't make connections to clients
	 * that aren't running.  Note the confusing (but necessary)
	 * orientation of the driver backend ports: playback ports are
	 * "input" to the backend, and capture ports are "output" from
	 * it.
	 */

	ports = jack_get_ports (client, NULL, NULL,
				JackPortIsPhysical|JackPortIsOutput);
	if (ports == NULL) {
		fprintf(stderr, "no physical capture ports\n");
		exit (1);
	}


	free (ports);
	
	ports = jack_get_ports (client, NULL, NULL,
				JackPortIsPhysical|JackPortIsInput);
	if (ports == NULL) {
		fprintf(stderr, "no physical playback ports\n");
		exit (1);
	}

	if (jack_connect (client, jack_port_name (output_port), ports[0])) {
		fprintf (stderr, "cannot connect output ports\n");
	}

	free (ports);
    
    // register event sources
    al_register_event_source(eq,al_get_display_event_source(display));
    al_register_event_source(eq,al_get_timer_event_source(frame));
    
    // clear the display
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(0,0,0));
    al_wait_for_vsync();
    al_flip_display();
    
    //athread=al_create_thread(audiothread,NULL);
    
    // begin the game
    double thing=0;
    //al_start_thread(athread);
    al_start_timer(frame);
    
    while(1){
      // get next event
      ALLEGRO_EVENT event;
      al_wait_for_event(eq,&event);
      if (event.type==ALLEGRO_EVENT_TIMER){
	thing+=0.05;
	doredraw=true;
	//events();
      } else if (event.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
	break;
      }
      if (doredraw && al_is_event_queue_empty(eq)){
	doredraw=false;
	al_set_target_backbuffer(display);
	al_clear_to_color(al_map_rgb(0,0,0));
	/*for (int i=0; i<16; i++) {
	  al_draw_textf(font,al_map_rgb(255,255,255),0,i*16,0,"note[%d]: %d",i,note[i]);
	}*/
	al_get_mouse_state(&ms);
	leftpressold=leftpress;
	if (ms.buttons&1){
	  leftpress=true;
	  if (leftpressold!=leftpress) {
	    if (ms.y<20) {curcontrol=0;} else
          if (ms.y<40) {curcontrol=1;} else
	  if (ms.y<60) {curcontrol=2;} else
	  if (ms.y<80) {curcontrol=3;} else
	  if (ms.y<100) {curcontrol=4;} else
          if (ms.y<120) {curcontrol=5;} else
	  if (ms.y<140) {curcontrol=6;} else
	  if (ms.y<160) {curcontrol=7;} else
	  if (ms.y<180) {curcontrol=8;} else
	  if (ms.y<200) {curcontrol=9;} else
	    if (ms.y<220) {curcontrol=10;} else
	    if (ms.y<240) {curcontrol=11;}
	  }
	  if (curcontrol==0) {a=fmax(ms.x*16,0);} else
          if (curcontrol==1) {d=fmax(ms.x*16,0);} else
	  if (curcontrol==2) {s=fmax((float)ms.x/(float)dw,0);} else
	  if (curcontrol==3) {r=fmax(ms.x*16,0);} else
	  if (curcontrol==4) {fa=fmax(ms.x*16,0);} else
          if (curcontrol==5) {fd=fmax(ms.x*16,0);} else
	  if (curcontrol==6) {fs=fmax((float)ms.x/(float)dw,0);} else
	  if (curcontrol==7) {fr=fmax(ms.x*16,0);} else
	  if (curcontrol==8) {cut=fmax((float)ms.x/(float)dw,0);} else
	  if (curcontrol==9) {res=fmax((float)ms.x/(float)dw,0);} else
	  if (curcontrol==10) {shape=ms.x/200;} else
	  if (curcontrol==11) {duty=fmax((float)ms.x/(float)dw,0);}
	} else {
	  leftpress=false;
	}
	al_draw_textf(font,al_map_rgb(255,255,255),0,0,0,"a: %f",a);
	al_draw_textf(font,al_map_rgb(255,255,255),0,20,0,"d: %f",d);
	al_draw_textf(font,al_map_rgb(255,255,255),0,40,0,"s: %f",s);
	al_draw_textf(font,al_map_rgb(255,255,255),0,60,0,"r: %f",r);
	al_draw_textf(font,al_map_rgb(255,255,255),0,80,0,"fa: %f",fa);
	al_draw_textf(font,al_map_rgb(255,255,255),0,100,0,"fd: %f",fd);
	al_draw_textf(font,al_map_rgb(255,255,255),0,120,0,"fs: %f",fs);
	al_draw_textf(font,al_map_rgb(255,255,255),0,140,0,"fr: %f",fr);
	al_draw_textf(font,al_map_rgb(255,255,255),0,160,0,"cut: %f",cut);
	al_draw_textf(font,al_map_rgb(255,255,255),0,180,0,"res: %f",res);
	al_draw_textf(font,al_map_rgb(255,255,255),0,200,0,"shape: %d",shape);
	al_draw_textf(font,al_map_rgb(255,255,255),0,220,0,"duty: %f",duty);
	
	//al_draw_circle(80,80,32,al_map_rgb(128,128,128),8);
	//al_draw_filled_rectangle((dw/2)+sin(thing)*(dw/2),0,(dw/2)+sin(thing)*(dw/2)+16,480,al_map_rgb(255,255,255));
	/*for (int ii=0;ii<64;ii++){
	  al_draw_line(ii*32,0,ii*32,dh,al_map_rgb(255,255,255),1);
	}
	for (int ii=0;ii<6;ii++){
	  al_draw_ellipse(48+(ii*32*12),dh-32,10,10,al_map_rgb(255,255,255),2);
	}*/
	al_flip_display();
      }
    }
    
    // end the game
    al_stop_timer(frame);
    jack_client_close (client);
    exit (0);
    return 0;
}

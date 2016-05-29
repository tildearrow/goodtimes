/* 

Copyright (c) 2015-2016 tildearrow

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
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

double time1;
double time2;

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
#define cpuclock 1022727
#define muffleconstant 0.1
float muffles1, muffles2;
unsigned char freq[4]={0,0,0,0};
unsigned char vol;
unsigned char shape[4]={0,0,0,0};
unsigned char count[4]={0,0,0,0};
unsigned char shift[4]={0,0,0,0};
int nppos=0;
bool con[4]={0,0,0,0};
float buf[bufsize];
float* sbuf;
float bufinc;
float prevbufinc;
float nextbv;
int thesr;
int acycle=0;
//ALLEGRO_AUDIO_STREAM* str;
unsigned char note[4]={0,0,0,0};
unsigned char program[4]={0,0,0,0};

ALLEGRO_EVENT_QUEUE* strq;

/// GPL start, see LICENSE.txt for info
const unsigned char noisepattern[1024] = {
      7, 30, 30, 28, 28, 62, 60, 56,120,248,124, 30, 31,143,  7,  7,193,192,224,
    241,224,240,227,225,192,224,120,126, 60, 56,224,225,195,195,135,199,  7, 30,
     28, 31, 14, 14, 30, 14, 15, 15,195,195,241,225,227,193,227,195,195,252, 60,
     30, 15,131,195,193,193,195,195,199,135,135,199, 15, 14, 60,124,120, 60, 60,
     60, 56, 62, 28,124, 30, 60, 15, 14, 62,120,240,240,224,225,241,193,195,199,
    195,225,241,224,225,240,241,227,192,240,224,248,112,227,135,135,192,240,224,
    241,225,225,199,131,135,131,143,135,135,199,131,195,131,195,241,225,195,199,
    129,207,135,  3,135,199,199,135,131,225,195,  7,195,135,135,  7,135,195,135,
    131,225,195,199,195,135,135,143, 15,135,135, 15,207, 31,135,142, 14,  7,129,
    195,227,193,224,240,224,227,131,135,  7,135,142, 30, 15,  7,135,143, 31,  7,
    135,193,240,225,225,227,199, 15,  3,143,135, 14, 30, 30, 15,135,135, 15,135,
     31, 15,195,195,240,248,240,112,241,240,240,225,240,224,120,124,120,124,112,
    113,225,225,195,195,199,135, 28, 60, 60, 28, 60,124, 30, 30, 30, 28, 60,120,
    248,248,225,195,135, 30, 30, 60, 62, 15, 15,135, 31,142, 15, 15,142, 30, 30,
     30, 30, 15, 15,143,135,135,195,131,193,225,195,193,195,199,143, 15, 15, 15,
     15,131,199,195,193,225,224,248, 62, 60, 60, 60, 60, 60,120, 62, 30, 30, 30,
     15, 15, 15, 30, 14, 30, 30, 15, 15,135, 31,135,135, 28, 62, 31, 15, 15,142,
     62, 14, 62, 30, 28, 60,124,252, 56,120,120, 56,120,112,248,124, 30, 60, 60,
     48,241,240,112,112,224,248,240,248,120,120,113,225,240,227,193,240,113,227,
    199,135,142, 62, 14, 30, 62, 15,  7,135, 12, 62, 15,135, 15, 30, 60, 60, 56,
    120,241,231,195,195,199,142, 60, 56,240,224,126, 30, 62, 14, 15, 15, 15,  3,
    195,195,199,135, 31, 14, 30, 28, 60, 60, 15,  7,  7,199,199,135,135,143, 15,
    192,240,248, 96,240,240,225,227,227,195,195,195,135, 15,135,142, 30, 30, 63,
     30, 14, 28, 60,126, 30, 60, 56,120,120,120, 56,120, 60,225,227,143, 31, 28,
    120,112,126, 15,135,  7,195,199, 15, 30, 60, 14, 15, 14, 30,  3,240,240,241,
    227,193,199,192,225,225,225,225,224,112,225,240,120,112,227,199, 15,193,225,
    227,195,192,240,252, 28, 60,112,248,112,248,120, 60,112,240,120,112,124,124,
     60, 56, 30, 62, 60,126,  7,131,199,193,193,225,195,195,195,225,225,240,120,
    124, 62, 15, 31,  7,143, 15,131,135,193,227,227,195,195,225,240,248,240, 60,
    124, 60, 15,142, 14, 31, 31, 14, 60, 56,120,112,112,240,240,248,112,112,120,
     56, 60,112,224,240,120,241,240,120, 62, 60, 15,  7, 14, 62, 30, 63, 30, 14,
     15,135,135,  7, 15,  7,199,143, 15,135, 30, 30, 31, 30, 30, 60, 30, 28, 62,
     15,  3,195,129,224,240,252, 56, 60, 62, 14, 30, 28,124, 30, 31, 14, 62, 28,
    120,120,124, 30, 62, 30, 60, 31, 15, 31, 15, 15,143, 28, 60,120,248,240,248,
    112,240,120,120, 60, 60,120, 60, 31, 15,  7,134, 28, 30, 28, 30, 30, 31,  3,
    195,199,142, 60, 60, 28, 24,240,225,195,225,193,225,227,195,195,227,195,131,
    135,131,135, 15,  7,  7,225,225,224,124,120, 56,120,120, 60, 31, 15,143, 14,
      7, 15,  7,131,195,195,129,240,248,241,224,227,199, 28, 62, 30, 15, 15,195,
    240,240,227,131,195,199,  7, 15, 15, 15, 15, 15,  7,135, 15, 15, 14, 15, 15,
     30, 15, 15,135,135,135,143,199,199,131,131,195,199,143,135,  7,195,142, 30,
     56, 62, 60, 56,124, 31, 28, 56, 60,120,124, 30, 28, 60, 63, 30, 14, 62, 28,
     60, 31, 15,  7,195,227,131,135,129,193,227,207, 14, 15, 30, 62, 30, 31, 15,
    143,195,135, 14,  3,240,240,112,224,225,225,199,142, 15, 15, 30, 14, 30, 31,
     28,120,240,241,241,224,241,225,225,224,224,241,193,240,113,225,195,131,199,
    131,225,225,248,112,240,240,240,240,240,112,248,112,112, 97,224,240,225,224,
    120,113,224,240,248, 56, 30, 28, 56,112,248, 96,120, 56, 60, 63, 31, 15, 31,
     15, 31,135,135,131,135,131,225,225,240,120,241,240,112, 56, 56,112,224,227,
    192,224,248,120,120,248, 56,241,225,225,195,135,135, 14, 30, 31, 14, 14, 15,
     15,135,195,135,  7,131,192,240, 56, 60, 60, 56,240,252, 62, 30, 28, 28, 56,
    112,240,241,224,240,224,224,241,227,224,225,240,240,120,124,120, 60,120,120,
     56,120,120,120,120,112,227,131,131,224,195,193,225,193,193,193,227,195,199,
     30, 14, 31, 30, 30, 15, 15, 14, 14, 14,  7,131,135,135, 14,  7,143, 15, 15,
     15, 14, 28,112,225,224,113,193,131,131,135, 15, 30, 24,120,120,124, 62, 28,
     56,240,225,224,120,112, 56, 60, 62, 30, 60, 30, 28,112, 60, 56, 63
};
/// GPL end
/*

// init audio
void initaudio(){
  str=al_create_audio_stream(buffers,bufsize,44100,ALLEGRO_AUDIO_DEPTH_FLOAT32,ALLEGRO_CHANNEL_CONF_1);
  al_attach_audio_stream_to_mixer(str,al_get_default_mixer());
  al_register_event_source(strq,al_get_audio_stream_event_source(str));
}

// next sample




// events
void events(){

}

static void *audiothread(ALLEGRO_THREAD *thread, void *arg){
  al_install_audio();
  client=jack_client_open (client_name, options, &status, server_name);
  al_reserve_samples(1);
  strq=al_create_event_queue();
  initaudio();
  while (1) {
    ALLEGRO_EVENT aev;
    fillbuffer();
    al_wait_for_event(strq,&aev);
    if (aev.type==ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT){
      sbuf=(float*)al_get_audio_stream_fragment(str);
      if (!sbuf) {continue;}
      memcpy(sbuf,buf,bufsize*4);
      al_set_audio_stream_fragment(str,sbuf);
      al_set_audio_stream_playmode(str,ALLEGRO_PLAYMODE_ONCE);
      al_set_audio_stream_gain(str,0.8);
      al_set_audio_stream_playing(str,true);
    }
      al_get_mouse_state(&ms);
      if (ms.buttons&1){
    //freq[0]=(unsigned char)((float)ms.x*256.0/(float)dw);
    freq[0]=127-noteperiod(48+(ms.x/32));
    if (!con[0]){
      shift[0]=0;
      con[0]=true;
      //freq[0]=127-((127-freq[0])/2);
    }
  }
  else {con[0]=false;}
  }
}*/

void fillbuffer(int howmanysamples){
  float factor=((float)thesr/((float)cpuclock/4));
  while (true) {
    prevbufinc=bufinc;
    bufinc+=factor;
    // channel 0
    count[0]++;
    if (count[0]==0x7f){
    count[0]=freq[0];
    shift[0]=(shift[0]<<1)|(((shift[0]>>7)^1)&con[0]);
    }
    if (acycle==2 || acycle==0) {
    // channel 1
    count[1]++;
    if (count[1]==0x7f){
    count[1]=freq[1];
    shift[1]=(shift[1]<<1)|(((shift[1]>>7)^1)&con[1]);
    }
    }
    if (acycle==0) {
    // channel 2
    count[2]++;
    if (count[2]==0x7f){
    count[2]=freq[2];
    shift[2]=(shift[2]<<1)|(((shift[2]>>7)^1)&con[2]);
    }
    }
    // channel 3
    count[3]++;
    if (count[3]==0x7f){
    count[3]=freq[3];
    nppos++; if (nppos>8191) {nppos=0;}
    shift[3]=(((noisepattern[nppos>>3]>>(7-(nppos%8)))&1)&con[3]);
    }
    nextbv=((float)(shift[0]&1)+(float)(shift[1]&1)+(float)(shift[2]&1)+(float)(shift[3]&1))/4;
    muffles1=muffles1+muffleconstant*(nextbv-muffles1);
    muffles2=muffles2+muffleconstant*(muffles1-muffles2);
    nextbv=muffles2;
    acycle++; if (acycle>3) {acycle=0;}
    
    if ((int)bufinc!=(int)prevbufinc) {
      buf[(int)bufinc-1]=nextbv;
    }
    if ((int)bufinc>=howmanysamples) {
     bufinc=fmod(bufinc,1); break;
    }
  }
  /*for (int ii=0;ii<howmanysamples*((cpuclock/4)/44100);ii++){
  // channel 0
  count[0]++;
  if (count[0]==0x7f){
    count[0]=freq[0];
    shift[0]=(shift[0]<<1)|(((shift[0]>>7)^1)&con[0]);
  }
  buf[(int)((float)ii*(44100.0f/((float)cpuclock/4.0f)))]=(float)(shift[0]&1);
  }*/
}

//int transpose[4]={-1,-1,-5,0};
//int transpose[4]={-1,11,23,-36};
int transpose[4]={0,12,24,0};
//int transpose[4]={-27,-15,-9,3};

int noteperiod(float note) {
	return (int)round(((cpuclock/32))/(440*(pow(2.0f,(float)(((float)note-58)/12))))); //+1000
}

const int midimap[16]={0,1,2,3,0,1,2,0,0,0,0,0,0,0,0,0};
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
	    note[midimap[((*(in_event.buffer)&15))]]=*(in_event.buffer + 1)+1;
	    con[midimap[((*(in_event.buffer)&15))]]=false;
	    freq[midimap[((*(in_event.buffer)&15))]]=127-noteperiod(note[midimap[((*(in_event.buffer)&15))]]+transpose[midimap[((*(in_event.buffer)&15))]]);
	  }
	  else if (((*(in_event.buffer)&0xf0))==0x90) { // note on
	    note[midimap[((*(in_event.buffer)&15))]]=*(in_event.buffer + 1)+1;
	    con[midimap[((*(in_event.buffer)&15))]]=true;
	    freq[midimap[((*(in_event.buffer)&15))]]=127-noteperiod(note[midimap[((*(in_event.buffer)&15))]]+transpose[midimap[((*(in_event.buffer)&15))]]);
	    //shift[midimap[((*(in_event.buffer)&15))]]=program[midimap[((*(in_event.buffer)&15))]];
	  }
	  /*else if (((*(in_event.buffer)&0xf0))==0xc0) { // program change
	    program[midimap[((*(in_event.buffer)&15))]]=*(in_event.buffer + 1);
	    shift[midimap[((*(in_event.buffer)&15))]]=program[midimap[((*(in_event.buffer)&15))]];
	  }*/
	  else if (((*(in_event.buffer)&0xf0))==0xe0) { // pitch bend
	    freq[midimap[((*(in_event.buffer)&15))]]=127-noteperiod(note[midimap[((*(in_event.buffer)&15))]]+1+transpose[midimap[((*(in_event.buffer)&15))]]+
	    ((float)(((*(in_event.buffer+2)<<7)+(*(in_event.buffer+1)))-0x2000)/8192.0f)
	    );
	  }
	  event_count--;
	}
	
	al_get_mouse_state(&ms);
      if (ms.buttons&1){
	con[0]=false;
	con[1]=false;
	con[2]=false;
	con[3]=false;
      } /*
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
	const char *client_name = "vicsynth";
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
				JackPortIsPhysical|JackPortIsInput);
	if (ports == NULL) {
		fprintf(stderr, "no physical playback ports\n");
		exit (1);
	}

	if (jack_connect (client, jack_port_name (output_port), ports[0])) {
		fprintf (stderr, "cannot connect output ports\n");
	}
if (jack_connect (client, jack_port_name (output_port), ports[1])) {
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
	al_draw_textf(font,al_map_rgb(255,255,255),0,0,0,"transp: %d, period %.2X",transpose[0],(con[0]*(0x80+freq[0])));
	al_draw_textf(font,al_map_rgb(255,255,255),0,32,0,"transp: %d, period %.2X",transpose[1],(con[1]*(0x80+freq[1])));
	al_draw_textf(font,al_map_rgb(255,255,255),0,64,0,"transp: %d, period %.2X",transpose[2],(con[2]*(0x80+freq[2])));
	al_draw_textf(font,al_map_rgb(255,255,255),0,96,0,"transp: %d, period %.2X",transpose[3],(con[3]*(0x80+freq[3])));
	//al_draw_filled_rectangle((dw/2)+sin(thing)*(dw/2),0,(dw/2)+sin(thing)*(dw/2)+16,480,al_map_rgb(255,255,255));
	/*for (int ii=0;ii<64;ii++){
	  al_draw_line(ii*32,0,ii*32,dh,al_map_rgb(255,255,255),1);
	}
	for (int ii=0;ii<6;ii++){
	  al_draw_ellipse(48+(ii*32*12),dh-32,10,10,al_map_rgb(255,255,255),2);
	}*/
	al_draw_textf(font,al_map_rgb(255,255,255),0,120,0,"timings: %f",time1-time2);
	al_flip_display();
	time2=time1;
	time1=al_get_time();
      }
    }
    
    // end the game
    al_stop_timer(frame);
    jack_client_close (client);
    exit (0);
    return 0;
}

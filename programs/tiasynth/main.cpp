/*
    tiasynth, a MIDI tool to produce sounds using an emulated TIA chip
    Copyright (C) 2016 tildearrow

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA.
*/
// a MIDI tool to produce sounds using an emulated VIC chip
// wait what, i mean, tiasynth, a MIDI tool to produce sounds using an emulated TIA chip
// uses code from stella, sorry
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
#include "TIASnd.hxx"

#define DEFAULT_FPS 60.0

// graphic stuff
ALLEGRO_DISPLAY* display; // main display
int dw, dh;
bool doredraw=false;
ALLEGRO_FONT* font;
TIASound* chip[8];

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
#define cpuclock 10000
//#define cpuclock 1022727
#define muffleconstant 0.1
float muffles1, muffles2;
unsigned char freq[16]={0,0,0,0};
unsigned char vol;
unsigned char cvol[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char shape[16]={0,0,0,0};
//int dividers[16]={1,2050,140,1100,31000,31000,1010,1010,1010,1010,1010,1,30000/3,30000/3,340,340};
int dividers[16]={1,5100,280,2200,62000,62000,2020,2020,2020,2020,2020,1,60000/3,60000/3,680,680};
unsigned char count[16]={0,0,0,0};
unsigned char shift[16]={0,0,0,0};
int nppos=0;
bool con[16]={0,0,0,0};
float buf[bufsize];
float* sbuf;
float bufinc;
float prevbufinc;
float nextbv;
int thesr;
int acycle=0;
short* buf16;
//ALLEGRO_AUDIO_STREAM* str;
unsigned char note[16]={0,0,0,0};
unsigned char program[16]={0,0,0,0};

ALLEGRO_EVENT_QUEUE* strq;

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

//int transpose[4]={-1,-1,-5,0};
//int transpose[4]={-1,11,23,-36};
int transpose[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//int transpose[4]={-27,-15,-9,3};


const int midimap[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
//const int midimap[16]={0,1,0,2,0,1,0,1,0,1,0,2,0,1,0,1};
//const int midimap[16]={2,0,3,1,0,0,0,0,0,0,0,0,0,0,0,0};


int noteperiod(int channel,float note) {
	return (int)((dividers[program[channel]])/(440*(pow(2.0f,(float)(((float)note-58)/12))))); //+1000
}
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
	    freq[midimap[((*(in_event.buffer)&15))]]=noteperiod(((*(in_event.buffer)&15)),note[midimap[((*(in_event.buffer)&15))]]+transpose[midimap[((*(in_event.buffer)&15))]]);
	    chip[midimap[((*(in_event.buffer)&15))]/2]->set(0x19+(midimap[((*(in_event.buffer)&15))]%2),0);
	  }
	  else if (((*(in_event.buffer)&0xf0))==0x90) { // note on
	    note[midimap[((*(in_event.buffer)&15))]]=*(in_event.buffer + 1)+1;
	    con[midimap[((*(in_event.buffer)&15))]]=true;
	    freq[midimap[((*(in_event.buffer)&15))]]=noteperiod(((*(in_event.buffer)&15)),note[midimap[((*(in_event.buffer)&15))]]+transpose[midimap[((*(in_event.buffer)&15))]]);
	    //chip->set(0x15+(midimap[((*(in_event.buffer)&15))]%2),0x01);
	    chip[midimap[((*(in_event.buffer)&15))]/2]->set(0x15+(midimap[((*(in_event.buffer)&15))]%2),program[midimap[((*(in_event.buffer)&15))]]);
	    chip[midimap[((*(in_event.buffer)&15))]/2]->set(0x17+(midimap[((*(in_event.buffer)&15))]%2),freq[midimap[((*(in_event.buffer)&15))]]);
	    chip[midimap[((*(in_event.buffer)&15))]/2]->set(0x19+(midimap[((*(in_event.buffer)&15))]%2),(*(in_event.buffer + 2))/8);
	    cvol[midimap[((*(in_event.buffer)&15))]]=(*(in_event.buffer + 2))/8;
	    //shift[midimap[((*(in_event.buffer)&15))]]=program[midimap[((*(in_event.buffer)&15))]];
	  }
	  else if (((*(in_event.buffer)&0xf0))==0xc0) { // program change
	    program[midimap[((*(in_event.buffer)&15))]]=*(in_event.buffer + 1);
	    chip[midimap[((*(in_event.buffer)&15))]/2]->set(0x15+(midimap[((*(in_event.buffer)&15))]%2),program[midimap[((*(in_event.buffer)&15))]]);
	    //shift[midimap[((*(in_event.buffer)&15))]]=program[midimap[((*(in_event.buffer)&15))]];
	  }
	  /*else if (((*(in_event.buffer)&0xf0))==0xe0) { // pitch bend
	    freq[midimap[((*(in_event.buffer)&15))]]=noteperiod(note[midimap[((*(in_event.buffer)&15))]]+1+transpose[midimap[((*(in_event.buffer)&15))]]+
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
	
	//fillbuffer(nframes);
	//printf("done\n");
    buf16=new short[nframes];
     for (int i=0; i<nframes; i++) {
   out[i]=0;
 }
    for (int j=0; j<8; j++) {
    chip[j]->process(buf16,nframes);
	/*memcpy (out, buf,
		sizeof (jack_default_audio_sample_t) * nframes);*/
 for (int i=0; i<nframes; i++) {
   out[i]+=(float)buf16[i]/16384.0;
 }
    }
	delete[] buf16;
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
    printf("initing\n");
    
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    
    font=al_load_ttf_font("text.ttf",20,0);
    
    // create stuff
    dw=600; dh=400; // default sizes
    if (!(display=al_create_display(dw,dh))) {printf("the heck\n");return 1;}
    if (!(frame=al_create_timer(1.0/DEFAULT_FPS))) {printf("the heck\n");return 1;}
    if (!(eq=al_create_event_queue())) {printf("the heck\n");return 1;}
    
    // load audio
    	const char **ports;
	const char *client_name = "tiasynth";
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
printf("registering ports\n");
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
	printf("initing sound\n");
chip[0]=new TIASound(22050);
chip[1]=new TIASound(22050);
chip[2]=new TIASound(22050);
chip[3]=new TIASound(22050);
chip[4]=new TIASound(22050);
chip[5]=new TIASound(22050);
chip[6]=new TIASound(22050);
chip[7]=new TIASound(22050);
for (int i=0; i<8; i++) {
chip[i]->reset();
chip[i]->channels(1,1);
chip[i]->volume(13);
}
/*chip->set(0x15,1);
chip->set(0x17,2);
chip->set(0x19,4);*/
printf("done\n");
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
	/*
	al_draw_textf(font,al_map_rgb(255,255,255),0,0,0,"transp: %d, period %.2X vol %.1x shape %.1x",transpose[0],(con[0]*(freq[0])),con[0],program[0]);
	al_draw_textf(font,al_map_rgb(255,255,255),0,32,0,"transp: %d, period %.2X vol %.1x shape %.1x",transpose[1],(con[1]*(freq[1])),con[1],program[1]);
	al_draw_textf(font,al_map_rgb(255,255,255),0,64,0,"transp: %d, period %.2X vol %.1x shape %.1x",transpose[2],(con[2]*(freq[2])),con[2],program[2]);
	al_draw_textf(font,al_map_rgb(255,255,255),0,96,0,"transp: %d, period %.2X vol %.1x shape %.1x",transpose[3],(con[3]*(freq[3])),con[3],program[3]);
	al_draw_textf(font,al_map_rgb(255,255,255),0,128,0,"transp: %d, period %.2X vol %.1x shape %.1x",transpose[4],(con[4]*(freq[4])),con[4],program[4]);
	al_draw_textf(font,al_map_rgb(255,255,255),0,160,0,"transp: %d, period %.2X vol %.1x shape %.1x",transpose[5],(con[5]*(freq[5])),con[5],program[5]);
	al_draw_textf(font,al_map_rgb(255,255,255),0,192,0,"transp: %d, period %.2X vol %.1x shape %.1x",transpose[6],(con[6]*(freq[6])),con[6],program[6]);
	al_draw_textf(font,al_map_rgb(255,255,255),0,224,0,"transp: %d, period %.2X vol %.1x shape %.1x",transpose[7],(con[7]*(freq[7])),con[7],program[7]);
	*/
	for (int ii=0;ii<16;ii++) {
	al_draw_line(freq[ii],ii*16,freq[ii],((ii+1)*16)-1,al_map_rgb(0,cvol[ii]*15,0),2);
	al_draw_line(128+((program[ii]%16)*4),ii*16,128+((program[ii]%16)*4),((ii+1)*16)-1,al_map_rgb(255,0,0),2);
	al_draw_filled_rectangle(256,ii*16,272,((ii+1)*16)-1,al_map_rgb(0,0,con[ii]?255:0));
	}
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

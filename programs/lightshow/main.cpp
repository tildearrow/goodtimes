// lightshow
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <unistd.h>
#define DEFAULT_FPS 60.0

// graphic stuff
int dw, dh;
bool doredraw=false;

// sound stuff
jack_port_t *input_port;
jack_port_t *output_port;
jack_client_t *client;


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
	void *in, *out;
	jack_midi_data_t* buffer1;
	in = jack_port_get_buffer (input_port, nframes);	
	out = jack_port_get_buffer (output_port, nframes);
	jack_midi_clear_buffer(out);
	jack_midi_event_t in_event;
	//jack_nframes_t event_index = 0;
	jack_nframes_t event_count = jack_midi_get_event_count(in);
	jack_nframes_t total_event_count = event_count;
	while (event_count) {
	  jack_midi_event_get(&in_event, in, total_event_count-event_count);
	  //printf("OK\n");
	  buffer1 = jack_midi_event_reserve(out, in_event.time, in_event.size);
	  //buffer1[0]=0x90;
	  if (((*(in_event.buffer)&0xf0))==0x80) { // note off
	    for (int j=0; j<in_event.size; j++) {
	    buffer1[j]=in_event.buffer[j];
	    }
	    buffer1[0]=(buffer1[0]%2)+0x97;
	    buffer1[1]=buffer1[1]%8;
	    buffer1[2]=0x00;
	  }
	  else if (((*(in_event.buffer)&0xf0))==0x90) {
	    for (int j=0; j<in_event.size; j++) {
	    buffer1[j]=in_event.buffer[j];
	    }
	    buffer1[0]=(buffer1[0]%2)+0x97;
	    buffer1[1]=buffer1[1]%8;
	    buffer1[2]=buffer1[2]%65;
	  }
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
	
	/*fillbuffer(nframes);
	//printf("done\n");
	memcpy (out, buf,
		sizeof (jack_default_audio_sample_t) * nframes);*/

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
    bufinc=0;
    
    // load audio
    	const char **ports;
	const char *client_name = "lightshow";
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
					  JACK_DEFAULT_MIDI_TYPE,
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


    
    // register event sources
   sleep (-1);
    jack_client_close (client);
    exit (0);
    return 0;
}

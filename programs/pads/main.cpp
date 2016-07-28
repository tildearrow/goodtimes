#include <stdio.h>
#include <string.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#if defined(_WIN32) || defined(NO_JACK)
#define SDL_INSTEAD
#endif
#ifdef SDL_INSTEAD
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#else
#include <jack/jack.h>
#endif
//#include <mpg123.h>
#include <vector>
int dw,dh;
ALLEGRO_DISPLAY* display;
ALLEGRO_TIMER* frame;
#define DEFAULT_FPS 60.0
#define DEFAULT_SR 44100
#define DEFAULT_FRAGMENTS 3
#define DEFAULT_BUFSIZE 880
#define seekamount 0.0001
#define interpolate
ALLEGRO_EVENT_QUEUE* eq;
ALLEGRO_FONT* fText;
ALLEGRO_MOUSE_STATE ms;
ALLEGRO_KEYBOARD_STATE kbstate;
bool kb[256], kbpressed[256], kblast[256];
ALLEGRO_AUDIO_STREAM* stream;
ALLEGRO_EVENT_QUEUE* aeq;
ALLEGRO_SAMPLE* testdummy;
ALLEGRO_THREAD* athread;
#ifdef SDL_INSTEAD
SDL_AudioDeviceID audioID;
SDL_AudioSpec* out;
SDL_AudioSpec* outRes;
#else
jack_port_t* outL;
jack_port_t* outR;
jack_client_t *client;
#endif
bool leftclick;
bool leftclick1;
int rcrc;
#define padcount 64
bool doredraw;
int padwidth=64;
int padheight=64;
int sepx=8;
int sepy=8;
int offx=80;
int offy=32;
int maxx=12;
const int ver=5;
bool hdp=false;
bool drawpadmenu=false;
bool drawtextinput=false;
int textinputid=0;
int textinputpos=0;
int padmenux=0;
int padmenuy=0;
int padmenuid=0;
int wheelrel=0;
// mp3 stuff
/*
mpg123_handle* mp3h;
int mp3error=MPG123_OK;
std::vector<float> mp3buf;
unsigned char* mp3buf1;
*/

#define PointInRect(x1,y1,x2,y2,checkx,checky) ((checkx>x1&&checkx<x2&&checky>y1&&checky<y2)?(1):(0))
// strings
#define strNoWindow "no se pudo crear la ventana\n"
#define strNoTimer "no se pudo crear el temporizador\n"
#define strNoEQ "no se pudo crear la cola de eventos\n"
#define strNoAudio "no se pudo iniciar el audio\n"
#define strChooseFile "seleccionar archivo"
#define strHeight "altura"
#define strWidth "anchura"
#define strSepX "sep. X"
#define strSepY "sep. Y"
#define strMaxX "máx# hor"
#define strOffX "pos. X"
#define strOffY "pos. Y"
#define strUsing "usando %s, %d búferes %d samples %dHz"
#ifdef SDL_INSTEAD
#define strbackend "SDL" // for now
#else
#define strbackend "jack" // for now
#endif
#define strLatency "latencia estimada de %fms"

#define strMenuLoad "cargar"
#define strMenuRename "renombrar"
#define strMenuDelete "borrar"
#define strMenuColor "colorizar"

#define strHelpHeader "Ayuda rápida:"
#define strHelpLine1 "Clic izquierdo: reproducir botón"
#define strHelpLine2 "Clic central: parar botón"
#define strHelpLine3 "Clic derecho: menú de botón"
#define strHelpLine4 "Rueda del mouse: Ajustar volumen"
#define strHelpLine5 "Ctrl+Rueda del mouse: Ajustar tonalidad"
#define strHelpLine6 "Shift+Rueda del mouse: Ajustar balance"
#define strHelpLine7 "F1: Ayuda rápida"
#define strHelpLine8 "F2: Configuración"
#define strNoMP3 "no se pudo inicializar el códec mp3 debido a %s"

#define strSaveBank "guardar banco"
#define strLoadInfo "Pulsa <L> para cargar un banco"
#define strSaveInfo "Pulsa <S> para guardar banco"

#define strSave "guardar"
#define strLoad "cargar"

#define strInputHeader "Renombrar: (terminar con <Intro>)"

#define frac(x) fmod(x,1.0f)
#define interp(a,b,c) (a+((b-a)*c))
class pad {
  public:
    unsigned char keycode;
    int x, y;
    float volume;
    float panning;
    float pitch;
    float newpitch;
    float position;
    bool playing;
    float* sample;
    unsigned long int samplesize;
    bool cm;
    bool loop;
    float samplerate;
    unsigned char r,g,b;
    char name[65];
    bool checkmouse(ALLEGRO_MOUSE_STATE* thems) {
      return (PointInRect(x,y,x+64,y+64,thems->x,thems->y));
    }
    void draw() {
      if (!playing) {al_draw_rectangle(x,y,x+padwidth-9,y+padheight,al_map_rgb(r,g,b),1+(checkmouse(&ms)&&!drawpadmenu&&!drawtextinput));}
      else {al_draw_filled_rectangle(x,y,x+padwidth-9,y+padheight,al_map_rgb(r,g,b));}
      al_draw_filled_rectangle(x+padwidth-8,y+(padheight-(volume*padheight/2)),x+padwidth,y+padheight,al_map_rgb(r/2,g/2,b/2));
      al_draw_textf(fText,al_map_rgb(r,g,b),x+(padwidth/2),y+padheight-14,ALLEGRO_ALIGN_CENTER,"%s",name);
      al_draw_textf(fText,(playing)?(al_map_rgb(0,0,0)):(al_map_rgb(r,g,b)),x+3,y+3,0,"%.1fx",newpitch);
    }

};

pad pads[padcount];

/*int initaudio_allegro(int buffers, int samples, int samplerate){
  stream=al_create_audio_stream(buffers,samples,samplerate,ALLEGRO_AUDIO_DEPTH_FLOAT32,ALLEGRO_CHANNEL_CONF_2);
  al_attach_audio_stream_to_mixer(stream,al_get_default_mixer());
  al_register_event_source(aeq,al_get_audio_stream_event_source(stream));
}*/

#ifdef SDL_INSTEAD
static void efficientaudioroutine(void*  userdata,
  Uint8* stream,
  int    len)
#else
int efficientaudioroutine(jack_nframes_t totalsamples, void* arguments)
#endif
{
   // ALLEGRO_EVENT aev;
   // al_wait_for_event(aeq,&aev);
   // if (aev.type==ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT){
#ifdef SDL_INSTEAD
  float* sbufL=(float*)stream;
  float* sbufR= (float*)stream;
int totalsamples=len/8;
#else
      float* sbufL=(float*)jack_port_get_buffer (outL, totalsamples);
      float* sbufR=(float*)jack_port_get_buffer (outR, totalsamples);
#endif
      //if (!sbuf) {return 1;}
      #ifdef SDL_INSTEAD
      memset(sbufL,0,totalsamples*8);
      #else
      memset(sbufL,0,totalsamples*4);
      memset(sbufR,0,totalsamples*4);
      #endif
      // audio routine
      for (int ii=0;ii<totalsamples;ii++){
	for (int j=0;j<padcount;j++){
	  if (pads[j].playing){
            if (pads[j].pitch<pads[j].newpitch) {pads[j].pitch+=seekamount;
            if (pads[j].pitch>pads[j].newpitch){pads[j].pitch=pads[j].newpitch;}}
            if (pads[j].pitch>pads[j].newpitch) {pads[j].pitch-=seekamount;
            if (pads[j].pitch<pads[j].newpitch){pads[j].pitch=pads[j].newpitch;}}

	    pads[j].position+=(pads[j].samplerate/(float)DEFAULT_SR)*pads[j].pitch;
	    if ((int)pads[j].position>pads[j].samplesize){
	      pads[j].position=0;
	      if (!pads[j].loop) {
	      pads[j].playing=false;
	      }
	      doredraw=true;
	    } else {
              #ifdef interpolate
	      #ifdef SDL_INSTEAD
	      sbufL[ii*2]+=(interp(pads[j].sample[((int)pads[j].position)*2],pads[j].sample[((int)pads[j].position+1)*2],frac(pads[j].position)))*pads[j].volume;
	      sbufR[(ii*2)+1]+=(interp(pads[j].sample[((int)pads[j].position)*2+1],pads[j].sample[((int)pads[j].position+1)*2+1],frac(pads[j].position)))*pads[j].volume;
	      #else
	      sbufL[ii]+=(interp(pads[j].sample[((int)pads[j].position)*2],pads[j].sample[((int)pads[j].position+1)*2],frac(pads[j].position)))*pads[j].volume;
	      sbufR[ii]+=(interp(pads[j].sample[((int)pads[j].position)*2+1],pads[j].sample[((int)pads[j].position+1)*2+1],frac(pads[j].position)))*pads[j].volume;
	      #endif
              #else
	      #ifdef SDL_INSTEAD
	      sbufL[ii*2]+=pads[j].sample[((int)pads[j].position)*2]*pads[j].volume;
	      sbufR[(ii*2)+1]+=pads[j].sample[((int)pads[j].position)*2+1]*pads[j].volume;
	      #else
	      sbufL[ii]+=pads[j].sample[((int)pads[j].position)*2]*pads[j].volume;
	      sbufR[ii]+=pads[j].sample[((int)pads[j].position)*2+1]*pads[j].volume;
	      #endif
	      #endif
            }
	  }
	}
      }
      ///al_set_audio_stream_fragment(stream,sbuf);
      //al_set_audio_stream_playmode(stream,ALLEGRO_PLAYMODE_ONCE);
      //al_set_audio_stream_gain(stream,1);
      //al_set_audio_stream_playing(stream,true);
   // }
#ifndef SDL_INSTEAD
return 0;
#endif
}
void writebank(){
    ALLEGRO_FILECHOOSER* filechooser;
    filechooser=al_create_native_file_dialog(".",strSaveBank,NULL,ALLEGRO_FILECHOOSER_SAVE);
    al_show_native_file_dialog(display,filechooser);
    char rfn[257];
    rfn[0]=0;
    if (al_get_native_file_dialog_path(filechooser,0)!=NULL){
      strcpy(rfn,al_get_native_file_dialog_path(filechooser,0));
      printf("file: %s\n",rfn);
      FILE* savefile;
      savefile=fopen(rfn,"wb");
      fseek(savefile,0,SEEK_SET);
      // header
      fputc('P',savefile);
      fputc('A',savefile);
      fputc('D',savefile);
      fputc('B',savefile);
      fputc('A',savefile);
      fputc('N',savefile);
      fputc('K',savefile);
      fputc(0,savefile);
      // version
      fputc(ver&255,savefile);
      fputc((ver>>8)&255,savefile);
      // pad count
      fputc(padcount,savefile);
      // master volume
      fputc(0,savefile);
      fputc(0,savefile);
      fputc(128,savefile);
      fputc(63,savefile);
      // end of header
      fputc(0,savefile);
      // pads
      fseek(savefile,padcount*4,SEEK_CUR);
      unsigned long int padpointer[256];
      for (int _pad=0;_pad<padcount;_pad++){
	padpointer[_pad]=ftell(savefile);
	// keycode
	fputc(pads[_pad].keycode,savefile);
	// color
	fputc(pads[_pad].r,savefile);
	fputc(pads[_pad].g,savefile);
	fputc(pads[_pad].b,savefile);
	// panning
	fputc(*((unsigned int*)&(pads[_pad].panning))&255,savefile);
	fputc((*((unsigned int*)&(pads[_pad].panning))>>8)&255,savefile);
	fputc((*((unsigned int*)&(pads[_pad].panning))>>16)&255,savefile);
	fputc((*((unsigned int*)&(pads[_pad].panning))>>24)&255,savefile);
	// volume
	fputc(*((unsigned int*)&(pads[_pad].volume))&255,savefile);
	fputc((*((unsigned int*)&(pads[_pad].volume))>>8)&255,savefile);
	fputc((*((unsigned int*)&(pads[_pad].volume))>>16)&255,savefile);
	fputc((*((unsigned int*)&(pads[_pad].volume))>>24)&255,savefile);
	// samplerate
	fputc(*((unsigned int*)&(pads[_pad].samplerate))&255,savefile);
	fputc((*((unsigned int*)&(pads[_pad].samplerate))>>8)&255,savefile);
	fputc((*((unsigned int*)&(pads[_pad].samplerate))>>16)&255,savefile);
	fputc((*((unsigned int*)&(pads[_pad].samplerate))>>24)&255,savefile);
	// pitch
	fputc(*((unsigned int*)&(pads[_pad].pitch))&255,savefile);
	fputc((*((unsigned int*)&(pads[_pad].pitch))>>8)&255,savefile);
	fputc((*((unsigned int*)&(pads[_pad].pitch))>>16)&255,savefile);
	fputc((*((unsigned int*)&(pads[_pad].pitch))>>24)&255,savefile);
	// length
	fputc(pads[_pad].samplesize&255,savefile);
	fputc((pads[_pad].samplesize>>8)&255,savefile);
	fputc((pads[_pad].samplesize>>16)&255,savefile);
	fputc((pads[_pad].samplesize>>24)&255,savefile);
	// name
	for (int npos=0;npos<32;npos++){
	  fputc(pads[_pad].name[npos],savefile);
	}
	// end of pad header
	fputc(0,savefile);
	// sample data
	for (int spos=0;spos<pads[_pad].samplesize*2;spos++){
	  fputc(*((unsigned int*)&(pads[_pad].sample[spos]))&255,savefile);
	  fputc((*((unsigned int*)&(pads[_pad].sample[spos]))>>8)&255,savefile);
	  fputc((*((unsigned int*)&(pads[_pad].sample[spos]))>>16)&255,savefile);
	  fputc((*((unsigned int*)&(pads[_pad].sample[spos]))>>24)&255,savefile);
	}
      }
      // pointers to samples
      fseek(savefile,16,0);
      for (int lastone=0;lastone<padcount;lastone++){
	fputc(padpointer[lastone]&255,savefile);
	fputc((padpointer[lastone]>>8)&255,savefile);
	fputc((padpointer[lastone]>>16)&255,savefile);
	fputc((padpointer[lastone]>>24)&255,savefile);
      }
      fclose(savefile);
    } else {printf("closed\n");}
    al_destroy_native_file_dialog(filechooser);
}

void loadbank(){
    printf("pointer: %d\n",pads[0].sample);
    ALLEGRO_FILECHOOSER* filechooser;
    filechooser=al_create_native_file_dialog(".",strChooseFile,NULL,0);
    al_show_native_file_dialog(display,filechooser);
    char rfn[257];
    rfn[0]=0;
    if (al_get_native_file_dialog_path(filechooser,0)!=NULL){
      strcpy(rfn,al_get_native_file_dialog_path(filechooser,0));
      printf("file: %s\n",rfn);
      FILE* savefile;
      savefile=fopen(rfn,"rb");
      if (savefile!=NULL){
      printf("shall seek\n");
      fseek(savefile,16,0);
      // pads
      //fseek(savefile,padcount*4,SEEK_CUR);
      printf("shall create padpointers\n");
      unsigned long int padpointer[256];
      printf("shall load padpointers\n");
      for (int notlast=0;notlast<padcount;notlast++){
	padpointer[notlast]=fgetc(savefile);
	padpointer[notlast]+=fgetc(savefile)<<8;
	padpointer[notlast]+=fgetc(savefile)<<16;
	padpointer[notlast]+=fgetc(savefile)<<24;
      }
      printf("shall load pad\n");
      for (int _pad=0;_pad<padcount;_pad++){
	printf("shall seek\n");
	fseek(savefile,padpointer[_pad],0);
	// keycode
	printf("shall read kc\n");
	pads[_pad].keycode=fgetc(savefile);
	// color
	printf("shall read colors\n");
	pads[_pad].r=fgetc(savefile);
	pads[_pad].g=fgetc(savefile);
	pads[_pad].b=fgetc(savefile);
	// panning
	printf("shall read panning!!!\n");
	memset(&pads[_pad].panning,fgetc(savefile),1);
	memset(&pads[_pad].panning+1,fgetc(savefile),1);
	memset(&pads[_pad].panning+2,fgetc(savefile),1);
	memset(&pads[_pad].panning+3,fgetc(savefile),1);
	// volume
	printf("shall read volume!!!\n");
	memset(&pads[_pad].volume,fgetc(savefile),1);
	memset(&pads[_pad].volume+1,fgetc(savefile),1);
	memset(&pads[_pad].volume+2,fgetc(savefile),1);
	memset(&pads[_pad].volume+3,fgetc(savefile),1);
	// samplerate
	printf("shall read sr!!!\n");
	memset(&pads[_pad].samplerate,fgetc(savefile),1);
	memset(&pads[_pad].samplerate+1,fgetc(savefile),1);
	memset(&pads[_pad].samplerate+2,fgetc(savefile),1);
	memset(&pads[_pad].samplerate+3,fgetc(savefile),1);
	// pitch
	printf("shall read pitch!!!\n");
	memset(&pads[_pad].pitch,fgetc(savefile),1);
	memset(&pads[_pad].pitch+1,fgetc(savefile),1);
	memset(&pads[_pad].pitch+2,fgetc(savefile),1);
	memset(&pads[_pad].pitch+3,fgetc(savefile),1);
	// length
	printf("shall read lengthhhhhhh!!!\n");
	pads[_pad].samplesize=(fgetc(savefile))+(fgetc(savefile)<<8)+(fgetc(savefile)<<16)+(fgetc(savefile)<<24);
	// name
	printf("shall read name!!!\n");
	for (int npos=0;npos<32;npos++){
	  pads[_pad].name[npos]=fgetc(savefile);
	}
	printf("shall set name[32] to 0!!!\n");
	pads[_pad].name[32]=0;
	// sample data
	printf("shall delete block!!!\n");
	printf("pointer: %d\n",pads[_pad].sample);
	free((float*)pads[_pad].sample);
	if (pads[_pad].samplesize!=0) {
	  printf("shall create block with size %lu!!!\n",pads[_pad].samplesize*2);
	  pads[_pad].sample=(float*)calloc(pads[_pad].samplesize*2,sizeof(float));
	for (int spos=0;spos<pads[_pad].samplesize*2;spos++){
	  memset(&pads[_pad].sample[spos],fgetc(savefile),1);
	  memset(&pads[_pad].sample[spos]+1,fgetc(savefile),1);
	  memset(&pads[_pad].sample[spos]+2,fgetc(savefile),1);
	  memset(&pads[_pad].sample[spos]+3,fgetc(savefile),1);
	}
	} else {printf("shall create block with size 6!!!\n"); pads[_pad].sample=new float[6];}
      }
      printf("donedone\n");
      fclose(savefile);
      printf("done\n");
      }
      else {perror("error");}
     } else {printf("closed\n");}
    al_destroy_native_file_dialog(filechooser);
    printf("fd was closed\n");
}

void KeyboardEvents(){
	// keyboard states
	al_get_keyboard_state(&kbstate);
	// check for presses
	for (int cntkb=0;cntkb<255;cntkb++){
	kblast[cntkb]=kb[cntkb];
	kbpressed[cntkb]=false;
	kb[cntkb]=al_key_down(&kbstate,cntkb);
	if (kb[cntkb]!=kblast[cntkb] && kb[cntkb]==true) {
		kbpressed[cntkb]=true;
		//if (verbose) {cout << cntkb;}
	}}
	// main code here
	if (kbpressed[ALLEGRO_KEY_F1]) {hdp=!hdp;}
	if (!drawtextinput) {
	if (kbpressed[ALLEGRO_KEY_L]) {loadbank();printf("out of loadbank\n");}
	if (kbpressed[ALLEGRO_KEY_S]) {writebank();}
	}
}

/*static void *audiothread(ALLEGRO_THREAD *thread, void *arg){
  if (!(aeq=al_create_event_queue())) {printf(strNoEQ);return NULL;}
  initaudio_allegro(DEFAULT_FRAGMENTS,DEFAULT_BUFSIZE,DEFAULT_SR);
  while (1) {
    efficientaudioroutine(DEFAULT_BUFSIZE,NULL);
  }
}*/

int main(int argc, char **argv) {
    al_init();
#ifdef SDL_INSTEAD
    SDL_Init(SDL_INIT_AUDIO);
    out=new SDL_AudioSpec;
outRes=new SDL_AudioSpec;
    out->freq=44100;
    out->format=AUDIO_F32;
    out->channels=2;
    out->samples=2048;
    out->callback=efficientaudioroutine;
    out->userdata=NULL;
    audioID=SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0,0),0,out,outRes, SDL_AUDIO_ALLOW_ANY_CHANGE);
#else
    const char **ports;
    const char *client_name = "pads";
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;
	
	client = jack_client_open (client_name, options, &status, server_name);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		return 1;
	}
	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}
#endif
    
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    if (!(al_install_audio())) {printf(strNoAudio);} else {al_reserve_samples(1);}
    al_init_acodec_addon();
    /*mp3error=mpg123_init();
    if(mp3error!=MPG123_OK){
      printf(strNoMP3,mpg123_plain_strerror(mp3error));
    } else {
      mp3h=mpg123_new(NULL,&mp3error);
      mpg123_param(mp3h,MPG123_ADD_FLAGS,MPG123_FORCE_FLOAT,0.); 
    }*/
    // create stuff
    //al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    dw=1024; dh=600; // default sizes
    if (!(display=al_create_display(dw,dh))) {printf(strNoWindow);return 1;}
    //dw=al_get_display_width(display); dh=al_get_display_height(display); // default sizes
    if (!(frame=al_create_timer(1.0/DEFAULT_FPS))) {printf(strNoTimer);return 1;}
    if (!(eq=al_create_event_queue())) {printf(strNoEQ);return 1;}
    
    //padcount=32;
    //pad pads[padcount];
    for (int i=0;i<padcount;i++){
      pads[i].r=255;
      pads[i].g=255;
      pads[i].b=255;
      pads[i].sample=(float*)calloc(6,sizeof(float));
      printf("pointer: %d\n",pads[i].sample);
      pads[i].keycode=0;
      for (int iii=0;iii<65;iii++){
      pads[i].name[iii]=0;
      }
      pads[i].panning=0;
      pads[i].volume=1;
      pads[i].playing=0;
      pads[i].position=0;
      pads[i].samplesize=0;
      pads[i].pitch=1;
      pads[i].newpitch=1;
      pads[i].loop=0;
      pads[i].x=offx+(i%maxx*(padwidth+sepx));
      pads[i].y=offy+((i/maxx)*(padheight+sepy));
    }
    // test thing...
    //loadbank();
    // load fonts
    fText=al_load_ttf_font("text.ttf",12,0);
    //testdummy=al_load_sample("testsample.wav");
    
    // reserve samples
    //al_reserve_samples(1);
    
    // register event sources
    al_register_event_source(eq,al_get_display_event_source(display));
    al_register_event_source(eq,al_get_timer_event_source(frame));
    al_register_event_source(eq,al_get_keyboard_event_source());

#ifndef SDL_INSTEAD
    jack_set_process_callback (client, efficientaudioroutine, NULL);
outL = jack_port_register (client, "outL",
					 JACK_DEFAULT_AUDIO_TYPE,
					 JackPortIsOutput, 0);
	outR = jack_port_register (client, "outR",
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsOutput, 0);
	if ((outL == NULL) || (outR == NULL)) {
		fprintf(stderr, "no more JACK ports available\n");
		return 1;
	}
#endif
    // clear the display
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(0,0,0));
    al_wait_for_vsync();
    al_flip_display();
    
    // wait a second
    //al_rest(1);
    //athread=al_create_thread(audiothread,NULL);
    // begin the game
    al_start_timer(frame);
    //al_start_thread(athread);
#ifdef SDL_INSTEAD
    SDL_PauseAudioDevice(audioID,0);
#else
    if (jack_activate (client)) {
		fprintf (stderr, "cannot activate client");
		return 1;
	}

	ports = jack_get_ports (client, NULL, NULL,
				JackPortIsPhysical|JackPortIsInput);
	if (ports == NULL) {
		fprintf(stderr, "no physical capture ports\n");
		exit (1);
	}

	if (jack_connect (client, jack_port_name (outL), ports[0] )) {
		fprintf (stderr, "cannot connect outL\n");
	}

	
	//ports = jack_get_ports (client, NULL, NULL,
	//			JackPortIsPhysical|JackPortIsInput);
	if (ports == NULL) {
		fprintf(stderr, "no physical playback ports\n");
		exit (1);
	}

	if (jack_connect (client, jack_port_name (outR), ports[1])) {
		fprintf (stderr, "cannot connect outR\n");
	}

	free (ports);
	#endif
    doredraw=true;
    while(1){
      // get next event
      ALLEGRO_EVENT event;
      al_wait_for_event(eq,&event);
      if (event.type==ALLEGRO_EVENT_TIMER){
	//doredraw=true;
	//rcrc--; if (rcrc<0) {rcrc=0;}
	KeyboardEvents();
	al_get_mouse_state(&ms);
	for (int ii=0;ii<padcount;ii++){
	  if (pads[ii].checkmouse(&ms)) {
	   if (!drawpadmenu && !drawtextinput){
	     doredraw=true;
	     if (kb[ALLEGRO_KEY_LCTRL]) {
             pads[ii].newpitch-=(float)(wheelrel-ms.z)/10.0f;
	     if (pads[ii].newpitch>4){pads[ii].newpitch=4;}
	     if (pads[ii].newpitch<0){pads[ii].newpitch=0;}
             } else {
		     pads[ii].volume-=(float)(wheelrel-ms.z)/10.0f;
	     if (pads[ii].volume>2){pads[ii].volume=2;}
	     if (pads[ii].volume<0){pads[ii].volume=0;}
             }
	   }
	   if (ms.buttons&2) { // load sample
	     if (!rcrc){
	     rcrc=true;
	     //al_play_sample(testdummy,1,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
	     drawpadmenu=true;
	     doredraw=true;
	     padmenux=ms.x;
	     padmenuy=ms.y;
	     padmenuid=ii;
	     }} else {rcrc=false;}
	   if (ms.buttons&1){
	     if (!leftclick){
	     leftclick=true;
	     printf("cliiiiiiick\n");
	     if (!drawpadmenu && !drawtextinput){
             if (!kb[ALLEGRO_KEY_LCTRL]) {
	     printf("checking for length in %d: %lu\n",ii,pads[ii].samplesize);
	     if (pads[ii].samplesize>0){
	     pads[ii].position=0;
	     pads[ii].playing=true;
	     if (kb[ALLEGRO_KEY_LSHIFT] || kb[ALLEGRO_KEY_RSHIFT]) {
	       pads[ii].loop=true;
	     } else {
	       pads[ii].loop=false;
	     }
	     doredraw=true;
	     // floating-point hack
	     /*FILE* no=fopen("thing","wb");
	     fputc(*((unsigned int*)&(pads[0].volume))&255,no);
	     fputc((*((unsigned int*)&(pads[0].volume))>>8)&255,no);
	     fputc((*((unsigned int*)&(pads[0].volume))>>16)&255,no);
	     fputc((*((unsigned int*)&(pads[0].volume))>>24)&255,no);
	     fclose(no);*/
	     printf("playing pad %d\n",ii);
	     }} else {pads[ii].newpitch=1;}}
	     }
	   } else {leftclick=false;}
	   if (ms.buttons&4){
	     pads[ii].position=0;
	     pads[ii].playing=false;
	   }
	  }
	  if (ms.buttons&1){
	     if (!leftclick1){
	     leftclick1=true;
	     if (drawpadmenu){
	       // pad menu main routine
	       if (PointInRect(padmenux,padmenuy,padmenux+128,padmenuy+54,ms.x,ms.y)) {
	       switch (((ms.y-padmenuy)/18)){
		 case 0: { ////////
		   ALLEGRO_FILECHOOSER* filechooser;
	     filechooser=al_create_native_file_dialog(".",strChooseFile,NULL,0);
	     al_show_native_file_dialog(display,filechooser);
	     char rfn[257];
	     char tempname[257];
	     rfn[0]=0;
	     tempname[0]=0;
	     if (al_get_native_file_dialog_path(filechooser,0)!=NULL){
	     strcpy(rfn,al_get_native_file_dialog_path(filechooser,0));
	     printf("DIR: %s\n",rfn);
	    } else {printf("USER CLOSED THE DIALOG\n");}
	     al_destroy_native_file_dialog(filechooser);
	     ALLEGRO_SAMPLE* tempsample;
	     tempsample=al_load_sample(rfn);
	     char* ext=strstr(rfn,".");
	     if (ext!=NULL) {
	     printf("extension: %s\n",ext);
	     }
	     printf("padid=%d\n",padmenuid);
	     if (tempsample!=NULL){
	       printf("ts not null\n");
	       ALLEGRO_AUDIO_DEPTH thedepth=al_get_sample_depth(tempsample);
	       ALLEGRO_CHANNEL_CONF chanconf=al_get_sample_channels(tempsample);
	       int tempsr=al_get_sample_frequency(tempsample);
	       pads[padmenuid].samplerate=tempsr;
	       free((float*)pads[padmenuid].sample);
	       #if defined(__MINGW32__) || defined(_WIN32)
	       strcpy(pads[padmenuid].name,strrchr(rfn,'\\')+1);
	       #else
	       strcpy(pads[padmenuid].name,strrchr(rfn,'/')+1);
	       #endif
               memset(strrchr(pads[padmenuid].name,'.'),0,1);
	       switch (chanconf){
		 case ALLEGRO_CHANNEL_CONF_1:
		  switch (thedepth){
		    case ALLEGRO_AUDIO_DEPTH_FLOAT32:
		    {
		      float* samplepointer;
		      unsigned long int slength=al_get_sample_length(tempsample);
		      pads[padmenuid].samplesize=slength;
		      samplepointer=(float*)al_get_sample_data(tempsample);
		      pads[padmenuid].sample=(float*)calloc((slength*2)+2,sizeof(float));
		      for (int j=0;j<slength;j++){
			pads[padmenuid].sample[j*2]=samplepointer[j];
			pads[padmenuid].sample[(j*2)+1]=samplepointer[j];
		      }
		      break;
		    }
		    case ALLEGRO_AUDIO_DEPTH_INT16:
		    {
		      short* samplepointer;
		      unsigned long int slength=al_get_sample_length(tempsample);
		      pads[padmenuid].samplesize=slength;
		      samplepointer=(short*)al_get_sample_data(tempsample);
		      pads[padmenuid].sample=(float*)calloc((slength*2)+2,sizeof(float));
		      for (int j=0;j<slength;j++){
			pads[padmenuid].sample[j*2]=(float)samplepointer[j]/32768.0f;
			pads[padmenuid].sample[(j*2)+1]=(float)samplepointer[j]/32768.0f;
		      }
		      break;
		    }
		    case ALLEGRO_AUDIO_DEPTH_UINT16:
		    {
		      unsigned short* samplepointer;
		      unsigned long int slength=al_get_sample_length(tempsample);
		      pads[padmenuid].samplesize=slength;
		      samplepointer=(unsigned short*)al_get_sample_data(tempsample);
		      pads[padmenuid].sample=(float*)calloc((slength*2)+2,sizeof(float));
		      for (int j=0;j<slength;j++){
			pads[padmenuid].sample[j*2]=((float)samplepointer[j]/32768.0f)-1.0f;
			pads[padmenuid].sample[(j*2)+1]=((float)samplepointer[j]/32768.0f)-1.0f;
		      }
		      break;
		    }
		  }
		  break;
		 case ALLEGRO_CHANNEL_CONF_2:
		  switch (thedepth){
		    case ALLEGRO_AUDIO_DEPTH_FLOAT32:
		    {
		      float* samplepointer;
		      unsigned long int slength=al_get_sample_length(tempsample);
		      pads[padmenuid].samplesize=slength;
		      samplepointer=(float*)al_get_sample_data(tempsample);
		      pads[padmenuid].sample=(float*)calloc((slength*2)+2,sizeof(float));
		      for (int j=0;j<(slength*2)+2;j++){
			pads[padmenuid].sample[j]=samplepointer[j];
		      }
		      break;
		    }
		    case ALLEGRO_AUDIO_DEPTH_INT16:
		    {
		      short* samplepointer;
		      unsigned long int slength=al_get_sample_length(tempsample);
		      pads[padmenuid].samplesize=slength;
		      samplepointer=(short*)al_get_sample_data(tempsample);
		      pads[padmenuid].sample=(float*)calloc((slength*2)+2,sizeof(float));
		      for (int j=0;j<(slength*2)+2;j++){
			pads[padmenuid].sample[j]=(float)samplepointer[j]/32768.0f;
		      }
		      break;
		    }
		    case ALLEGRO_AUDIO_DEPTH_UINT16:
		    {
		      unsigned short* samplepointer;
		      unsigned long int slength=al_get_sample_length(tempsample);
		      pads[padmenuid].samplesize=slength;
		      samplepointer=(unsigned short*)al_get_sample_data(tempsample);
		      pads[padmenuid].sample=(float*)calloc((slength*2)+2,sizeof(float));
		      for (int j=0;j<(slength*2)+2;j++){
			pads[padmenuid].sample[j]=((float)samplepointer[j]/32768.0f)-1.0f;
		      }
		      break;
		    }
		  }
		  break;
	       }
	       al_destroy_sample(tempsample);
	       printf("length %lu\n",pads[padmenuid].samplesize);
	     } else if (ext!=NULL) {if (strcmp(ext,".mp3") || strcmp(ext,".mP3") || strcmp(ext,".Mp3") || strcmp(ext,".MP3")) {
	       // mp3 thingy
	       /*if (mpg123_open(mp3h,rfn)==MPG123_OK){
		 printf("can open\n");
		 long int tempsr=0;
		 int chanconf=0;
		 int encoding=0;
		 int slength=0;
		 int pos=0;
		 if (mpg123_getformat(mp3h,&tempsr,&chanconf,&encoding)==MPG123_OK) {
		   printf("can get format\n");
		   pads[padmenuid].samplerate=tempsr;
		   if(encoding==MPG123_ENC_SIGNED_16 || encoding==MPG123_ENC_FLOAT_32){
		     delete[] pads[padmenuid].sample;
		     mpg123_format_none(mp3h);
		     mpg123_format(mp3h,tempsr,chanconf,encoding);
		     size_t bs=mpg123_outblock(mp3h);
		     pads[padmenuid].sample=new float[bs];
		     printf("ob: %d\n",(int)bs);
		     size_t sdone;
		     mp3buf1=new unsigned char[bs];
		     do {
		       mp3error=mpg123_read(mp3h,mp3buf1,bs,&sdone);
		       mp3buf.resize(bs);
		       for (int iii=0;iii<bs;iii++){
			pads[padmenuid].sample[iii]=(float)mp3buf1[iii];
		       }
		       pos+=bs;
		     } while (mp3error!=MPG123_OK);
		     slength=pos;
		     pads[padmenuid].samplesize=slength;
		     printf("%d samples readen.\n",slength);
		   }
		 }
	       }*/
	      }else {printf("sample does not exist or can't be loaded\n");}}
	     else {printf("sample does not exist or can't be loaded\n");}
		 } ////////
		 break;
		    case 1: textinputid=padmenuid; textinputpos=(pads[textinputid].name[0]!=0)?(strlen(pads[textinputid].name)):(0); drawtextinput=true; break;
		 case 2: free((float*)pads[padmenuid].sample); pads[padmenuid].sample=(float*)calloc(6,sizeof(float)); pads[padmenuid].name[0]=0; break;
		 default: printf("ok this should not happen!\n"); abort(); break;
	       }
	       }
	       drawpadmenu=false;
	       doredraw=true;
	     }
	     }} else {leftclick1=false;}
	}
	wheelrel=ms.z;
      } else if (event.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
	break;
      } else if (event.type==45 || event.type==46) {
        doredraw=true;
      }  else if(event.type == ALLEGRO_EVENT_KEY_CHAR){
	if (drawtextinput){
	  switch (event.keyboard.keycode){
	    case ALLEGRO_KEY_ENTER: drawtextinput=false; break;
	    case ALLEGRO_KEY_BACKSPACE: textinputpos--; if (textinputpos<0) {textinputpos=0;} else {pads[textinputid].name[textinputpos]=0;}; break;
	    default: if (textinputpos<32) {pads[textinputid].name[textinputpos]=event.keyboard.unichar; textinputpos++;}; break;
	  }
	  doredraw=true;
	}
      }
      if (doredraw && al_is_event_queue_empty(eq)){
	doredraw=false;
	al_set_target_backbuffer(display);
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_textf(fText,al_map_rgb(255,255,255),0,0,0,"pads dev%d",ver);
	al_draw_textf(fText,al_map_rgb(255,255,255),0,10,0,"%s: %d",strWidth,padwidth);
	al_draw_textf(fText,al_map_rgb(255,255,255),96,10,0,"%s: %d",strHeight,padheight);
	al_draw_textf(fText,al_map_rgb(255,255,255),192,10,0,"%s: %d",strSepX,sepx);
	al_draw_textf(fText,al_map_rgb(255,255,255),288,10,0,"%s: %d",strSepY,sepy);
	al_draw_textf(fText,al_map_rgb(255,255,255),384,10,0,"%s: %d",strMaxX,maxx);
	al_draw_textf(fText,al_map_rgb(255,255,255),480,10,0,"%s: %d",strOffX,offx);
	al_draw_textf(fText,al_map_rgb(255,255,255),576,10,0,"%s: %d",strOffY,offy);
	al_draw_textf(fText,al_map_rgb(255,255,255),dw,0,ALLEGRO_ALIGN_RIGHT,strUsing,strbackend,DEFAULT_FRAGMENTS,DEFAULT_BUFSIZE,DEFAULT_SR);
	al_draw_textf(fText,al_map_rgb(255,255,255),dw,10,ALLEGRO_ALIGN_RIGHT,strLatency,1000.0f*(float)(DEFAULT_BUFSIZE*DEFAULT_FRAGMENTS)/(float)DEFAULT_SR);
	for (int ii=0;ii<padcount;ii++){
	pads[ii].draw();
	}
	//al_draw_textf(fText,al_map_rgb(255,255,255),0,0,ALLEGRO_ALIGN_LEFT,"no");
	// help dialog
	if (hdp){
	  al_draw_filled_rectangle(dw/2-128,dh/2-64,dw/2+128,dh/2+64,al_map_rgb(0,0,0));
	  al_draw_rectangle(dw/2-128,dh/2-64,dw/2+128,dh/2+64,al_map_rgb(255,255,255),1);
	  al_draw_text(fText,al_map_rgb(255,255,255),dw/2,dh/2-64,ALLEGRO_ALIGN_CENTER,strHelpHeader);
	  al_draw_text(fText,al_map_rgb(255,255,255),dw/2,dh/2-44,ALLEGRO_ALIGN_CENTER,strHelpLine1);
	  al_draw_text(fText,al_map_rgb(255,255,255),dw/2,dh/2-34,ALLEGRO_ALIGN_CENTER,strHelpLine2);
	  al_draw_text(fText,al_map_rgb(255,255,255),dw/2,dh/2-24,ALLEGRO_ALIGN_CENTER,strHelpLine3);
	  al_draw_text(fText,al_map_rgb(255,255,255),dw/2,dh/2-14,ALLEGRO_ALIGN_CENTER,strHelpLine4);
	  al_draw_text(fText,al_map_rgb(255,255,255),dw/2,dh/2-4 ,ALLEGRO_ALIGN_CENTER,strHelpLine5);
	  al_draw_text(fText,al_map_rgb(255,255,255),dw/2,dh/2+6 ,ALLEGRO_ALIGN_CENTER,strHelpLine6);
	  al_draw_text(fText,al_map_rgb(255,255,255),dw/2,dh/2+16,ALLEGRO_ALIGN_CENTER,strHelpLine7);
	  al_draw_text(fText,al_map_rgb(255,255,255),dw/2,dh/2+26,ALLEGRO_ALIGN_CENTER,strHelpLine8);
	}
	if (drawpadmenu){
	  al_draw_filled_rectangle(padmenux,padmenuy,padmenux+128,padmenuy+55,al_map_rgb(0,0,0));
	  al_draw_rectangle(padmenux,padmenuy,padmenux+128,padmenuy+55,al_map_rgb(255,255,255),1);
	  if (PointInRect(padmenux,padmenuy,padmenux+128,padmenuy+54,ms.x,ms.y)) {
	  al_draw_filled_rectangle(padmenux+1,padmenuy+(((ms.y-padmenuy)/18)*18),padmenux+127,padmenuy+(((ms.y-padmenuy)/18)*18)+18,al_map_rgb(128,128,128));
	  }
	  al_draw_text(fText,al_map_rgb(255,255,255),padmenux+4,padmenuy+4,0,strMenuLoad);
	  al_draw_text(fText,al_map_rgb(255,255,255),padmenux+4,padmenuy+22,0,strMenuRename);
	  al_draw_text(fText,al_map_rgb(255,255,255),padmenux+4,padmenuy+40,0,strMenuDelete);
	}
	if (drawtextinput){
	  al_draw_filled_rectangle(dw/2-128,dh/2-24,dw/2+128,dh/2+24,al_map_rgb(0,0,0));
	  al_draw_rectangle(dw/2-128,dh/2-24,dw/2+128,dh/2+24,al_map_rgb(255,255,255),1);
	  al_draw_text(fText,al_map_rgb(255,255,255),dw/2,dh/2-24,ALLEGRO_ALIGN_CENTER,strInputHeader);
	  al_draw_textf(fText,al_map_rgb(255,255,255),dw/2-128,dh/2,0,"%s",pads[textinputid].name);
	  al_draw_textf(fText,al_map_rgb(255,255,255),dw/2-128,dh/2+50,0,"pos: %d id: %d",textinputpos, textinputid);
	}
	al_flip_display();
      }
    }
    
    // end the game
    al_stop_timer(frame);
#ifdef SDL_INSTEAD
    SDL_CloseAudioDevice(audioID);
#else
    jack_client_close (client);
#endif
    return 0;
}

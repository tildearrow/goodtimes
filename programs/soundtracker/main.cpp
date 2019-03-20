//>[[[[[[[[[[[[[[[[<~>]]]]]]]]]]]]]]]]<//
//+++++++++++++++++++++++++++++++++++++//
//     sound tracker for chiptunes     //
//+++++++++++++++++++++++++++++++++++++//
// written by tildearrow in C++ during //
//             2014-2015.              //
//>++++++++++-+-++-*.*-++-+-++++++++++<//

#define PROGRAM_NAME "soundtracker"
#ifdef _WIN32
#define PROGRAM_NAME "soundtracker.exe"
#endif
#ifdef __APPLE__
#define PROGRAM_NAME "soun..."
#endif
float rt1=0;
float rt2=0;

float rt3=0;
float rt4=0;
double time1=0;
double time2=0;

//// DEFINITIONS ////
#define minval(a,b) (((a)<(b))?(a):(b)) // for Linux compatibility
#define maxval(a,b) (((a)>(b))?(a):(b)) // for Linux compatibility
#define sign(a) ((a>0)?(1):((a<0)?(-1):(0)))
//#define MOUSE_GRID
//#define NTSC // define for NTSC mode
//#define FILM // define for film mode (NTSC should be defined first)
//#define SMOOTH_SCROLL
#define SOUNDS
//#define JACK
int dpiScale;
//#define VBLANK
//#define PRESERVE_INS

bool ntsc=false;

float DETUNE_FACTOR_GLOBAL;

//// INCLUDES AND STUFF ////
#include "tracker.h"
#ifdef JACK
#include <jack/jack.h>
#include <jack/midiport.h>
jack_port_t *joutl;
jack_port_t *joutr;
jack_port_t *midi;
jack_client_t *jclient;
jack_status_t jstatus;
jack_nframes_t jacksr;
#else
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
SDL_AudioDeviceID audioID;
SDL_AudioSpec* sout;
SDL_AudioSpec* spout;
uint32_t jacksr;
#endif

#include "soundchip.h"
soundchip chip[4]; // up to 4 soundchips
#define soundchips 2

using namespace std;
   ALLEGRO_TIMER *timer = NULL;
const bool verbose=false; // change this to turn on verbose mode
char songdf=0;
double FPS=50;
int tempo;

int doframe;
ALLEGRO_BITMAP *bpatterns=NULL;
unsigned char colorof[6]={0x00, 0x5f, 0x87, 0xaf, 0xd7, 0xff};
int degrees=0; // global all-purpose sine
int prevX=0;
int prevY=0;
int cycle=0;
float totalrender=0;
int cycle1=0;
// init sound stuff
float detunefactor;
unsigned int cfreq[32]={1,1,1,1,1,1,1,1,
         1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1};
short cvol[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
short chanvol[32]={128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128};
unsigned char cshape[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char cshapeprev[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char cduty[32]={31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
               31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31};
int cstep[32]={};
int crstep[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int crrmstep[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int coff[32]={262144,262144,262144,262144,262144,262144,262144,
              262144,262144,262144,262144,262144,262144,262144,
              262144,262144,262144,262144,262144,262144,262144,
              262144,262144,262144,262144,262144,262144,262144,
        262144,262144,262144,262144};
unsigned char creso[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char cfmode[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char cmode[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char cpan[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool cfsweep[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int cfcycles[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int cfperiod[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool cvisweep[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool cviloop[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int cvicycles[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool cvdsweep[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool cvdloop[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int cvdcycles[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int cvperiod[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool cvdir[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double cpcmpos[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned short crmfreq[32]={100,1,1,1,1,1,1,1,
         1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1};
unsigned char crmshape[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char crm[32]={0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0};
unsigned char crmduty[32]={31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
               31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31};
int cpcmstart[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int cbound[32]={131071,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool cloop[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int cglobvol=128;
bool AlreadySkipped=false;
enum filters {
  fNone, fLowPass, fHighPass, fNotch, fBandPass, fLowBand, fHighBand, fAllPass
};
float cseek[32]={};
float crseek[32]={};
#ifdef NTSC
#ifdef FILM
const unsigned int bufsize=8000;
#else
const unsigned int bufsize=8000;
#endif
#else
const unsigned int bufsize=8000;
#endif
int rngpgv[32]={0};
int scrollpos=0;
int valuewidth=4;
int oldpat=-1;
unsigned char CurrentIns=0;
unsigned char CurrentEnv=0;
// init tracker stuff
int pattern=0;
unsigned char patid[256]={};
unsigned char* patlength;
struct Instrument {
  char name[32];
  unsigned char id, pcmMult, activeEnv;
  unsigned char env[8];
  unsigned char noteOffset;
  unsigned char FPt, FPR, DFM, LFO;
  unsigned char vol, pan;
  unsigned short pcmLen, filterH;
  unsigned char res;
  unsigned char pcmPos[2]; // alignment
  unsigned char pcmLoop[2];
  unsigned char FTm;
  unsigned short ver;
  unsigned char flags, RMf;
};
Instrument instrument[256]; // instrument[id][position]
unsigned char bytable[8][256][256]={}; // bytable[table][indextab][position]
unsigned char pat[256][256][32][5]={}; // pat[patid][patpos][channel][bytepos]
int scroll[32][7]={}; // scroll[channel][envelope]
unsigned char songlength=255;
int orders=0;
int instruments=0;
int patterns=0;
int seqs=255;
char chpan[32]={};
char chvol[32]={};
bool muted[32]={};
bool leftpress=false;
bool rightpress=false;
bool hexmode=false;
bool quit=false;
// channel memory stuff
unsigned char Msmp[32]={};
int Mvol[32]={};
int Mport[32]={};
int Mvib[32]={};
int Mtrem[32]={};
int Marp[32]={};
int Mins[32]={};
bool EnvelopesRunning[32][8]={}; // EnvelopesRunning[channel][envelope]

char name[32]=""; // song name
unsigned char defspeed=6; // default song speed
unsigned char speed=6; // current speed
char playmode=0; // playmode (-1: reverse, 0: stopped, 1: playing, 2: paused)
int curstep=0; // current step
unsigned char curpat=0; // current pattern
int curtick=0; // current tick
int curins=1; // selected instrument
int curoctave=2;
int curedchan=0; // cureditingchannel
int curedmode=0; // current editing mode, 0=note, 1=instrument number 2=volume 3=effect name 4=effect value
int curedpage=0; // current page, 0-3
int curselchan=0;
int curselmode=0;
int selStart=0;
int selEnd=0;
bool follow=true;
int curpatrow=0;
int chanstodisplay=8;
int maxCTD=8;
double patseek=0;
int screen=0; // 0=patterns, 1=instruments 2=diskop 3=song 4=mixer 5=config 6=help 7=about
int diskopscrollpos=0;
bool kb[256]={false};
bool kblast[256]={false};
bool kbpressed[256]={false};
bool IRP[32][8]={}; // in release point
bool released[32]={}; // note has been released
bool edittype=true; // 0=ModPlug 1=FT2
int ins[32]={}; // current playing instrument on each channel
int insnote[32]={}; // current playing note on a channel
int insvol[32]={}; // current volume on a channel
bool insfading[32]={}; // checks if the instrument is fading
int insrelease[32]={}; // checks if the instrument is on its release state
int inspos[32][8]={}; // current instrument position per envelope, inspos[chan][env]
int curvol[32]={}; // current volume of a step
int curchanvol[32]={}; // current volume of a channel
float curnote[32]={}; // current note of a channel
int curvibdepth[32]={}; // current vibrato offset
int curvibspeed[32]={}; // current vibrato speed
int curvibpos[32]={};
int curvibshape[32]={};
int curtrmdepth[32]={}; // current tremolo offset
int curtrmspeed[32]={}; // current tremolo speed
int curtrmpos[32]={};
int curtrmshape[32]={};
int curpandepth[32]={}; // current panbrello offset
int curpanspeed[32]={}; // current panbrello speed
int curpanpos[32]={};
int curpanshape[32]={};
int slidememory[32]={}; // current Dxx value
int chanslidememory[32]={}; // current Nxx value
int globslidememory[32]={}; // current Wxx value
int tcmdmemory[32]={}; // current Zxx value
int curhandle[32]={}; // current Zxx handle number
int defshape[32]={0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0}; // default shape per channel (Zc8-Zcf)
int cutcount[32]={0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0}; // SCx ticks to cut
int retrigger[32]={0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0}; // Qxx memory
int retrig[32]={0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0}; // Qxx processor
int plpos[32]={0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0}; // SBx processor - position
int plcount[32]={0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0}; // SBx processor - count
char chanpan[32]={0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0}; // channel panning
int linex1=0;
int liney1=0;
int finepitch[32]={0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0}; // fine pitch envelope value
int finedelay=0;
unsigned char defchanvol[32]={128,128,128,128,128,128,128,128,
                  128,128,128,128,128,128,128,128,
                  128,128,128,128,128,128,128,128,
                  128,128,128,128,128,128,128,128}; // default channel volume
char defchanpan[32]={0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0}; // default channel panning
bool doretrigger[32]={0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0};
bool tickstart=false;
bool linearslides=true;
bool playermode=false;
bool fileswitch=false;
bool reversemode=false;
unsigned char nvolu[32]={}; // next volume value
unsigned char nfxid[32]={}; // next effect
unsigned char nfxvl[32]={}; // next effect value
int portastatic[32]={}; // current note
int sfxpos=-1; // sound effect position
unsigned char sfxdata[32][1024]={
                {39,0,8,127,0,15,4,0,2,0,0,0,
                1,0,31,63,
                1,1,0,63,
                1,2,0,53,
                1,3,0,53,
                1,4,0,43,
                1,5,0,43,
                1,6,0,33,
                1,7,0,33,
                1,8,0,23,
                1,9,0,23,
                1,10,0,11,
                1,11,0,5,
                0},{},{5,0,10,16,4,4,1,0,20,32,1,0,30,64,1,0,40,96,1,0,50,96,1,0,60,96
                     ,1,0,70,96,1,0,80,96,1,0,90,96,1,0,100,88,1,0,110,80,1,0,120,72,1,0,130,64
                    ,1,0,140,56,1,0,150,48,1,0,160,40,1,0,170,32,1,0,180,24,1,0,190,16,1,0,200,8,0},
                {5,1,64,127,31,0,1,1,64,127,1,1,64,127,0},
                {5,6,64,127,31,0,1,6,64,112,1,6,0,96,1,5,192,80,1,5,128,64,1,5,64,48,
                1,5,0,32,1,4,192,16,1,4,128,8,0}}; // sound effect data
int cursfx=0; // current effect

const int sine[256]={
        0,  2,  3,  5,  6,  8,  9, 11, 12, 14, 16, 17, 19, 20, 22, 23,
                24, 26, 27, 29, 30, 32, 33, 34, 36, 37, 38, 39, 41, 42, 43, 44,
                45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 56, 57, 58, 59,
                59, 60, 60, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 64,
                64, 64, 64, 64, 64, 64, 63, 63, 63, 62, 62, 62, 61, 61, 60, 60,
                59, 59, 58, 57, 56, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46,
                45, 44, 43, 42, 41, 39, 38, 37, 36, 34, 33, 32, 30, 29, 27, 26,
                24, 23, 22, 20, 19, 17, 16, 14, 12, 11,  9,  8,  6,  5,  3,  2,
                 0, -2, -3, -5, -6, -8, -9,-11,-12,-14,-16,-17,-19,-20,-22,-23,
               -24,-26,-27,-29,-30,-32,-33,-34,-36,-37,-38,-39,-41,-42,-43,-44,
               -45,-46,-47,-48,-49,-50,-51,-52,-53,-54,-55,-56,-56,-57,-58,-59,
               -59,-60,-60,-61,-61,-62,-62,-62,-63,-63,-63,-64,-64,-64,-64,-64,
               -64,-64,-64,-64,-64,-64,-63,-63,-63,-62,-62,-62,-61,-61,-60,-60,
               -59,-59,-58,-57,-56,-56,-55,-54,-53,-52,-51,-50,-49,-48,-47,-46,
               -45,-44,-43,-42,-41,-39,-38,-37,-36,-34,-33,-32,-30,-29,-27,-26,
               -24,-23,-22,-20,-19,-17,-16,-14,-12,-11, -9, -8, -6, -5, -3, -2
}; // taken directly from Jeffrey Lim's fine sine table
const char HEXVALS[17]="0123456789ABCDEF"; // 17 for the null
const int modeOff[6]={
  24,48,64,88,96,112
};
int sfxcvol=0;
int sfxcpan=0;
int sfxcshape=0;
int sfxcduty=0;
int sfxcoff=0;
int sfxcreso=0;
int sfxcfmode=0;
int sfxcfreq=0;
int sfxcmode=0;
int sfxcrmfreq=0;
int sfxcrm=0;

int sfxspan=0;
int sfxsshape=0;
int sfxsduty=0;
int sfxsoff=0;
int sfxsreso=0;
int sfxsfmode=0;
int sfxsfreq=0;

bool sfxplaying=false;

namespace ASC {
  int interval=119000;
  int currentclock=0;
}
namespace fakeASC {
  int interval=119000;
  int currentclock=0;
}

enum envTypes {
  envVolume=0,
  envCutoff,
  envReson,
  envDuty,
  envShape,
  envPitch,
  envHiPitch,
  envPan
};

ALLEGRO_FONT *text=NULL;
ALLEGRO_AUDIO_STREAM *chan[33]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
                           ,     NULL            };
ALLEGRO_MOUSE_STATE mstate;
ALLEGRO_KEYBOARD_STATE kbstate;
ALLEGRO_BITMAP *logo=NULL;
bool leftclick=false;
bool leftclickprev=false;
bool rightclick=false;  
bool rightclickprev=false;
bool skipframe=false;
int prevZ=0;
int hover[16]={}; // hover time per button
int16_t ver=144; // version number
unsigned char chs0[5000];
char* helptext;
char* comments;
int inputcurpos=0;
int chantoplayfx=0;
char* inputvar=NULL;
int inputwhere=0; // 0=none, 1=songname, 2=insname, 3=filepath, 4=comments
int maxinputsize=0;
int LastUsedChannel[8]={0,0,0,0,0,0,0,0};
int LastUsedChannelMax;
char* curdir;
int pcmeditscale=0;
int pcmeditseek=0;
int pcmeditoffset=0;
bool pcmeditenable=false;
int UPDATEPATTERNS=false;
bool speedlock=false;
bool tempolock=false;
unsigned long int framecounter=0;
unsigned long int audioframecounter=0;
// init the audio
float *buf;
int pitch = 0x20;
int val = 0;
int i;

float nsL[33];
float nsR[33];
float ns[33];

double raster1, raster2, maxrasterdelta;

#define mufflef 0.08
#define mufflefb 0
float muffleb0[2]={0,0};
float muffleb1[2]={0,0};
int filecount=0;
int selectedfileindex=-1;
struct FileInList {
  string name;
  bool isdir;
};
vector<FileInList> filenames;
vector<FileInList> filessorted;
int scrW,scrH;
ALLEGRO_BITMAP *patternbitmap = NULL;
ALLEGRO_BITMAP *pianoroll=NULL;
ALLEGRO_BITMAP *pianoroll_temp=NULL;
ALLEGRO_BITMAP *piano=NULL;
ALLEGRO_BITMAP *mixer=NULL;
ALLEGRO_DISPLAY *display=NULL;
bool firstframe=true;
float* abuf;
float oscbuf[65536]={}; // safe oscilloscope buffer
float oscbuf2[65536]={}; // safe oscilloscope buffer
// only way for 7 channels glitch-free - 7 event queues
ALLEGRO_EVENT_QUEUE *aqueue[33];
// settings
namespace settings {
  // default colors are:    NOTE INST VOLU FXTM FXSN FXVL FXPT FXNO FXSP FXPN FXUK BROW SEL1 SEL2 SEL3 PEAK DEFA DARK LOOP RELE CPOS UPPP DOWN ABSO CRHL SELE NOVL
  unsigned char colorR[27]={0xd0,0x5f,0x00,0xd0,0xff,0x00,0xff,0x60,0xff,0x60,0x80,0xa0,0x00,0xff,0x00,0x00,0xff,0xc0,0x00,0xff,0x00,0x00,0xff,0xff,0x40,0x80,0x5f};
  unsigned char colorG[27]={0xd0,0xd7,0x50,0x30,0x00,0xff,0xff,0x60,0x00,0xff,0x80,0xa0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0x00,0xff,0xff,0x00,0xff,0x40,0x80,0xd7};
  unsigned char colorB[27]={0xd0,0xff,0xff,0xd0,0x00,0x00,0x00,0xff,0xff,0xff,0x80,0xa0,0xff,0x00,0x00,0x00,0xff,0xc0,0xff,0xff,0x00,0x00,0x00,0x00,0x40,0x80,0xff};
  // audio settings
  bool distortion=false;
  bool threading=false;
  bool cubicspline=false;
  int buffersize=5950;
  // importer settings
  bool names=true;
  bool split=false;
  bool samples=true;
  // filter settings
  bool nofilters=false;
  bool muffle=true;
}

// NEW VARIABLES BEGIN //
Graphics g;
// NEW VARIABLES END //

enum colorindexes {
  colNOTE, colINST, colVOLU, colFXTM, colFXSN, colFXVL, colFXPT, colFXNO, colFXSP, colFXPN, colFXUK, colBROW,
  colSEL1, colSEL2, colSEL3, colPEAK, colDEFA, colDARK, colLOOP, colRELE, colCPOS, colUPPP, colDOWN, colABSO,
  colCRHL, colSELE, colNOVL
};
ALLEGRO_COLOR getconfigcol(int colid) {
  return al_map_rgb(settings::colorR[colid],settings::colorG[colid],settings::colorB[colid]);
}
void Playback();
void MuteAllChannels();
int playfx(const unsigned char* fxdata,int fxpos,int achan);
bool PIR(float x1, float y1, float x2, float y2, float checkx, float checky) {
  // point-in-rectangle collision detection
  if (checkx>x1 && checkx<x2 && checky>y1 && checky<y2) {
    return true;}
  return false;
}
#define interpolatee(aa,bb,cc) (aa+((bb-aa)*cc))

double getScale() {
  char* env;
  // try with environment variable
  env=getenv("TRACKER_SCALE");
  if (env!=NULL) {
    return atof(env);
  }
#if defined(__linux__)
  // linux (wayland) code here
#elif defined(_WIN32)
  // windows code here
  HDC disp;
  int dpi;
  disp=GetDC(NULL);
  if (disp==NULL) {
    return 1;
  }
  dpi=GetDeviceCaps(disp,LOGPIXELSX);
  ReleaseDC(NULL,disp);
  return (double)dpi/96.0;
#elif defined(__APPLE__)
  // macOS code here
  double dpi;
  if ((dpi=nsStubDPI())>0) {
    return dpi;
  }
#elif defined(__ANDROID__)
  // android code here
#endif
#if defined(__unix__)
  // X11
  Display* disp;
  int dpi;
  disp=XOpenDisplay(NULL);
  if (disp!=NULL) {
    dpi=(int)(0.5+(25.4*(double)XDisplayWidth(disp,XDefaultScreen(disp))/(double)XDisplayWidthMM(disp,XDefaultScreen(disp))));
    XCloseDisplay(disp);
    return (double)dpi/96.0;
  }
#endif
  // assume 1
  return 1;
}


bool reservedevent[32]={0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0};
int newperiod[32]={0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0};
int oldperiod[32]={0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0};
bool offinstead[32]={0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0};
bool midion[32]={0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0,
       0,0,0,0,0,0,0,0};
       #ifdef JACK
int nothing (jack_nframes_t nframes, void *arg) {
#else
  static void nothing(void*  userdata,
  Uint8* stream,
  int    len) {
#endif
  rt1=al_get_time();
#ifdef JACK
  jack_default_audio_sample_t *outl, *outr;
  outl = (float*)jack_port_get_buffer (joutl, nframes);
  outr = (float*)jack_port_get_buffer (joutr, nframes);
  void* port_buf = jack_port_get_buffer(midi, nframes);
  unsigned char* midibuffer;
  jack_midi_clear_buffer(port_buf);
#else
  float *outl, *outr;
  outl=(float*)stream;
  outr=(float*)stream;
  int nframes=len/8;
#endif
      if (ntsc) {
      ASC::interval=(int)(6180000/FPS);
      totalrender=(309000.0f/(float)jacksr)*nframes;
      if (tempo==150) {ASC::interval=103103;}
      } else {
      totalrender=(297500.0f/(float)jacksr)*nframes;
      ASC::interval=(int)(5950000/FPS);
      }
      if (kb[ALLEGRO_KEY_ESCAPE] || (PIR((scrW/2)+21,37,(scrW/2)+61,48,mstate.x,mstate.y) && leftclick)) {ASC::interval=16384;}
      fakeASC::interval=ASC::interval;
      memset(abuf,0,totalrender*2*sizeof(float));
      for (cycle=0;cycle<(int)totalrender;cycle++) {
        for (cycle1=0;cycle1<20;cycle1++) {
#ifndef VBLANK    
                            ASC::currentclock--;
                            if (ASC::currentclock<1) {
#else
                             if (doframe) {
#endif
          
                                    doframe=0;
          if (ntsc) {
          #ifdef FILM
            raster1=(((double)cycle*20)+(double)cycle1)/190.4;
          #else
            raster1=(((double)cycle*20)+(double)cycle1)/190.47619047619047619047619047619;
          #endif
          } else {
            raster1=(((double)cycle*20)+(double)cycle1)/190.4;
          }
            double partialtime=al_get_time();
            ASC::currentclock=ASC::interval;
            for (int ii=0;ii<32;ii++) {
              cshapeprev[ii]=cshape[ii];
            }
            if (playmode>0) {
              Playback();
#ifdef JACK
              for (int iiiii=0;iiiii<32;iiiii++) {
              if (reservedevent[iiiii] && midion[iiiii]) {
                reservedevent[iiiii]=0;
                midibuffer = jack_midi_event_reserve(port_buf, 0, 3);
              midibuffer[2] = cvol[iiiii];  // velocity
              midibuffer[1] = (unsigned char)(((log((((((6203.34-(songdf*2)))/**(1/detunefactor)*/)*(float)oldperiod[iiiii]/14.0)))/log(2.0f))*12.0f)+69.1);
              midibuffer[0] = 0x80+(iiiii%16);  // note on or off
              if (!offinstead[iiiii]) {
              midibuffer = jack_midi_event_reserve(port_buf, 0, 3);
              midibuffer[2] = cvol[iiiii];  // velocity
              midibuffer[1] = (unsigned char)(((log((((((6203.34-(songdf*2)))/**(1/detunefactor)*/)*(float)newperiod[iiiii]/14.0)))/log(2.0f))*12.0f)+69.1);
              midibuffer[0] = 0x90+(iiiii%16);  // note on or off
              } else {offinstead[iiiii]=false;}
              }}
#endif
            }
            else {
              MuteAllChannels();
            }
            sfxpos=playfx(sfxdata[cursfx],sfxpos,chantoplayfx);
            for (int updateindex1=0;updateindex1<32;updateindex1++) {
              if (muted[updateindex1]) { cvol[updateindex1]=0;
                                                    if (updateindex1<(8*soundchips)) {
                                                      chip[updateindex1>>3].chan[updateindex1&7].vol=0;
                                                    }
                                                  }
            }
            LastUsedChannel[7]=LastUsedChannel[6];
            LastUsedChannel[6]=LastUsedChannel[5];
            LastUsedChannel[5]=LastUsedChannel[4];
            LastUsedChannel[4]=LastUsedChannel[3];
            LastUsedChannel[3]=LastUsedChannel[2];
            LastUsedChannel[2]=LastUsedChannel[1];
            LastUsedChannel[1]=LastUsedChannel[0];
            LastUsedChannel[0]=0;
            for (int updateindex2=32;updateindex2>0;updateindex2--) {
              if (cvol[updateindex2-1]!=0) { LastUsedChannel[0]=updateindex2; break; }
            }
            LastUsedChannelMax=0;
            for (int ii=0;ii<8;ii++) {
              LastUsedChannelMax=(LastUsedChannelMax<LastUsedChannel[ii])?(LastUsedChannel[ii]):(LastUsedChannelMax);
            }
            if (ntsc) {
            #ifdef FILM
            raster2=fmod(al_get_time()*50,1)*525;
            #else
            raster2=((((double)cycle*20)+(double)cycle1)/190.47619047619047619047619047619)+((al_get_time()-partialtime)*525);
            #endif
            } else {
            raster2=((((double)cycle*20)+(double)cycle1)/190.4)+((al_get_time()-partialtime)*625);
            }
          }
        }
          float heyl, heyr;
                                  abuf[((cycle%bufsize)*2)]=0;
                                  abuf[((cycle%bufsize)*2)+1]=0;
                                  for (int sci=0; sci<soundchips; sci++) {
                                    chip[sci].NextSample(&heyl,&heyr);
                                    abuf[((cycle%bufsize)*2)]+=heyl;
                                    abuf[((cycle%bufsize)*2)+1]+=heyr;
                                  }

        
        #define fff 0.33631372025095791864295318996109
       if (settings::muffle) {
        muffleb0[0]=muffleb0[0]+fff*(abuf[((cycle%bufsize)*2)]-muffleb0[0]);
        muffleb1[0]=muffleb1[0]+fff*(muffleb0[0]-muffleb1[0]);
        abuf[((cycle%bufsize)*2)]=muffleb1[0];
        
        muffleb0[1]=muffleb0[1]+fff*(abuf[((cycle%bufsize)*2)+1]-muffleb0[1]);
        muffleb1[1]=muffleb1[1]+fff*(muffleb0[1]-muffleb1[1]);
        abuf[((cycle%bufsize)*2)+1]=muffleb1[1];
       }
      }
      audioframecounter++;
      for (int ii=0;ii<nframes;ii++) {
      oscbuf[ii*128/nframes]=abuf[ii*2];
      oscbuf2[ii*128/nframes]=abuf[(ii*2)+1];
      }
#define dointerpolate
  for (int iii=0; iii<nframes; iii++) {
#ifdef dointerpolate
  outl[
#ifdef JACK
  iii
#else
  iii*2
#endif
  ]=interpolatee(abuf[(int)((float)iii*totalrender/(float)nframes)*2],abuf[2+(int)((float)iii*totalrender/(float)nframes)*2],fmod((float)iii*totalrender/(float)nframes,1))/2;
  outr[
#ifdef JACK
  iii
#else
  (iii*2)+1
#endif 
  ]=interpolatee(abuf[1+(int)((float)iii*totalrender/(float)nframes)*2],abuf[3+(int)((float)iii*totalrender/(float)nframes)*2],fmod((float)iii*totalrender/(float)nframes,1))/2;
#else
  outl[
#ifdef JACK
  iii
#else
  iii*2
#endif  
  ]=abuf[(int)((float)iii*totalrender/(float)nframes)*2]/2;
  outr[
#ifdef JACK
  iii
#else
  (iii*2)+1
#endif   
  ]=abuf[1+(int)((float)iii*totalrender/(float)nframes)*2]/2;
#endif
  }
  rt2=al_get_time();
#ifdef JACK
  return 0;      
#endif
}
void initaudio() {
  //cout << "\npreparing audio system... ";
  printf("ok\n");
#ifdef JACK
  const char** jports;
  jclient=jack_client_open("soundtracker",JackNullOption,&jstatus);
  if (jclient == NULL) {
    fprintf (stderr, "jack_client_open() failed, "
       "status = 0x%2.0x\n", jstatus);
    if (jstatus & JackServerFailed) {
      fprintf (stderr, "Unable to connect to JACK server\n");
    }
    //exit (1);
  }
  if (jstatus & JackServerStarted) {
    fprintf (stderr, "JACK server started\n");
  }
  if (jstatus & JackNameNotUnique) {
    fprintf (stderr, "boom\n");
  }
  jack_set_process_callback (jclient, nothing, 0);
  printf ("engine sample rate: %" PRIu32 "\n",
    jack_get_sample_rate (jclient));
  jacksr=jack_get_sample_rate (jclient);
  joutl = jack_port_register (jclient, "outL",
            JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsOutput, 0);
  if (joutl == NULL) {
    fprintf(stderr, "no more JACK ports available\n");
  }
  joutr = jack_port_register (jclient, "outR",
            JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsOutput, 0);
  if (joutr == NULL) {
    fprintf(stderr, "no more JACK ports available\n");
  }

  midi = jack_port_register (jclient, "midi",
            JACK_DEFAULT_MIDI_TYPE,
            JackPortIsOutput, 0);
  if ((midi == NULL)) {
    fprintf(stderr, "no more JACK ports available\n");
  }
#else
      //////////////// SDL CODE HERE ////////////////
      SDL_Init(SDL_INIT_AUDIO);
      sout=new SDL_AudioSpec;
      spout=new SDL_AudioSpec;
      sout->freq=44100;
      sout->format=AUDIO_F32;
      sout->channels=2;
      sout->samples=1024;
      sout->callback=nothing;
      sout->userdata=NULL;
      audioID=SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0,0),0,sout,spout, SDL_AUDIO_ALLOW_ANY_CHANGE);
      jacksr=44100;
#endif
      abuf=new float[65536];
#ifdef JACK
      if (jack_activate (jclient)) {
    fprintf (stderr, "cannot activate client");
    //exit (1);
  }
  jports = jack_get_ports (jclient, NULL, NULL,
        JackPortIsPhysical|JackPortIsInput);
  if (jports == NULL) {
    fprintf(stderr, "no physical playback ports\n");
    //exit (1);
  }
  if (jack_connect (jclient, jack_port_name (joutl), jports[0])) {
    printf("cannot connect output port l\n");
  }
  if (jack_connect (jclient, jack_port_name (joutr), jports[1])) {
    printf("cannot connect output port r\n");
  }
  free (jports);
  
#else
  //////////////// SDL CODE HERE ////////////////
#endif
  printf("done\n");
}

unsigned short bswapu16(unsigned short x) {
  return ((x&0xff)<<8)|((x&0xff00)>>8);
}

ALLEGRO_COLOR getucol(unsigned char thecol) {
  if (thecol<16) {
    if (thecol==8) {return (al_map_rgb(128,128,128));} else {
      if (thecol<8) {al_map_rgb((thecol&1)*192,((thecol&2)>>1)*192,((thecol&3)>>2)*192);}
      else {al_map_rgb((thecol&1)*256,((thecol&2)>>1)*256,((thecol&3)>>2)*256);}
    }} else {
  if (thecol>231) {
    // shade of gray/grey
    return (al_map_rgb(8+(10*(thecol-232)),8+(10*(thecol-232)),8+(10*(thecol-232))));
  }
  else {
    return (al_map_rgb(colorof[((thecol-16)/36)],colorof[((thecol-16)/6)%6],colorof[(thecol-16)%6]));
  }}
  return al_map_rgb(255,255,255);
}
ALLEGRO_COLOR GetFXColor(unsigned char fxval) {
  switch (fxval) {
  case 1: case 20: return getconfigcol(colFXTM); break; // speed control
  case 2: case 3: case 22: case 23: return getconfigcol(colFXSN); break; // song control
  case 4: case 13: case 14: case 18: return getconfigcol(colFXVL); break; // volume control
  case 5: case 6: case 7: case 8: case 11: case 12: case 21: return getconfigcol(colFXPT); break; // pitch control
  case 9: case 10: case 15: case 17: return getconfigcol(colFXNO); break; // note control
  case 19: case 26: return  getconfigcol(colFXSP); break; // special commands
  case 16: case 24: case 25: return getconfigcol(colFXPN); break; // panning commands
  default: return getconfigcol(colFXUK); break; // unknown commands
  }
}
int getpatlen(int thelen) {
  if (patlength[thelen]==0) {return 256;}
  return patlength[thelen];
}

float interpolate(float p1, float p2, float amt) {
  return p1+((p2-p1)*amt);
}
float lengthdir_x(float len,float dir) {
  return len*cos(dir*(ALLEGRO_PI/180));
}
float lengthdir_y(float len,float dir) {
  return len*sin(dir*(ALLEGRO_PI/180));
}
const char* gethnibble(int nval) {
  switch(nval>>4) {
    case 0: return "0"; break;
    case 1: return "1"; break;
    case 2: return "2"; break;
    case 3: return "3"; break;
    case 4: return "4"; break;
    case 5: return "5"; break;
    case 6: return "6"; break;
    case 7: return "7"; break;
    case 8: return "8"; break;
    case 9: return "9"; break;
    case 10: return "A"; break;
    case 11: return "B"; break;
    case 12: return "C"; break;
    case 13: return "D"; break;
    case 14: return "E"; break;
    case 15: return "F"; break;
  }
  return "?";
}
const char* getlnibble(int nval) {
  switch(nval%16) {
    case 0: return "0"; break;
    case 1: return "1"; break;
    case 2: return "2"; break;
    case 3: return "3"; break;
    case 4: return "4"; break;
    case 5: return "5"; break;
    case 6: return "6"; break;
    case 7: return "7"; break;
    case 8: return "8"; break;
    case 9: return "9"; break;
    case 10: return "A"; break;
    case 11: return "B"; break;
    case 12: return "C"; break;
    case 13: return "D"; break;
    case 14: return "E"; break;
    case 15: return "F"; break;
  }
  return "?";
}
const char* getnote(int nval) {
  switch(nval%16) {
    case 0: return ".."; break;
    case 1: return "C-"; break;
    case 2: return "C#"; break;
    case 3: return "D-"; break;
    case 4: return "D#"; break;
    case 5: return "E-"; break;
    case 6: return "F-"; break;
    case 7: return "F#"; break;
    case 8: return "G-"; break;
    case 9: return "G#"; break;
    case 10: return "A-"; break;
    case 11: return "A#"; break;
    case 12: return "B-"; break;
    case 13: return "=="; break;
    case 14: return "~~"; break;
    case 15: return "^^"; break;
  }
  return "?";
}
const char* getnotetransp(int nval) {
  switch(nval%12) {
    case 0: return "C-"; break;
    case 1: return "C#"; break;
    case 2: return "D-"; break;
    case 3: return "D#"; break;
    case 4: return "E-"; break;
    case 5: return "F-"; break;
    case 6: return "F#"; break;
    case 7: return "G-"; break;
    case 8: return "G#"; break;
    case 9: return "A-"; break;
    case 10: return "A#"; break;
    case 11: return "B-"; break;
  }
  return "?-";
}
const char* getoctavetransp(int nval) {
  switch(nval/12) {
    case 0: return "0"; break;
    case 1: return "1"; break;
    case 2: return "2"; break;
    case 3: return "3"; break;
    case 4: return "4"; break;
    case 5: return "5"; break;
    case 6: return "6"; break;
    case 7: return "7"; break;
    case 8: return "8"; break;
    case 9: return "9"; break;
    case 10: return "I"; break;
    case 11: return "I"; break;
    case 12: return "I"; break;
    case 13: return "I"; break;
    case 14: return "I"; break;
    case 15: return "I"; break;
  }
  return "?";
}
const char* getoctave(int nval) {
  if ((nval%16)==0) {return ".";}
  if (nval>12) {
    switch(nval%16) {
      case 13: return "="; break;
      case 14: return "~"; break;
      case 15: return "^"; break;
    }
  }
  switch(nval/16) {
    case 0: return "0"; break;
    case 1: return "1"; break;
    case 2: return "2"; break;
    case 3: return "3"; break;
    case 4: return "4"; break;
    case 5: return "5"; break;
    case 6: return "6"; break;
    case 7: return "7"; break;
    case 8: return "8"; break;
    case 9: return "9"; break;
    case 10: return "I"; break;
    case 11: return "I"; break;
    case 12: return "I"; break;
    case 13: return "I"; break;
    case 14: return "I"; break;
    case 15: return "I"; break;
  }
  return "?";
}
const char* getinsL(int nval) {
  if (nval==0) {return ".";}
  else{return getlnibble(nval);}
}
const char* getinsH(int nval) {
  if (nval==0) {return ".";}
  else{return gethnibble(nval);}
}
const char* getVFX(int fxval) {
  if (fxval==0) {return ".";} // 0
  if (fxval<128 && fxval>63) {return "v";} // 64-127
  if (fxval<193 && fxval>127) {return "p";} // 128-192
  switch((fxval-1)/10) {
  case 0: return "a"; break; // 1-10
  case 1: return "b"; break; // 11-20
  case 2: return "c"; break; // 21-30
  case 3: return "d"; break; // 31-40
  case 4: return "e"; break; // 41-50
  case 5: return "f"; break; // 51-60
  }
  switch((fxval-193)/10) {
  case 0: return "g"; break; // 193-202
  case 1: return "h"; break; // 203-212
  case 2: return "o"; break; // 213-222
  }
  return "?";
}
unsigned char getVFXval(int nval) {
  if (nval==0) {return 255;} // 0
  if (nval<128 && nval>63) {return nval-64;} // 64-127
  if (nval<193 && nval>127) {return nval-128;} // 128-192
  switch((nval-1)/10) {
  case 0: return nval-1; break; // 1-10
  case 1: return nval-11; break; // 11-20
  case 2: return nval-21; break; // 21-30
  case 3: return nval-31; break; // 31-40
  case 4: return nval-41; break; // 41-50
  case 5: return nval-51; break; // 51-60
  }
  switch((nval-193)/10) {
  case 0: return nval-193; break; // 193-202
  case 1: return nval-203; break; // 203-212
  case 2: return nval-213; break; // 213-222
  }
  return 254;
}
const char* getVFXL(int nval) {
  if (nval==0) {return ".";}
  else{return getlnibble(nval&0x3f);}
}
const char* getVFXH(int nval) {
  if (nval==0) {return ".";}
  else{return gethnibble(nval&0x3f);}
}
const char* getFX(int fxval) {
  switch(fxval) {
  case 0: return "."; break;
  case 1: return "A"; break;
  case 2: return "B"; break;
  case 3: return "C"; break;
  case 4: return "D"; break;
  case 5: return "E"; break;
  case 6: return "F"; break;
  case 7: return "G"; break;
  case 8: return "H"; break;
  case 9: return "I"; break;
  case 10: return "J"; break;
  case 11: return "K"; break;
  case 12: return "L"; break;
  case 13: return "M"; break;
  case 14: return "N"; break;
  case 15: return "O"; break;
  case 16: return "P"; break;
  case 17: return "Q"; break;
  case 18: return "R"; break;
  case 19: return "S"; break;
  case 20: return "T"; break;
  case 21: return "U"; break;
  case 22: return "V"; break;
  case 23: return "W"; break;
  case 24: return "X"; break;
  case 25: return "Y"; break;
  case 26: return "Z"; break;
  }
  return "?";
}
const char* getFX_PT(int fxval) {
  switch(fxval) {
  case 0: return "."; break;
  case 1: return "F"; break;
  case 2: return "B"; break;
  case 3: return "D"; break;
  case 4: return "A"; break;
  case 5: return "2"; break;
  case 6: return "1"; break;
  case 7: return "3"; break;
  case 8: return "4"; break;
  case 9: return "T"; break;
  case 10: return "0"; break;
  case 11: return "6"; break;
  case 12: return "5"; break;
  case 13: return "M"; break;
  case 14: return "N"; break;
  case 15: return "9"; break;
  case 16: return "P"; break;
  case 17: return "R"; break;
  case 18: return "7"; break;
  case 19: return "E"; break;
  case 20: return "f"; break;
  case 21: return "U"; break;
  case 22: return "G"; break;
  case 23: return "H"; break;
  case 24: return "8"; break;
  case 25: return "Y"; break;
  case 26: return "Z"; break;
  }
  return "?";
}
int hscale(int nval) {
  // playback to storage note converter
  return ((nval/12)*16)+(nval%12)+1;
}
int mscale(int nval) {
  // storage to playback note converter
  return ((nval/16)*12)+((nval-1)%16);
}
ALLEGRO_COLOR getmixerposcol(int channel,int envid) {
  if (!EnvelopesRunning[channel][envid]) {return al_map_rgb(128,128,128);}
  if (inspos[channel][envid]>=bytable[envid][instrument[Mins[channel]].env[envid]][254]) {return al_map_rgb(255,255,0);}
  if (IRP[channel][envid] && released[channel]) {return al_map_rgb(0,255,0);}
  if (released[channel]) {return al_map_rgb(255,0,0);}
  if (IRP[channel][envid]) {return al_map_rgb(0,0,255);}
  return al_map_rgb(0,255,255);
}

int noteperiod(unsigned char note) {
  return 300000/(440*(pow(2.0f,(float)((hscale(note)-57)/12))));
}

int mnoteperiod(float note, int chan) {
  return (int)((6203.34-(songdf*2))*(pow(2.0f,(float)(((float)note-58)/12.0f))));
}

int msnoteperiod(float note, int chan) {
  return ((297500+(songdf*100))/(440*(pow(2.0f,(float)(((float)note-58)/12)))));
}

int AllocateSequence(int seqid) {
  bool nonfree[256];
  // finds the next free sequence for seqid
  for (int usedindex=0;usedindex<256;usedindex++) {
    nonfree[usedindex]=false;
  }
  for (int usedindex=0;usedindex<256;usedindex++) {
    // finds what sequences are used
    nonfree[instrument[usedindex].env[seqid]]=true;
  }
  for (int usedindex=0;usedindex<256;usedindex++) {
    // finds what sequences don't have their default values
    for (int checkpos=0;checkpos<253;checkpos++) {
      if (bytable[seqid][usedindex][checkpos]!=0) {nonfree[usedindex]=true;break;}
    }
    if (bytable[seqid][usedindex][253]!=0) {
      nonfree[usedindex]=true;
    }
    if (bytable[seqid][usedindex][254]!=255) {
      nonfree[usedindex]=true;
    }
    if (bytable[seqid][usedindex][255]!=255) {
      nonfree[usedindex]=true;
    }
  }
  for (int usedindex=0;usedindex<256;usedindex++) {
    // using the previous variables, find the first free envelope
    if (!nonfree[usedindex]) {return usedindex;}
  }
  printf("could not allocate sequence! %d\n",seqid); 
  #ifdef SOUNDS
  cursfx=1;sfxpos=0;
  #endif
  return 0;
}
int ProcessPitch(int insnumb, float offset) {
  // output (pitch logic)
  if (((instrument[Mins[insnumb]].activeEnv&32)>>5)) {
    if (bytable[5][instrument[Mins[insnumb]].env[envPitch]][inspos[insnumb][5]]<0x40) {
    return mnoteperiod(curnote[insnumb]+(bytable[5][instrument[Mins[insnumb]].env[envPitch]][inspos[insnumb][5]])+(((unsigned char)instrument[Mins[insnumb]].noteOffset)-47)+((float)finepitch[insnumb]/64)+offset,insnumb);
    } else {
    if (bytable[5][instrument[Mins[insnumb]].env[envPitch]][inspos[insnumb][5]]<0x80) {
    return mnoteperiod(curnote[insnumb]-((bytable[5][instrument[Mins[insnumb]].env[envPitch]][inspos[insnumb][5]]-64))+(((unsigned char)instrument[Mins[insnumb]].noteOffset)-47)+((float)finepitch[insnumb]/64)+offset,insnumb);
    } else {
    return mnoteperiod(bytable[5][instrument[Mins[insnumb]].env[envPitch]][inspos[insnumb][5]]-128+((float)finepitch[insnumb]/64)+offset,insnumb);
    }}
  }
  else {return mnoteperiod(curnote[insnumb]+(((unsigned char)instrument[Mins[insnumb]].noteOffset)-47)+((float)finepitch[insnumb]/64)+offset,insnumb);}
}
void Zxx(unsigned char value) {
  // process Zxx effect
  if (value>0x7f) {
  switch(value) {
    //case 
  }
  }
}
int FreeChannel() {
  // returns which channel is free
  // routine is similar to IT's next free channel routine
  // 1. find first inactive channel
  // failed? 2. find first channel with zero volume
  for (int ii=0;ii<32;ii++) {
    if (cvol[ii]==0) {return ii;}
  }
  // failed? 3. find channel with lowest volume
  int candidate=0;
  for (int ii=1;ii<32;ii++) {
    if (cvol[candidate]>cvol[ii]) {candidate=ii;}
  }
  return candidate;
}
void NextRow() {
  //// PROCESS NEXT ROW ////
  // forward code
  if (!reversemode) {
  // increase step counter
  curstep++;
  // did we reach end of pattern?
  if (curstep>(getpatlen(patid[curpat])-1)) {
       // set current step to 0 and go to next pattern
       curstep=0; 
       curpat++;
       // reset pattern loop stuff
       for (int ii=0;ii<32;ii++) {
         plcount[ii]=0;
         plpos[ii]=0;
       }
       // did we reach end of song? if yes then restart song
       if (curpat>songlength) {curpat=0;}
  }
  } else {
    // backward code
    curstep--;
    // did we reach start of pattern?
    if (curstep<0) {
      // are we NOT at the beginning of the song?
      if (curpat!=0) {
        // set current step to 0 and go to next pattern
        curstep=getpatlen(patid[curpat-1])-1; 
        curpat--;
        // reset pattern loop stuff
        for (int ii=0;ii<32;ii++) {
          plcount[ii]=0;
          plpos[ii]=0;
        }
      } else {
        curstep=-1;
      }
      // did we reach end of song? if yes then restart song
      if (curpat>songlength) {curpat=0;}
    }
  }
  // MAKE SURE PATTERNS ARE UPDATED
  UPDATEPATTERNS=true;
  // next note and instrument
  unsigned char nnote;
  unsigned char nnnote;
  unsigned char ninst;
  unsigned char SEMINOTE;
  finedelay=0; // S6x value
  // run playback routine over 32 channels
  for (int loop=0;loop<32;loop++) {
    // get next row variables
    nnote=pat[patid[curpat]][curstep][loop][0]; // finds out next note
    if (curstep!=(getpatlen(patid[curpat])-1)) {
    nnnote=pat[patid[curpat]][curstep+1][loop][0]; // finds out next note past next note
    } else {nnnote=0;}
    if ((nnnote%16)!=0 && (nnnote%16)!=15 && (nnnote%16)!=14 && (nnnote%16)!=13 && ((instrument[Mins[loop]].flags>>6)!=0)) {
      cutcount[loop]=speed-(instrument[Mins[loop]].flags>>6);
    }
    ninst=pat[patid[curpat]][curstep][loop][1]; // finds out next instrument
    nvolu[loop]=pat[patid[curpat]][curstep][loop][2]; // finds out next volume value
    // is there a note and instrument, but no volume value? assume v3f
    if ((nnote%16)!=0 && (nnote%16)!=15 && (nnote%16)!=14 && (nnote%16)!=13 && nvolu[loop]==0 && ninst!=0) {nvolu[loop]=0x7f;}
    nfxid[loop]=pat[patid[curpat]][curstep][loop][3]; // finds out next effect
    nfxvl[loop]=pat[patid[curpat]][curstep][loop][4]; // finds out next effect value
    // Txx
    if (nfxid[loop]==20)
    {if (nfxvl[loop]!=0)
    {if (!tempolock) {tempo=maxval(31,nfxvl[loop]);FPS=(double)tempo/2.5;}
    }}
    // volume value set?
    if (nvolu[loop]!=0 && nvolu[loop]>63 && nvolu[loop]<128 && !(nfxid[loop]==19 && (nfxvl[loop]>>4)==0x0d)) {
      // set note volume
      Mvol[loop]=(nvolu[loop]%64)*2;
      // volume envelope enabled?
      if (instrument[Mins[loop]].activeEnv&1) {
      // set current volume to (volume in envelope)*(note volume/127)
      // else set current volume to note volume
      cvol[loop]=((bytable[0][instrument[Mins[loop]].env[envVolume]][inspos[loop][0]]/2)*(Mvol[loop]))/127;
      EnvelopesRunning[loop][0]=true;}
      else {EnvelopesRunning[loop][0]=false;cvol[loop]=Mvol[loop];}
    }
    // is there a new instrument? if yes then reset volume
    if (ninst!=0 && (nnote%16)!=15 && (nnote%16)!=14 && (nnote%16)!=13) {
      if (nvolu[loop]==0) {
        if (EnvelopesRunning[loop][0]) {Mvol[loop]=126;} else {cvol[loop]=126;}}
    }
    if (ninst!=0) {
      Mins[loop]=ninst;
    }
    if ((nnote%16)==15) {
      Mvol[loop]=0;
      cvol[loop]=0;
      EnvelopesRunning[loop][0]=false;EnvelopesRunning[loop][1]=false;
      EnvelopesRunning[loop][2]=false;EnvelopesRunning[loop][3]=false;
      EnvelopesRunning[loop][4]=false;EnvelopesRunning[loop][5]=false;
      EnvelopesRunning[loop][6]=false;EnvelopesRunning[loop][7]=false;
      reservedevent[loop]=1; offinstead[loop]=1;
      continue;
    }
    // is there SDx? if yes then don't trigger note
    // is there a new note?
    if ((nnote%16)!=0 && !(nfxid[loop]==19 && (nfxvl[loop]>>4)==0x0d)) {
      // is new note a release note? set released flag to true and exit if yes
      if ((nnote%16)==13) {released[loop]=true; reservedevent[loop]=1; offinstead[loop]=1; continue;}
      // is new note a NOTE, and effect isn't Gxx/Lxx/gxx?
      if ((nnote%16)!=13 && (nnote%16)!=14 && (nnote%16)!=15 && nfxid[loop]!=7 && nfxid[loop]!=12 && !(nvolu[loop]>192 && nvolu[loop]<203)) {
      // set current note
      curnote[loop]=mscale(nnote);
      }
      // reset the "release" flags
      // this won't happen if note is a release note
      released[loop]=0;
      // is there a new instrument value, along with the new note? if yes then change instrument
      if (ninst!=0) {
      Mins[loop]=ninst;
      }
      // is there a new volume value, along with the new note?
      if (nvolu[loop]!=0 && nvolu[loop]>63 && nvolu[loop]<128) {
      // set note volume
      Mvol[loop]=(nvolu[loop]%64)*2;
      } else {if ((nnote%16)!=15 && ninst!=0) {Mvol[loop]=126;}} // no volume, assuming v3f if not a note release
      // set seminote
      SEMINOTE=((nnote%16)+((nnote>>4)*12));
      // set portamento note
      portastatic[loop]=SEMINOTE;
      // reset all envelope cursors if effect isn't Gxx/Lxx/gxx
      if (nfxid[loop]!=7 && nfxid[loop]!=12 && !(nvolu[loop]>192 && nvolu[loop]<203)) {
      // is it a pcm instrument? (pcm check)
      if (instrument[Mins[loop]].DFM&8) {
        // set channel mode to PCM
        chip[0].chan[loop].flags.pcm=1;
                                //chip[0].pcmmult[loop]=127;
                                chip[0].chan[loop].flags.pcmloop=instrument[Mins[loop]].pcmMult>>7;
        //chip[0].pcmmult[loop]|=(instrument[Mins[loop]][0x21]&128);
        chip[0].chan[loop].pcmrst=*(unsigned short*)instrument[Mins[loop]].pcmPos+*(unsigned short*)instrument[Mins[loop]].pcmLoop;
        // set respective PCM pointers
        chip[0].chan[loop].pcmpos=*(unsigned short*)instrument[Mins[loop]].pcmPos;
        chip[0].chan[loop].pcmbnd=*(unsigned short*)instrument[Mins[loop]].pcmPos+instrument[Mins[loop]].pcmLen;
      } else {chip[0].chan[loop].flags.pcm=0;}
      // is ringmod on? (ringmod check)
      if (instrument[Mins[loop]].DFM&16) {
        // set ring modulation to on
        crm[loop]=1;
        crmduty[loop]=63;
        crmfreq[loop]=msnoteperiod(SEMINOTE+((instrument[Mins[loop]].LFO<0x40)?(instrument[Mins[loop]].LFO):(64-instrument[Mins[loop]].LFO)),loop);
        crrmstep[loop]=0;
        // is sync on?
        if (instrument[Mins[loop]].flags&32) {crm[loop]=2;}
      } else {crm[loop]=0;}
      // is oscreset on? (osc reset check)
      if (instrument[Mins[loop]].flags&1) {crstep[loop]=0;} // osc reset
      // volume (if turned on and effect isn't S77, or effect is S78)
      if (((instrument[Mins[loop]].activeEnv&1) || ((nfxid[loop]==19) && (nfxvl[loop]==0x78))) && !((nfxid[loop]==19) && (nfxvl[loop]==0x77))) {
      cvol[loop]=((bytable[0][instrument[Mins[loop]].env[envVolume]][0]/2)*(Mvol[loop]))/127;
      EnvelopesRunning[loop][0]=true;}
      else {EnvelopesRunning[loop][0]=false;cvol[loop]=Mvol[loop];}
      // duty
      if ((instrument[Mins[loop]].activeEnv&8)>>3) {
      cduty[loop]=bytable[3][instrument[Mins[loop]].env[envDuty]][0]/2;
      EnvelopesRunning[loop][3]=true;}
      else {EnvelopesRunning[loop][3]=false;cduty[loop]=63;}
      // shape
      if ((instrument[Mins[loop]].activeEnv&16)>>4) {
      cshape[loop]=bytable[4][instrument[Mins[loop]].env[envShape]][0]/32;
      EnvelopesRunning[loop][4]=true;}
      else {EnvelopesRunning[loop][4]=false;cshape[loop]=0;}
      // cutoff
      if ((instrument[Mins[loop]].activeEnv&2)>>1) { 
      if (nfxid[loop]!=15) {
      coff[loop]=(bytable[1][instrument[Mins[loop]].env[envCutoff]][0]*(0xffff-instrument[Mins[loop]].filterH))>>7;cfmode[loop]=instrument[Mins[loop]].DFM&7;
      } else {
      coff[loop]=(bytable[1][instrument[Mins[loop]].env[envCutoff]][(int)((float)bytable[1][instrument[Mins[loop]].env[envCutoff]][253]*((float)nfxvl[loop]/256))]*(0xffff-instrument[Mins[loop]].filterH))>>7;
      cfmode[loop]=instrument[Mins[loop]].DFM&7;
      //coff[loop]=bytable[1][instrument[Mins[loop]].env[envCutoff]][(int)((float)bytable[1][instrument[Mins[loop]].env[envCutoff]][253]*((float)nfxvl[loop]/256))]*512;cfmode[loop]=instrument[Mins[loop]].DFM&7;
      }
      EnvelopesRunning[loop][1]=true;}
      else {EnvelopesRunning[loop][1]=false;coff[loop]=262144;cfmode[loop]=fNone;}
      // resonance
      if ((instrument[Mins[loop]].activeEnv&4)>>2) {
      creso[loop]=bytable[2][instrument[Mins[loop]].env[envReson]][0];
      EnvelopesRunning[loop][2]=true;}
      else {EnvelopesRunning[loop][2]=false;creso[loop]=48;finepitch[loop]=0;}
      // panning
      if ((instrument[Mins[loop]].activeEnv&128)>>7) {
      cpan[loop]=bytable[7][instrument[Mins[loop]].env[envPan]][0]-128;
      EnvelopesRunning[loop][7]=true;}
      else {EnvelopesRunning[loop][7]=false;
        cpan[loop]=chanpan[loop];
      }
      // finepitch
      if ((instrument[Mins[loop]].activeEnv&64)>>6) {
      finepitch[loop]=(char)bytable[6][instrument[Mins[loop]].env[envHiPitch]][0];
      cfreq[loop]=ProcessPitch(loop,0);
      EnvelopesRunning[loop][6]=true;}
      else {EnvelopesRunning[loop][6]=false;finepitch[loop]=0;}
      // pitch
      if ((instrument[Mins[loop]].activeEnv&32)>>5) {
      // output (pitch logic)
      if (bytable[5][instrument[Mins[loop]].env[envPitch]][0]<0x40) {
      cfreq[loop]=mnoteperiod(curnote[loop]+(bytable[5][instrument[Mins[loop]].env[envPitch]][0])+(((unsigned char)instrument[Mins[loop]].noteOffset)-47),loop);
      } else {
      if (bytable[5][instrument[Mins[loop]].env[envPitch]][0]<0x80) {
      cfreq[loop]=mnoteperiod(curnote[loop]-((bytable[5][instrument[Mins[loop]].env[envPitch]][0]-64))+(((unsigned char)instrument[Mins[loop]].noteOffset)-47),loop);
      } else {
      cfreq[loop]=mnoteperiod(bytable[5][instrument[Mins[loop]].env[envPitch]][0]-128,loop);
      }}
      EnvelopesRunning[loop][5]=true;}
      else {EnvelopesRunning[loop][5]=false;}
      }
      // is it a note cut? if yes, set volume to 0
      if ((nnote%16)==15) {Mvol[loop]=0;} else 
      {
        // set note frequency if effect isn't Gxx/Lxx/gxx and pitch/finepitch envelopes are disabled
      if (nfxid[loop]!=7 && nfxid[loop]!=12 && !(nvolu[loop]>192 && nvolu[loop]<203) && !((instrument[Mins[loop]].activeEnv&32)>>5) && !((instrument[Mins[loop]].activeEnv&64)>>6)) {
      SEMINOTE=((nnote%16)+((nnote>>4)*12))+(((unsigned char)instrument[Mins[loop]].noteOffset)-48);
      cfreq[loop]=mnoteperiod(SEMINOTE,loop); // sets the frequency to match the current note and applies instrument transposition
      oldperiod[loop]=newperiod[loop];
      newperiod[loop]=cfreq[loop];
      }
      }
      // only reset envelope positions if effect isn't Gxx/Lxx/gxx
      if (nfxid[loop]!=7 && nfxid[loop]!=12 && !(nvolu[loop]>192 && nvolu[loop]<203)) {
      // check for Oxx effect
      if (nfxid[loop]!=15) {
      inspos[loop][0]=0; // sets the instrument position to 0
      inspos[loop][1]=0; // sets the instrument position to 0
      inspos[loop][2]=0; // sets the instrument position to 0
      inspos[loop][3]=0; // sets the instrument position to 0
      inspos[loop][4]=0; // sets the instrument position to 0
      inspos[loop][5]=0; // sets the instrument position to 0
      inspos[loop][6]=0; // sets the instrument position to 0
      inspos[loop][7]=0; // sets the instrument position to 0
      } else { // Oxx
      inspos[loop][0]=nfxvl[loop]; // sets the instrument position to 0
      inspos[loop][1]=(int)((float)bytable[1][instrument[Mins[loop]].env[envCutoff]][253]*((float)nfxvl[loop]/256)); // sets the instrument position to 0
      inspos[loop][2]=nfxvl[loop]; // sets the instrument position to 0
      inspos[loop][3]=nfxvl[loop]; // sets the instrument position to 0
      inspos[loop][4]=0; // sets the instrument position to 0
      inspos[loop][5]=0; // sets the instrument position to 0
      inspos[loop][6]=0; // sets the instrument position to 0
      inspos[loop][7]=nfxvl[loop]; // sets the instrument position to 0
      }
      // set In-Release-Point (IRP) flags to false
      IRP[loop][0]=0;
      IRP[loop][1]=0;
      IRP[loop][2]=0;
      IRP[loop][3]=0;
      IRP[loop][4]=0;
      IRP[loop][5]=0;
      IRP[loop][6]=0;
      IRP[loop][7]=0;
      // note is not released yet
      released[loop]=false;
      reservedevent[loop]=1;
      // reset vibrato position
      curvibpos[loop]=0;
      }
      // Qxx
      if (nfxid[loop]==17)
      {if (nfxvl[loop]!=0) {doretrigger[loop]=true;retrigger[loop]=nfxvl[loop];retrig[loop]=(retrigger[loop]%16);}
      else {retrig[loop]=(retrigger[loop]%16);}}
    } else {
      if (nfxid[loop]==19 && (nfxvl[loop]>>4)==0x0d) {
        curnote[loop]=mscale(nnote);
        doretrigger[loop]=true;retrigger[loop]=128+(nfxvl[loop]%16);retrig[loop]=maxval(1,retrigger[loop]%16);
      }
    }
  }
  // effect checks
  for (int looper=0;looper<32;looper++) {
  // Axx
  if (nfxid[looper]==1)
    {if (nfxvl[looper]!=0 && !speedlock)
    {speed=nfxvl[looper];}}
  // Dfx - Dxf - Kfx - Kxf - Lfx - Lxf
  if (nfxid[looper]==4 || nfxid[looper]==11 || nfxid[looper]==12)
    {if (nfxvl[looper]!=0) {slidememory[looper]=nfxvl[looper];}
    if (EnvelopesRunning[looper][0]) {
    if (slidememory[looper]>0xf0) {Mvol[looper]-=((slidememory[looper]%16)*2);
    if (Mvol[looper]<0) {Mvol[looper]=0;}} else {if (slidememory[looper]%16==0x0f && slidememory[looper]!=0x0f) {Mvol[looper]+=((slidememory[looper]>>4)*2);if (Mvol[looper]>127) {Mvol[looper]=127;}}}}
    else {
    if (slidememory[looper]>0xf0) {cvol[looper]-=((slidememory[looper]%16)*2);
    if (cvol[looper]<0) {cvol[looper]=0;}} else {if (slidememory[looper]%16==0x0f && slidememory[looper]!=0x0f) {cvol[looper]+=((slidememory[looper]>>4)*2);if (cvol[looper]>127) {cvol[looper]=127;}}}}
    }
  // Nfx - Nxf
  if (nfxid[looper]==14)
    {if (nfxvl[looper]!=0) {chanslidememory[looper]=nfxvl[looper];}
    if (chanslidememory[looper]>0xf0) {chanvol[looper]-=((chanslidememory[looper]%16)*2);
    if (chanvol[looper]<0) {chanvol[looper]=0;}} else {if (chanslidememory[looper]%16==0x0f && chanslidememory[looper]!=0x0f) {chanvol[looper]+=((chanslidememory[looper]>>4)*2);if (chanvol[looper]>127) {chanvol[looper]=127;}}}
    }
  // Efx - Eex
  if (nfxid[looper]==5)
    {if (nfxvl[looper]>0xef) {curnote[looper]-=((float)(nfxvl[looper]%16)/16);
      if (!EnvelopesRunning[looper][5]) {
            cfreq[looper]=mnoteperiod(curnote[looper]+((unsigned char)instrument[Mins[looper]].noteOffset)-47,looper)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
            } else {
            cfreq[looper]=ProcessPitch(looper,0)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
            }} else {
      if (nfxvl[looper]>0xdf) {curnote[looper]-=((float)(nfxvl[looper]%16)/64);
          if (!EnvelopesRunning[looper][5]) {
            cfreq[looper]=mnoteperiod(curnote[looper]+((unsigned char)instrument[Mins[looper]].noteOffset)-47,looper)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
            } else {
            cfreq[looper]=ProcessPitch(looper,0)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
            }}
    }}
  // Ffx - Fex
  if (nfxid[looper]==6)
    {if (nfxvl[looper]>0xef) {curnote[looper]+=((float)(nfxvl[looper]%16)/16);
      if (!EnvelopesRunning[looper][5]) {
            cfreq[looper]=mnoteperiod(curnote[looper]+((unsigned char)instrument[Mins[looper]].noteOffset)-47,looper)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
            } else {
            cfreq[looper]=ProcessPitch(looper,0)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
            }} else {
      if (nfxvl[looper]>0xdf) {curnote[looper]+=((float)(nfxvl[looper]%16)/64);
          if (!EnvelopesRunning[looper][5]) {
            cfreq[looper]=mnoteperiod(curnote[looper]+((unsigned char)instrument[Mins[looper]].noteOffset)-47,looper)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
            } else {
            cfreq[looper]=ProcessPitch(looper,0)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
            }}
    }}
  // Mxx
  if (nfxid[looper]==13)
  {chanvol[looper]=minval(nfxvl[looper]*2,128);}
  // Vxx
  if (nfxid[looper]==22)
  {cglobvol=minval(nfxvl[looper],128);}
  // Qxx
  if (nfxid[looper]==17)
  {if (nfxvl[looper]!=0) {if (nfxvl[looper]%16!=retrigger[looper]%16) {
    if (retrig[looper]==(retrigger[looper]%16)) {retrig[looper]=(nfxvl[looper]%16);}
    retrigger[looper]=nfxvl[looper];}}} else {
    if (!(nfxid[looper]==19 && (nfxvl[looper]>>4)==0x0d)) {retrig[looper]=0;}
  }
  // Sxx
  if (nfxid[looper]==19)
    {
    // S3x
    if ((nfxvl[looper]>>4)==0x03) {
      curvibshape[looper]=nfxvl[looper]%4;
    }
    // S8x
    if ((nfxvl[looper]>>4)==0x08)
      {cpan[looper]=(((nfxvl[looper]%16)*16)+(nfxvl[looper]%16))-128;
       chanpan[looper]=(((nfxvl[looper]%16)*16)+(nfxvl[looper]%16))-128;}
    // SEx
    if ((nfxvl[looper]>>4)==0x0e) {finedelay+=(nfxvl[looper]%16)*speed;}
    // S6x
    if ((nfxvl[looper]>>4)==0x06) {finedelay+=(nfxvl[looper]%16);}
    // SCx
    if ((nfxvl[looper]>>4)==0x0c) {
      if ((nfxvl[looper]%16)<speed) {
      cutcount[looper]=maxval(1,nfxvl[looper]%16);
      }
    }
    // SBx
    if ((nfxvl[looper]>>4)==0x0b) {
      if ((nfxvl[looper]%16)==0) {plpos[looper]=curstep;} else {
        if (plcount[looper]<(nfxvl[looper]%16)) {
          plcount[looper]++;
          curstep=plpos[looper]-1;
        } else {plcount[looper]=0;}
      }}}
  // Xxx
  if (nfxid[looper]==24)
    {cpan[looper]=nfxvl[looper]-128;
     chanpan[looper]=nfxvl[looper]-128;}
  // Zxx
  if (nfxid[looper]==26)
    {Zxx(nfxvl[looper]);}
  }
  curtick=speed+finedelay; // sets the current tick to the song speed
  //curstep++; // increases the step counter
  // did we reach end of pattern?
  /*if (curstep>(getpatlen(patid[curpat])-1)) { // yes
       curstep=0; // set current step to 0
       curpat++; // increases current pattern
       for (int ii=0;ii<32;ii++) {
         plcount[ii]=0;
         plpos[ii]=0;
       }
       if (curpat>songlength) {curpat=0;}
  }*/
}
void SkipPattern(int chanval) {
  if (!AlreadySkipped) {
    AlreadySkipped=true;curpat++;curstep=nfxvl[chanval]-1;NextRow();
  }
}

void UpdateEnvelope(int channel, int envid) {
  if (!IRP[channel][envid] || released[channel] ||
    (bytable[envid][instrument[Mins[channel]].env[envid]][254])<(bytable[envid][instrument[Mins[channel]].env[envid]][255])) {
  inspos[channel][envid]++;
  if (inspos[channel][envid]==bytable[envid][instrument[Mins[channel]].env[envid]][255]) {IRP[channel][envid]=true;}
  }
  if (inspos[channel][envid]>
    ((IRP[channel][envid] && !released[channel])?(minval(252,bytable[envid][instrument[Mins[channel]].env[envid]][255])):(minval(252,bytable[envid][instrument[Mins[channel]].env[envid]][253]))
    )) // if envelope exceeds 252, or the length
    {if ((bytable[envid][instrument[Mins[channel]].env[envid]][254]<252) &&
    (!released[channel])
    ) { // if loop is defined
      inspos[channel][envid]=bytable[envid][instrument[Mins[channel]].env[envid]][254];
  } else {
    if (!((bytable[envid][instrument[Mins[channel]].env[envid]][254]<252) &&
      (bytable[envid][instrument[Mins[channel]].env[envid]][254])>(bytable[envid][instrument[Mins[channel]].env[envid]][255])
      ) && 
      (bytable[envid][instrument[Mins[channel]].env[envid]][255]<252)
      ) {
    // end of envelope
    inspos[channel][envid]=minval(252,bytable[envid][instrument[Mins[channel]].env[envid]][253]);
    EnvelopesRunning[channel][envid]=false;
    } else {
      if ((bytable[envid][instrument[Mins[channel]].env[envid]][254]>252)) {
      // end of envelope
      inspos[channel][envid]=minval(252,bytable[envid][instrument[Mins[channel]].env[envid]][253]);
      EnvelopesRunning[channel][envid]=false;
      } else {
      inspos[channel][envid]=bytable[envid][instrument[Mins[channel]].env[envid]][254];
      }
    }
  }
  }
}

void NextTick() {
  // process the next tick
  curtick--;
  // run envelopes
  for (int loop2=0;loop2<32;loop2++) {
        //crstep[loop2]=0; // no
  // Qxx
  if (doretrigger[loop2])
    {retrig[loop2]--;
     if (retrig[loop2]==0) {retrig[loop2]=retrigger[loop2]%16;
     doretrigger[loop2]=false;
     if (nfxid[loop2]==17) {doretrigger[loop2]=true;} else {
      if (nvolu[loop2]!=0 && nvolu[loop2]>63 && nvolu[loop2]<128) {
      // set note volume
      Mvol[loop2]=(nvolu[loop2]%64)*2;
    }
     }
     if (nfxid[loop2]==17) {
      // process RVCT
      switch (retrigger[loop2]>>4) {
        case 1: Mvol[loop2]-=2; if (Mvol[loop2]<0) {Mvol[loop2]=0;}; break;
        case 2: Mvol[loop2]-=4; if (Mvol[loop2]<0) {Mvol[loop2]=0;}; break;
        case 3: Mvol[loop2]-=8; if (Mvol[loop2]<0) {Mvol[loop2]=0;}; break;
        case 4: Mvol[loop2]-=16; if (Mvol[loop2]<0) {Mvol[loop2]=0;}; break;
        case 5: Mvol[loop2]-=32; if (Mvol[loop2]<0) {Mvol[loop2]=0;}; break;
        case 6: Mvol[loop2]*=2; Mvol[loop2]/=3; if (Mvol[loop2]<0) {Mvol[loop2]=0;}; break;
        case 7: Mvol[loop2]/=2; if (Mvol[loop2]<0) {Mvol[loop2]=0;}; break;
        case 9: Mvol[loop2]+=2; if (Mvol[loop2]>126) {Mvol[loop2]=126;}; break;
        case 10: Mvol[loop2]+=4; if (Mvol[loop2]>126) {Mvol[loop2]=126;}; break;
        case 11: Mvol[loop2]+=8; if (Mvol[loop2]>126) {Mvol[loop2]=126;}; break;
        case 12: Mvol[loop2]+=16; if (Mvol[loop2]>126) {Mvol[loop2]=126;}; break;
        case 13: Mvol[loop2]+=32; if (Mvol[loop2]>126) {Mvol[loop2]=126;}; break;
        case 14: Mvol[loop2]*=3; Mvol[loop2]/=2; if (Mvol[loop2]>126) {Mvol[loop2]=126;}; break;
        case 15: Mvol[loop2]*=2; if (Mvol[loop2]>126) {Mvol[loop2]=126;}; break;
      }
     }
      // reset all envelope cursors if effect isn't Gxx/Lxx/gxx
      if (nfxid[loop2]!=7 && nfxid[loop2]!=12 && !(nvolu[loop2]>192 && nvolu[loop2]<203)) {
      // is there a new instrument value, along with the new note? if yes then change instrument
      if (pat[patid[curpat]][curstep][loop2][1]!=0) {
      Mins[loop2]=pat[patid[curpat]][curstep][loop2][1];
      }
      // is it a pcm instrument? (pcm check)
      if (instrument[Mins[loop2]].DFM&8) {
        // set channel mode to PCM
        chip[0].chan[loop2].flags.pcm=1;
        // set respective PCM pointers
        chip[0].chan[loop2].pcmpos=*(unsigned short*)instrument[Mins[loop2]].pcmPos;
        chip[0].chan[loop2].pcmbnd=*(unsigned short*)instrument[Mins[loop2]].pcmPos+instrument[Mins[loop2]].pcmLen;
      } else {chip[0].chan[loop2].flags.pcm=0;}
      // is oscreset on? (osc reset check)
      if (instrument[Mins[loop2]].flags&1) {crstep[loop2]=0;} // osc reset
      // volume (if turned on and effect isn't S77, or effect is S78)
      if (((instrument[Mins[loop2]].activeEnv&1) || ((nfxid[loop2]==19) && (nfxvl[loop2]==0x78))) && !((nfxid[loop2]==19) && (nfxvl[loop2]==0x77))) {
      cvol[loop2]=((bytable[0][instrument[Mins[loop2]].env[envVolume]][0]/2)*(Mvol[loop2]))/127;
      EnvelopesRunning[loop2][0]=true;}
      else {EnvelopesRunning[loop2][0]=false;cvol[loop2]=Mvol[loop2];}
      // duty
      if ((instrument[Mins[loop2]].activeEnv&8)>>3) {
      cduty[loop2]=bytable[3][instrument[Mins[loop2]].env[envDuty]][0]/2;
      EnvelopesRunning[loop2][3]=true;}
      else {EnvelopesRunning[loop2][3]=false;cduty[loop2]=63;}
      // shape
      if ((instrument[Mins[loop2]].activeEnv&16)>>4) {
      cshape[loop2]=bytable[4][instrument[Mins[loop2]].env[envShape]][0]/32;
      EnvelopesRunning[loop2][4]=true;}
      else {EnvelopesRunning[loop2][4]=false;cshape[loop2]=0;}
      // cutoff
      if ((instrument[Mins[loop2]].activeEnv&2)>>1) {
      if (nfxid[loop2]!=15) {
      coff[loop2]=(bytable[1][instrument[Mins[loop2]].env[envCutoff]][0]*(0xffff-instrument[Mins[loop2]].filterH))>>7;cfmode[loop2]=instrument[Mins[loop2]].DFM&7;
      } else {
      coff[loop2]=(bytable[1][instrument[Mins[loop2]].env[envCutoff]][(int)((float)bytable[1][instrument[Mins[loop2]].env[envCutoff]][253]*((float)nfxvl[loop2]/256))]*(0xffff-instrument[Mins[loop2]].filterH))>>7;
      cfmode[loop2]=instrument[Mins[loop2]].DFM&7;
      }
      EnvelopesRunning[loop2][1]=true;}
      else {EnvelopesRunning[loop2][1]=false;coff[loop2]=262144;cfmode[loop2]=fNone;}
      // resonance
      if ((instrument[Mins[loop2]].activeEnv&4)>>2) {
      creso[loop2]=bytable[2][instrument[Mins[loop2]].env[envReson]][0];
      EnvelopesRunning[loop2][2]=true;}
      else {EnvelopesRunning[loop2][2]=false;creso[loop2]=48;finepitch[loop2]=0;}
      // panning
      if ((instrument[Mins[loop2]].activeEnv&128)>>7) {
      cpan[loop2]=bytable[7][instrument[Mins[loop2]].env[envPan]][0]-128;
      EnvelopesRunning[loop2][7]=true;}
      else {EnvelopesRunning[loop2][7]=false;
        cpan[loop2]=chanpan[loop2];
      }
      // finepitch
      if ((instrument[Mins[loop2]].activeEnv&64)>>6) {
      finepitch[loop2]=(char)bytable[6][instrument[Mins[loop2]].env[envHiPitch]][0];
      cfreq[loop2]=ProcessPitch(loop2,0);
      EnvelopesRunning[loop2][6]=true;}
      else {EnvelopesRunning[loop2][6]=false;finepitch[loop2]=0;}
      // pitch
      if ((instrument[Mins[loop2]].activeEnv&32)>>5) {
      // output (pitch logic)
      if (bytable[5][instrument[Mins[loop2]].env[envPitch]][0]<0x40) {
      cfreq[loop2]=mnoteperiod(curnote[loop2]+(bytable[5][instrument[Mins[loop2]].env[envPitch]][0])+(((unsigned char)instrument[Mins[loop2]].noteOffset)-47),loop2);
      } else {
      if (bytable[5][instrument[Mins[loop2]].env[envPitch]][0]<0x80) {
      cfreq[loop2]=mnoteperiod(curnote[loop2]-((bytable[5][instrument[Mins[loop2]].env[envPitch]][0]-64))+(((unsigned char)instrument[Mins[loop2]].noteOffset)-47),loop2);
      } else {
      cfreq[loop2]=mnoteperiod(bytable[5][instrument[Mins[loop2]].env[envPitch]][0]-128,loop2);
      }}
      EnvelopesRunning[loop2][5]=true;}
      else {EnvelopesRunning[loop2][5]=false;}
      }
      if (nfxid[loop2]!=7 && nfxid[loop2]!=12 && !(nvolu[loop2]>192 && nvolu[loop2]<203) && !((instrument[Mins[loop2]].activeEnv&32)>>5) && !((instrument[Mins[loop2]].activeEnv&64)>>6)) {
      cfreq[loop2]=mnoteperiod(((hscale(curnote[loop2])%16)+((hscale(curnote[loop2])>>4)*12))+(((unsigned char)instrument[Mins[loop2]].noteOffset)-48),loop2);
      }
      // only reset envelope positions if effect isn't Gxx/Lxx/gxx
      if (nfxid[loop2]!=7 && nfxid[loop2]!=12 && !(nvolu[loop2]>192 && nvolu[loop2]<203)) {
      // check for Oxx effect
      if (nfxid[loop2]!=15) {
      inspos[loop2][0]=0; // sets the instrument position to 0
      inspos[loop2][1]=0; // sets the instrument position to 0
      inspos[loop2][2]=0; // sets the instrument position to 0
      inspos[loop2][3]=0; // sets the instrument position to 0
      inspos[loop2][4]=0; // sets the instrument position to 0
      inspos[loop2][5]=0; // sets the instrument position to 0
      inspos[loop2][6]=0; // sets the instrument position to 0
      inspos[loop2][7]=0; // sets the instrument position to 0
      } else { // Oxx
      inspos[loop2][0]=nfxvl[loop2]; // sets the instrument position to 0
      inspos[loop2][1]=(int)((float)bytable[1][instrument[Mins[loop2]].env[envCutoff]][253]*((float)nfxvl[loop2]/256)); // sets the instrument position to 0
      inspos[loop2][2]=nfxvl[loop2]; // sets the instrument position to 0
      inspos[loop2][3]=nfxvl[loop2]; // sets the instrument position to 0
      inspos[loop2][4]=0; // sets the instrument position to 0
      inspos[loop2][5]=0; // sets the instrument position to 0
      inspos[loop2][6]=0; // sets the instrument position to 0
      inspos[loop2][7]=nfxvl[loop2]; // sets the instrument position to 0
      }
      // set In-Release-Point (IRP) flags to false
      IRP[loop2][0]=0;
      IRP[loop2][1]=0;
      IRP[loop2][2]=0;
      IRP[loop2][3]=0;
      IRP[loop2][4]=0;
      IRP[loop2][5]=0;
      IRP[loop2][6]=0;
      IRP[loop2][7]=0;
      // note is not released yet
      released[loop2]=false;
      // reset vibrato position
      curvibpos[loop2]=0;
      }
      continue;
     }}
    // volume
    if (EnvelopesRunning[loop2][0]) {
      UpdateEnvelope(loop2,0);
      cvol[loop2]=((bytable[0][instrument[Mins[loop2]].env[envVolume]][inspos[loop2][0]]/2)*(Mvol[loop2]))/127;
    }
    // duty
    if (EnvelopesRunning[loop2][3]) {
      UpdateEnvelope(loop2,3);
      // output
      cduty[loop2]=bytable[3][instrument[Mins[loop2]].env[envDuty]][inspos[loop2][3]]/2;
    }
    // shape
    if (EnvelopesRunning[loop2][4]) {
      UpdateEnvelope(loop2,4);
      // output
      cshape[loop2]=bytable[4][instrument[Mins[loop2]].env[envShape]][inspos[loop2][4]]/32;
    }
    // cutoff
    if (EnvelopesRunning[loop2][1]) {
      UpdateEnvelope(loop2,1);
      // output
      coff[loop2]=(bytable[1][instrument[Mins[loop2]].env[envCutoff]][inspos[loop2][1]]*(0xffff-instrument[Mins[loop2]].filterH))>>7;
    }
    // resonance
    if (EnvelopesRunning[loop2][2]) {
      UpdateEnvelope(loop2,2);
      // output
      creso[loop2]=bytable[2][instrument[Mins[loop2]].env[envReson]][inspos[loop2][2]];
    }
    // panning
    if (EnvelopesRunning[loop2][7]) {
      UpdateEnvelope(loop2,7);
      // output
      cpan[loop2]=bytable[7][instrument[Mins[loop2]].env[envPan]][inspos[loop2][7]]-128;
    }
    // pitch
    if (EnvelopesRunning[loop2][5]) {
      UpdateEnvelope(loop2,5);
      // output (pitch logic)
      if (bytable[5][instrument[Mins[loop2]].env[envPitch]][inspos[loop2][5]]<0x40) {
      cfreq[loop2]=mnoteperiod(curnote[loop2]+(bytable[5][instrument[Mins[loop2]].env[envPitch]][inspos[loop2][5]])+(((unsigned char)instrument[Mins[loop2]].noteOffset)-47),loop2);
      } else {
      if (bytable[5][instrument[Mins[loop2]].env[envPitch]][inspos[loop2][5]]<0x80) {
      cfreq[loop2]=mnoteperiod(curnote[loop2]-((bytable[5][instrument[Mins[loop2]].env[envPitch]][inspos[loop2][5]]-64))+(((unsigned char)instrument[Mins[loop2]].noteOffset)-47),loop2);
      } else {
      cfreq[loop2]=mnoteperiod(bytable[5][instrument[Mins[loop2]].env[envPitch]][inspos[loop2][5]]-128,loop2);
      }}
    }
    // finepitch
    if (EnvelopesRunning[loop2][6]) {
      UpdateEnvelope(loop2,6);
      // output
      if (EnvelopesRunning[loop2][6]) {
      finepitch[loop2]+=(char)bytable[6][instrument[Mins[loop2]].env[envHiPitch]][inspos[loop2][6]];
      cfreq[loop2]=ProcessPitch(loop2,0);
      }
    }
  }
  // run effects
  for (int looper=0;looper<32;looper++) {
  // Bxx
  if (nfxid[looper]==2)
    {if (curtick==0) {curpat=nfxvl[looper];curstep=-1;NextRow();}}
  // Cxx
  if (nfxid[looper]==3)
    {if (curtick==0) {SkipPattern(looper);}}
  // Dxx
  if (nfxid[looper]==4 || nfxid[looper]==11 || nfxid[looper]==12)
    {if (EnvelopesRunning[looper][0]) {
    if (slidememory[looper]<0x10) {Mvol[looper]-=(slidememory[looper]*2);
    if (Mvol[looper]<0) {Mvol[looper]=0;}} else {if ((slidememory[looper]%16)!=0x0f && slidememory[looper]<0xf1) {Mvol[looper]+=((slidememory[looper]>>4)*2);if (Mvol[looper]>127) {Mvol[looper]=127;}}}}
    else {
    if (slidememory[looper]<0x10) {cvol[looper]-=(slidememory[looper]*2);
    if (cvol[looper]<0) {cvol[looper]=0;}} else {if ((slidememory[looper]%16)!=0x0f && slidememory[looper]<0xf1) {cvol[looper]+=((slidememory[looper]>>4)*2);if (cvol[looper]>127) {cvol[looper]=127;}}}}}
  // Nxx
  if (nfxid[looper]==14)
    {if (chanslidememory[looper]<0x10) {chanvol[looper]-=(chanslidememory[looper]*2);
    if (chanvol[looper]<0) {chanvol[looper]=0;}} else {if ((chanslidememory[looper]%16)!=0x0f && chanslidememory[looper]<0xf1) {chanvol[looper]+=((chanslidememory[looper]>>4)*2);if (chanvol[looper]>127) {chanvol[looper]=127;}}}}
  // Exx
  if (nfxid[looper]==5)
    {if (nfxvl[looper]<0xe0) {
      if (!linearslides) {cfreq[looper]+=nfxvl[looper];}
      else {curnote[looper]-=((float)nfxvl[looper]/16);
          if (!EnvelopesRunning[looper][5]) {
            cfreq[looper]=mnoteperiod(curnote[looper]+((unsigned char)instrument[Mins[looper]].noteOffset)-47,looper)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
            } else {
            cfreq[looper]=ProcessPitch(looper,0)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
            }}}}
  // Fxx
  if (nfxid[looper]==6)
    {if (nfxvl[looper]<0xe0) {
      if (!linearslides) {cfreq[looper]-=nfxvl[looper];}
      else {curnote[looper]+=((float)nfxvl[looper]/16);
          cfreq[looper]=mnoteperiod(curnote[looper]+1,looper);
          if (!EnvelopesRunning[looper][5]) {
            cfreq[looper]=mnoteperiod(curnote[looper]+((unsigned char)instrument[Mins[looper]].noteOffset)-47,looper)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
            } else {
            cfreq[looper]=ProcessPitch(looper,0)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
            }}}}
  // Gxx
  if (nfxid[looper]==7 || nfxid[looper]==12 || (nvolu[looper]>192 && nvolu[looper]<203))
  {if (nfxvl[looper]!=0 && nfxid[looper]!=12 && !(nvolu[looper]>193 && nvolu[looper]<203)) {Mport[looper]=nfxvl[looper];} // portamento memory
  if (nvolu[looper]>193 && nvolu[looper]<203) {
    switch (nvolu[looper]) {
      case 194: Mport[looper]=1; break;
      case 195: Mport[looper]=4; break;
      case 196: Mport[looper]=8; break;
      case 197: Mport[looper]=16; break;
      case 198: Mport[looper]=32; break;
      case 199: Mport[looper]=64; break;
      case 200: Mport[looper]=96; break;
      case 201: Mport[looper]=128; break;
      case 202: Mport[looper]=255; break;
  }}
    if (!linearslides) {
    if (cfreq[looper]>mnoteperiod(portastatic[looper],looper)) {
      cfreq[looper]=maxval(cfreq[looper]-(Mport[looper]*4),mnoteperiod(portastatic[looper],looper));
    } else {if (cfreq[looper]<mnoteperiod(portastatic[looper],looper)) {
      cfreq[looper]=minval(cfreq[looper]+(Mport[looper]*4),mnoteperiod(portastatic[looper],looper));
    }}
    } else {
    if (curnote[looper]>(portastatic[looper])) {
      curnote[looper]=maxval(curnote[looper]-((float)Mport[looper]/16),portastatic[looper]-1);
      cfreq[looper]=ProcessPitch(looper,0);
    } else {if (curnote[looper]<(portastatic[looper])) {
      curnote[looper]=minval(curnote[looper]+((float)Mport[looper]/16),portastatic[looper]-1);
      cfreq[looper]=ProcessPitch(looper,0);
    } else {curnote[looper]=portastatic[looper]-1;
      cfreq[looper]=ProcessPitch(looper,0);}}
    }
  }
  // Hxx
  if (nfxid[looper]==8 || nfxid[looper]==11) {
    if (nfxid[looper]!=11) {
    if ((nfxvl[looper]%16)!=0) {curvibdepth[looper]=nfxvl[looper]%16;}
    if ((nfxvl[looper]/16)!=0) {curvibspeed[looper]=nfxvl[looper]/16;}
    }
    curvibpos[looper]++;
    switch (curvibshape[looper]) {
    case 0: cfreq[looper]=ProcessPitch(looper,-(float)(sin(((float)curvibpos[looper]*(float)curvibspeed[looper]*4/256)*2*ALLEGRO_PI))*((float)curvibdepth[looper]/8)); break; // sine
    case 1: cfreq[looper]=ProcessPitch(looper,0)+
    (int)((float)((((curvibpos[looper]*curvibspeed[looper]*4)%256)/2)-64)*((float)curvibdepth[looper]/16)); break; // saw
    case 2: cfreq[looper]=ProcessPitch(looper,0)+
    (int)((float)((((curvibpos[looper]*curvibspeed[looper]*4)%256)>127)?(64):(-64))*((float)curvibdepth[looper]/16)); break; // square
    case 3: cfreq[looper]=ProcessPitch(looper,0)+
    (int)((float)((rand()%128)-64)*((float)curvibdepth[looper]/16)); break; // random
    }
  }
  // Jxx
  if (nfxid[looper]==10) {
    // cycle between the 3 frequencies
    switch ((speed-curtick)%3) {
      case 0: cfreq[looper]=mnoteperiod(curnote[looper]+((unsigned char)instrument[Mins[looper]].noteOffset)-47,looper); break;
      case 1: cfreq[looper]=mnoteperiod(curnote[looper]+(nfxvl[looper]/16)+(((unsigned char)instrument[Mins[looper]].noteOffset)-47),looper); break;
      case 2: cfreq[looper]=mnoteperiod(curnote[looper]+(nfxvl[looper]%16)+(((unsigned char)instrument[Mins[looper]].noteOffset)-47),looper); break;
    }
  }
  // Kxx
  /*if (nfxid[looper]==11) {
    if (EnvelopesRunning[looper][0]) {
    if (nfxvl[looper]<0x10) {Mvol[looper]-=(nfxvl[looper]*2);
    if (Mvol[looper]<0) {Mvol[looper]=0;}} else {Mvol[looper]+=((nfxvl[looper]>>4)*2);if (Mvol[looper]>127) {Mvol[looper]=127;}}}
    else {
    if (nfxvl[looper]<0x10) {cvol[looper]-=(nfxvl[looper]*2);
    if (cvol[looper]<0) {cvol[looper]=0;}} else {cvol[looper]+=((nfxvl[looper]>>4)*2);if (cvol[looper]>127) {cvol[looper]=127;}}}
    curvibpos[looper]++;
    if (!EnvelopesRunning[looper][5]) {
    cfreq[looper]=mnoteperiod(curnote[looper]+((unsigned char)instrument[Mins[looper]].noteOffset)-47)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
    } else {
    cfreq[looper]=ProcessPitch(looper)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/16));
    }
  }*/
  // SCx
  cutcount[looper]--;
  if (cutcount[looper]==0) {
    if (EnvelopesRunning[looper][0]) {Mvol[looper]=0;} else {cvol[looper]=0;}
  }
  if (cutcount[looper]<-1) {cutcount[looper]=-1;}
  /*if (nfxid[looper]==19)
    {if ((nfxvl[looper]>>4)==0x0c) {cutcount[looper]--;
      if (cutcount[looper]==0) {
        if (EnvelopesRunning[looper][0]) {Mvol[looper]=0;} else {cvol[looper]=0;}
      }}}*/
  // Uxx
  if (nfxid[looper]==21) {
    if ((nfxvl[looper]%16)!=0) {curvibdepth[looper]=nfxvl[looper]%16;}
    if ((nfxvl[looper]/16)!=0) {curvibspeed[looper]=nfxvl[looper]/16;}
    curvibpos[looper]++;
    cfreq[looper]=mnoteperiod(curnote[looper]+((unsigned char)instrument[Mins[looper]].noteOffset)-47,looper)+(int)((float)sine[(curvibpos[looper]*curvibspeed[looper]*4)%256]*((float)curvibdepth[looper]/64));
  }
  // Yxx
  if (nfxid[looper]==25) {
    if ((nfxvl[looper]%16)!=0) {curpandepth[looper]=nfxvl[looper]%16;}
    if ((nfxvl[looper]/16)!=0) {curpanspeed[looper]=nfxvl[looper]/16;}
    curpanpos[looper]++;
    cpan[looper]=(char)((float)sine[(curpanpos[looper]*curpanspeed[looper]*4)%256]*((float)curpandepth[looper]/16));
  }
  }
  AlreadySkipped=false;
  if (curtick==0) { // is tick counter 0?
    if (playmode==1 || playmode==4) {NextRow();} else {curtick=1;}
  }
}
void Playback() {
  NextTick();
  for (int iiii=0; iiii<8*soundchips; iiii++) {
    chip[iiii>>3].chan[iiii&7].vol=(cvol[iiii]*chanvol[iiii])>>7;
    chip[iiii>>3].chan[iiii&7].pan=cpan[iiii];
          if (cshape[iiii]==4) {
            chip[iiii>>3].chan[iiii&7].freq=minval(cfreq[iiii]>>2,0xffff);
          } else {
            chip[iiii>>3].chan[iiii&7].freq=cfreq[iiii];
          }
          chip[iiii>>3].chan[iiii&7].restimer=crmfreq[iiii];
    chip[iiii>>3].chan[iiii&7].flags.fmode=cfmode[iiii];
          chip[iiii>>3].chan[iiii&7].flags.shape=cshape[iiii];
          chip[iiii>>3].chan[iiii&7].flags.ring=crm[iiii];
          chip[iiii>>3].chan[iiii&7].flags.restim=(crm[iiii]&2)?(1):(0);
    chip[iiii>>3].chan[iiii&7].duty=cduty[iiii];
    chip[iiii>>3].chan[iiii&7].cutoff=coff[iiii]/4;
    chip[iiii>>3].chan[iiii&7].reson=creso[iiii];
  }
}
void JustSkip() {
  // skipping
  if (playmode==0) {
       curtick=1;
       curstep++;
       if (curstep>(getpatlen(patid[curpat])-1)) {
         curstep=0;
         curpat++;
       }
     }
}
void GoBack() {
  // go back
  if (playmode==0) {
       curtick=1;
       curstep--;
       if (curstep<0) {
         curstep=0;
       }
     }
}

void CleanupPatterns() {
  // cleans up all patterns
  memset(pat,0,256*256*32*5);
  // cleans up all envelopes
  memset(bytable,0,8*256*256);
  for (int kk=0;kk<8;kk++) {
    for (int jj=0;jj<256;jj++) {
      bytable[kk][jj][254]=255;
      bytable[kk][jj][255]=255;
    }
  }
  // cleans up all instruments
  memset(instrument,0,256*64);
  for (int jj=0;jj<256;jj++) {
    instrument[jj].noteOffset=48;
  }
}

ALLEGRO_COLOR mapHSV(float hue,float saturation,float value) {
  float c=value*saturation;
  float x=c*(1-fabs(fmod(hue/60,2)-1));
  float m=value-c;
  float r=0,g=0,b=0;
  if (hue<60) {r=c;g=x;b=0;}
  else if (hue<120) {r=x;g=c;b=0;}
  else if (hue<180) {r=0;g=c;b=x;}
  else if (hue<240) {r=0;g=x;b=c;}
  else if (hue<300) {r=x;g=0;b=c;}
  else if (hue<360) {r=c;g=0;b=x;}
  return al_map_rgba_f(r+m,g+m,b+m,64);
}
void drawpatterns(bool force) {
  if (playermode) {return;}
  if (follow) {curpatrow=curstep; if (curpatrow<0) curpatrow=0;}
  //curpatrow=0;
  // will be replaced
  if ((!UPDATEPATTERNS || playmode==0 || playmode==1) && !force && oldpat==curpat) {oldpat=curpat;return;}
  oldpat=curpat;
  UPDATEPATTERNS=true;
  al_destroy_bitmap(patternbitmap);
  patternbitmap=al_create_bitmap(scrW,(((patlength[patid[curpat]]==0)?(256):(patlength[patid[curpat]]))*12)+8);
  al_set_target_bitmap(patternbitmap);
  al_clear_to_color(al_map_rgb(0,0,0));
  al_draw_filled_rectangle(0,60,scrW,scrH,al_map_rgb(0,0,0));
  for (int i=0;i<getpatlen(patid[curpat]);i++) {
  //if (i>curpatrow+15+((scrH-450)/12)) {continue;}
  //if (i<curpatrow-16) {continue;}
  al_draw_textf(text,al_map_rgb(128,128,128),0,((i)*12),ALLEGRO_ALIGN_LEFT,"%.2X",i); //pattern pos, optimized
  al_draw_text(text,getconfigcol(colDEFA),16,((i)*12),ALLEGRO_ALIGN_LEFT,"|");
  al_draw_text(text,getconfigcol(colDEFA),((scrW/2)-400)+16+((8-chanstodisplay)*45),((i)*12),ALLEGRO_ALIGN_LEFT,"|");
  // channel drawing routine, replicated 8 times
  for (int j=0;j<chanstodisplay;j++) {
  al_draw_text(text,getconfigcol(colDEFA),((scrW/2)-400)+112+(j*96)+((8-chanstodisplay)*45),((i)*12),ALLEGRO_ALIGN_LEFT,"|");
  if (pat[patid[curpat]][i][j+(curedpage*8)][0]==0 && pat[patid[curpat]][i][j+(curedpage*8)][1]==0
    && pat[patid[curpat]][i][j+(curedpage*8)][2]==0 && pat[patid[curpat]][i][j+(curedpage*8)][3]==0
    && pat[patid[curpat]][i][j+(curedpage*8)][4]==0)
  {al_draw_text(text,al_map_rgb(0xa0,0xa0,0xa0),((scrW/2)-400)+24+(j*96)+((8-chanstodisplay)*45),((i)*12),ALLEGRO_ALIGN_LEFT,"...........");continue;}
  // note
  al_draw_textf(text,al_map_rgb(0xd0,0xd0,0xd0),((scrW/2)-400)+24+(j*96)+((8-chanstodisplay)*45),((i)*12),ALLEGRO_ALIGN_LEFT,"%s%s",getnote(pat[patid[curpat]][i][j+(curedpage*8)][0]),getoctave(pat[patid[curpat]][i][j+(curedpage*8)][0]));
  // instrument
  al_draw_textf(text,al_map_rgb(0x5f,0xd7,0xff),((scrW/2)-400)+48+(j*96)+((8-chanstodisplay)*45),((i)*12),ALLEGRO_ALIGN_LEFT,"%s%s",getinsH(pat[patid[curpat]][i][j+(curedpage*8)][1]),getinsL(pat[patid[curpat]][i][j+(curedpage*8)][1]));
  if (pat[patid[curpat]][i][j+(curedpage*8)][2]==0 && pat[patid[curpat]][i][j+(curedpage*8)][0]!=0) {
    al_draw_text(text,al_map_rgb(0x5f,0xd7,0xff),((scrW/2)-400)+64+(j*96)+((8-chanstodisplay)*45),((i)*12),ALLEGRO_ALIGN_LEFT,"v40");
  } else {
    al_draw_textf(text,al_map_rgb(0x00,0x50,0xff),((scrW/2)-400)+64+(j*96)+((8-chanstodisplay)*45),((i)*12),ALLEGRO_ALIGN_LEFT,"%s%s%s",getVFX(pat[patid[curpat]][i][j+(curedpage*8)][2]),getVFXH(pat[patid[curpat]][i][j+(curedpage*8)][2]),getVFXL(pat[patid[curpat]][i][j+(curedpage*8)][2]));}
  // effect
  al_draw_textf(text,GetFXColor(pat[patid[curpat]][i][j+(curedpage*8)][3]),((scrW/2)-400)+88+(j*96)+((8-chanstodisplay)*45),((i)*12),ALLEGRO_ALIGN_LEFT,"%s%s%s",getFX(pat[patid[curpat]][i][j+(curedpage*8)][3]),getinsH(pat[patid[curpat]][i][j+(curedpage*8)][4]),getinsL(pat[patid[curpat]][i][j+(curedpage*8)][4]));
  }
  }
  al_set_target_bitmap(al_get_backbuffer(display));
}

void drawinsedit() {
  // draws the instrument edit dialog
  g.tPos(0,5);
  g.tColor(15);
  g.printf("Instrument Editor|INS   ^v|+|-|Save|Load|                   |HEX|\n");
  g.printf("  |Volume|Cutoff|Reson|Duty|Shape|Pitch|HiPitch|Pan|Seq  ^v|NF|X|\n");

  g.tNLPos((float)(scrW-272)/8.0);
  g.tPos((float)(scrW-272)/8.0,7); g.printf("RelNote    ^v+-\n");
  g.printf("VibType sin|squ|saw|\n");
  g.printf("TrmType sin|squ|saw|tri\n\n");
  
  g.printf("________________________________\n\n");
  
  g.printf("Filter low|high|band\n\n");
  
  g.printf("PCM|pos $     |length $    |\n");
  g.printf("   |loop$     |seekmult x00|\n\n");
  
  g.printf("filterH \n\n\n");
  
  
  g.printf("RM|freq   ^v+-|shape squ^v|\n");
  g.printf("Sy|duty 00^v+-|\n\n");
  
  g.printf("ResetOsc|ResetFilter|\n");
  g.printf("ResetRMOsc|\n\n");
  
  g.printf("AutoCut  ^v\n\n");
  
  g.printf("DefVol   ^v");

  g.tNLPos(0);
  g.tPos(0,(float)(scrH-18)/12);
  g.printf("< Loop   ^v+- Release   ^v+- Length   ^v+-                     >");
  
  al_draw_rectangle(0,94,516+(scrW-800),scrH-34,getconfigcol(colDEFA),1);
  // draws envelope waveform
  if (!hexmode) {
  for (int ii=scrollpos;ii<minval(253,(516/valuewidth)+scrollpos+((scrW-800)/valuewidth));ii++) {
  if (CurrentEnv==5) { // pitch
    if (bytable[5][instrument[CurrentIns].env[CurrentEnv]][ii]!=0) {
    if (bytable[5][instrument[CurrentIns].env[CurrentEnv]][ii]<0x40) {
      // up
      al_draw_pixel((ii-scrollpos)*valuewidth,145,al_map_rgb(128,255,128));
      al_draw_pixel((ii-scrollpos)*valuewidth,146,al_map_rgb(128,255,128));
      al_draw_pixel((ii-scrollpos)*valuewidth,147,al_map_rgb(128,255,128));
      al_draw_pixel((ii-scrollpos)*valuewidth,148,al_map_rgb(128,255,128));
      al_draw_pixel((ii-scrollpos)*valuewidth,149,al_map_rgb(128,255,128));
      al_draw_pixel(1+((ii-scrollpos)*valuewidth),149,al_map_rgb(128,255,128));
      al_draw_pixel(2+((ii-scrollpos)*valuewidth),149,al_map_rgb(128,255,128));
      al_draw_pixel(2+((ii-scrollpos)*valuewidth),148,al_map_rgb(128,255,128));
      al_draw_pixel(2+((ii-scrollpos)*valuewidth),147,al_map_rgb(128,255,128));
      al_draw_pixel(2+((ii-scrollpos)*valuewidth),146,al_map_rgb(128,255,128));
      al_draw_pixel(2+((ii-scrollpos)*valuewidth),145,al_map_rgb(128,255,128));
      } else {
      if (bytable[5][instrument[CurrentIns].env[CurrentEnv]][ii]<0x80) {
      // down
      al_draw_pixel((ii-scrollpos)*valuewidth,145,al_map_rgb(255,128,128));
      al_draw_pixel((ii-scrollpos)*valuewidth,146,al_map_rgb(255,128,128));
      al_draw_pixel((ii-scrollpos)*valuewidth,147,al_map_rgb(255,128,128));
      al_draw_pixel((ii-scrollpos)*valuewidth,148,al_map_rgb(255,128,128));
      al_draw_pixel((ii-scrollpos)*valuewidth,149,al_map_rgb(255,128,128));
      al_draw_pixel(1+((ii-scrollpos)*valuewidth),149,al_map_rgb(255,128,128));
      al_draw_pixel(2+((ii-scrollpos)*valuewidth),148,al_map_rgb(255,128,128));
      al_draw_pixel(2+((ii-scrollpos)*valuewidth),147,al_map_rgb(255,128,128));
      al_draw_pixel(2+((ii-scrollpos)*valuewidth),146,al_map_rgb(255,128,128));
      al_draw_pixel(1+((ii-scrollpos)*valuewidth),145,al_map_rgb(255,128,128));
      } else {
      // absolute
      al_draw_pixel((ii-scrollpos)*valuewidth,146,al_map_rgb(255,255,128));
      al_draw_pixel((ii-scrollpos)*valuewidth,147,al_map_rgb(255,255,128));
      al_draw_pixel((ii-scrollpos)*valuewidth,148,al_map_rgb(255,255,128));
      al_draw_pixel((ii-scrollpos)*valuewidth,149,al_map_rgb(255,255,128));
      al_draw_pixel(1+((ii-scrollpos)*valuewidth),145,al_map_rgb(255,255,128));
      al_draw_pixel(2+((ii-scrollpos)*valuewidth),146,al_map_rgb(255,255,128));
      al_draw_pixel(2+((ii-scrollpos)*valuewidth),147,al_map_rgb(255,255,128));
      al_draw_pixel(2+((ii-scrollpos)*valuewidth),148,al_map_rgb(255,255,128));
      al_draw_pixel(2+((ii-scrollpos)*valuewidth),149,al_map_rgb(255,255,128));
      al_draw_pixel(1+((ii-scrollpos)*valuewidth),147,al_map_rgb(255,255,128));
      }
    }}
  }
  al_draw_filled_rectangle((ii-scrollpos)*valuewidth,
    (CurrentEnv==7)?402-bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][ii]:
    (CurrentEnv==6)?interpolate(94,scrH-42,0.5):(scrH-42),
    ((ii-scrollpos)*valuewidth)+valuewidth,
    (CurrentEnv==6)?interpolate(94,scrH-42,0.5)-(char)bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][ii]:
    (scrH-43)-(bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][ii]*((scrH-128)/256)),
    (ii>bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][253])?al_map_rgb(63,63,63): // out of range
    (ii>bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][255])?al_map_rgb(192,0,192): // release
    (ii>=bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][254])?al_map_rgb(0,192,192): // loop
    al_map_rgb(255,255,255));
  }
  for (int jj=0;jj<32;jj++) {
  if (EnvelopesRunning[jj][CurrentEnv] && Mins[jj]==CurrentIns) {
    al_draw_filled_rectangle((inspos[jj][CurrentEnv]-scrollpos)*valuewidth,
      (CurrentEnv==7)?402-bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][inspos[jj][CurrentEnv]]:
    (CurrentEnv==6)?interpolate(94,scrH-42,0.5):(scrH-42),
    ((inspos[jj][CurrentEnv]-scrollpos)*valuewidth)+valuewidth,
    (CurrentEnv==6)?interpolate(94,scrH-42,0.5)-(char)bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][inspos[jj][CurrentEnv]]:
    (scrH-43)-(bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][inspos[jj][CurrentEnv]]*((scrH-128)/256)),
    al_map_rgb(0,255,0)); continue;
  }
  }
  } else {
  for (int ii=0;ii<253;ii++) {
    g.tColor((ii>bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][253])?240:15);
    g.tPos(1+(ii%21)*3,8.5+(ii/21)*2);
    g.printf("%.2x",bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][ii]);
  }
  }
  // draws some GUI stuff
  g.tPos((float)(scrW-272)/8.0,11);
  g.tColor((inputwhere==2)?11:15);
  g.printf("%s",instrument[CurrentIns].name);
  if (inputwhere==2) {
    al_draw_line((scrW-272)+(inputcurpos*8),133,(scrW-272)+(inputcurpos*8),145,getconfigcol(colSEL2),1);
  }
  
  g.tPos(55,6);
  g.tColor(10);
  g.printf("%.2X",instrument[CurrentIns].env[CurrentEnv]);
  
  g.tPos(22,5);
  g.printf("%.2X",CurrentIns);
  
  // the right pane
  g.tColor(11);
  g.tPos((float)(scrW-208)/8.0,7);
  g.printf("%s%s\n",getnotetransp(instrument[CurrentIns].noteOffset),getoctavetransp(instrument[CurrentIns].noteOffset));
  // the thing at the bottom
  g.tPos(7,(float)(scrH-18)/12.0);
  g.printf("%.2X\n",bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][254]);
  g.tPos(22,(float)(scrH-18)/12.0);
  g.printf("%.2X\n",bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][255]);
  g.tPos(36,(float)(scrH-18)/12.0);
  g.printf("%.2X\n",bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][253]);
  
  g.tPos(0,6);
  if (((instrument[CurrentIns].activeEnv)>>CurrentEnv)&1) {
    g.tColor(10);
    g.printf("ON");
  } else {
    g.tColor(9);
    g.printf("NO");
  }
  
  g.tColor(10);
  switch (CurrentEnv) {
    case 0:
      g.tPos(3,6);
      g.printf("Volume");
      break;
    case 1:
      g.tPos(10,6);
      g.printf("Cutoff");
      break;
    case 2:
      g.tPos(17,6);
      g.printf("Reson");
      break;
    case 3:
      g.tPos(23,6);
      g.printf("Duty");
      break;
    case 4:
      g.tPos(28,6);
      g.printf("Shape");
      break;
    case 5:
      g.tPos(34,6);
      g.printf("Pitch");
      break;
    case 6:
      g.tPos(40,6);
      g.printf("HiPitch");
      break;
    case 7:
      g.tPos(48,6);
      g.printf("Pan");
      break;
  }
  // the right pane is back
  
  g.tColor(11);
  if (instrument[CurrentIns].DFM&1) {
    g.tPos((float)(scrW-216)/8.0,13);
    g.printf("low");
  }
  if (instrument[CurrentIns].DFM&2) {
    g.tPos((float)(scrW-184)/8.0,13);
    g.printf("high");
  }
  if (instrument[CurrentIns].DFM&4) {
    g.tPos((float)(scrW-144)/8.0,13);
    g.printf("band");
  }
  if (instrument[CurrentIns].DFM&8) {
    g.tPos((float)(scrW-272)/8.0,15);
    g.printf("PCM");
  }
  if (instrument[CurrentIns].DFM&16) {
    g.tPos((float)(scrW-272)/8.0,21);
    g.printf("RM");
  }
  if (instrument[CurrentIns].flags&32) {
    g.tPos((float)(scrW-272)/8.0,22);
    g.printf("Sy");
  }
  if (instrument[CurrentIns].flags&1) {
    g.tPos((float)(scrW-272)/8.0,24);
    g.printf("ResetOsc");
  }
  
  g.tPos((float)(scrW-192)/8.0,15);
  g.printf("%.2x%.2x",instrument[CurrentIns].pcmPos[1],instrument[CurrentIns].pcmPos[0]);
  
  g.tPos((float)(scrW-192)/8.0,16);
  g.printf("%.2x%.2x",instrument[CurrentIns].pcmLoop[1],instrument[CurrentIns].pcmLoop[0]);
  
  g.tPos((float)(scrW-88)/8.0,15);
  g.printf("%.4x",instrument[CurrentIns].pcmLen);
  
  g.tPos((float)(scrW-208)/8.0,21);
  g.printf("%.2x",instrument[CurrentIns].LFO);
  
  g.tPos((float)(scrW-208)/8.0,18);
  g.printf("%.4x",0xffff-instrument[CurrentIns].filterH);
  
  g.tPos((float)(scrW-208)/8.0,27);
  g.printf("%.1x",instrument[CurrentIns].flags>>6);
  
  if (hexmode) {
    g.tPos(61,5);
    g.printf("HEX");
  }
  
  if (rightclick && PIR(0,90,515,420,mstate.x,mstate.y) && !hexmode) {
    al_draw_circle((linex1/4)*4,liney1,4,al_map_rgb(255,255,255),1);
    al_draw_circle((mstate.x/4)*4,mstate.y,4,al_map_rgb(255,255,255),1);
    al_draw_line((linex1/4)*4,liney1,(mstate.x/4)*4,mstate.y,al_map_rgb(255,255,255),1);
  }
}

void EditSkip() {
  // autovolume
  /*if (pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]!=0 && (pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]%16)<13 && pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]==0) {
    pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=0x7f;
  } // no more autovolume because of the "no-value" switch*/
  // autoinstrument
  if (pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]!=0 && (pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]%16)<13) {
    pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=curins;
  }
  // skipping
  if (playmode==0) {
    curtick=1;
    curstep++;
    if (curstep>(getpatlen(patid[curpat])-1)) {
      curstep=0;
      curpat++;
    }
    selStart=curstep;
    selEnd=curstep;
    curpatrow=curstep;
    curselchan=curedchan;
    curselmode=curedmode;
  }
  drawpatterns(true);
}
void ParentDir(char *thedir) {
  // set thedir to parent directory
  #if defined(__unix__) || defined(__APPLE__) // slashes
  if (strrchr(thedir,'/')!=NULL) {
    memset(strrchr(thedir,'/'),0,1);
    if (strchr(thedir,'/')==NULL) {
      int littlestr=strlen(thedir);
      memset(thedir+littlestr,'/',1);
      memset(thedir+littlestr+1,0,1);
    }
  }
  #else // backslashes
  if (strrchr(thedir,'\\')!=NULL) {
    memset(strrchr(thedir,'\\'),0,1);
    if (strchr(thedir,'\\')==NULL) {
      int littlestr=strlen(thedir);
      memset(thedir+littlestr,'\\',1);
      memset(thedir+littlestr+1,0,1);
    }
  }
  #endif
}
int NumberLetter(char cval) {
  switch (cval) {
    case '0': return 0; break;
    case '1': return 1; break;
    case '2': return 2; break;
    case '3': return 3; break;
    case '4': return 4; break;
    case '5': return 5; break;
    case '6': return 6; break;
    case '7': return 7; break;
    case '8': return 8; break;
    case '9': return 9; break;
  }
  fprintf(stderr,"invalid number value entered");
  return 0;
}
void drawmixerlayer() {
  al_set_target_bitmap(mixer);
  al_clear_to_color(al_map_rgba(0,0,0,0));
  int mixerdrawoffset=(scrW/2)-chanstodisplay*48-12;
  for (int chantodraw=0;chantodraw<chanstodisplay;chantodraw++) {
    al_draw_line(12.5+(chantodraw*96)+mixerdrawoffset,60,12.5+(chantodraw*96)+mixerdrawoffset,scrW-0.5,getconfigcol(colDEFA),1);
    al_draw_textf(text,getconfigcol(colDEFA),16+(chantodraw*96)+mixerdrawoffset,24+60,ALLEGRO_ALIGN_LEFT,"Vol  ^v");
    al_draw_textf(text,getconfigcol(colDEFA),16+(chantodraw*96)+mixerdrawoffset,36+60,ALLEGRO_ALIGN_LEFT,"Pan  ^v");
    al_draw_textf(text,getconfigcol(colDEFA),16+(chantodraw*96)+mixerdrawoffset,60+60,ALLEGRO_ALIGN_LEFT,"Freq");
    al_draw_textf(text,getconfigcol(colDEFA),16+(chantodraw*96)+mixerdrawoffset,72+60,ALLEGRO_ALIGN_LEFT,"Cut");
    al_draw_textf(text,getconfigcol(colDEFA),16+(chantodraw*96)+mixerdrawoffset,84+60,ALLEGRO_ALIGN_LEFT,"Ins  |Vol  ");
    al_draw_textf(text,getconfigcol(colDEFA),16+(chantodraw*96)+mixerdrawoffset,96+60,ALLEGRO_ALIGN_LEFT,"EP0  |EP1  ");
    al_draw_textf(text,getconfigcol(colDEFA),16+(chantodraw*96)+mixerdrawoffset,108+60,ALLEGRO_ALIGN_LEFT,"EP2  |EP3  ");
    al_draw_textf(text,getconfigcol(colDEFA),16+(chantodraw*96)+mixerdrawoffset,120+60,ALLEGRO_ALIGN_LEFT,"EP4  |EP5  ");
    al_draw_textf(text,getconfigcol(colDEFA),16+(chantodraw*96)+mixerdrawoffset,132+60,ALLEGRO_ALIGN_LEFT,"EP6  |EP7  ");
    al_draw_textf(text,getconfigcol(colDEFA),16+(chantodraw*96)+mixerdrawoffset,144+60,ALLEGRO_ALIGN_LEFT,"Not  |Por  ");
    al_draw_textf(text,getconfigcol(colDEFA),16+(chantodraw*96)+mixerdrawoffset,156+60,ALLEGRO_ALIGN_LEFT,"Vib  |Trm  ");
    al_draw_textf(text,getconfigcol(colDEFA),16+(chantodraw*96)+mixerdrawoffset,168+60,ALLEGRO_ALIGN_LEFT,"Trr  |Pbr  ");
  }
  al_draw_line(0,21.5+60,scrW,21.5+60,getconfigcol(colDEFA),1);
  al_draw_line(0,57.5+60,scrW,57.5+60,getconfigcol(colDEFA),1);
  al_draw_line(12.5+(chanstodisplay*96)+mixerdrawoffset,60,12.5+(chanstodisplay*96)+mixerdrawoffset,scrW-0.5,getconfigcol(colDEFA),1);
  al_set_target_bitmap(al_get_backbuffer(display));
}
void drawmixer() {
  // draws the mixer dialog
  int mixerdrawoffset=(scrW/2)-chanstodisplay*48-12;
  al_draw_bitmap(mixer,0,0,0);
  for (int chantodraw=0;chantodraw<chanstodisplay;chantodraw++) {
    al_draw_textf(text,(midion[chantodraw+(curedpage*8)])?(al_map_rgb(255,0,0)):(getconfigcol(colDEFA)),24+(chantodraw*96)+mixerdrawoffset,60,ALLEGRO_ALIGN_LEFT,"Channel");
    al_draw_textf(text,(!muted[chantodraw+(curedpage*8)])?(al_map_rgb(0,255,255)):(al_map_rgb(128,128,128)),88+(chantodraw*96)+mixerdrawoffset,60,ALLEGRO_ALIGN_LEFT,"%d",chantodraw+(curedpage*8));

    al_draw_filled_rectangle(16+(chantodraw*96)+mixerdrawoffset-1,scrH-4-1,58+(chantodraw*96)+mixerdrawoffset+1,(scrH-4)-(((float)cvol[chantodraw+(curedpage*8)]*((127-(maxval(0,(float)cpan[chantodraw+(curedpage*8)])))/127))*((scrH-244)/128))+1,al_map_rgb(0,200,0));
    al_draw_filled_rectangle(62+(chantodraw*96)+mixerdrawoffset-1,scrH-4-1,104+(chantodraw*96)+mixerdrawoffset+1,(scrH-4)-(((float)cvol[chantodraw+(curedpage*8)]*((128+(minval(0,(float)cpan[chantodraw+(curedpage*8)])))/128))*((scrH-244)/128))+1,al_map_rgb(0,200,0));
    al_draw_filled_rectangle(16+(chantodraw*96)+mixerdrawoffset,scrH-4,58+(chantodraw*96)+mixerdrawoffset,(scrH-4)-(((float)cvol[chantodraw+(curedpage*8)]*((127-(maxval(0,(float)cpan[chantodraw+(curedpage*8)])))/127))*((scrH-244)/128)),al_map_rgb(0,255,0));
    al_draw_filled_rectangle(62+(chantodraw*96)+mixerdrawoffset,scrH-4,104+(chantodraw*96)+mixerdrawoffset,(scrH-4)-(((float)cvol[chantodraw+(curedpage*8)]*((128+(minval(0,(float)cpan[chantodraw+(curedpage*8)])))/128))*((scrH-244)/128)),al_map_rgb(0,255,0));
    
    al_draw_textf(text,al_map_rgb(0,255,255),40+(chantodraw*96)+mixerdrawoffset,84,ALLEGRO_ALIGN_LEFT,"%.2x",defchanvol[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),40+(chantodraw*96)+mixerdrawoffset,96,ALLEGRO_ALIGN_LEFT,"%.2x",(unsigned char)defchanpan[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),64+(chantodraw*96)+mixerdrawoffset,120,ALLEGRO_ALIGN_LEFT,"%.4x",cfreq[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),40+(chantodraw*96)+mixerdrawoffset,144,ALLEGRO_ALIGN_LEFT,"%.2x",Mins[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),88+(chantodraw*96)+mixerdrawoffset,144,ALLEGRO_ALIGN_LEFT,"%.2x",Mvol[chantodraw+(curedpage*8)]);
    al_draw_textf(text,getmixerposcol(chantodraw+(curedpage*8),0),40+(chantodraw*96)+mixerdrawoffset,156,ALLEGRO_ALIGN_LEFT,"%.2x",inspos[chantodraw+(curedpage*8)][0]);
    al_draw_textf(text,getmixerposcol(chantodraw+(curedpage*8),1),88+(chantodraw*96)+mixerdrawoffset,156,ALLEGRO_ALIGN_LEFT,"%.2x",inspos[chantodraw+(curedpage*8)][1]);
    al_draw_textf(text,getmixerposcol(chantodraw+(curedpage*8),2),40+(chantodraw*96)+mixerdrawoffset,168,ALLEGRO_ALIGN_LEFT,"%.2x",inspos[chantodraw+(curedpage*8)][2]);
    al_draw_textf(text,getmixerposcol(chantodraw+(curedpage*8),3),88+(chantodraw*96)+mixerdrawoffset,168,ALLEGRO_ALIGN_LEFT,"%.2x",inspos[chantodraw+(curedpage*8)][3]);
    al_draw_textf(text,getmixerposcol(chantodraw+(curedpage*8),4),40+(chantodraw*96)+mixerdrawoffset,180,ALLEGRO_ALIGN_LEFT,"%.2x",inspos[chantodraw+(curedpage*8)][4]);
    al_draw_textf(text,getmixerposcol(chantodraw+(curedpage*8),5),88+(chantodraw*96)+mixerdrawoffset,180,ALLEGRO_ALIGN_LEFT,"%.2x",inspos[chantodraw+(curedpage*8)][5]);
    al_draw_textf(text,getmixerposcol(chantodraw+(curedpage*8),6),40+(chantodraw*96)+mixerdrawoffset,192,ALLEGRO_ALIGN_LEFT,"%.2x",inspos[chantodraw+(curedpage*8)][6]);
    al_draw_textf(text,getmixerposcol(chantodraw+(curedpage*8),7),88+(chantodraw*96)+mixerdrawoffset,192,ALLEGRO_ALIGN_LEFT,"%.2x",inspos[chantodraw+(curedpage*8)][7]);
    al_draw_textf(text,al_map_rgb(0,255,255),40+(chantodraw*96)+mixerdrawoffset,204,ALLEGRO_ALIGN_LEFT,"%.2x",(int)(curnote[chantodraw+(curedpage*8)])%256);
    al_draw_textf(text,al_map_rgb(0,255,255),88+(chantodraw*96)+mixerdrawoffset,204,ALLEGRO_ALIGN_LEFT,"%.2x",portastatic[chantodraw+(curedpage*8)]%256);
    al_draw_textf(text,al_map_rgb(0,0,255),80+(chantodraw*96)+mixerdrawoffset,84,ALLEGRO_ALIGN_LEFT,"%.2x",cduty[chantodraw+(curedpage*8)]%256);
    switch (cshape[chantodraw+(curedpage*8)]) {
      case 0: al_draw_text(text,al_map_rgb(0,0,255),96+(chantodraw*96)+mixerdrawoffset,84,ALLEGRO_ALIGN_LEFT,"s"); break;
      case 1: al_draw_text(text,al_map_rgb(0,0,255),96+(chantodraw*96)+mixerdrawoffset,84,ALLEGRO_ALIGN_LEFT,"R"); break;
      case 2: al_draw_text(text,al_map_rgb(0,0,255),96+(chantodraw*96)+mixerdrawoffset,84,ALLEGRO_ALIGN_LEFT,"S"); break;
      case 3: al_draw_text(text,al_map_rgb(0,0,255),96+(chantodraw*96)+mixerdrawoffset,84,ALLEGRO_ALIGN_LEFT,"t"); break;
      case 4: al_draw_text(text,al_map_rgb(0,0,255),96+(chantodraw*96)+mixerdrawoffset,84,ALLEGRO_ALIGN_LEFT,"N"); break;
      case 5: al_draw_text(text,al_map_rgb(0,0,255),96+(chantodraw*96)+mixerdrawoffset,84,ALLEGRO_ALIGN_LEFT,"n"); break;
    }
    al_draw_textf(text,al_map_rgb(0,0,255),48+(chantodraw*96)+mixerdrawoffset,132,ALLEGRO_ALIGN_LEFT,"%.1f",(float)coff[chantodraw+(curedpage*8)]/10);
    switch (cfmode[chantodraw+(curedpage*8)]) {
      case 0: al_draw_text(text,al_map_rgb(0,255,255),80+(chantodraw*96)+mixerdrawoffset,96,ALLEGRO_ALIGN_LEFT,"   "); break;
      case 1: al_draw_text(text,al_map_rgb(0,255,255),80+(chantodraw*96)+mixerdrawoffset,96,ALLEGRO_ALIGN_LEFT,"  l"); break;
      case 2: al_draw_text(text,al_map_rgb(0,255,255),80+(chantodraw*96)+mixerdrawoffset,96,ALLEGRO_ALIGN_LEFT," h "); break;
      case 3: al_draw_text(text,al_map_rgb(0,255,255),80+(chantodraw*96)+mixerdrawoffset,96,ALLEGRO_ALIGN_LEFT," hl"); break;
      case 4: al_draw_text(text,al_map_rgb(0,255,255),80+(chantodraw*96)+mixerdrawoffset,96,ALLEGRO_ALIGN_LEFT,"b  "); break;
      case 5: al_draw_text(text,al_map_rgb(0,255,255),80+(chantodraw*96)+mixerdrawoffset,96,ALLEGRO_ALIGN_LEFT,"b l"); break;
      case 6: al_draw_text(text,al_map_rgb(0,255,255),80+(chantodraw*96)+mixerdrawoffset,96,ALLEGRO_ALIGN_LEFT,"bh "); break;
      case 7: al_draw_text(text,al_map_rgb(0,255,255),80+(chantodraw*96)+mixerdrawoffset,96,ALLEGRO_ALIGN_LEFT,"bhl"); break;
    }

  }
}
void drawdiskop() {
  // draws the disk operations dialog
  g.tPos(0,5);
  g.tColor(15);
  g.printf("Disk Operations|Open|Load|Save|ImportMOD|ImportS3M|ImportIT|ImportXM|LoadSample|LoadRawSample\n");
  g.printf("---------------------------------------------------------------------------------------------------\n");
  g.printf("FilePath __________________________________________________________________________________________\n");
  g.printf("---------------------------------------------------------------------------------------------------\n");
  g.printf("<..>                                                                                               ^");
  al_draw_text(text,getconfigcol(colDEFA),0,432,ALLEGRO_ALIGN_LEFT,"                                                                                                   v");
  al_draw_text(text,al_map_rgb(255,255,255),72,84,ALLEGRO_ALIGN_LEFT,curdir);
  if (selectedfileindex>(diskopscrollpos)) {
  al_draw_filled_rectangle(0,111+((selectedfileindex-diskopscrollpos)*12),scrW,123+((selectedfileindex-diskopscrollpos)*12),getconfigcol(colSELE));
  }
  for (int i=diskopscrollpos; i<minval(diskopscrollpos+27,filecount); i++) {
    g.tPos(0,10+i-diskopscrollpos);
    g.tColor(filenames[i].isdir?14:15);
#ifdef _WIN32
    g.printf(strrchr(filenames[i].name.c_str(),'\\')+1);
#else
    g.printf(strrchr(filenames[i].name.c_str(),'/')+1);
#endif
  }
}

void drawmemory() {
  // draws soundchip memory view
  g.tPos(0,5);
  g.tColor(15);
  g.printf("Memory  00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n");
  g.printf("   $00\n");
  g.printf("   $10\n");
  g.printf("   $20\n");
  g.printf("   $30\n");
  g.printf("   $40\n");
  g.printf("   $50\n");
  g.printf("   $60\n");
  g.printf("   $70\n");
  g.printf("   $80\n");
  g.printf("   $90\n");
  g.printf("   $a0\n");
  g.printf("   $b0\n");
  g.printf("   $c0\n");
  g.printf("   $d0\n");
  g.printf("   $e0\n");
  g.printf("   $f0\n");
  g.tColor(14);
  for (int j=0; j<16; j++) {
    g.tPos(8,6+j);
    for (int i=0; i<16; i++) {
      g.printf("%.2x ",((unsigned char*)(chip[curedpage].chan))[i+(j<<4)]);
    }
  }
}

void drawsong() {
  // draws the disk operations dialog
  g.tColor(15);
  g.tPos(0,5);
  g.printf("DefSpeed   v^|Channels  v^|beat   v^|bar   v^|detune   v^-+|length   v^-+|tempo   v^-+|speed   v^\n");
  g.printf("---------------------------------------------------------------------------------------------------\n");
  g.printf("|Song Name ________________________________________________________\n");
  g.printf("---------------------------------------------------------------------------------------------------\n");
  
  g.tPos(0,18);
  g.printf("---------------------------------------------------------------------------------------------------\n");
  g.printf("PlaybackMode Normal|IT|FT2|PT|ST3\n");
  g.printf("CompatibleGxx|DoubleFilter|\n");
  g.printf("Slides Linear|Periods|Amiga\n");
  g.printf("\n");
  g.printf("PCM|Edit|Load|SaveDump|PlayDump\n");
  g.printf("\n");
  g.printf("Comments\n");
  
  g.tColor((inputwhere==1)?11:15);
  g.tPos(11,7);
  g.printf(name);
  if (inputwhere==1) {
      al_draw_line(89+(inputcurpos*8),85,89+(inputcurpos*8),97,getconfigcol(colSEL2),1);
  }
  al_draw_textf(text,getconfigcol(colSEL2),424,60,ALLEGRO_ALIGN_LEFT,"%.2x",(unsigned char)songdf);
  al_draw_textf(text,getconfigcol(colSEL2),536,60,ALLEGRO_ALIGN_LEFT,"%.2x",songlength);
  al_draw_textf(text,getconfigcol(colSEL2),744,60,ALLEGRO_ALIGN_LEFT,"%.2X",defspeed);
  for (int chantodraw=0;chantodraw<8;chantodraw++) {
    al_draw_textf(text,al_map_rgb(0,255,255),112,120+(chantodraw*12),ALLEGRO_ALIGN_LEFT,"%.4x",cfreq[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),152,120+(chantodraw*12),ALLEGRO_ALIGN_LEFT,"%.2x",cvol[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),184,120+(chantodraw*12),ALLEGRO_ALIGN_LEFT,"%.2x",(unsigned char)cpan[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),216,120+(chantodraw*12),ALLEGRO_ALIGN_LEFT,"%.2x",cduty[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),248,120+(chantodraw*12),ALLEGRO_ALIGN_LEFT,"%.5x",coff[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),296,120+(chantodraw*12),ALLEGRO_ALIGN_LEFT,"%.2x",creso[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),328,120+(chantodraw*12),ALLEGRO_ALIGN_LEFT,"%.1x%.1x%.1x%.1x",cfmode[chantodraw+(curedpage*8)],cshape[chantodraw+(curedpage*8)],
                                                         cmode[chantodraw+(curedpage*8)],crm[chantodraw+(curedpage*8)]);
    /*al_draw_textf(text,al_map_rgb(0,255,255),112,120+(chantodraw*12),ALLEGRO_ALIGN_LEFT,"%.2x",cfreq[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),112,120+(chantodraw*12),ALLEGRO_ALIGN_LEFT,"%.4x",cfreq[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),112,120+(chantodraw*12),ALLEGRO_ALIGN_LEFT,"%.4x",cfreq[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),112,120+(chantodraw*12),ALLEGRO_ALIGN_LEFT,"%.4x",cfreq[chantodraw+(curedpage*8)]);
    al_draw_textf(text,al_map_rgb(0,255,255),112,120+(chantodraw*12),ALLEGRO_ALIGN_LEFT,"%.4x",cfreq[chantodraw+(curedpage*8)]);*/
  }
}
void drawhelp() {
  // draws the help screen
  al_draw_text(text,al_map_rgb(255,255,255),0,60,ALLEGRO_ALIGN_LEFT,helptext);
}
void drawconfig() {
  g.tColor(15);
  g.tPos(0,6);
  
  g.printf(
  "Color Palette          |Load|Save| Audio Settings\n"
  "                                 |\n"
  "Note              |          C-4 | simulate distortion\n"
  "Instrument        |          01  | \n"
  "Volume            |          v20 | \n"
  "Effect (tempo)    |          A06 | cubic spline PCM\n"
  "Effect (song)     |          B02 |---------------------\n"
  "Effect (volume)   |          D06 | Importer Settings\n"
  "Effect (pitch)    |          GFF | \n"
  "Effect (note)     |          J37 | import samples\n"
  "Effect (special)  |          SC1 | split instruments\n"
  "Effect (pan)      |          Y64 |---------------------\n"
  "Effect (unknown)  |          ?FF | Filter Settings\n"
  "Blank Row         |          ... | \n"
  "Selected 1        |          abc | disable filters\n"
  "Selected 2        |          abc | high quality\n"
  "Selected 3        |          abc |\n"
  "Peak Meter        |          abc |\n"
  "Default           |          abc |\n"
  "Dark              |          abc |\n"
  "Loop Highlight    |          abc |\n"
  "Release Highlight |          abc |\n"
  "CurPos Highlight  |          abc |\n"
  "On/Up             |          ON  |\n"
  "Off/Down          |          NO  |\n"
  "Absolute          |          A   |\n"
  "CurrentRow HL     |          abc |\n"
  "Selection         |          abc |\n"
  "No Volume         |          v40 |"
  );
  
  for (int ii=0;ii<27;ii++) {
    al_draw_textf(text,getconfigcol(ii),160,96+(ii*12),ALLEGRO_ALIGN_LEFT,"%.2X %.2X %.2X",settings::colorR[ii],settings::colorG[ii],settings::colorB[ii]);
  }
  al_draw_text(text,(settings::distortion)?getconfigcol(colSEL2):getconfigcol(colDEFA),280,96,ALLEGRO_ALIGN_LEFT,"simulate distortion");
  al_draw_text(text,(settings::cubicspline)?getconfigcol(colSEL2):getconfigcol(colDEFA),280,132,ALLEGRO_ALIGN_LEFT,"cubic spline PCM");
  al_draw_text(text,(settings::nofilters)?getconfigcol(colSEL2):getconfigcol(colDEFA),280,240,ALLEGRO_ALIGN_LEFT,"disable filters");
  al_draw_text(text,(settings::muffle)?getconfigcol(colSEL2):getconfigcol(colDEFA),280,252,ALLEGRO_ALIGN_LEFT,"high quality");
}
void drawabout() {
  // draws about screen
  al_draw_text(text,al_map_rgb(255,255,255),scrW/2,60,ALLEGRO_ALIGN_CENTER,PROGRAM_NAME);
  al_draw_textf(text,getconfigcol(colSEL1),scrW/2,72,ALLEGRO_ALIGN_CENTER,"r%d",ver);
  al_draw_rotated_bitmap(logo,180,86.5,scrW/2,scrH/2,(sin((((float)curstep*(float)speed)+((float)speed-(float)curtick))/(8*(float)speed)*2*ALLEGRO_PI)/8)*(playmode!=0),0);
}

float getLKeyOff(int tone) {
  switch (tone) {
    case 0: case 3:
      return 1;
    default: return 4;
  }
}

float getRKeyOff(int tone) {
  switch (tone) {
    case 2: case 6:
      return 10;
    default: return 7;
  }
}

void drawpiano() {
  double prefreq;
  int postfreq;
  al_draw_scaled_bitmap(piano,0,0,700,60,(scrW/2)-(((scrW)/700)*700)/2,scrH-(60*((scrW)/700)),((scrW)/700)*700,((scrW)/700)*60,0);
  al_set_target_bitmap(pianoroll_temp);
  al_draw_bitmap(pianoroll,0,-1,0);
  al_set_target_bitmap(pianoroll);
  al_clear_to_color(al_map_rgb(0,0,0));
  al_draw_bitmap(pianoroll_temp,0,0,0);
  al_set_target_bitmap(al_get_backbuffer(display));
  //printf("--------------\n");
  for (int ii=0;ii<32;ii++) {
    if (muted[ii] || cvol[ii]==0) continue;
    prefreq=((log(((4.53948974609375*(double)cfreq[ii])/440.0)/64)/log(2.0))*12.0)+57.5;
    if (prefreq<0 || prefreq>120) continue;
    if (
      (int)prefreq%12==0 || (int)prefreq%12==2 || (int)prefreq%12==4 ||
      (int)prefreq%12==5 || (int)prefreq%12==7 || (int)prefreq%12==9 ||
      (int)prefreq%12==11
      ) {
      postfreq=round(((int)prefreq%12)/2.0);
      // upper key
      al_draw_filled_rectangle(
        ((((int)prefreq/12)*70+((postfreq*10)+getLKeyOff(postfreq)))*((scrW)/700))+(scrW/2)-(((scrW)/700)*700)/2,
        ((scrW)/700)+scrH-(((scrW)/700)*60),
        ((((int)prefreq/12)*70+((postfreq*10)+getRKeyOff(postfreq)))*((scrW)/700))+(scrW/2)-(((scrW)/700)*700)/2,
        35*((scrW)/700)+scrH-(((scrW)/700)*60),
        al_map_rgba(
        (cshape[ii]==4 || cshape[ii]==1 || cshape[ii]==5)?(255):(0),
        (cshape[ii]!=5)?(255):(0),
        (cshape[ii]!=1 && cshape[ii]!=2)?(255):(0),
        cvol[ii]*2));
      // lower key
      al_draw_filled_rectangle(
        ((((int)prefreq/12)*70+(((postfreq*10)+1)))*((scrW)/700))+(scrW/2)-(((scrW)/700)*700)/2,
        35*((scrW)/700)+scrH-(((scrW)/700)*60),
        ((((int)prefreq/12)*70+(((postfreq*10)+10)))*((scrW)/700))+(scrW/2)-(((scrW)/700)*700)/2,
        59*((scrW)/700)+scrH-(((scrW)/700)*60),
        al_map_rgba(
        (cshape[ii]==4 || cshape[ii]==1 || cshape[ii]==5)?(255):(0),
        (cshape[ii]!=5)?(255):(0),
        (cshape[ii]!=1 && cshape[ii]!=2)?(255):(0),
        cvol[ii]*2));
    } else {
      postfreq=((int)prefreq%12)/2.0;
      al_draw_filled_rectangle(
        ((((int)prefreq/12)*70+(((postfreq*10)+8)))*((scrW)/700))+(scrW/2)-(((scrW)/700)*700)/2,
        ((scrW)/700)+scrH-(((scrW)/700)*60),
        ((((int)prefreq/12)*70+(((postfreq*10)+13)))*((scrW)/700))+(scrW/2)-(((scrW)/700)*700)/2,
        (34*((scrW)/700))+scrH-(((scrW)/700)*60),
        al_map_rgba(
        (cshape[ii]==4 || cshape[ii]==1 || cshape[ii]==5)?(cvol[ii]*2):(0),
        (cshape[ii]!=5)?(cvol[ii]*2):(0),
        (cshape[ii]!=1 && cshape[ii]!=2)?(cvol[ii]*2):(0),
        255));
    }
    al_set_target_bitmap(pianoroll);
                al_set_blender(ALLEGRO_ADD,ALLEGRO_ONE,ALLEGRO_ONE);
                if (cshape[ii]==0) {
                 al_draw_filled_rectangle(((prefreq-0.5)*6)+1-0.5,127,((prefreq-0.5)*6)+5,128,
      mapHSV(240-(cduty[ii]),1,(float)cvol[ii]/127)); 
                } else {
    al_draw_filled_rectangle(((prefreq-0.5)*5.83333333)+1-0.5,127,((prefreq-0.5)*5.833333333)+5,128,
      al_map_rgb(
      (cshape[ii]==4 || cshape[ii]==1 || cshape[ii]==5)?(cvol[ii]*2):(0),
      (cshape[ii]!=5)?(cvol[ii]*2):(0),
      (cshape[ii]!=1 && cshape[ii]!=2)?(cvol[ii]*2):(0)
      ));
                }
                al_set_blender(ALLEGRO_ADD,ALLEGRO_ONE,ALLEGRO_INVERSE_ALPHA);
    al_set_target_bitmap(al_get_backbuffer(display));
  }
  al_draw_scaled_bitmap(pianoroll,0,0,700,128,(scrW/2)-((((scrW)/700)*700)/2),scrH-(((scrH-(60+((((scrW)/700)*60))))/128)*128)-(((scrW)/700)*60),((scrW)/700)*700,((scrH-(60+((((scrW)/700)*60))))/128)*128,0);
  for (int ii=0;ii<32;ii++) {
    if (muted[ii] || cvol[ii]==0) continue;
    prefreq=((log(((4.53948974609375*(double)cfreq[ii])/440.0)/64)/log(2.0))*12.0)+57.5;
    if (prefreq<0 || prefreq>120) continue;
    al_draw_textf(text,al_map_rgb(255,255,255),0,60+(ii*12),ALLEGRO_ALIGN_LEFT,"%d: %s%s %c%.2d",
      ii,getnotetransp((int)prefreq),getoctavetransp((int)prefreq),(sign((int)(fmod(prefreq,1)*100)-50)>-1)?('+'):('-'),abs((int)(fmod(prefreq,1)*100)-50));
  }
}
void drawcomments() {
  int DRAWCUR_X=0;
  int DRAWCUR_Y=0;
  for (int ii=0;ii<strlen(comments);ii++) {
  if (comments[ii]==13) {DRAWCUR_X=0;DRAWCUR_Y++;continue;}
  al_draw_textf(text,al_map_rgb(255,255,255),DRAWCUR_X*8,60+DRAWCUR_Y*12,ALLEGRO_ALIGN_LEFT,"%c",comments[ii]);
  DRAWCUR_X++;
  if (DRAWCUR_X>(scrW/8)) {DRAWCUR_X=0;DRAWCUR_Y++;}
  if (DRAWCUR_Y>((scrH/12)-5)) {break;}
  if (ii==(inputcurpos-1) && inputwhere==4) {
    al_draw_line(1+(DRAWCUR_X*8),60+(DRAWCUR_Y*12),1+(DRAWCUR_X*8),75+(DRAWCUR_Y*12),getconfigcol(colSEL2),1);
  }
  }
}
void drawsfxpanel() {
  al_draw_text(text,al_map_rgb(255,255,255),scrW/2,60,ALLEGRO_ALIGN_CENTER,"--- Sound Effects ---");
}
void drawsfxeditor() {
  al_draw_text(text,al_map_rgb(255,255,255),scrW/2,60,ALLEGRO_ALIGN_CENTER,"--- Sound Effect Editor ---");
  int sfxframe=0;
  for (int fxoffset=0;fxoffset<1024;fxoffset++) {
  al_draw_text(text,getconfigcol(colDEFA),8,72+(sfxframe*12),ALLEGRO_ALIGN_LEFT,"PV|P|S|RM|PCM|F");
  int fxdata=sfxdata[cursfx][fxoffset];
  if (fxdata==0) {al_draw_text(text,getconfigcol(colDEFA),136,72+(sfxframe*12),ALLEGRO_ALIGN_LEFT,"--- end of effect ---");break;}
  if (fxdata&1) {
    al_draw_textf(text,getconfigcol(colDEFA),136,72+(sfxframe*12),ALLEGRO_ALIGN_LEFT,"%.2x%.2x %.2x",sfxdata[cursfx][fxoffset+1],sfxdata[cursfx][fxoffset+2],sfxdata[cursfx][fxoffset+3]);
    //cfreq[achan]=(fxdata[fxpos+1+fxoffset]*256)+fxdata[fxpos+2+fxoffset];
    //cvol[achan]=fxdata[fxpos+3+fxoffset];
    fxoffset+=3;
  }
  if (fxdata&2) {
    //cpan[achan]=fxdata[fxpos+1+fxoffset];
    fxoffset++;
  }
  if (fxdata&4) {
    //cduty[achan]=fxdata[fxpos+1+fxoffset];
    //cshape[achan]=fxdata[fxpos+2+fxoffset];
    fxoffset+=2;
  }
  if (fxdata&8) {
    fxoffset+=5;
  }
  if (fxdata&16) {
    fxoffset+=9;
  }
  if (fxdata&32) {
    //cfmode[achan]=fxdata[fxpos+1+fxoffset];
    //coff[achan]=(fxdata[fxpos+2+fxoffset]*65536)+(fxdata[fxpos+2+fxoffset]*256)+fxdata[fxpos+4+fxoffset];
    //creso[achan]=fxdata[fxpos+5+fxoffset];
    fxoffset+=5;
  }
  sfxframe++;
  }
    //al_draw_text(text,getconfigcol(colDEFA),8,72+(sfxframe*12),ALLEGRO_ALIGN_LEFT,"PV|P|S|RM|PCM|F");
    //al_draw_textf(text,(sfxpos==ii)?(getconfigcol(colCPOS)):(al_map_rgb(255,255,255)),(ii%(scrW/24))*24,84+((ii/(scrW/24))*24),ALLEGRO_ALIGN_LEFT,"%.2x",sfxdata[cursfx][ii]);
}
void drawpcmeditor() {
  pcmeditseek=mstate.x;
  if (sign(pcmeditscale)>-1) {
  al_draw_textf(text,getconfigcol(colDEFA),0,60,ALLEGRO_ALIGN_LEFT,"scale: %d:1",(int)pow(2.0f,pcmeditscale));
  } else {
  al_draw_textf(text,getconfigcol(colDEFA),0,60,ALLEGRO_ALIGN_LEFT,"scale: 1:%d",(int)pow(2.0f,-pcmeditscale));
  }
  al_draw_textf(text,getconfigcol(colDEFA),0,72,ALLEGRO_ALIGN_LEFT,"position: %.5x",(int)((float)pcmeditseek*pow(2.0f,-pcmeditscale)));
  al_draw_line((int)((float)pcmeditseek*pow(2.0f,-pcmeditscale))*pow(2.0f,pcmeditscale),(scrH/2)-128,
    (int)((float)pcmeditseek*pow(2.0f,-pcmeditscale))*pow(2.0f,pcmeditscale),(scrH/2)+128,getconfigcol(colSEL2),1);
  for (float ii=0;ii<(scrW*pow(2.0f,-pcmeditscale));ii+=pow(2.0f,-pcmeditscale)) {
    al_draw_pixel(ii*pow(2.0f,pcmeditscale),(scrH/2)+chip[0].pcm[(int)ii+pcmeditoffset],(pcmeditenable)?getconfigcol(colSEL2):getconfigcol(colDEFA));
  }
  for (int ii=0;ii<32;ii++) {
    if (cmode[ii]==1 && cvol[ii]>0) {
      al_draw_line(((float)cpcmpos[ii]*pow(2.0f,pcmeditscale)),(scrH/2)-140,
    ((float)cpcmpos[ii]*pow(2.0f,pcmeditscale)),(scrH/2)-129,getconfigcol(colSEL1),1);
      al_draw_textf(text,getconfigcol(colSEL1),(cpcmpos[ii]*pow(2.0f,pcmeditscale)),(scrH/2)-153,
        ALLEGRO_ALIGN_CENTER,"%d",ii);
    }
  }
}
void StepPlay() {
  NextRow();
  playmode=2;
}
void Play() {
  //// PLAY SONG ////
  // set speed to song speed and other variables
  if (!speedlock) {speed=defspeed;}
  if (!tempolock) {
  if (ntsc) {
  #ifdef FILM
  tempo=60;
  #else
  tempo=150;
  #endif
  } else {
  tempo=125;
  }
  }
  for (int ii=0;ii<32;ii++) {
    if (pat[patid[curpat]][0][ii][3]==20)
    {if (pat[patid[curpat]][0][ii][4]!=0 && !tempolock)
    {tempo=maxval(31,pat[patid[curpat]][0][ii][4]);FPS=(double)tempo/2.5;
    }}
  }
  FPS=tempo/2.5;
  //detunefactor=DETUNE_FACTOR_GLOBAL*(50/FPS);
  // reset cursor position
  curtick=2;curstep=-1;playmode=1;
  tickstart=true;
  // reset channels
  for (int su=0;su<32;su++) {
  cvol[su]=0;
  cfreq[su]=1;
  cduty[su]=63;
  cshape[su]=0;
  coff[su]=262144;
  creso[su]=0;
  cfmode[su]=0;
  Mvol[su]=0;
  Mins[su]=0;
  curvibshape[su]=0;
  EnvelopesRunning[su][0]=false;
  EnvelopesRunning[su][1]=false;
  EnvelopesRunning[su][2]=false;
  EnvelopesRunning[su][3]=false;
  EnvelopesRunning[su][4]=false;
  EnvelopesRunning[su][5]=false;
  EnvelopesRunning[su][6]=false;
  EnvelopesRunning[su][7]=false;
  inspos[su][0]=0;
  inspos[su][1]=0;
  inspos[su][2]=0;
  inspos[su][3]=0;
  inspos[su][4]=0;
  inspos[su][5]=0;
  inspos[su][6]=0;
  inspos[su][7]=0;
  released[su]=false;
  plcount[su]=0;
  plpos[su]=0;
  chanvol[su]=defchanvol[su];
  doretrigger[su]=false;
  //chanpan[su]=defchanpan[su];
  if ((su+1)&2) {chanpan[su]=96;} else {chanpan[su]=-96;} // amiga auto-pan logic
  //if (su&1) {chanpan[su]=96;} else {chanpan[su]=-96;} // normal auto-pan logic
  finedelay=0;
  cutcount[su]=-1;
  }
  // reset global volume
  cglobvol=128;
  // process next row
  NextTick();
}
unsigned char ITVolumeConverter(unsigned char itvol) {
  if (itvol<65) {return minval(itvol+64,127);} // 64-127
  if (itvol<193 && itvol>127) {return itvol;} // 128-192
  switch((itvol-65)/10) {
  case 0: return itvol-65; break; // 1-10
  case 1: return itvol-65; break; // 11-20
  case 2: return itvol-65; break; // 21-30
  case 3: return itvol-65; break; // 31-40
  case 4: return itvol-65; break; // 41-50
  case 5: return itvol-65; break; // 51-60
  }
  return itvol;
}
int ImportIT() {
  // import IT file, after YEARS I wasn't able to do this.
  // check out http://schismtracker.org/wiki/ITTECH.TXT for specs in IT format
    int64_t size;
  char * memblock;
  int sk;
  bool istherenote=false;
  int samples;
  ALLEGRO_FILE *it;
  //string fn;
  int NextByte;
  int NextChannel;
  int CurrentRow;
  int NextMask[32];
  int LastNote[32];
  int LastIns[32];
  int LastVol[32];
  int LastFX[32];
  int LastFXVal[32];
  ALLEGRO_FILECHOOSER* filechooser;
  filechooser=al_create_native_file_dialog(".","load IT module",NULL,0);
  al_show_native_file_dialog(display,filechooser);
  char rfn[256];
  if (al_get_native_file_dialog_path(filechooser,0)!=NULL) {
  strcpy(rfn,al_get_native_file_dialog_path(filechooser,0));} else {printf("no file given\n");}
  al_destroy_native_file_dialog(filechooser);
  //int insparas[256];
  int patparas[256];
  //gets(rfn);
  it=al_fopen(rfn,"rb");
  if (it!=NULL) { // read the file
  printf("loading IT file, ");
    size=al_fsize(it);
  printf("%d bytes\n",size);
    memblock=new char[size];
    al_fseek(it,0,ALLEGRO_SEEK_SET);
    al_fread(it,memblock,size);
    al_fclose(it);
  if (memblock[0]=='I' && memblock[1]=='M' && memblock[2]=='P' && memblock[3]=='M') {
    printf("IT module detected\n");
    // name
    printf("module name is ");
    for (sk=4;sk<30;sk++) {
      name[sk-4]=memblock[sk];
      if (sk==4) {printf(&memblock[sk]);}
    }
    printf("\n");
    // orders, instruments, samples and patterns
    printf("%d orders, %d instruments, %d samples, %d patterns\n",(int)memblock[0x20],(int)memblock[0x22],(int)memblock[0x24],(int)memblock[0x26]);
    orders=(unsigned char)memblock[0x20]; instruments=(unsigned char)memblock[0x22]; patterns=(unsigned char)memblock[0x26]; samples=(unsigned char)memblock[0x24];
    //cout << (int)memblock[0x29] << "." << (int)memblock[0x28];
    printf("\n");
    //cout << "volumes: global " << (int)(unsigned char)memblock[0x30] << ", mixing " << (int)(unsigned char)memblock[0x31] << "\n";
    //cout << "speeds: " << (int)memblock[0x32] << ":" << (int)(unsigned char)memblock[0x33] << "\n";
    defspeed=memblock[0x32];
    printf("---pans---\n");
    for (sk=0x40;sk<0x60;sk++) {
      defchanpan[sk-64]=memblock[sk];
      printf("%d ",(int)memblock[sk]);
    }
    printf("\n");
    printf("---volumes---\n");
    for (sk=0x80;sk<0xa0;sk++) {
      defchanvol[sk-128]=memblock[sk]*2;
      printf("%d ",(int)memblock[sk]);
    }
    printf("\n");
    printf("---ORDER LIST---\n");
    for (sk=0xc0;sk<(0xc0+orders);sk++) {
      patid[sk-0xc0]=memblock[sk];
      switch(memblock[sk]) {
      case -2: printf("+++ "); break;
      case -1: printf("--- "); break;
      default: printf("%d ",(int)(unsigned char)memblock[sk]); break;
      }
    }
    printf("\n");
    // pointers
    printf("\n---POINTERS---\n");
    for (sk=0;sk<patterns;sk++) {
    patparas[sk]=((unsigned char)memblock[0xc0+orders+(instruments*4)+(samples*4)+(sk*4)])+(((unsigned char)memblock[0xc0+orders+(instruments*4)+(samples*4)+(sk*4)+1])*256)+(((unsigned char)memblock[0xc0+orders+(instruments*4)+(samples*4)+(sk*4)+2])*65536)+(((unsigned char)memblock[0xc0+orders+(instruments*4)+(samples*4)+(sk*4)+3])*16777216);
    printf("pattern %d offset: ",sk);
    printf("%d\n",patparas[sk]);
    }
    // load/unpack patterns
    for (int pointer=0;pointer<patterns;pointer++) {
    printf("-unpacking pattern %d-\n",pointer);
    CurrentRow=0;
    sk=patparas[pointer];
    int patsize=(unsigned char)memblock[sk]+((unsigned char)memblock[sk+1]*256);
    int plength=(unsigned char)memblock[sk+2]+((unsigned char)memblock[sk+3]*256);
    printf("%d bytes in pattern\n",patsize);
    patlength[pointer]=plength; // set length
    sk=patparas[pointer]+8;
    for (int a=0;a<patsize;a++) {
    NextByte=(unsigned char)memblock[sk+a];
    if (NextByte==0) {
      CurrentRow++;
      if (CurrentRow==plength) {break;}
      continue;
    }
    NextChannel=(NextByte-1)&31;
    if ((NextByte&128)==128) {
      a++;
      NextMask[NextChannel]=(unsigned char)memblock[sk+a];
    }
    if (NextMask[NextChannel]&1) {
      a++;
      // decode melodical byte into raw byte
      LastNote[NextChannel]=(unsigned char)memblock[sk+a];
      switch (LastNote[NextChannel]) {
        case 255: pat[pointer][CurrentRow][NextChannel][0]=13; break;
        case 254: pat[pointer][CurrentRow][NextChannel][0]=15; break;
        default: if (LastNote[NextChannel]<120) {pat[pointer][CurrentRow][NextChannel][0]=((LastNote[NextChannel]/12)*16)+(LastNote[NextChannel]%12)+1;/*pat[pointer][CurrentRow][NextChannel][2]=0x7f;*/} else {pat[pointer][CurrentRow][NextChannel][0]=14;}; break;
      }
    }
    if (NextMask[NextChannel]&2) {
      a++;
      LastIns[NextChannel]=(unsigned char)memblock[sk+a];
      pat[pointer][CurrentRow][NextChannel][1]=LastIns[NextChannel];
    }
    if (NextMask[NextChannel]&4) {
      a++;
      LastVol[NextChannel]=(unsigned char)memblock[sk+a];
      pat[pointer][CurrentRow][NextChannel][2]=ITVolumeConverter(LastVol[NextChannel]);
    }
    if (NextMask[NextChannel]&8) {
      a++;
      LastFX[NextChannel]=(unsigned char)memblock[sk+a];
      pat[pointer][CurrentRow][NextChannel][3]=LastFX[NextChannel];
      a++;
      LastFXVal[NextChannel]=(unsigned char)memblock[sk+a];
      pat[pointer][CurrentRow][NextChannel][4]=LastFXVal[NextChannel];
    }
    if (NextMask[NextChannel]&16) {
      switch (LastNote[NextChannel]) {
        case 255: pat[pointer][CurrentRow][NextChannel][0]=13; break;
        case 254: pat[pointer][CurrentRow][NextChannel][0]=15; break;
        default: if (LastNote[NextChannel]<120) {pat[pointer][CurrentRow][NextChannel][0]=((LastNote[NextChannel]/12)*16)+(LastNote[NextChannel]%12)+1;/*pat[pointer][CurrentRow][NextChannel][2]=0x7f;*/} else {pat[pointer][CurrentRow][NextChannel][0]=14;}; break;
      }
    }
    if (NextMask[NextChannel]&32) {
      pat[pointer][CurrentRow][NextChannel][1]=LastIns[NextChannel];
    }
    if (NextMask[NextChannel]&64) {
      pat[pointer][CurrentRow][NextChannel][2]=ITVolumeConverter(LastVol[NextChannel]);
    }
    if (NextMask[NextChannel]&128) {
      pat[pointer][CurrentRow][NextChannel][3]=LastFX[NextChannel];
      pat[pointer][CurrentRow][NextChannel][4]=LastFXVal[NextChannel];
    }
    }
  }

  } else {printf("error while importing file! not an IT module file\n"); delete[] memblock; return 1;}
    delete[] memblock;
  }
  else {printf("error while importing file! file doesn't exist\n"); return 1;}
  return 0;
}
int ImportMOD(const char* rfn) {
  // import MOD file
  // check out http://www.fileformat.info/format/mod/corion.htm for specs in MOD format
    int64_t size;
  char * memblock;
  int sk;
  bool istherenote=false;
  ALLEGRO_FILE *mod;
  //string fn;
  int chans;
  int CurrentSampleSeek=0;
  printf("\nplease write filename? ");
  mod=al_fopen(rfn,"rb");
  if (mod!=NULL) { // read the file
  printf("loading MOD file, ");
    size=al_fsize(mod);
  printf("%d bytes\n",size);
    memblock=new char[size];
    al_fseek(mod,0,ALLEGRO_SEEK_SET);
    al_fread(mod,memblock,size);
    al_fclose(mod);
    printf("success, now importing file\n");
  for (int nonsense=0;nonsense<256;nonsense++) {
    patlength[nonsense]=64;
    //instrument[nonsense].env[envPan]=48;
  }
#ifndef PRESERVE_INS
  for (int ii=0;ii<31;ii++) {
    for (int jj=0;jj<22;jj++) {
      instrument[ii+1].name[jj]=memblock[0x14+(ii*30)+jj];
    }
    if (settings::samples) {
      instrument[ii+1].pcmPos[1]=CurrentSampleSeek>>8;
      instrument[ii+1].pcmPos[0]=CurrentSampleSeek%256;
      instrument[ii+1].DFM|=(CurrentSampleSeek>>16)?(128):(0);
      int tempsize;
      tempsize=(((unsigned char)(memblock[0x14+(ii*30)+0x16])<<8)+(unsigned char)(memblock[0x14+(ii*30)+0x17]))*2;
      int repeatpos;
      repeatpos=(((unsigned char)(memblock[0x14+(ii*30)+0x1a])<<8)+(unsigned char)(memblock[0x14+(ii*30)+0x1b]))*2;
      int repeatlen;
      repeatlen=(((unsigned char)(memblock[0x14+(ii*30)+0x1c])<<8)+(unsigned char)(memblock[0x14+(ii*30)+0x1d]))*2;
      printf("sample %d size: %.5x repeat: %.4x replen: %.4x\n",ii,tempsize,repeatpos,repeatlen);
      instrument[ii+1].pcmLen=(repeatpos>0 || repeatlen>2)?(minval(tempsize,repeatpos+repeatlen)):(tempsize);
      instrument[ii+1].pcmLoop[1]=repeatpos>>8;
      instrument[ii+1].pcmLoop[0]=repeatpos&0xff;
      instrument[ii+1].pcmMult|=(repeatpos>0 || repeatlen>2)?(128):(0);
      CurrentSampleSeek+=tempsize;
      instrument[ii+1].noteOffset=12;
      instrument[ii+1].DFM|=8;
                        /*
    instrument[ii+1].pcmPos[0]=CurrentSampleSeek>>8;
    instrument[ii+1].pcmPos[1]=CurrentSampleSeek%256;
    instrument[ii+1].DFM|=(CurrentSampleSeek>>16)<<7;
    CurrentSampleSeek+=(((memblock[0x14+(ii*30)+22]<<8)+memblock[0x14+(ii*30)+23])*2);
    if ((((((memblock[0x14+(ii*30)+22]<<8)+memblock[0x14+(ii*30)+23])*2)>>4)%8)>3) {CurrentSampleSeek+=256;}
    instrument[ii+1][0x33]=(((memblock[0x14+(ii*30)+23]<<8)+memblock[0x14+(ii*30)+22])*2)>>8;
    instrument[ii+1][0x32]=(((memblock[0x14+(ii*30)+23]<<8)+memblock[0x14+(ii*30)+22])*2)%256;*/
    }
  }
#endif
  if ((memblock[1080]=='M' && memblock[1081]=='.' && memblock[1082]=='K' && memblock[1083]=='.')||
     (memblock[1081]=='C' && memblock[1082]=='H' && memblock[1083]=='N')||
     (memblock[1082]=='C' && memblock[1083]=='H')) {
       switch(memblock[1080]) {
      case 'M': printf("4-channel original MOD module detected\n"); chans=4; break;
      default: printf("multi-channel MOD module detected\n"); break;
       }
       if (memblock[1082]=='C' && memblock[1083]=='H') {chans=(NumberLetter(memblock[1080])*10)+NumberLetter(memblock[1081]);}
       if (memblock[1081]=='C' && memblock[1082]=='H' && memblock[1083]=='N') {chans=NumberLetter(memblock[1080]);}
    // name
    printf("module name is ");
    for (sk=0;sk<20;sk++) {
      name[sk]=memblock[sk];
      if (sk==0) {printf(&memblock[sk]);}
    }
    printf("\n");
    printf("---ORDER LIST---\n");
    for (sk=952;sk<1080;sk++) {
      patid[sk-952]=memblock[sk];
      switch(memblock[sk]) {
      case -2: printf("+++ "); break;
      case -1: printf("--- "); break;
      default: printf("%d ",(int)(unsigned char)memblock[sk]); break;
      }
    }
    printf("\nretrieving pattern count\n");
    patterns=0;
    for (sk=0;sk<128;sk++) {
      if (patid[sk]>patterns) {patterns=patid[sk];}
    }
    printf("%d patterns\n",patterns);
    if (settings::samples) {
    printf("putting samples to PCM memory if possible\n");
    printf("%d bytes",size-1084-(patterns*chans*64*4));
    memcpy(chip[0].pcm,memblock+1084+((patterns+1)*chans*64*4),minval(65280,size-1084-((patterns+1)*chans*64*4)));
    }
    printf("---PATTERNS---\n");
    for (int importid=0;importid<patterns+1;importid++) {
    printf("-PATTERN %d-\n",importid);
    /*if (verbose) {cout << "-Channel1- -Channel2- -Channel3- -Channel4- ";
    if (chans==6) {cout << "-Channel5- -Channel6-\n";} else {
      if (chans==8) {cout << "-Channel5- -Channel6- -Channel7- -Channel8-\n";} else {cout << "\n";}
    }}*/
    sk=1084; // import position
    for (int indxr=0;indxr<64;indxr++) {
      int NPERIOD;
      int NINS;
      int NFX;
      int NFXVAL;
      for (int ichan=0;ichan<chans;ichan++) {
        // import pattern row
        NPERIOD=(((unsigned char)memblock[sk+(importid*(chans*256))+(indxr*(chans*4))+(ichan*4)])%16*256)+((unsigned char)memblock[sk+(importid*(chans*256))+(indxr*(chans*4))+(ichan*4)+1]);
        NINS=(((unsigned char)memblock[sk+(importid*(chans*256))+(indxr*(chans*4))+(ichan*4)]>>4)*16)+((unsigned char)memblock[sk+(importid*(chans*256))+(indxr*(chans*4))+(ichan*4)+2]>>4);
        NFX=(unsigned char)memblock[sk+(importid*(chans*256))+(indxr*(chans*4))+(ichan*4)+2]%16;
        NFXVAL=(unsigned char)memblock[sk+(importid*(chans*256))+(indxr*(chans*4))+(ichan*4)+3];
        // conversion stuff
        istherenote=true;
        switch(NPERIOD) {
          case 56: if (verbose) printf("B-7 "); pat[importid][indxr][ichan][0]=0x7c; break;
          case 60: if (verbose) printf("A#7 "); pat[importid][indxr][ichan][0]=0x7b; break;
          case 63: if (verbose) printf("A-7 "); pat[importid][indxr][ichan][0]=0x7a; break;
          case 67: if (verbose) printf("G#7 "); pat[importid][indxr][ichan][0]=0x79; break;
          case 71: if (verbose) printf("G-7 "); pat[importid][indxr][ichan][0]=0x78; break;
          case 75: if (verbose) printf("F#7 "); pat[importid][indxr][ichan][0]=0x77; break;
          case 80: if (verbose) printf("F-7 "); pat[importid][indxr][ichan][0]=0x76; break;
          case 85: if (verbose) printf("E-7 "); pat[importid][indxr][ichan][0]=0x75; break;
          case 90: if (verbose) printf("D#7 "); pat[importid][indxr][ichan][0]=0x74; break;
          case 95: if (verbose) printf("D-7 "); pat[importid][indxr][ichan][0]=0x73; break;
          case 101: if (verbose) printf("C#7 "); pat[importid][indxr][ichan][0]=0x72; break;
          case 107: if (verbose) printf("C-7 "); pat[importid][indxr][ichan][0]=0x71; break;
          case 113: if (verbose) printf("B-6 "); pat[importid][indxr][ichan][0]=0x6c; break;
          case 120: if (verbose) printf("A#6 "); pat[importid][indxr][ichan][0]=0x6b; break;
          case 127: if (verbose) printf("A-6 "); pat[importid][indxr][ichan][0]=0x6a; break;
          case 135: if (verbose) printf("G#6 "); pat[importid][indxr][ichan][0]=0x69; break;
          case 143: if (verbose) printf("G-6 "); pat[importid][indxr][ichan][0]=0x68; break;
          case 151: if (verbose) printf("F#6 "); pat[importid][indxr][ichan][0]=0x67; break;
          case 160: if (verbose) printf("F-6 "); pat[importid][indxr][ichan][0]=0x66; break;
          case 170: if (verbose) printf("E-6 "); pat[importid][indxr][ichan][0]=0x65; break;
          case 180: if (verbose) printf("D#6 "); pat[importid][indxr][ichan][0]=0x64; break;
          case 190: if (verbose) printf("D-6 "); pat[importid][indxr][ichan][0]=0x63; break;
          case 202: if (verbose) printf("C#6 "); pat[importid][indxr][ichan][0]=0x62; break;
          case 214: if (verbose) printf("C-6 "); pat[importid][indxr][ichan][0]=0x61; break;
          case 226: if (verbose) printf("B-5 "); pat[importid][indxr][ichan][0]=0x5c; break;
          case 240: if (verbose) printf("A#5 "); pat[importid][indxr][ichan][0]=0x5b; break;
          case 254: if (verbose) printf("A-5 "); pat[importid][indxr][ichan][0]=0x5a; break;
          case 269: if (verbose) printf("G#5 "); pat[importid][indxr][ichan][0]=0x59; break;
          case 285: if (verbose) printf("G-5 "); pat[importid][indxr][ichan][0]=0x58; break;
          case 302: if (verbose) printf("F#5 "); pat[importid][indxr][ichan][0]=0x57; break;
          case 320: if (verbose) printf("F-5 "); pat[importid][indxr][ichan][0]=0x56; break;
          case 339: if (verbose) printf("E-5 "); pat[importid][indxr][ichan][0]=0x55; break;
          case 360: if (verbose) printf("D#5 "); pat[importid][indxr][ichan][0]=0x54; break;
          case 381: if (verbose) printf("D-5 "); pat[importid][indxr][ichan][0]=0x53; break;
          case 404: if (verbose) printf("C#5 "); pat[importid][indxr][ichan][0]=0x52; break;
          case 428: if (verbose) printf("C-5 "); pat[importid][indxr][ichan][0]=0x51; break;
          case 453: if (verbose) printf("B-4 "); pat[importid][indxr][ichan][0]=0x4c; break;
          case 480: if (verbose) printf("A#4 "); pat[importid][indxr][ichan][0]=0x4b; break;
          case 508: if (verbose) printf("A-4 "); pat[importid][indxr][ichan][0]=0x4a; break;
          case 538: if (verbose) printf("G#4 "); pat[importid][indxr][ichan][0]=0x49; break;
          case 570: if (verbose) printf("G-4 "); pat[importid][indxr][ichan][0]=0x48; break;
          case 604: if (verbose) printf("F#4 "); pat[importid][indxr][ichan][0]=0x47; break;
          case 640: if (verbose) printf("F-4 "); pat[importid][indxr][ichan][0]=0x46; break;
          case 678: if (verbose) printf("E-4 "); pat[importid][indxr][ichan][0]=0x45; break;
          case 720: if (verbose) printf("D#4 "); pat[importid][indxr][ichan][0]=0x44; break;
          case 762: if (verbose) printf("D-4 "); pat[importid][indxr][ichan][0]=0x43; break;
          case 808: if (verbose) printf("C#4 "); pat[importid][indxr][ichan][0]=0x42; break;
          case 856: if (verbose) printf("C-4 "); pat[importid][indxr][ichan][0]=0x41; break;
          case 906: if (verbose) printf("B-3 "); pat[importid][indxr][ichan][0]=0x3c; break;
          case 907: if (verbose) printf("B-3 "); pat[importid][indxr][ichan][0]=0x3c; break; // OpenMPT?
          case 960: if (verbose) printf("A#3 "); pat[importid][indxr][ichan][0]=0x3b; break;
          case 1016: if (verbose) printf("A-3 "); pat[importid][indxr][ichan][0]=0x3a; break;
          case 1076: if (verbose) printf("G#3 "); pat[importid][indxr][ichan][0]=0x39; break;
          case 1140: if (verbose) printf("G-3 "); pat[importid][indxr][ichan][0]=0x38; break;
          case 1208: if (verbose) printf("F#3 "); pat[importid][indxr][ichan][0]=0x37; break;
          case 1280: if (verbose) printf("F-3 "); pat[importid][indxr][ichan][0]=0x36; break;
          case 1356: if (verbose) printf("E-3 "); pat[importid][indxr][ichan][0]=0x35; break;
          case 1440: if (verbose) printf("D#3 "); pat[importid][indxr][ichan][0]=0x34; break;
          case 1524: if (verbose) printf("D-3 "); pat[importid][indxr][ichan][0]=0x33; break;
          case 1616: if (verbose) printf("C#3 "); pat[importid][indxr][ichan][0]=0x32; break;
          case 1712: if (verbose) printf("C-3 "); pat[importid][indxr][ichan][0]=0x31; break;
          case 0: if (verbose) printf("--- "); pat[importid][indxr][ichan][0]=0x00; istherenote=false; break;
          default: if (verbose) printf("??? "); pat[importid][indxr][ichan][0]=0x00; istherenote=false; printf("invalid note! %d at row %d channel %d\n",NPERIOD,indxr,ichan); break;
        }
        //if (verbose) cout << gethnibble(NINS) << getlnibble(NINS) << " ";
        pat[importid][indxr][ichan][1]=NINS;
        //if (verbose) cout << getlnibble(NFX) << gethnibble(NFXVAL) << getlnibble(NFXVAL) << " ";
        switch(NFX) {
          case 0: if (NFXVAL!=0) {pat[importid][indxr][ichan][3]=10;pat[importid][indxr][ichan][4]=NFXVAL;} else {pat[importid][indxr][ichan][3]=0;pat[importid][indxr][ichan][4]=0;}; break;
          case 1: pat[importid][indxr][ichan][3]=6;pat[importid][indxr][ichan][4]=NFXVAL; break;
          case 2: pat[importid][indxr][ichan][3]=5;pat[importid][indxr][ichan][4]=NFXVAL; break;
          case 3: pat[importid][indxr][ichan][3]=7;pat[importid][indxr][ichan][4]=NFXVAL;istherenote=false; break;
          case 4: pat[importid][indxr][ichan][3]=8;pat[importid][indxr][ichan][4]=NFXVAL; break;
          case 5: pat[importid][indxr][ichan][3]=12;pat[importid][indxr][ichan][4]=NFXVAL;istherenote=false; break;
          case 6: pat[importid][indxr][ichan][3]=11;pat[importid][indxr][ichan][4]=NFXVAL; break;
          case 7: pat[importid][indxr][ichan][3]=18;pat[importid][indxr][ichan][4]=NFXVAL; break;
          case 8: pat[importid][indxr][ichan][3]=24;pat[importid][indxr][ichan][4]=NFXVAL; break;
          case 9: pat[importid][indxr][ichan][3]=15;pat[importid][indxr][ichan][4]=NFXVAL; break;
          case 10: pat[importid][indxr][ichan][3]=4;pat[importid][indxr][ichan][4]=NFXVAL; break;
          case 11: pat[importid][indxr][ichan][3]=2;pat[importid][indxr][ichan][4]=NFXVAL; break;
          case 12: pat[importid][indxr][ichan][2]=0x40+minval(NFXVAL,0x3f);pat[importid][indxr][ichan][4]=0;istherenote=false; break;
          case 13: pat[importid][indxr][ichan][3]=3;pat[importid][indxr][ichan][4]=NFXVAL; break;
          case 14: pat[importid][indxr][ichan][3]=19;switch (NFXVAL>>4) {
            case 1: pat[importid][indxr][ichan][4]=0xf0+(NFXVAL%16); pat[importid][indxr][ichan][3]=6; break;
            case 2: pat[importid][indxr][ichan][4]=0xf0+(NFXVAL%16); pat[importid][indxr][ichan][3]=5; break;
            case 3: pat[importid][indxr][ichan][4]=0x20+(NFXVAL%16); break;
            case 4: pat[importid][indxr][ichan][4]=0x30+(NFXVAL%16); break;
            case 6: pat[importid][indxr][ichan][4]=0xb0+(NFXVAL%16); break;
            case 7: pat[importid][indxr][ichan][4]=0x40+(NFXVAL%16); break;
            case 9: pat[importid][indxr][ichan][4]=(NFXVAL%16); pat[importid][indxr][ichan][3]=17; break;
            case 10: pat[importid][indxr][ichan][4]=0x0f+((NFXVAL%16)<<4); pat[importid][indxr][ichan][3]=4; break;
            case 11: pat[importid][indxr][ichan][4]=0xf0+(NFXVAL%16); pat[importid][indxr][ichan][3]=4; break;
            default: pat[importid][indxr][ichan][4]=NFXVAL; break;
            }; break;
          case 15: pat[importid][indxr][ichan][3]=1;pat[importid][indxr][ichan][4]=NFXVAL;if (NFXVAL>0x20) {pat[importid][indxr][ichan][3]=20;}; break;
        }
        //if (istherenote) {pat[importid][indxr][ichan][2]=0x7f;}
      }
      //if (verbose) cout << "\n";
    }
    }

  } else {/*cout << "error while importing file! not a MOD module file\n";*/ delete[] memblock; return 1;}
    delete[] memblock;
  }
  else {/*cout << "error while importing file! file doesn't exist\n";*/ return 1;}
  return 0;
}
int ImportS3M() {
  // import S3M file
    int64_t size;
  char * memblock;
  int sk;
  int NextByte;
  int NextChannel;
  int CurrentRow;
  bool istherenote=false;
  ALLEGRO_FILE *s3m;
  int insparas[99];
  int patparas[256];
  //string fn;
  printf("\nplease write filename? ");
  char rfn[256];
  //gets(rfn);
  s3m=al_fopen(rfn,"rb");
  if (s3m!=NULL) { // read the file
  printf("loading S3M file, ");
    size=al_fsize(s3m);
  printf("%d bytes\n",size);
    memblock=new char[size];
    al_fseek(s3m,0,ALLEGRO_SEEK_SET);
    al_fread(s3m,memblock,size);
    al_fclose(s3m);
    printf("success, now importing file\n");
  for (int nonsense=0;nonsense<256;nonsense++) {
    patlength[nonsense]=64;
    //instrument[nonsense].env[envPan]=48;
  }
  // module name
  printf("module name is ");
  for (sk=0;sk<28;sk++) {
    name[sk]=memblock[sk];
    if (sk==0) {printf(&memblock[sk]);}
  }
  printf("\n");
  orders=memblock[0x20];
  instruments=memblock[0x22];
  patterns=memblock[0x24]*2;
  printf("%d orders, %d instruments, %d patterns\n",orders,instruments,patterns);
  // order list
  printf("---ORDER LIST---\n");
  for (sk=0x60;sk<orders+0x60;sk++) {
    patid[sk-0x60]=memblock[sk];
    switch(memblock[sk]) {
      case -2: printf("+++ "); break;
      case -1: printf("--- "); break;
      default: printf("%d ",(int)(unsigned char)memblock[sk]); break;
      }
    }
  // pointers
  printf("\n---POINTERS---\n");
  for (sk=0x60+orders;sk<(0x60+orders+instruments);sk+=2) {
    insparas[(sk-(0x60+orders))/2]=((unsigned char)memblock[sk]*16)+((unsigned char)memblock[sk+1]*4096);
    printf("instrument %d offset: ",(sk-(0x60+orders))/2);
    printf("%d\n",insparas[(sk-(0x60+orders))/2]);
    }
  for (sk=0x60+orders+(instruments*2);sk<(0x60+orders+(instruments*2)+patterns);sk+=2) {
    patparas[(sk-(0x60+orders+(instruments*2)))/2]=((unsigned char)memblock[sk]*16)+((unsigned char)memblock[sk+1]*4096);
    printf("pattern %d offset: ",(sk-(0x60+orders+(instruments*2)))/2);
    printf("%d\n",patparas[(sk-(0x60+orders+(instruments*2)))/2]);
    }
  // unpack patterns
  for (int pointer=0;pointer<(patterns/2);pointer++) {
    printf("-unpacking pattern %d-\n",pointer);
    CurrentRow=0;
    sk=patparas[pointer];
    int patsize=(unsigned char)memblock[sk]+((unsigned char)memblock[sk+1]*256);
    printf("%d bytes in pattern\n",patsize);
    sk=patparas[pointer]+2;
    for (int a=0;a<patsize;a++) {
    NextByte=(unsigned char)memblock[sk+a];
    if (NextByte==0) {
      CurrentRow++;
      if (CurrentRow==64) {break;}
      continue;
    }
    NextChannel=NextByte%32;
    if ((NextByte>>5)%2) {
      a++;
      pat[pointer][CurrentRow][NextChannel][0]=(unsigned char)memblock[sk+a]+17;
      a++;
      pat[pointer][CurrentRow][NextChannel][1]=(unsigned char)memblock[sk+a];
      //pat[pointer][CurrentRow][NextChannel][2]=127;
    }
    if ((NextByte>>6)%2) {
      a++;
      pat[pointer][CurrentRow][NextChannel][2]=minval(127,(unsigned char)64+memblock[sk+a]);
    }
    if ((NextByte>>7)%2) {
      a++;
      pat[pointer][CurrentRow][NextChannel][3]=(unsigned char)memblock[sk+a];
      a++;
      pat[pointer][CurrentRow][NextChannel][4]=(unsigned char)memblock[sk+a];
    }
    }
  }


  }
  delete[] memblock;
  return 0;
}
int SaveFile() {
  // save file
  ALLEGRO_FILE *sfile;
  //printf("\nplease write filename? ");
  //char rfn[256];
  int sk=0;
  int maskdata=0; // mask byte
  int CPL=0; // current pattern packlength
  int insparas[256];
  int patparas[256];
  int seqparas[256];
  int commentpointer=0;
  int pcmpointer=0;
  bool IS_INS_BLANK[256];
  bool IS_PAT_BLANK[256];
  bool IS_SEQ_BLANK[256];
  int oldseek=0;
  
  // temporary, gonna get replaced by a better thing soon
  // just for the sake of linux
  ALLEGRO_FILECHOOSER* filechooser;
  filechooser=al_create_native_file_dialog(curdir,"save",NULL,ALLEGRO_FILECHOOSER_SAVE);
  al_show_native_file_dialog(display,filechooser);
  char rfn[256];
  if (al_get_native_file_dialog_path(filechooser,0)!=NULL) {
  strcpy(rfn,al_get_native_file_dialog_path(filechooser,0));} else {printf("no file given\n");}
  al_destroy_native_file_dialog(filechooser);
  
  sfile=al_fopen(rfn,"wb");
  if (sfile!=NULL) { // write the file
    al_fseek(sfile,0,ALLEGRO_SEEK_SET); // seek to 0
    printf("writing headers...\n");
    printf("%d ",al_ftell(sfile));
    al_fputs(sfile,"TRACK8BT"); // magic number
    printf("%d ",al_ftell(sfile));
    al_fwrite16le(sfile,ver); // version
    printf("%d ",al_ftell(sfile));
    al_fputc(sfile,instruments); // instruments
    al_fputc(sfile,patterns); // patterns
    al_fputc(sfile,orders); // orders
    al_fputc(sfile,defspeed); // speed
    al_fputc(sfile,seqs); // sequences
    al_fputc(sfile,125); // tempo
    al_fputs(sfile,name); // name
    printf("%d ",al_ftell(sfile));
    al_fseek(sfile,48,ALLEGRO_SEEK_SET); // seek to 0x30
    printf("%d ",al_ftell(sfile));
    al_fputc(sfile,0); // default filter mode
    al_fputc(sfile,32); // channels
    al_fwrite16le(sfile,0); // flags
    al_fputc(sfile,128); // global volume
    al_fputc(sfile,0); // global panning
    al_fwrite32le(sfile,0); // mute flags
    al_fwrite32le(sfile,0); // PCM data pointer
    al_fwrite16le(sfile,0); // reserved
    al_fseek(sfile,0x3e,ALLEGRO_SEEK_SET); // seek to 0x3e
    al_fputc(sfile,songdf); // detune factor
    printf("%d ",al_ftell(sfile));
    al_fseek(sfile,0x80,ALLEGRO_SEEK_SET); // seek to 0x80
    printf("%d ",al_ftell(sfile));
    for (int ii=0; ii<256; ii++) {
      al_fputc(sfile,patid[ii]); // order list
    }
    printf("writing instruments...\n");
    al_fseek(sfile,0xd80,ALLEGRO_SEEK_SET); // seek to 0xD80, and start writing the instruments
    sk=0xd80;
    for (int ii=0; ii<256; ii++) {
      IS_INS_BLANK[ii]=true;
      // check if the instrument is blank
      for (int ii1=0; ii1<64; ii1++) {
        if (ii1==0x2b) {
          if (instrument[ii].noteOffset!=48) {IS_INS_BLANK[ii]=false;break;}
        } else {
          if (((unsigned char*)&instrument[ii])[ii1]!=0) {IS_INS_BLANK[ii]=false;break;}
        }
      }
      if (IS_INS_BLANK[ii]) {
        insparas[ii]=0;continue;
      }
      insparas[ii]=al_ftell(sfile);
      al_fwrite(sfile,&instrument[ii],64);
    }
    printf("writing sequences...\n");
    //sk=0x4d80;
    //al_fseek(sfile,0x4d80,ALLEGRO_SEEK_SET); // seek to 0x4D80, and start writing the sequence tables
    for (int ii=0; ii<256; ii++) {
      IS_SEQ_BLANK[ii]=true;
      for (int ii1=0; ii1<8; ii1++) {
        for (int ii2=0; ii2<256; ii2++) {
            if (ii2==254 || ii2==255) {
              if (bytable[ii1][ii][ii2]!=255) {IS_SEQ_BLANK[ii]=false;break;}
            } else {
              if (bytable[ii1][ii][ii2]!=0) {IS_SEQ_BLANK[ii]=false;break;}
            }
        }
        if (!IS_SEQ_BLANK[ii]) {break;}
      }
      if (IS_SEQ_BLANK[ii]) {
        seqparas[ii]=0;continue;
      }
      seqparas[ii]=al_ftell(sfile);
      for (int jj=0; jj<8; jj++) {
                                al_fputc(sfile,bytable[jj][ii][253]);
                                al_fputc(sfile,bytable[jj][ii][254]);
                                al_fputc(sfile,bytable[jj][ii][255]);
        for (int kk=0; kk<(bytable[jj][ii][253]+1); kk++) {
                                  al_fputc(sfile,bytable[jj][ii][kk]); // seqtable
        }
      }
    }
    //sk=0x84d80;
    printf("packing/writing patterns...\n");
    //al_fseek(sfile,0x84d80,ALLEGRO_SEEK_SET); // seek to 0x84D80, and start packing the patterns for further writing
    // pattern packer
    for (int ii=0; ii<256; ii++) {
      IS_PAT_BLANK[ii]=true;
      for (int ii1=0; ii1<256; ii1++) {
        for (int ii2=0; ii2<32; ii2++) {
          for (int ii3=0; ii3<5; ii3++) {
            if (pat[ii][ii1][ii2][ii3]!=0) {IS_PAT_BLANK[ii]=false;break;}
          }
          if (!IS_PAT_BLANK[ii]) {break;}
        }
        if (!IS_PAT_BLANK[ii]) {break;}
      }
      if (IS_PAT_BLANK[ii]) {
        patparas[ii]=0;continue;
      }
      CPL=0; // reset the size
      sk=al_ftell(sfile);
      patparas[ii]=sk;
      sk+=5; // increase the seek
      al_fseek(sfile,sk,ALLEGRO_SEEK_SET);
      al_fputc(sfile,patlength[ii]);
      sk+=11;
      al_fseek(sfile,sk,ALLEGRO_SEEK_SET);
      for (int jj=0; jj<getpatlen(ii); jj++) {
        // pack row
        for (int cpack=0; cpack<32; cpack++) {
          // first check if channel is used
          if (pat[ii][jj][cpack][0]==0 &&
            pat[ii][jj][cpack][1]==0 &&
            pat[ii][jj][cpack][2]==0 &&
            pat[ii][jj][cpack][3]==0 &&
            pat[ii][jj][cpack][4]==0) {continue;} // channel is unused
          // then pack this channel
          maskdata=cpack; // set maskbyte to channel number
          if (pat[ii][jj][cpack][0]!=0 || pat[ii][jj][cpack][1]!=0) {maskdata=maskdata|32;} // note AND/OR instrument
          if (pat[ii][jj][cpack][2]!=0) {maskdata=maskdata|64;} // volume value
          if (pat[ii][jj][cpack][3]!=0 || pat[ii][jj][cpack][4]!=0) {maskdata=maskdata|128;} // effect AND/OR effect value
          al_fputc(sfile,maskdata); CPL++; // write maskbyte
          if (maskdata&32) {al_fputc(sfile,pat[ii][jj][cpack][0]);al_fputc(sfile,pat[ii][jj][cpack][1]); CPL+=2;} // write NOTE and INSTRUMENT if required
          if (maskdata&64) {al_fputc(sfile,pat[ii][jj][cpack][2]); CPL++;} // write VOLUME if required
          if (maskdata&128) {al_fputc(sfile,pat[ii][jj][cpack][3]);al_fputc(sfile,pat[ii][jj][cpack][4]); CPL+=2;} // write EFFECT and EFFECT VALUE if required
        }
        al_fputc(sfile,0); // write end of row
        CPL++;
      }
      oldseek=al_ftell(sfile);
      al_fseek(sfile,patparas[ii]+1,ALLEGRO_SEEK_SET);
      al_fwrite32le(sfile,CPL);
      al_fseek(sfile,oldseek,ALLEGRO_SEEK_SET);
    }
    printf("%d ",al_ftell(sfile));
    // write comments
    commentpointer=al_ftell(sfile);
    al_fwrite(sfile,comments,strlen(comments));
    al_fseek(sfile,strlen(comments)+1,ALLEGRO_SEEK_CUR);
    printf("%d ",al_ftell(sfile));
    // write PCM data
    pcmpointer=al_ftell(sfile);
    bool IS_PCM_DATA_BLANK=true;
    int maxpcmwrite=0;
    for (int ii=0;ii<65280;ii++) {
      if (chip[0].pcm[ii]!=0) {IS_PCM_DATA_BLANK=false; maxpcmwrite=ii;}
    }
    if (!IS_PCM_DATA_BLANK) {
      al_fwrite32le(sfile,maxpcmwrite);
      al_fwrite(sfile,chip[0].pcm,maxpcmwrite);
    } else {pcmpointer=0;}
    // write pointers
    printf("writing offsets...\n");
    al_fseek(sfile,0x180,ALLEGRO_SEEK_SET);
    for (int ii=0;ii<256;ii++) {
      al_fwrite32le(sfile,insparas[ii]);
    }
    for (int ii=0;ii<256;ii++) {
      al_fwrite32le(sfile,seqparas[ii]);
    }
    for (int ii=0;ii<256;ii++) {
      al_fwrite32le(sfile,patparas[ii]);
    }
    al_fseek(sfile,0x3a,ALLEGRO_SEEK_SET);
    al_fwrite32le(sfile,commentpointer);
    al_fseek(sfile,0x36,ALLEGRO_SEEK_SET);
    al_fwrite32le(sfile,pcmpointer);
    al_fclose(sfile);
    printf("done\n");
    return 0;
  }
  return 1;
}
int LoadFile(const char* filename) {
  // load file
  ALLEGRO_FILE *sfile;
  int sk=0;
  int CurrentRow=0;
  int maskdata=0; // mask byte
  int NextByte=0;
  int NextChannel=0;
  int CPL=0; // current pattern packlength
  int insparas[256];
  int patparas[256];
  int seqparas[256];
  int commentpointer=0;
  int pcmpointer=0;
  int maxpcmread=0;
  int TVER;
  int oplaymode;
  bool IS_SEQ_BLANK[256];
  char *checkstr=new char[8];
  sfile=al_fopen(filename,"rb");
  if (sfile!=NULL) { // LOAD the file
    al_fseek(sfile,0,ALLEGRO_SEEK_SET); // seek to 0
    printf("loading file...\n");
    //printf("%d ",al_ftell(sfile));
    al_fgets(sfile,checkstr,9); // magic number
    //printf("%s",checkstr);
    if (checkstr[0]!='T' ||
      checkstr[1]!='R' ||
      checkstr[2]!='A' ||
      checkstr[3]!='C' ||
      checkstr[4]!='K' ||
      checkstr[5]!='8' ||
      checkstr[6]!='B' ||
      checkstr[7]!='T') {printf("error: not a soundtracker file!\n");al_fclose(sfile);
    #ifdef SOUNDS
    cursfx=1;sfxpos=0;
    #endif
    return 1;}
    oplaymode=playmode;
    playmode=0;
    CleanupPatterns();
    //printf("%d ",al_ftell(sfile));
    TVER=al_fread16le(sfile); // version
    printf("module version %d\n",TVER);
    if (TVER<60) {printf("-applying filter mode compatibility\n");}
    if (TVER<65) {printf("-applying volume column compatibility\n");}
    if (TVER<106) {printf("-applying loop point fix compatibility\n");}
    if (TVER<143) {printf("-applying old sequence format compatibility\n");}
    if (TVER<144) {printf("-applying endianness compatibility\n");}
    //if (TVER<145) {printf("-applying legacy instrument compatibility\n");}
    //printf("%d ",al_ftell(sfile));
    instruments=al_fgetc(sfile); // instruments
    patterns=al_fgetc(sfile); // patterns
    orders=al_fgetc(sfile); // orders
    defspeed=al_fgetc(sfile); // speed
    seqs=al_fgetc(sfile); // sequences
    //al_fputc(sfile,125); // tempo
    al_fseek(sfile,16,ALLEGRO_SEEK_SET);
    al_fgets(sfile,name,32); // name
    //printf("%d ",al_ftell(sfile));
    al_fseek(sfile,48,ALLEGRO_SEEK_SET); // seek to 0x30
    //printf("%d ",al_ftell(sfile));
    //al_fputc(sfile,0); // default filter mode
    //al_fputc(sfile,32); // channels
    //al_fwrite16le(sfile,0); // flags
    //al_fputc(sfile,128); // global volume
    //al_fputc(sfile,0); // global panning
    //al_fwrite32le(sfile,0); // mute flags
    //al_fwrite32le(sfile,0); // PCM data pointer
    //al_fwrite16le(sfile,0); // reserved
    al_fseek(sfile,0x3e,ALLEGRO_SEEK_SET); // seek to 0x3e
    songdf=al_fgetc(sfile); // detune factor
    //printf("%d ",al_ftell(sfile));
    al_fseek(sfile,0x80,ALLEGRO_SEEK_SET); // seek to 0x80
    //printf("%d ",al_ftell(sfile));
    for (int ii=0; ii<256; ii++) {
      patid[ii]=al_fgetc(sfile); // order list
    }
    al_fseek(sfile,0x3a,ALLEGRO_SEEK_SET); // seek to 0x3a
    memset(comments,0,65536); // clean comments
    commentpointer=al_fread32le(sfile);
    if (commentpointer!=0) {
    al_fseek(sfile,commentpointer,ALLEGRO_SEEK_SET);
    al_fgets(sfile,comments,65536);
    }
    al_fseek(sfile,0x36,ALLEGRO_SEEK_SET); // seek to 0x36
    memset(chip[0].pcm,0,65280); // clean PCM memory
    pcmpointer=al_fread32le(sfile);
    if (pcmpointer!=0) {
      al_fseek(sfile,pcmpointer,ALLEGRO_SEEK_SET);
      maxpcmread=min(65280,al_fread32le(sfile));
      al_fread(sfile,chip[0].pcm,maxpcmread);
    }
    al_fseek(sfile,0x180,ALLEGRO_SEEK_SET);
    for (int ii=0;ii<256;ii++) {
      insparas[ii]=al_fread32le(sfile);
    }
    for (int ii=0;ii<256;ii++) {
      seqparas[ii]=al_fread32le(sfile);
    }
    for (int ii=0;ii<256;ii++) {
      patparas[ii]=al_fread32le(sfile);
    }
    //printf("%d ",al_ftell(sfile));
    //printf("reading instruments...\n");
    for (int ii=0; ii<256; ii++) {
      al_fseek(sfile,insparas[ii],ALLEGRO_SEEK_SET);
      // is it blank?
      if (insparas[ii]==0) {continue;}
      al_fread(sfile,&instrument[ii],64);
      // version<60 filter mode fix
      if (TVER<60) {
        if (instrument[ii].activeEnv&2) {instrument[ii].DFM^=1;}
      }
      // version<144 endianness
      if (TVER<144) {
        instrument[ii].pcmLen=bswapu16(instrument[ii].pcmLen);
        instrument[ii].pcmPos[0]^=instrument[ii].pcmPos[1];
        instrument[ii].pcmPos[1]^=instrument[ii].pcmPos[0];
        instrument[ii].pcmPos[0]^=instrument[ii].pcmPos[1];
        instrument[ii].pcmLoop[0]^=instrument[ii].pcmLoop[1];
        instrument[ii].pcmLoop[1]^=instrument[ii].pcmLoop[0];
        instrument[ii].pcmLoop[0]^=instrument[ii].pcmLoop[1];
        instrument[ii].filterH=bswapu16(instrument[ii].filterH);
      }
      
      // version<145 force legacy instrument
      /*
      if (TVER<144) {
        instrument[ii].ver&=~0x8000;
      }
      if (instrument[ii].ver&0x8000) { // new instrument
        
      }*/
    }
    //printf("reading sequences...\n");
    if (TVER<143) { // old sequence format
    for (int ii=0; ii<256; ii++) { // right now this is a full dump... we'll later fix this
      al_fseek(sfile,seqparas[ii],ALLEGRO_SEEK_SET);
      // is it blank?
      if (seqparas[ii]==0) {continue;}
      for (int jj=0; jj<8; jj++) {
        for (int kk=0; kk<256; kk++) {
        bytable[jj][ii][kk]=al_fgetc(sfile); // seqtable
        }
      }
      // version<106 loop point fix conversion
      if (TVER<106) {
      IS_SEQ_BLANK[ii]=true;
      for (int ii1=0; ii1<8; ii1++) {
        for (int ii2=0; ii2<256; ii2++) {
            if (ii2==254 || ii2==255) {
              if (bytable[ii1][ii][ii2]!=255) {IS_SEQ_BLANK[ii]=false;break;}
            } else {
              if (bytable[ii1][ii][ii2]!=0) {IS_SEQ_BLANK[ii]=false;break;}
            }
        }
        if (!IS_SEQ_BLANK[ii]) {break;}
      }
      if (!IS_SEQ_BLANK[ii]) {
        if (bytable[0][ii][254]<252) {bytable[0][ii][254]=1;}
        if (bytable[1][ii][254]<252) {bytable[1][ii][254]=1;}
        if (bytable[2][ii][254]<252) {bytable[2][ii][254]=1;}
        if (bytable[3][ii][254]<252) {bytable[3][ii][254]=1;}
        if (bytable[4][ii][254]<252) {bytable[4][ii][254]=1;}
        if (bytable[5][ii][254]<252) {bytable[5][ii][254]=1;}
        if (bytable[6][ii][254]<252) {bytable[6][ii][254]=1;}
      }
      }
    }
                } else {
                  for (int ii=0; ii<256; ii++) {
      al_fseek(sfile,seqparas[ii],ALLEGRO_SEEK_SET);
      // is it blank?
      if (seqparas[ii]==0) {continue;}
      for (int jj=0; jj<8; jj++) {
                          bytable[jj][ii][253]=al_fgetc(sfile);
                          bytable[jj][ii][254]=al_fgetc(sfile);
                          bytable[jj][ii][255]=al_fgetc(sfile);
        for (int kk=0; kk<bytable[jj][ii][253]+1; kk++) {
         // seqtable
                                  bytable[jj][ii][kk]=al_fgetc(sfile);
        }
      }
                }
                }
  // unpack patterns
  for (int pointer=0;pointer<256;pointer++) {
    // is it blank?
    if (patparas[pointer]==0) {patlength[pointer]=64;continue;}
    //printf("-unpacking pattern %d-\n",pointer);
    CurrentRow=0;
    sk=patparas[pointer];
    al_fseek(sfile,sk+1,ALLEGRO_SEEK_SET);
    int patsize=al_fread32le(sfile);
    //printf("%d bytes in pattern\n",patsize);
    patlength[pointer]=al_fgetc(sfile);
    sk=patparas[pointer]+16;
    al_fseek(sfile,sk,ALLEGRO_SEEK_SET);
    for (int a=0;a<patsize;a++) {
    NextByte=al_fgetc(sfile);
    if (NextByte==0) {
      CurrentRow++;
      if (CurrentRow==patlength[pointer]) {break;}
      continue;
    }
    NextChannel=NextByte%32;
    if ((NextByte>>5)%2) {
      a++;
      pat[pointer][CurrentRow][NextChannel][0]=al_fgetc(sfile);
      a++;
      pat[pointer][CurrentRow][NextChannel][1]=al_fgetc(sfile);
    }
    if ((NextByte>>6)%2) {
      a++;
      pat[pointer][CurrentRow][NextChannel][2]=al_fgetc(sfile);
      // version<65 volume fix
      if (TVER<65) {
        if (pat[pointer][CurrentRow][NextChannel][0]!=0 && pat[pointer][CurrentRow][NextChannel][2]==0x7f) {pat[pointer][CurrentRow][NextChannel][2]=0;}
      }
    }
    if ((NextByte>>7)%2) {
      a++;
      pat[pointer][CurrentRow][NextChannel][3]=al_fgetc(sfile);
      a++;
      pat[pointer][CurrentRow][NextChannel][4]=al_fgetc(sfile);
    }
    }
  }
    //printf("%d ",al_ftell(sfile));
    al_fclose(sfile);
    printf("done\n");
    if (!playermode && !fileswitch) {curpat=0;}
    if (oplaymode==1) {Play();}
    return 0;
  } else {
    perror("can't open file");
    #ifdef SOUNDS
    cursfx=1;sfxpos=0;
    #endif
    }
    delete[] checkstr;
  return 1;
}
void SaveInstrument() {
  // save instrument
  ALLEGRO_FILE *sfile;
  printf("\nplease write filename? ");
  char rfn[256];
  //gets(rfn);
  sfile=al_fopen(rfn,"wb");
  if (sfile!=NULL) { // write the file
    al_fseek(sfile,0,ALLEGRO_SEEK_SET); // seek to 0
    printf("writing header...\n");
    printf("%d ",al_ftell(sfile));
    al_fputs(sfile,"TRACKINS"); // magic number
    al_fwrite(sfile,&instrument[CurrentIns],64);
    printf("writing envelopes...\n");
    for (int jj=0; jj<8; jj++) {
      for (int kk=0; kk<256; kk++) {
        al_fputc(sfile,bytable[jj][instrument[CurrentIns].env[jj]][kk]); // seqtable
      }
    }
    al_fclose(sfile);
    printf("done\n");
  } else {fprintf(stderr,"error: couldn't open file for writing!\n");}
}
void LoadInstrument() {
  // load instrument
  ALLEGRO_FILE *sfile;
  printf("\nplease write filename? ");
  char rfn[256];
  int NextFree=0;
  //gets(rfn);
  sfile=al_fopen(rfn,"rb");
  if (sfile!=NULL) { // read the file
    al_fseek(sfile,8,ALLEGRO_SEEK_SET); // seek to 8
    printf("reading header...\n");
    al_fread(sfile,&instrument[CurrentIns],64);
    printf("reading envelopes...\n");
    for (int jj=0; jj<8; jj++) {
      NextFree=AllocateSequence(jj);
      instrument[CurrentIns].env[jj]=NextFree;
      for (int kk=0; kk<256; kk++) {
        bytable[jj][NextFree][kk]=al_fgetc(sfile); // seqtable
      }
    }
    al_fclose(sfile);
    printf("done\n");
  } else {fprintf(stderr,"error: couldn't open file for reading!\n");}
}
void LoadSample(const char* filename,int position) {
  printf("what the?\n");
  short* thepointer;
  int samplelength=0;
  ALLEGRO_SAMPLE *tempsample=al_load_sample(filename);
  thepointer=(short*)al_get_sample_data(tempsample);
  samplelength=al_get_sample_length(tempsample);
  printf("%x bytes",samplelength);
  for (int ii=0;ii<samplelength;ii++) {
    chip[0].pcm[minval(ii+position,131071)]=thepointer[ii]>>8;
  }
  al_destroy_sample(tempsample);
}
void LoadRawSample(const char* filename,int position) {
  ALLEGRO_FILE *sfile;
  int samplelength=0;
  sfile=al_fopen(filename,"rb");
  samplelength=al_fsize(sfile);
  printf("%x bytes",samplelength);
  if (samplelength<(65280-position)) {
    //for (int ii=0;ii<samplelength;ii++) {
      al_fseek(sfile,position,ALLEGRO_SEEK_SET);
      al_fread(sfile,chip[0].pcm+position,samplelength);
    //}
  } else {
    printf(" don't fit!");
  }
  al_fclose(sfile);
}
static void print_entry(const char* filepath) {
  ALLEGRO_FS_ENTRY *flist=al_create_fs_entry(filepath);
  printf("listing dir...\n");
  int increment=0;
      ALLEGRO_FS_ENTRY *next;
      FileInList neext;
      neext.name="";
      neext.isdir=false;
      // clean file list
      filenames.resize(0);
      al_open_directory(flist);
      while (1) {
         next=al_read_directory(flist);
         if (!next) break;
     const char *name = al_get_fs_entry_name(next);
     //printf("%-36s\n",name);
     neext.name=name;
     neext.isdir=al_get_fs_entry_mode(next)&ALLEGRO_FILEMODE_ISDIR;
     filenames.push_back(neext);
         al_destroy_fs_entry(next);
     increment++;
      }
    filecount=increment;
      al_close_directory(flist);
    al_destroy_fs_entry(flist);
      // sort the files
      string tempname;
      bool tempisdir=false;
      tempname="";
      for (int ii=0; ii<filecount; ii++) { // oooooohhhhhh ok it was the "i"
      for (int j=0; j<filecount-1; j++) {
         if (strcmp(filenames[j].name.c_str(), filenames[j+1].name.c_str()) > 0) {
      tempname=filenames[j].name;
      tempisdir=filenames[j].isdir;
      filenames[j].name=filenames[j+1].name;
      filenames[j].isdir=filenames[j+1].isdir;
      filenames[j+1].name=tempname;
      filenames[j+1].isdir=tempisdir;
         }
      }
      }
      printf("finish.\n");
}


void ClickEvents() {
  reversemode=false;
  // mouse states
  al_get_mouse_state(&mstate);
  mstate.x/=dpiScale;
  mstate.y/=dpiScale;
  // click events
  leftclickprev=leftclick;
  leftclick=(mstate.buttons&1);
  leftpress=false;
  rightpress=false;
  if (leftclick!=leftclickprev && leftclick==true) {
    /*if (verbose) {
    cout << "leftclick ";
    cout << mstate.x;
    cout << " ";
    cout << mstate.y;
    cout << ", ";
    }*/
    leftpress=true;
  }
  rightclickprev=rightclick;
  rightclick=(mstate.buttons&2);
  if (rightclick!=rightclickprev && rightclick==true) {
    if (verbose) {
    ////cout << "rightclick, ";
    }
    rightpress=true;
  }
  // hover functions
  for (int ii=0; ii<16; ii++) {
    // decrease hover amount
    hover[ii]--;
    // is hover negative? bound to 0
    if (hover[ii]<0) {hover[ii]=0;}
  }
  if (PIR(32,12,56,24,mstate.x,mstate.y)) {hover[1]+=2;}
  if (PIR(0,12,24,24,mstate.x,mstate.y)) {hover[0]+=2;}
  if (PIR(32,24,56,36,mstate.x,mstate.y)) {hover[2]+=2;}
  if (PIR(0,24,24,36,mstate.x,mstate.y)) {hover[3]+=2;}
  if (PIR(0,36,24,48,mstate.x,mstate.y)) {hover[4]+=2;}
  if (PIR(32,36,56,48,mstate.x,mstate.y)) {hover[5]+=2;}
  if (PIR(640,12,672,28,mstate.x,mstate.y)) {hover[6]+=2;}
  if (PIR(scrW-80,12,scrW-56,28,mstate.x,mstate.y)) {hover[7]+=2;}
  if (PIR(scrW-24,12,scrW,28,mstate.x,mstate.y)) {hover[8]+=2;}
  if (PIR(scrW-48,12,scrW-32,28,mstate.x,mstate.y)) {hover[9]+=2;}
  for (int ii=0; ii<16; ii++) {
    if (hover[ii]>8) {hover[ii]=8;}
  }
  // screen event
  if (leftclick) {
  if (PIR(32,12,56,24,mstate.x,mstate.y)) {screen=1;}
  if (PIR(0,12,24,24,mstate.x,mstate.y)) {screen=0;drawpatterns(true);}
  if (PIR(32,24,56,36,mstate.x,mstate.y)) {screen=2;}
  if (PIR(0,24,24,36,mstate.x,mstate.y)) {screen=3;}
  if (PIR(0,36,24,48,mstate.x,mstate.y)) {screen=4;}
  if (PIR(32,36,56,48,mstate.x,mstate.y)) {screen=5;}
  /*
  if (PIR(640,12,672,28,mstate.x,mstate.y)) {screen=6;}
  */
  if (PIR(0,0,128,11,mstate.x,mstate.y)) {screen=7;}
  if (PIR(64,12,88,24,mstate.x,mstate.y)) {screen=9;}
  if (PIR(64,24,88,36,mstate.x,mstate.y)) {screen=10;}
  if (PIR(64,36,88,48,mstate.x,mstate.y)) {screen=12;}
  if (PIR((scrW/2)-61,13,(scrW/2)-21,37,mstate.x,mstate.y)) {
    if (curtick==0) {
      Play();
    } else {
      playmode=1;
    }
  }
  if (PIR((scrW/2)-61,37,(scrW/2)-21,48,mstate.x,mstate.y)) {reversemode=true;}
  if (PIR((scrW/2)+21,13,(scrW/2)+61,37,mstate.x,mstate.y)) {playmode=0;}
  }
  if (leftpress) {
    inputvar=NULL;inputcurpos=0;maxinputsize=32;inputwhere=0;
    if (PIR(272,24,279,36,mstate.x,mstate.y)) {curoctave--; if (curoctave<0) {curoctave=0;}}
    if (PIR(280,24,288,36,mstate.x,mstate.y)) {curoctave++; if (curoctave>8) {curoctave=8;}}
    if (PIR(160,12,167,23,mstate.x,mstate.y)) {speed--; if (speed<1) {speed=1;}}
    if (PIR(168,12,176,23,mstate.x,mstate.y)) {speed++; if (speed>31) {speed=31;}; if (speed<1) {speed=1;}}
    if (PIR(160,24,167,35,mstate.x,mstate.y)) {tempo--; if (tempo<31) {tempo=31;}; FPS=tempo/2.5;}
    if (PIR(168,24,176,35,mstate.x,mstate.y)) {tempo++; if (tempo>255) {tempo=255;}; FPS=tempo/2.5;}
    if (PIR(160,36,167,48,mstate.x,mstate.y)) {if (curpat>0) curpat--; if (playmode==1) Play();}
    if (PIR(168,36,176,48,mstate.x,mstate.y)) {if (curpat<255) curpat++; if (playmode==1) Play();}
    if (PIR(272,12,279,24,mstate.x,mstate.y)) {patid[curpat]--;}
    if (PIR(280,12,288,24,mstate.x,mstate.y)) {patid[curpat]++;}
    if (PIR(272,36,279,48,mstate.x,mstate.y)) {patlength[patid[curpat]]--;}
    if (PIR(280,36,288,48,mstate.x,mstate.y)) {patlength[patid[curpat]]++;}
    if (PIR((scrW/2)-20,37,(scrW/2)+20,48,mstate.x,mstate.y)) {StepPlay();}
    if (PIR((scrW/2)-20,13,(scrW/2)+20,37,mstate.x,mstate.y)) {Play();}
    if (PIR(scrW-34*8,24,scrW-28*8,36,mstate.x,mstate.y)) {follow=!follow;}
    
    if (PIR(scrW-28*8,12,scrW-27*8,24,mstate.x,mstate.y)) {
      curins--;
      if (curins<0) curins=0;
    }
    if (PIR(scrW-27*8,12,scrW-26*8,24,mstate.x,mstate.y)) {
      curins++;
      if (curins>255) curins=255;
    }
    
    if (PIR(96,12,136,24,mstate.x,mstate.y)) {speedlock=!speedlock;}
    if (PIR(96,24,136,36,mstate.x,mstate.y)) {tempolock=!tempolock;}
  }
  // events only in pattern view
  if (screen==0) {
    if (mstate.y>60) {
      if (leftpress) {
        selStart=(int)((mstate.y-255+curpatrow*12)/12);
        if (selStart<0) selStart=0;
        if (selStart>=patlength[patid[curpat]]) selStart=patlength[patid[curpat]]-1;
        printf("ss: %d\n",selStart);
        curedchan=minval(maxval(((400-(scrW/2))+mstate.x-16-((8-chanstodisplay)*44))/96,0),chanstodisplay-1);
        switch ((((400-(scrW/2))+mstate.x-16-((8-chanstodisplay)*44))/8)%12) {
        case 0: curedmode=0; break;
        case 1: curedmode=0; break;
        case 2: curedmode=0; break;
        case 3: curedmode=0; break;
        case 4: curedmode=1; break;
        case 5: curedmode=1; break;
        case 6: curedmode=2; break;
        case 7: curedmode=2; break;
        case 8: curedmode=2; break;
        case 9: curedmode=3; break;
        case 10: curedmode=4; break;
        case 11: curedmode=4; break;
        }
      }
      if (leftclick) {
        selEnd=(int)((mstate.y-255+curpatrow*12)/12);
        if (selEnd<0) selEnd=0;
        if (selEnd>=patlength[patid[curpat]]) selEnd=patlength[patid[curpat]]-1;
        curselchan=maxval(((400-(scrW/2))+mstate.x-16-((8-chanstodisplay)*44))/96,0);
        switch ((((400-(scrW/2))+mstate.x-16-((8-chanstodisplay)*44))/8)%12) {
        case 0: curselmode=0; break;
        case 1: curselmode=0; break;
        case 2: curselmode=0; break;
        case 3: curselmode=0; break;
        case 4: curselmode=1; break;
        case 5: curselmode=1; break;
        case 6: curselmode=2; break;
        case 7: curselmode=2; break;
        case 8: curselmode=2; break;
        case 9: curselmode=3; break;
        case 10: curselmode=4; break;
        case 11: curselmode=4; break;
        }
        if (curselchan>=chanstodisplay) {
          curselchan=chanstodisplay-1;
          curselmode=4;
        }
        drawpatterns(true);
      }
    }
    if ((mstate.z-prevZ)<0) {
      if (follow) {
        curstep-=(mstate.z-prevZ);
      } else {
        curpatrow-=(mstate.z-prevZ)*3;
        curpatrow=fmin(curpatrow,(unsigned char)(patlength[patid[curpat]]-1));
      }
      drawpatterns(true);
    }
    if ((mstate.z-prevZ)>0) {
      if (follow) {
        curstep-=(mstate.z-prevZ);
        if (curstep<0) {
          if (curpat!=0) {
            curpat--;
            curstep+=patlength[patid[curpat]];
          } else {
            curstep=-1;
          }
        }
      } else {
        curpatrow-=(mstate.z-prevZ)*3;
        curpatrow=fmax(curpatrow,0);
      }
      drawpatterns(true);
    }
  }
  // events only in instrument view
  if (screen==1) {
    // set the envelope ONLY in normal mode
    if (!hexmode) {
    if (mstate.buttons&1 && PIR(0,90,scrW-285,scrH-30,mstate.x,mstate.y)) {
      if (CurrentEnv==6) {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][minval(minval(252,(512/valuewidth)+(scrollpos)+scrW-285),((scrollpos*valuewidth)+mstate.x)/valuewidth)]=maxval(minval(127,interpolate(94,scrH-42,0.5)-mstate.y),-128);}
      else {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][minval(minval(252,(512/valuewidth)+(scrollpos)+scrW-285),((scrollpos*valuewidth)+mstate.x)/valuewidth)]=maxval(minval(255,((scrH-43)-mstate.y)/((scrH-128)/256)),0);}
    }
    // line draw tool
    if (PIR(0,90,scrW-285,scrH-30,mstate.x,mstate.y)) {
    if (rightpress) {
      linex1=mstate.x;
      liney1=mstate.y;
    }
    if (rightclick) {
      // make line
      for (int ii=(linex1/valuewidth)+scrollpos;ii<minval(253,(mstate.x/valuewidth)+scrollpos);ii++) {
        bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][ii]=interpolate(maxval(minval(255,(scrH-42)-liney1),0),
        maxval(minval(255,(scrH-42)-mstate.y),0),
        ((float)ii-((float)linex1/valuewidth)-(float)scrollpos)/(minval(253,(mstate.x/valuewidth)+scrollpos)-(linex1/valuewidth)-scrollpos));
      }
    }
    }
    } else {
    for (int ii=0;ii<253;ii++) {
      if (PIR(8+((ii%21)*24),102+((ii/21)*24),15+((ii%21)*24),114+((ii/21)*24),mstate.x,mstate.y)) {
        if (leftpress) bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][ii]+=16;
        if (rightpress) bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][ii]-=16;}
      if (PIR(16+((ii%21)*24),102+((ii/21)*24),32+((ii%21)*24),114+((ii/21)*24),mstate.x,mstate.y)) {
        if (leftpress) bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][ii]+=1;
        if (rightpress) bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][ii]-=1;}
    }
    }
    // change envelopes
    if (leftpress) {
      if (PIR(528,132,784,144,mstate.x,mstate.y)) {inputvar=instrument[CurrentIns].name;inputcurpos=minval((mstate.x-524)/8,strlen(inputvar));maxinputsize=32;inputwhere=2;}
      if (PIR(24,72,72,84,mstate.x,mstate.y)) {CurrentEnv=0;}
      if (PIR(80,72,128,84,mstate.x,mstate.y)) {CurrentEnv=1;}
      if (PIR(136,72,176,84,mstate.x,mstate.y)) {CurrentEnv=2;}
      if (PIR(184,72,216,84,mstate.x,mstate.y)) {CurrentEnv=3;}
      if (PIR(224,72,264,84,mstate.x,mstate.y)) {CurrentEnv=4;}
      if (PIR(272,72,312,84,mstate.x,mstate.y)) {CurrentEnv=5;}
      if (PIR(320,72,376,84,mstate.x,mstate.y)) {CurrentEnv=6;}
      if (PIR(384,72,408,84,mstate.x,mstate.y)) {CurrentEnv=7;}
      if (PIR(456,72,463,84,mstate.x,mstate.y)) {instrument[CurrentIns].env[CurrentEnv]++;}
      if (PIR(464,72,471,84,mstate.x,mstate.y)) {instrument[CurrentIns].env[CurrentEnv]--;}
      if (PIR(480,72,496,84,mstate.x,mstate.y)) {instrument[CurrentIns].env[CurrentEnv]=AllocateSequence(CurrentEnv);}
      if (PIR(504,72,512,84,mstate.x,mstate.y)) {
        for (int ii=0;ii<254;ii++) {
          bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][ii]=0;
        }
        bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][254]=255;
        bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][255]=255;}
      if (PIR(192,60,199,72,mstate.x,mstate.y)) {CurrentIns++;}
      if (PIR(200,60,207,72,mstate.x,mstate.y)) {CurrentIns--;}
      // the right pane buttons
      if (PIR(scrW-184,84,scrW-177,96,mstate.x,mstate.y)) {instrument[CurrentIns].noteOffset++;}
      if (PIR(scrW-176,84,scrW-169,96,mstate.x,mstate.y)) {instrument[CurrentIns].noteOffset--;}
      if (PIR(scrW-168,84,scrW-161,96,mstate.x,mstate.y)) {instrument[CurrentIns].noteOffset+=12;}
      if (PIR(scrW-160,84,scrW-153,96,mstate.x,mstate.y)) {instrument[CurrentIns].noteOffset-=12;}
      if (PIR(0,72,16,84,mstate.x,mstate.y)) {instrument[CurrentIns].activeEnv=(instrument[CurrentIns].activeEnv)^(1<<CurrentEnv);}
      // bottom thing
      if (PIR(88,scrH-18,95,scrH-6,mstate.x,mstate.y)) {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][254]++;}
      if (PIR(96,scrH-18,104,scrH-6,mstate.x,mstate.y)) {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][254]--;}
      if (PIR(208,scrH-18,215,scrH-6,mstate.x,mstate.y)) {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][255]++;}
      if (PIR(216,scrH-18,224,scrH-6,mstate.x,mstate.y)) {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][255]--;}
      if (PIR(320,scrH-18,327,scrH-6,mstate.x,mstate.y)) {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][253]++;}
      if (PIR(328,scrH-18,336,scrH-6,mstate.x,mstate.y)) {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][253]--;}
      // the right pane buttons
      if (PIR(scrW-216,156,scrW-192,168,mstate.x,mstate.y)) {instrument[CurrentIns].DFM^=1;} // LOW
      if (PIR(scrW-184,156,scrW-152,168,mstate.x,mstate.y)) {instrument[CurrentIns].DFM^=2;} // HIGH
      if (PIR(scrW-144,156,scrW-112,168,mstate.x,mstate.y)) {instrument[CurrentIns].DFM^=4;} // BAND
      if (PIR(scrW-272,180,scrW-248,192,mstate.x,mstate.y)) {instrument[CurrentIns].DFM^=8;} // pcm flag
      if (PIR(scrW-272,252,scrW-256,264,mstate.x,mstate.y)) {instrument[CurrentIns].DFM^=16;} // rm flag
      if (PIR(scrW-272,264,scrW-256,276,mstate.x,mstate.y)) {instrument[CurrentIns].flags^=32;} // sync flag
      if (PIR(scrW-272,288,scrW-208,300,mstate.x,mstate.y)) {instrument[CurrentIns].flags^=1;} // reset osc flag
      // not this one
      if (PIR(488,60,512,72,mstate.x,mstate.y)) {hexmode=!hexmode;}
      // but yes these ones
      if (PIR(scrW-200,180,scrW-193,191,mstate.x,mstate.y)) {instrument[CurrentIns].DFM^=128;} 
      if (PIR(scrW-192,180,scrW-185,191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmPos[1]+=16;}
      if (PIR(scrW-184,180,scrW-177,191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmPos[1]++;}
      if (PIR(scrW-176,180,scrW-169,191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmPos[0]+=16;}
      if (PIR(scrW-168,180,scrW-160,191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmPos[0]++;}
      if (PIR(scrW-88,180,scrW-81,191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmLen+=4096;}
      if (PIR(scrW-80,180,scrW-73,191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmLen+=256;}
      if (PIR(scrW-72,180,scrW-63,191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmLen+=16;}
      if (PIR(scrW-64,180,scrW-56,191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmLen++;}

      if (PIR(scrW-(800-592),216,scrW-(800-599),228,mstate.x,mstate.y)) {instrument[CurrentIns].filterH-=16;}
      if (PIR(scrW-(800-600),216,scrW-(800-607),228,mstate.x,mstate.y)) {instrument[CurrentIns].filterH--;}
      if (PIR(scrW-(800-608),216,scrW-(800-615),228,mstate.x,mstate.y)) {instrument[CurrentIns].filterH-=4096;}
      if (PIR(scrW-(800-616),216,scrW-(800-623),228,mstate.x,mstate.y)) {instrument[CurrentIns].filterH-=256;}

      if (PIR(scrW-(800-624),252,scrW-(800-631),264,mstate.x,mstate.y)) {instrument[CurrentIns].LFO++;}
      if (PIR(scrW-(800-632),252,scrW-(800-640),264,mstate.x,mstate.y)) {instrument[CurrentIns].LFO--;}

      if (PIR(scrW-(800-600),324,scrW-(800-607),336,mstate.x,mstate.y)) {instrument[CurrentIns].flags+=64;}
      if (PIR(scrW-(800-608),324,scrW-(800-616),336,mstate.x,mstate.y)) {instrument[CurrentIns].flags-=64;}
    }
    if (rightpress) {
      if (PIR(scrW-(800-600),180,scrW-(800-607),191,mstate.x,mstate.y)) {instrument[CurrentIns].DFM^=128;}
      if (PIR(scrW-(800-608),180,scrW-(800-615),191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmPos[1]-=16;}
      if (PIR(scrW-(800-616),180,scrW-(800-623),191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmPos[1]--;}
      if (PIR(scrW-(800-624),180,scrW-(800-631),191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmPos[0]-=16;}
      if (PIR(scrW-(800-632),180,scrW-(800-640),191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmPos[0]--;}
      if (PIR(scrW-(800-712),180,scrW-(800-719),191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmLen-=4096;}
      if (PIR(scrW-(800-720),180,scrW-(800-727),191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmLen-=256;}
      if (PIR(scrW-(800-728),180,scrW-(800-735),191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmLen-=16;}
      if (PIR(scrW-(800-736),180,scrW-(800-744),191,mstate.x,mstate.y)) {instrument[CurrentIns].pcmLen--;}
      if (PIR(scrW-(800-592),216,scrW-(800-599),228,mstate.x,mstate.y)) {instrument[CurrentIns].filterH+=16;}
      if (PIR(scrW-(800-600),216,scrW-(800-607),228,mstate.x,mstate.y)) {instrument[CurrentIns].filterH++;}
      if (PIR(scrW-(800-608),216,scrW-(800-615),228,mstate.x,mstate.y)) {instrument[CurrentIns].filterH+=4096;}
      if (PIR(scrW-(800-616),216,scrW-(800-623),228,mstate.x,mstate.y)) {instrument[CurrentIns].filterH+=256;}
    }
    if (leftclick) {
      if (PIR(0,scrH-18,8,scrH-6,mstate.x,mstate.y)) {scrollpos--; if (scrollpos<0) {scrollpos=0;};}
      if (PIR(500,scrH-18,512,scrH-6,mstate.x,mstate.y)) {scrollpos++; if (scrollpos>124) {scrollpos=124;};}
      // bottom thing
      if (PIR(72,scrH-18,79,scrH-6,mstate.x,mstate.y)) {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][254]++;}
      if (PIR(80,scrH-18,87,scrH-6,mstate.x,mstate.y)) {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][254]--;}
      if (PIR(192,scrH-18,199,scrH-6,mstate.x,mstate.y)) {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][255]++;}
      if (PIR(200,scrH-18,207,scrH-6,mstate.x,mstate.y)) {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][255]--;}
      if (PIR(304,scrH-18,311,scrH-6,mstate.x,mstate.y)) {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][253]++;}
      if (PIR(312,scrH-18,319,scrH-6,mstate.x,mstate.y)) {bytable[CurrentEnv][instrument[CurrentIns].env[CurrentEnv]][253]--;}
      // other stuff
      if (PIR(240,60,272,71,mstate.x,mstate.y)) {SaveInstrument();}
      if (PIR(280,60,312,71,mstate.x,mstate.y)) {LoadInstrument();}
      if (PIR(scrW-(800-608),252,scrW-(800-615),264,mstate.x,mstate.y)) {instrument[CurrentIns].LFO++;}
      if (PIR(scrW-(800-616),252,scrW-(800-623),264,mstate.x,mstate.y)) {instrument[CurrentIns].LFO--;}
    }

  }
  // events only in diskop view
  if (screen==2) {
    if (leftpress) {
      if (PIR(168,60,200,72,mstate.x,mstate.y)) {
        printf("\nplease write filename? ");
        char rfn[256];
        //gets(rfn);
        int success=LoadFile(rfn);}
      if (PIR(208,60,240,72,mstate.x,mstate.y)) {int success=SaveFile();}
      if (PIR(248,60,320,72,mstate.x,mstate.y)) {int success=ImportMOD(filenames[selectedfileindex-1].name.c_str());}
      if (PIR(328,60,400,72,mstate.x,mstate.y)) {int success=ImportS3M();}
      if (PIR(408,60,472,72,mstate.x,mstate.y)) {int success=ImportIT();}
      if (PIR(552,60,632,72,mstate.x,mstate.y)) {
        printf("\nplease write filename? ");
        char rfn[256];
        char rwpos[256];
        //gets(rfn);
        printf("\nwrite position? ");
        int writeposition=0;
        //gets(rwpos);
        writeposition=0;//atoi(rwpos);
        LoadSample(rfn,writeposition);}
      if (PIR(640,60,744,72,mstate.x,mstate.y)) {
        printf("\nplease write filename? ");
        char rfn[256];
        char rwpos[256];
        //gets(rfn);
        printf("\nwrite position? ");
        int writeposition=0;
        //gets(rwpos);
        writeposition=0;//atoi(rwpos);
        LoadRawSample(rfn,writeposition);}
      for (int ii=diskopscrollpos; ii<minval(diskopscrollpos+27,filecount); ii++) {
        if (PIR(0,120+(ii*12)-(diskopscrollpos*12),790,131+(ii*12)-(diskopscrollpos*12),mstate.x,mstate.y)) {
          if (selectedfileindex==ii+1) {
          if (filenames[ii].isdir) {strcpy(curdir,filenames[ii].name.c_str());print_entry(curdir);diskopscrollpos=0;selectedfileindex=-1;}
          else {
                                          int success;
                                          if (strstr(filenames[ii].name.c_str(),".mod")==NULL) {success=LoadFile(filenames[ii].name.c_str());} else {
                                            ImportMOD(filenames[ii].name.c_str());
                                          }
                                          
                                        }} else {
            selectedfileindex=ii+1;}
        }
      }
    if (PIR(0,108,790,119,mstate.x,mstate.y)) {ParentDir(curdir);print_entry(curdir);diskopscrollpos=0;}
    }
    if (leftclick) {
      if (PIR(792,108,800,119,mstate.x,mstate.y)) {diskopscrollpos--;if (diskopscrollpos<0) {diskopscrollpos=0;}}
      if (PIR(792,432,800,444,mstate.x,mstate.y)) {diskopscrollpos++;if (diskopscrollpos>maxval(0,filecount-27)) {diskopscrollpos=maxval(0,filecount-27);}}
    }
    if ((mstate.z-prevZ)<0) {
      diskopscrollpos-=(mstate.z-prevZ)*4;if (diskopscrollpos>maxval(0,filecount-27)) {diskopscrollpos=maxval(0,filecount-27);}
    }
    if ((mstate.z-prevZ)>0) {
      diskopscrollpos-=(mstate.z-prevZ)*4;if (diskopscrollpos<0) {diskopscrollpos=0;}
    }
  }
  // events only in song view
  if (screen==3) {
    if (leftpress) {
    if (PIR(88,84,344,96,mstate.x,mstate.y)) {inputvar=&name[0];inputcurpos=minval((mstate.x-84)/8,strlen(inputvar));maxinputsize=32;inputwhere=1;}
    if (PIR(760,60,767,72,mstate.x,mstate.y)) {defspeed--;if (defspeed<1) {defspeed=1;};speed=defspeed;}
    if (PIR(768,60,776,72,mstate.x,mstate.y)) {defspeed++;if (defspeed<1) {defspeed=1;};speed=defspeed;}
    if (PIR(456,60,463,72,mstate.x,mstate.y)) {songdf--;}
    if (PIR(464,60,472,72,mstate.x,mstate.y)) {songdf++;}
    if (PIR(568,60,575,72,mstate.x,mstate.y)) {songlength--;}
    if (PIR(576,60,584,72,mstate.x,mstate.y)) {songlength++;}
    if (PIR(0,300,64,312,mstate.x,mstate.y)) {screen=8;}
    if (PIR(184,276,248,288,mstate.x,mstate.y)) {playmode=2;curtick=1;cvol[0]=127;cpcmpos[0]=0;cmode[0]=1;cfreq[0]=2300;cbound[0]=131071;cloop[0]=0;}
    if (PIR(32,276,64,288,mstate.x,mstate.y)) {screen=11;}
    }
    if (leftclick) {
    if (PIR(440,60,447,72,mstate.x,mstate.y)) {songdf--;}
    if (PIR(448,60,455,72,mstate.x,mstate.y)) {songdf++;}
    if (PIR(552,60,559,72,mstate.x,mstate.y)) {songlength--;}
    if (PIR(560,60,567,72,mstate.x,mstate.y)) {songlength++;}
    }
  }
  // events only in mixer view
  if (screen==4) {
    int mixerdrawoffset=(scrW/2)-chanstodisplay*48-12;
    if (leftclick) {
      for (int ii=0;ii<chanstodisplay;ii++) {
        if (PIR(56+(ii*96)+mixerdrawoffset,84,63+(ii*96)+mixerdrawoffset,95,mstate.x,mstate.y)) {defchanvol[ii+(curedpage*8)]++;if (defchanvol[ii+(curedpage*8)]>128) {defchanvol[ii+(curedpage*8)]=128;}}
        if (PIR(64+(ii*96)+mixerdrawoffset,84,72+(ii*96)+mixerdrawoffset,95,mstate.x,mstate.y)) {defchanvol[ii+(curedpage*8)]--;if (defchanvol[ii+(curedpage*8)]>250) {defchanvol[ii+(curedpage*8)]=0;}}
        if (PIR(56+(ii*96)+mixerdrawoffset,96,63+(ii*96)+mixerdrawoffset,108,mstate.x,mstate.y)) {defchanpan[ii+(curedpage*8)]++;}
        if (PIR(64+(ii*96)+mixerdrawoffset,96,72+(ii*96)+mixerdrawoffset,108,mstate.x,mstate.y)) {defchanpan[ii+(curedpage*8)]--;}
      }
    }
    if (leftpress) {
      for (int ii=0;ii<chanstodisplay;ii++) {
        if (!kb[ALLEGRO_KEY_LSHIFT]) {if (PIR(16+(ii*96)+mixerdrawoffset,60,96+(ii*96)+mixerdrawoffset,72,mstate.x,mstate.y)) {muted[ii+(curedpage*8)]=!muted[ii+(curedpage*8)];}}
        else {if (PIR(16+(ii*96)+mixerdrawoffset,60,96+(ii*96)+mixerdrawoffset,72,mstate.x,mstate.y)) {midion[ii+(curedpage*8)]=!midion[ii+(curedpage*8)];}}
      }
    }
    if (rightpress) {
      for (int ii=0;ii<chanstodisplay;ii++) {
        if (PIR(16+(ii*96)+mixerdrawoffset,60,96+(ii*96)+mixerdrawoffset,72,mstate.x,mstate.y)) {
          bool solomode=true;
          for (int jj=0;jj<32;jj++) {
            if (jj==ii+(curedpage*8)) {continue;}
            if (!muted[jj]) {solomode=false;break;}
          }
          if (solomode) {
            for (int jj=0;jj<32;jj++) {
              muted[jj]=false;
            }
          } else {
            for (int jj=0;jj<32;jj++) {
              muted[jj]=true;
            }
            muted[ii+(curedpage*8)]=false;
          }
        }
      }
    }
  }
  // events only in config view
  if (screen==5) {
    if (leftpress) {
      if (PIR(280,96,368,107,mstate.x,mstate.y)) {settings::distortion=!settings::distortion;}
      if (PIR(280,132,368,143,mstate.x,mstate.y)) {settings::cubicspline=!settings::cubicspline;}
      if (PIR(280,240,368,251,mstate.x,mstate.y)) {settings::nofilters=!settings::nofilters;}
      if (PIR(280,252,368,264,mstate.x,mstate.y)) {settings::muffle=!settings::muffle;}
    for (int ii=0;ii<27;ii++) {
      if (PIR(160,96+(ii*12),167,107+(ii*12),mstate.x,mstate.y)) {settings::colorR[ii]+=16;}
      if (PIR(168,96+(ii*12),176,107+(ii*12),mstate.x,mstate.y)) {settings::colorR[ii]++;}

      if (PIR(184,96+(ii*12),191,107+(ii*12),mstate.x,mstate.y)) {settings::colorG[ii]+=16;}
      if (PIR(192,96+(ii*12),200,107+(ii*12),mstate.x,mstate.y)) {settings::colorG[ii]++;}

      if (PIR(208,96+(ii*12),215,107+(ii*12),mstate.x,mstate.y)) {settings::colorB[ii]+=16;}
      if (PIR(216,96+(ii*12),224,107+(ii*12),mstate.x,mstate.y)) {settings::colorB[ii]++;}
    }
    }
    if (rightpress) {
    for (int ii=0;ii<27;ii++) {
      if (PIR(160,96+(ii*12),167,107+(ii*12),mstate.x,mstate.y)) {settings::colorR[ii]-=16;}
      if (PIR(168,96+(ii*12),176,107+(ii*12),mstate.x,mstate.y)) {settings::colorR[ii]--;}

      if (PIR(184,96+(ii*12),191,107+(ii*12),mstate.x,mstate.y)) {settings::colorG[ii]-=16;}
      if (PIR(192,96+(ii*12),200,107+(ii*12),mstate.x,mstate.y)) {settings::colorG[ii]--;}

      if (PIR(208,96+(ii*12),215,107+(ii*12),mstate.x,mstate.y)) {settings::colorB[ii]-=16;}
      if (PIR(216,96+(ii*12),224,107+(ii*12),mstate.x,mstate.y)) {settings::colorB[ii]--;}
    }
    }
  }
  // events only in comments view
  if (screen==8) {
    if (leftpress) {
      if (PIR(0,60,scrW,scrH,mstate.x,mstate.y)) {inputvar=comments;inputcurpos=minval(mstate.x/8,strlen(inputvar));maxinputsize=65535;inputwhere=4;}
    }
  }
  // events only in sound effect editor
  if (screen==10) {
  for (int ii=0;ii<128;ii++) {
    if (leftpress) {
    if (PIR((ii%(scrW/24))*24,84+((ii/(scrW/24))*24),((ii%(scrW/24))*24)+7,(84+((ii/(scrW/24))*24))+12,mstate.x,mstate.y)) {sfxdata[cursfx][ii]+=16;}
    if (PIR(((ii%(scrW/24))*24)+8,84+((ii/(scrW/24))*24),((ii%(scrW/24))*24)+16,(84+((ii/(scrW/24))*24))+12,mstate.x,mstate.y)) {sfxdata[cursfx][ii]++;}
    }
    if (rightpress) {
    if (PIR((ii%(scrW/24))*24,84+((ii/(scrW/24))*24),((ii%(scrW/24))*24)+7,(84+((ii/(scrW/24))*24))+12,mstate.x,mstate.y)) {sfxdata[cursfx][ii]-=16;}
    if (PIR(((ii%(scrW/24))*24)+8,84+((ii/(scrW/24))*24),((ii%(scrW/24))*24)+16,(84+((ii/(scrW/24))*24))+12,mstate.x,mstate.y)) {sfxdata[cursfx][ii]--;}
    }
  }
  }
  // events only in PCM editor
  if (screen==11) {
    if (leftclick) {
      if (pcmeditenable) chip[0].pcm[(int)(((float)mstate.x)*pow(2.0f,-pcmeditscale))+pcmeditoffset]=minval(127,maxval(-127,mstate.y-(scrH/2)));
    }
  }
  prevX=mstate.x;
  prevY=mstate.y;
  prevZ=mstate.z;
}
void FastTracker() {
  // FT2-like pattern editor
  int firstChan, firstMode;
  int lastChan, lastMode;
  int selTop, selBottom;
  // scroll code
  if (kbpressed[ALLEGRO_KEY_UP]) {GoBack();drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_DOWN]) {JustSkip();drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_RIGHT]) {
  curedmode++;
  if (curedmode>4) {
    curedchan++;
    curedmode=0;
    if (curedchan>7) {curedchan=7;curedmode=5;}
    }
  drawpatterns(true);
  }
  if (kbpressed[ALLEGRO_KEY_LEFT]) {
    curedmode--;
    if (curedmode<0) {
  curedchan--;
  curedmode=4;
  if (curedchan<0) {curedchan=0;curedmode=0;}}
    drawpatterns(true);
  }
  if (kbpressed[ALLEGRO_KEY_DELETE]) {
    // delete from selection start to end
    firstChan=curedchan; firstMode=curedmode;
    lastChan=curselchan; lastMode=curselmode;
    selTop=selStart; selBottom=selEnd;
    if (lastChan<firstChan) {
      lastChan^=firstChan;
      firstChan^=lastChan;
      lastChan^=firstChan;
      
      lastMode^=firstMode;
      firstMode^=lastMode;
      lastMode^=firstMode;
    } else if (lastChan==firstChan) {
      if (lastMode<firstMode) {
        lastMode^=firstMode;
        firstMode^=lastMode;
        lastMode^=firstMode;
      }
    }
    if (selBottom<selTop) {
      selBottom^=selTop;
      selTop^=selBottom;
      selBottom^=selTop;
    }
    for (int i=firstChan; i<=lastChan; i++) {
      for (int j=(i==firstChan)?firstMode:0; (j<5 && (i<lastChan || j<=lastMode)); j++) {
        for (int k=selTop; k<=selBottom; k++) {
          pat[patid[curpat]][k][(8*curedpage)+i][j]=0x00;
        }
      }
    }
    drawpatterns(true);
    selStart=curstep;
    selEnd=curstep;
    curselchan=curedchan;
    curselmode=curedmode;
  }
  if (curedmode==0) {
  // silences and stuff
  if (kbpressed[71]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x0d;EditSkip();}
  if (kbpressed[70]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x0f;EditSkip();}
  if (kbpressed[28]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x0e;EditSkip();}
  // notes, main octave
  if (kbpressed[ALLEGRO_KEY_Z]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x01+minval(curoctave<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_X]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x03+minval(curoctave<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_C]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x05+minval(curoctave<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_V]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x06+minval(curoctave<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_B]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x08+minval(curoctave<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_N]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x0a+minval(curoctave<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_M]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x0c+minval(curoctave<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_S]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x02+minval(curoctave<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_D]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x04+minval(curoctave<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_G]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x07+minval(curoctave<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_H]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x09+minval(curoctave<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_J]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x0b+minval(curoctave<<4,0x90);EditSkip();}
  if (kbpressed[72]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x01+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[12]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x02+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[73]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x03+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[60]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x04+minval((curoctave+1)<<4,0x90);EditSkip();}
  // notes, 2nd octave
  if (kbpressed[ALLEGRO_KEY_Q]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x01+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_W]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x03+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_E]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x05+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_R]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x06+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_T]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x08+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_Y]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x0a+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_U]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x0c+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_2]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x02+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_3]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x04+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_5]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x07+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_6]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x09+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_7]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x0b+minval((curoctave+1)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_I]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x01+minval((curoctave+2)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_9]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x02+minval((curoctave+2)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_O]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x03+minval((curoctave+2)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_0]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x04+minval((curoctave+2)<<4,0x90);EditSkip();}
  if (kbpressed[ALLEGRO_KEY_P]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x05+minval((curoctave+2)<<4,0x90);EditSkip();}
  if (kbpressed[66]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x06+minval((curoctave+2)<<4,0x90);EditSkip();}
  if (kbpressed[65]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][0]=0x07+minval((curoctave+2)<<4,0x90);EditSkip();}
  }
  if (curedmode==1) {
  if (kbpressed[ALLEGRO_KEY_0]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4);drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_1]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+1;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_2]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+2;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_3]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+3;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_4]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+4;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_5]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+5;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_6]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+6;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_7]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+7;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_8]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+8;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_9]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+9;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_A]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+10;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_B]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+11;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_C]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+12;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_D]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+13;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_E]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+14;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_F]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][1]<<4)+15;drawpatterns(true);}
  }
  if (curedmode==2) {
  if (kbpressed[ALLEGRO_KEY_0]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4);drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_1]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+1;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_2]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+2;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_3]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+3;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_4]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+4;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_5]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+5;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_6]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+6;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_7]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+7;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_8]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+8;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_9]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+9;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_A]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+10;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_B]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+11;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_C]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+12;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_D]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+13;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_E]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+14;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_F]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][2]<<4)+15;drawpatterns(true);}
  }
  if (curedmode==3) {
  if (kbpressed[ALLEGRO_KEY_A]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=1;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_B]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=2;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_C]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=3;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_D]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=4;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_E]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=5;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_F]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=6;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_G]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=7;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_H]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=8;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_I]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=9;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_J]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=10;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_K]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=11;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_L]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=12;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_M]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=13;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_N]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=14;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_O]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=15;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_P]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=16;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_Q]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=17;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_R]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=18;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_S]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=19;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_T]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=20;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_U]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=21;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_V]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=22;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_W]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=23;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_X]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=24;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_Y]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=25;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_Z]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][3]=26;drawpatterns(true);}
  }
  if (curedmode==4) {
  if (kbpressed[ALLEGRO_KEY_0]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4);drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_1]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+1;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_2]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+2;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_3]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+3;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_4]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+4;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_5]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+5;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_6]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+6;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_7]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+7;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_8]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+8;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_9]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+9;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_A]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+10;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_B]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+11;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_C]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+12;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_D]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+13;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_E]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+14;drawpatterns(true);}
  if (kbpressed[ALLEGRO_KEY_F]) {pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]=(pat[patid[curpat]][curstep][(8*curedpage)+curedchan][4]<<4)+15;drawpatterns(true);}
  }
}
void InstrumentTest(int testnote, int testchan) {
  // code scrapped.
  // this will be fixed soon.
}
void ModPlug() {
  // ModPlug Tracker-OpenMPT-like pattern editor


}
void RunTestNote(int keycode) {
  //// PLAY SONG ////
  // set speed to song speed and other variables
  if (!speedlock) {speed=defspeed;}
  if (!tempolock) {
  if (ntsc) {
  #ifdef FILM
  tempo=60;
  #else
  tempo=150;
  #endif
  } else {
  tempo=125;
  }
  }
  FPS=tempo/2.5;
  //detunefactor=DETUNE_FACTOR_GLOBAL*(50/FPS);
  // reset cursor position
  //curtick=speed;curstep=-1;playmode=1;
  //tickstart=true;
  // reset channels
  for (int su=0;su<32;su++) {
  cvol[su]=0;
  Mvol[su]=0;
  plcount[su]=0;
  plpos[su]=0;
  chanvol[su]=defchanvol[su];
  //chanpan[su]=defchanpan[su];
  if ((su+1)&2) {chanpan[su]=96;} else {chanpan[su]=-96;} // amiga auto-pan logic
  //if (su&1) {chanpan[su]=96;} else {chanpan[su]=-96;} // normal auto-pan logic
  finedelay=0;
  }
  // reset global volume
  cglobvol=128;
  // process next row
  //NextRow();
  // notes, main octave
  switch (keycode) {
  case ALLEGRO_KEY_Z: InstrumentTest(0x01+minval(curoctave<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_X: InstrumentTest(0x03+minval(curoctave<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_C: InstrumentTest(0x05+minval(curoctave<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_V: InstrumentTest(0x06+minval(curoctave<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_B: InstrumentTest(0x08+minval(curoctave<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_N: InstrumentTest(0x0a+minval(curoctave<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_M: InstrumentTest(0x0c+minval(curoctave<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_S: InstrumentTest(0x02+minval(curoctave<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_D: InstrumentTest(0x04+minval(curoctave<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_G: InstrumentTest(0x07+minval(curoctave<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_H: InstrumentTest(0x09+minval(curoctave<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_J: InstrumentTest(0x0b+minval(curoctave<<4,0x90),FreeChannel()); break;
  case 72: InstrumentTest(0x01+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case 12: InstrumentTest(0x02+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case 73: InstrumentTest(0x03+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case 60: InstrumentTest(0x04+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  // notes, 2nd octave
  case ALLEGRO_KEY_Q: InstrumentTest(0x01+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_W: InstrumentTest(0x03+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_E: InstrumentTest(0x05+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_R: InstrumentTest(0x06+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_T: InstrumentTest(0x08+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_Y: InstrumentTest(0x0a+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_U: InstrumentTest(0x0c+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_2: InstrumentTest(0x02+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_3: InstrumentTest(0x04+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_5: InstrumentTest(0x07+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_6: InstrumentTest(0x09+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_7: InstrumentTest(0x0b+minval((curoctave+1)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_I: InstrumentTest(0x01+minval((curoctave+2)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_9: InstrumentTest(0x02+minval((curoctave+2)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_O: InstrumentTest(0x03+minval((curoctave+2)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_0: InstrumentTest(0x04+minval((curoctave+2)<<4,0x90),FreeChannel()); break;
  case ALLEGRO_KEY_P: InstrumentTest(0x05+minval((curoctave+2)<<4,0x90),FreeChannel()); break;
  case 66: InstrumentTest(0x06+minval((curoctave+2)<<4,0x90),FreeChannel()); break;
  case 65: InstrumentTest(0x07+minval((curoctave+2)<<4,0x90),FreeChannel()); break;
  }
}
void MuteControls() {
  if (kbpressed[ALLEGRO_KEY_1]) {muted[0]=!muted[0];}
  if (kbpressed[ALLEGRO_KEY_2]) {muted[1]=!muted[1];}
  if (kbpressed[ALLEGRO_KEY_3]) {muted[2]=!muted[2];}
  if (kbpressed[ALLEGRO_KEY_4]) {muted[3]=!muted[3];}
  if (kbpressed[ALLEGRO_KEY_5]) {muted[4]=!muted[4];}
  if (kbpressed[ALLEGRO_KEY_6]) {muted[5]=!muted[5];}
  if (kbpressed[ALLEGRO_KEY_7]) {muted[6]=!muted[6];}
  if (kbpressed[ALLEGRO_KEY_8]) {muted[7]=!muted[7];}
  if (kbpressed[ALLEGRO_KEY_Q]) {muted[8]=!muted[8];}
  if (kbpressed[ALLEGRO_KEY_W]) {muted[9]=!muted[9];}
  if (kbpressed[ALLEGRO_KEY_E]) {muted[10]=!muted[10];}
  if (kbpressed[ALLEGRO_KEY_R]) {muted[11]=!muted[11];}
  if (kbpressed[ALLEGRO_KEY_T]) {muted[12]=!muted[12];}
  if (kbpressed[ALLEGRO_KEY_Y]) {muted[13]=!muted[13];}
  if (kbpressed[ALLEGRO_KEY_U]) {muted[14]=!muted[14];}
  if (kbpressed[ALLEGRO_KEY_I]) {muted[15]=!muted[15];}
  if (kbpressed[ALLEGRO_KEY_A]) {muted[16]=!muted[16];}
  if (kbpressed[ALLEGRO_KEY_S]) {muted[17]=!muted[17];}
  if (kbpressed[ALLEGRO_KEY_D]) {muted[18]=!muted[18];}
  if (kbpressed[ALLEGRO_KEY_F]) {muted[19]=!muted[19];}
  if (kbpressed[ALLEGRO_KEY_G]) {muted[20]=!muted[20];}
  if (kbpressed[ALLEGRO_KEY_H]) {muted[21]=!muted[21];}
  if (kbpressed[ALLEGRO_KEY_J]) {muted[22]=!muted[22];}
  if (kbpressed[ALLEGRO_KEY_K]) {muted[23]=!muted[23];}
  if (kbpressed[ALLEGRO_KEY_Z]) {muted[24]=!muted[24];}
  if (kbpressed[ALLEGRO_KEY_X]) {muted[25]=!muted[25];}
  if (kbpressed[ALLEGRO_KEY_C]) {muted[26]=!muted[26];}
  if (kbpressed[ALLEGRO_KEY_V]) {muted[27]=!muted[27];}
  if (kbpressed[ALLEGRO_KEY_B]) {muted[28]=!muted[28];}
  if (kbpressed[ALLEGRO_KEY_N]) {muted[29]=!muted[29];}
  if (kbpressed[ALLEGRO_KEY_M]) {muted[30]=!muted[30];}
  if (kbpressed[ALLEGRO_KEY_COMMA]) {muted[31]=!muted[31];}
}
void MuteAllChannels() {
  for (int su=0;su<8*soundchips;su++) {
  cvol[su]=0;
        chip[su>>3].chan[su&7].vol=0;
  }
}
void SFXControls() {
  if (kbpressed[ALLEGRO_KEY_1]) {cursfx=0;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_2]) {cursfx=1;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_3]) {cursfx=2;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_4]) {cursfx=3;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_5]) {cursfx=4;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_6]) {cursfx=5;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_7]) {cursfx=6;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_8]) {cursfx=7;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_Q]) {cursfx=8;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_W]) {cursfx=9;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_E]) {cursfx=10;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_R]) {cursfx=11;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_T]) {cursfx=12;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_Y]) {cursfx=13;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_U]) {cursfx=14;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_I]) {cursfx=15;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_A]) {cursfx=16;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_S]) {cursfx=17;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_D]) {cursfx=18;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_F]) {cursfx=19;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_G]) {cursfx=20;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_H]) {cursfx=21;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_J]) {cursfx=22;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_K]) {cursfx=23;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_Z]) {cursfx=24;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_X]) {cursfx=25;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_C]) {cursfx=26;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_V]) {cursfx=27;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_B]) {cursfx=28;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_N]) {cursfx=29;sfxpos=0;}
  // pausers
  if (kbpressed[ALLEGRO_KEY_M]) {if (playmode==0) {playmode=1;} else {playmode=0;};cursfx=30;sfxpos=0;}
  if (kbpressed[ALLEGRO_KEY_COMMA]) {if (playmode==0) {playmode=1;} else {playmode=0;};cursfx=31;sfxpos=0;}
}
void KeyboardEvents() {
  // keyboard states
  al_get_keyboard_state(&kbstate);
  // check for presses
  for (int cntkb=0;cntkb<255;cntkb++) {
  kblast[cntkb]=kb[cntkb];
  kbpressed[cntkb]=false;
  kb[cntkb]=al_key_down(&kbstate,cntkb);
  if (kb[cntkb]!=kblast[cntkb] && kb[cntkb]==true) {
    kbpressed[cntkb]=true;
    //if (verbose) {cout << cntkb;}
  }}
  // main code here
  if (edittype && screen==0) {FastTracker();} else {ModPlug();}
  if (screen==4) {MuteControls();}
  if (kbpressed[ALLEGRO_KEY_PGDN]) {curedpage++; if (curedpage>3) {curedpage=3;
    #ifdef SOUNDS
    cursfx=1;sfxpos=0;
    #endif
  };drawpatterns(true);drawmixerlayer();}
  if (kbpressed[ALLEGRO_KEY_PGUP]) {curedpage--; if (curedpage<0) {
    #ifdef SOUNDS
    cursfx=1;sfxpos=0;
    #endif
    curedpage=0;
  };drawpatterns(true);drawmixerlayer();}
  maxCTD=(scrW-24)/96;
  if (kbpressed[ALLEGRO_KEY_END]) {chanstodisplay++;if (chanstodisplay>maxCTD) {
    chanstodisplay=maxCTD;
    #ifdef SOUNDS
    cursfx=1;sfxpos=0;
    #endif
  };drawpatterns(true);drawmixerlayer();}
  if (kbpressed[ALLEGRO_KEY_HOME]) {chanstodisplay--;if (chanstodisplay<1) {
    chanstodisplay=1;
    #ifdef SOUNDS
    cursfx=1;sfxpos=0;
    #endif
  };drawpatterns(true);drawmixerlayer();}
  if (kbpressed[ALLEGRO_KEY_TAB]) {MuteAllChannels();}
  if (kbpressed[ALLEGRO_KEY_MENU] || kbpressed[ALLEGRO_KEY_BACKQUOTE]) {ntsc=!ntsc; if (ntsc) {
    if (tempo==125) {
      tempo=150;
      FPS=60;
    }
    al_set_timer_speed(timer,1/60.0);
  } else {
    if (tempo==150) {
      tempo=125;
      FPS=50;
    }
    al_set_timer_speed(timer,1/50.0);
  }
  }
  if (screen==9 || screen==7) {
    SFXControls();
  }
  if (screen==1) {
    if (kbpressed[ALLEGRO_KEY_MINUS]) {valuewidth--; if (valuewidth<1) {valuewidth=1;}}
    if (kbpressed[ALLEGRO_KEY_EQUALS]) {valuewidth++;}
  }
  if (screen==11) {
    if (kb[ALLEGRO_KEY_LEFT]) {pcmeditoffset-=4*maxval(1,(int)pow(2.0f,-pcmeditscale));}
    if (kb[ALLEGRO_KEY_RIGHT]) {pcmeditoffset+=4*maxval(1,(int)pow(2.0f,-pcmeditscale));}
    if (kbpressed[ALLEGRO_KEY_MINUS]) {pcmeditscale--;}
    if (kbpressed[ALLEGRO_KEY_EQUALS]) {pcmeditscale++;}
    if (kbpressed[ALLEGRO_KEY_SPACE]) {pcmeditenable=!pcmeditenable;}
  }
}
void drawdisp() {
  int delta;
  int firstChan, firstMode;
  int lastChan, lastMode;
  int selTop, selBottom;
  if (playermode) {return;}
  ClickEvents();
  KeyboardEvents();
  if (firstframe) {drawpatterns(true);firstframe=false;}
  // -(int)((((float)speed-(float)curtick)/(float)speed)*12.0f)
  if (screen==0) {
#ifdef SMOOTH_SCROLL
    al_draw_bitmap_region(patternbitmap,0,-minval(0,192-(curpatrow*12)+(int)(((float)curtick/(float)speed)*12.0)),al_get_bitmap_width(patternbitmap),al_get_bitmap_height(patternbitmap),0,60+maxval(0,192-(curpatrow*12)+(int)(((float)curtick/(float)speed)*12.0)),0);
#else
    al_draw_bitmap_region(patternbitmap,0,-minval(0,192-(curpatrow*12)),al_get_bitmap_width(patternbitmap),al_get_bitmap_height(patternbitmap),0,60+maxval(0,192-(curpatrow*12)),0);
    #endif
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_SRC_COLOR);
    for (int j=0;j<8;j++) {
  //al_draw_filled_rectangle(24+(j*96),255,112+(j*96),266,mapHSV(((float)cduty[j+(curedpage*8)]/128)*360,(cshape[j+(curedpage*8)]==4)?(0):(1),(float)cvol[j+(curedpage*8)]/128));
  }
  al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
  firstChan=curedchan; firstMode=curedmode;
  lastChan=curselchan; lastMode=curselmode;
  selTop=selStart; selBottom=selEnd;
  if (lastChan<firstChan) {
    lastChan^=firstChan;
    firstChan^=lastChan;
    lastChan^=firstChan;
    
    lastMode^=firstMode;
    firstMode^=lastMode;
    lastMode^=firstMode;
  } else if (lastChan==firstChan) {
    if (lastMode<firstMode) {
      lastMode^=firstMode;
      firstMode^=lastMode;
      lastMode^=firstMode;
    }
  }
  if (selBottom<selTop) {
    selBottom^=selTop;
    selTop^=selBottom;
    selBottom^=selTop;
  }
  for (int i=firstChan; i<=lastChan; i++) {
    for (int j=(i==firstChan)?firstMode:0; (j<5 && (i<lastChan || j<=lastMode)); j++) {
      al_draw_filled_rectangle(((scrW/2)-400)+modeOff[j]+(i*96)+((8-chanstodisplay)*45),
                               maxval(60,255-(curpatrow-selTop)*12),
                               ((scrW/2)-400)+modeOff[j+1]+(i*96)+((8-chanstodisplay)*45),
                               maxval(60,266-(curpatrow-selBottom)*12),
                               al_map_rgba(128,128,128,128));
    }
  }
  /*
  switch(curselmode) {
  case 0: al_draw_filled_rectangle(((scrW/2)-400)+24+(curselchan*96)+((8-chanstodisplay)*45),255,((scrW/2)-400)+48+(curselchan*96)+((8-chanstodisplay)*45),266,al_map_rgb(128,128,128)); break;
  case 1: al_draw_filled_rectangle(((scrW/2)-400)+48+(curselchan*96)+((8-chanstodisplay)*45),255,((scrW/2)-400)+64+(curselchan*96)+((8-chanstodisplay)*45),266,al_map_rgb(128,128,128)); break;
  case 2: al_draw_filled_rectangle(((scrW/2)-400)+64+(curselchan*96)+((8-chanstodisplay)*45),255,((scrW/2)-400)+88+(curselchan*96)+((8-chanstodisplay)*45),266,al_map_rgb(128,128,128)); break;
  case 3: al_draw_filled_rectangle(((scrW/2)-400)+88+(curselchan*96)+((8-chanstodisplay)*45),255,((scrW/2)-400)+96+(curselchan*96)+((8-chanstodisplay)*45),266,al_map_rgb(128,128,128)); break;
  case 4: al_draw_filled_rectangle(((scrW/2)-400)+96+(curselchan*96)+((8-chanstodisplay)*45),255,((scrW/2)-400)+112+(curselchan*96)+((8-chanstodisplay)*45),266,al_map_rgb(128,128,128)); break;
  }
  */
  al_draw_filled_rectangle(0,(follow)?(255):(fmax(60,255+(maxval(0,curstep)-curpatrow)*12)),scrW+1,((follow)?(266):(fmax(60,(255+(maxval(0,curstep)-curpatrow)*12)+11))),al_map_rgba(64,64,64,128));
  }
  // grid markers
  #ifdef MOUSE_GRID
  al_draw_rectangle(1+((mstate.x/8)*8),1+((mstate.y/12)*12),((mstate.x/8)*8)+9,((mstate.y/12)*12)+14,al_map_rgb(0,255,255),1);
  al_draw_textf(text,getconfigcol(colSEL1),mstate.x,mstate.y-12,ALLEGRO_ALIGN_LEFT,"%d, %d",(mstate.x/8)*8,(mstate.y/12)*12);
  #endif
  
  // header
  g.tPos(0,0);
  g.tColor(14);
  g.printf(PROGRAM_NAME);
  g.tPos(14,0);
  g.printf("r%d",ver);

  // properties - buttons
  //al_draw_text(text,getconfigcol(colSEL1),0,12,ALLEGRO_ALIGN_LEFT,"|");
  patseek+=(curpat-patseek)/4;
  if (fmod(patseek,1)>0.999 || fmod(patseek,1)<0.001) {
    patseek=round(patseek);
  }
  g.tPos(0,1);
  g.tColor(8);
  g.printf("pat|ins|sfx|speed   v^|  |patID   v^|\n");
  g.printf("sng|dsk|mem|tempo   v^|  |octave  v^|\n");
  g.printf("lvl|cfg|vis|order   v^|  |length  v^|\n");
  g.tPos(((float)scrW)/8.0-37,1);
  g.tNLPos(((float)scrW)/8.0-37);
  g.printf("|instr   v^\n");
  g.printf("|  follow  \n");
  g.printf("|   loop   \n");
  g.tNLPos(0);
  al_draw_text(text,(speedlock)?(al_map_rgb(0,255,255)):(getucol(8)),96,12,ALLEGRO_ALIGN_LEFT,"speed");
  al_draw_text(text,(tempolock)?(al_map_rgb(0,255,255)):(getucol(8)),96,24,ALLEGRO_ALIGN_LEFT,"tempo");
  //al_draw_text(text,getucol(15),8,12,ALLEGRO_ALIGN_LEFT,"speed   v^|fx|fxed|pos   v^|patID   v^|diskop|PATTERN|INSTR|SONG|LEVELS|config|help|ED|f|   |  |   ");

  al_draw_text(text,(leftclick && PIR(32,12,56,24,mstate.x,mstate.y))?getconfigcol(colSEL2):al_map_rgb(128+(hover[1]*10),128+(hover[1]*10),128-(hover[1]*5)),32,12,ALLEGRO_ALIGN_LEFT,"ins");
  al_draw_text(text,(leftclick && PIR(0,12,24,24,mstate.x,mstate.y))?getconfigcol(colSEL2):al_map_rgb(128+(hover[0]*10),128+(hover[0]*10),128-(hover[0]*5)),0,12,ALLEGRO_ALIGN_LEFT,"pat");
  al_draw_text(text,(leftclick && PIR(32,24,56,36,mstate.x,mstate.y))?getconfigcol(colSEL2):al_map_rgb(128+(hover[2]*10),128+(hover[2]*10),128-(hover[2]*5)),32,24,ALLEGRO_ALIGN_LEFT,"dsk");
  al_draw_text(text,(leftclick && PIR(0,24,24,36,mstate.x,mstate.y))?getconfigcol(colSEL2):al_map_rgb(128+(hover[3]*10),128+(hover[3]*10),128-(hover[3]*5)),0,24,ALLEGRO_ALIGN_LEFT,"sng");
  al_draw_text(text,(leftclick && PIR(0,36,24,48,mstate.x,mstate.y))?getconfigcol(colSEL2):al_map_rgb(128+(hover[4]*10),128+(hover[4]*10),128-(hover[4]*5)),0,36,ALLEGRO_ALIGN_LEFT,"lvl");
  al_draw_text(text,(leftclick && PIR(32,36,56,48,mstate.x,mstate.y))?getconfigcol(colSEL2):al_map_rgb(128+(hover[5]*10),128+(hover[5]*10),128-(hover[5]*5)),32,36,ALLEGRO_ALIGN_LEFT,"cfg");
  //al_draw_text(text,(leftclick && PIR(640,12,672,28,mstate.x,mstate.y))?getconfigcol(colSEL2):al_map_rgb(128+(hover[6]*10),128+(hover[6]*10),128-(hover[6]*5)),640,12,ALLEGRO_ALIGN_LEFT,"?");
  // because of the new top bar
  //al_draw_text(text,(leftclick && PIR(scrW-80,12,scrW-56,28,mstate.x,mstate.y))?getconfigcol(colSEL2):al_map_rgb(128+(hover[7]*10),128+(hover[7]*10),128-(hover[7]*5)),scrW-80,12,ALLEGRO_ALIGN_LEFT,"ply");
  //al_draw_text(text,(leftclick && PIR(scrW-24,12,scrW,28,mstate.x,mstate.y))?getconfigcol(colSEL2):al_map_rgb(128+(hover[8]*10),128+(hover[8]*10),128-(hover[8]*5)),scrW-24,12,ALLEGRO_ALIGN_LEFT,"stp");
  //al_draw_text(text,(leftclick && PIR(scrW-48,12,scrW-32,28,mstate.x,mstate.y))?getconfigcol(colSEL2):al_map_rgb(128+(hover[9]*10),128+(hover[9]*10),128-(hover[9]*5)),scrW-48,12,ALLEGRO_ALIGN_LEFT,"pt");
  g.tColor(14);
  g.tPos(18,1); g.printf("%.2X",speed);
  g.tPos(17,2); g.printf("%d",tempo);
  g.tPos(18,3); g.printf("%.2X",curpat);
  al_draw_textf(text,al_map_rgb(0,255,255),256,12,ALLEGRO_ALIGN_LEFT,"%.2X",patid[curpat]);
  al_draw_textf(text,al_map_rgb(0,255,255),256,24,ALLEGRO_ALIGN_LEFT,"%.2X",curoctave);
  al_draw_textf(text,al_map_rgb(0,255,255),256,36,ALLEGRO_ALIGN_LEFT,"%.2X",patlength[patid[curpat]]);
  
  if (curins==0) {
    al_draw_text(text,al_map_rgb(0,255,255),scrW-30*8,12,ALLEGRO_ALIGN_LEFT,"--");
  } else {
    al_draw_textf(text,al_map_rgb(0,255,255),scrW-30*8,12,ALLEGRO_ALIGN_LEFT,"%.2X",curins);
  }
  if (follow) {
    al_draw_text(text,al_map_rgb(0,255,255),scrW-34*8,24,ALLEGRO_ALIGN_LEFT,"follow");
  }
  
  //al_draw_text(text,getconfigcol(colDEFA),0,24,ALLEGRO_ALIGN_LEFT,"----------------------------------------------------------------------------------------------------");
  //al_draw_text(text,getucol(15),0,36,ALLEGRO_ALIGN_LEFT,"|octave   ^v|reverse|step|curstep   |curpat   |curtick   |speed ");
  //al_draw_text(text,getucol(15),512,36,ALLEGRO_ALIGN_LEFT,name);
  al_draw_textf(text,getconfigcol(colDEFA),scrW-180,8,ALLEGRO_ALIGN_LEFT,"%.2x/%.2x",curtick,speed);
  al_draw_textf(text,getconfigcol(colDEFA),scrW-180,20,ALLEGRO_ALIGN_LEFT,"%.2x/%.2x",maxval(0,curstep),patlength[patid[curpat]]);
  al_draw_textf(text,getconfigcol(colDEFA),scrW-192,32,ALLEGRO_ALIGN_LEFT,"%.2x:%.2x/%.2x",patid[curpat],curpat,songlength);
  /*al_draw_textf(text,al_map_rgb(255,255,255),272,36,ALLEGRO_ALIGN_LEFT,"%.2x",curstep);
  al_draw_textf(text,al_map_rgb(255,255,255),352,36,ALLEGRO_ALIGN_LEFT,"%.2x",curpat);
  al_draw_textf(text,al_map_rgb(255,255,255),440,36,ALLEGRO_ALIGN_LEFT,"%.2x",curtick);
  al_draw_textf(text,al_map_rgb(255,255,255),512,36,ALLEGRO_ALIGN_LEFT,"%.2x",speed);*/
  // draw orders
  al_set_clipping_rectangle(184*dpiScale,16*dpiScale,16*dpiScale,36*dpiScale);
  delta=(64*fmod(patseek,1));
  al_draw_textf(text,al_map_rgb(128-delta,128-delta,128-delta),184,0-(fmod(patseek,1)*12),ALLEGRO_ALIGN_LEFT,"%.2X",patid[maxval((int)patseek-2,0)]);
  al_draw_textf(text,al_map_rgb(192-delta,192-delta,192-delta),184,12-(fmod(patseek,1)*12),ALLEGRO_ALIGN_LEFT,"%.2X",patid[maxval((int)patseek-1,0)]);
  al_draw_textf(text,al_map_rgb(255-delta,255-delta,255-delta),184,24-(fmod(patseek,1)*12),ALLEGRO_ALIGN_LEFT,"%.2X",patid[(int)patseek]);
  al_draw_textf(text,al_map_rgb(191+delta,191+delta,191+delta),184,36-(fmod(patseek,1)*12),ALLEGRO_ALIGN_LEFT,"%.2X",patid[minval((int)patseek+1,255)]);
  al_draw_textf(text,al_map_rgb(128+delta,128+delta,128+delta),184,48-(fmod(patseek,1)*12),ALLEGRO_ALIGN_LEFT,"%.2X",patid[maxval((int)patseek+2,0)]);
  al_reset_clipping_rectangle();
  // boundaries
  al_draw_line(scrW-200,0,scrW-200,59,al_map_rgb(255,255,255),1);
  al_draw_line(0,59,scrW,59,al_map_rgb(255,255,255),1);
  // play/pattern/stop buttons
  al_draw_rectangle((scrW/2)-61,13,(scrW/2)-21,37,al_map_rgb(255,255,255),2);
  al_draw_rectangle((scrW/2)-20,13,(scrW/2)+20,37,al_map_rgb(255,255,255),2);
  al_draw_rectangle((scrW/2)+21,13,(scrW/2)+61,37,al_map_rgb(255,255,255),2);
  // reverse/step/follow buttons
  al_draw_rectangle((scrW/2)-61,37,(scrW/2)-21,48,al_map_rgb(255,255,255),2);
  al_draw_rectangle((scrW/2)-20,37,(scrW/2)+20,48,al_map_rgb(255,255,255),2);
  al_draw_rectangle((scrW/2)+21,37,(scrW/2)+61,48,al_map_rgb(255,255,255),2);
  // play button
  al_draw_line((scrW/2)-48,18,(scrW/2)-48,32,al_map_rgb(255,255,255),2);
  al_draw_line((scrW/2)-48,18,(scrW/2)-34,25,al_map_rgb(255,255,255),2);
  al_draw_line((scrW/2)-48,32,(scrW/2)-34,25,al_map_rgb(255,255,255),2);
  // pattern button
  al_draw_line((scrW/2)-8,17,(scrW/2)-8,33,al_map_rgb(255,255,255),2);
  al_draw_line((scrW/2)-5,18,(scrW/2)-5,32,al_map_rgb(255,255,255),2);
  al_draw_line((scrW/2)-5,18,(scrW/2)+9,25,al_map_rgb(255,255,255),2);
  al_draw_line((scrW/2)-5,32,(scrW/2)+9,25,al_map_rgb(255,255,255),2);
  // stop button
  al_draw_rectangle((scrW/2)+34,18,(scrW/2)+48,32,al_map_rgb(255,255,255),2);
  // oscilloscope
  al_draw_line(scrW-128,0,scrW-128,59,al_map_rgb(255,255,255),1);
  al_set_blender(ALLEGRO_ADD,ALLEGRO_ONE,ALLEGRO_ONE);
  for (int ii=0;ii<128;ii++) {
    al_draw_pixel((ii+scrW-128)+0.5,36+(int)(oscbuf[ii]*18)-9+0.5,al_map_rgb_f(0,(1-(((oscbuf[ii]+1)*18)-(int)((oscbuf[ii]+1)*18)))*1.5,0));
    al_draw_pixel((ii+scrW-128)+0.5,36+(int)(oscbuf2[ii]*18)-9+0.5,al_map_rgb_f((1-(((oscbuf2[ii]+1)*18)-(int)((oscbuf2[ii]+1)*18)))*1.5,0,0));
    //al_draw_pixel((ii+scrW-128)+0.5,36+(int)(oscbuf2[ii]*18)-8+0.5,getconfigcol(colDOWN));
    al_draw_pixel((ii+scrW-128)+0.5,36+(int)(oscbuf[ii]*18)-8+0.5,al_map_rgb_f(0,((((oscbuf[ii]+1)*18)-(int)((oscbuf[ii]+1)*18)))*1.5,0));
    al_draw_pixel((ii+scrW-128)+0.5,36+(int)(oscbuf2[ii]*18)-8+0.5,al_map_rgb_f(((((oscbuf2[ii]+1)*18)-(int)((oscbuf2[ii]+1)*18)))*1.5,0,0));
    //al_draw_pixel((ii+scrW-128)+0.5,36+(int)(oscbuf2[ii]*18)-7+0.5,getconfigcol(colDOWN));
  }
  al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
  switch(screen) {
    case 0: drawpatterns(false); break;
    case 1: drawinsedit(); break;
    case 2: drawdiskop(); break;
    case 3: drawsong(); break;
    case 4: drawmixer(); break;
    case 5: drawconfig(); break;
    case 6: drawhelp(); break;
    case 7: drawabout(); break;
    case 8: drawcomments(); break;
    case 9: drawsfxpanel(); break;
    case 10: drawmemory(); break;
    case 11: drawpcmeditor(); break;
    case 12: drawpiano(); break;
  }
  //al_draw_text(text,getucol(10),56,12,ALLEGRO_ALIGN_LEFT,(string)defspeed);
}
int playfx(const unsigned char* fxdata,int fxpos,int achan) {
  // returns number if not in the end, otherwise -1
  int fxoffset=0;
  if (fxpos==-1) {sfxplaying=false;return -1;}
  if (fxpos==0) {
    // save current channel's state
    if (!sfxplaying) {
    sfxsduty=cduty[achan];
    sfxsshape=cshape[achan];
    sfxspan=cpan[achan];
    sfxsfmode=cfmode[achan];
    sfxsoff=coff[achan];
    sfxsreso=creso[achan];
    sfxsfreq=cfreq[achan];
    }
    // reset channel
    sfxcvol=0;sfxcpan=0;sfxcfmode=0;sfxcoff=262144;sfxcreso=0;
    sfxcfreq=1;sfxcmode=0;sfxcrm=0;sfxcshape=0;sfxcduty=63;
    sfxplaying=true;
  }
  if (fxdata[fxpos]==0) {
    sfxcvol=0;
    cvol[achan]=0;
    cduty[achan]=sfxsduty;
    cshape[achan]=sfxsshape;
    cpan[achan]=sfxspan;
    cfmode[achan]=sfxsfmode;
    coff[achan]=sfxsoff;
    creso[achan]=sfxsreso;
    cfreq[achan]=sfxsfreq;
    sfxplaying=false;
    return -1;}
  if (fxdata[fxpos]&1) {
    sfxcfreq=(fxdata[fxpos+1+fxoffset]*256)+fxdata[fxpos+2+fxoffset];
    sfxcvol=fxdata[fxpos+3+fxoffset];
    fxoffset+=3;
  }
  if (fxdata[fxpos]&2) {
    sfxcpan=fxdata[fxpos+1+fxoffset];
    fxoffset++;
  }
  if (fxdata[fxpos]&4) {
    sfxcduty=fxdata[fxpos+1+fxoffset];
    sfxcshape=fxdata[fxpos+2+fxoffset];
    fxoffset+=2;
  }
  if (fxdata[fxpos]&8) {
    fxoffset+=5;
  }
  if (fxdata[fxpos]&16) {
    fxoffset+=9;
  }
  if (fxdata[fxpos]&32) {
    sfxcfmode=fxdata[fxpos+1+fxoffset];
    sfxcoff=(fxdata[fxpos+2+fxoffset]*65536)+(fxdata[fxpos+2+fxoffset]*256)+fxdata[fxpos+4+fxoffset];
    sfxcreso=fxdata[fxpos+5+fxoffset];
    fxoffset+=5;
  }
  cvol[achan]=sfxcvol;
  cfreq[achan]=sfxcfreq;
  cduty[achan]=sfxcduty;
  cshape[achan]=sfxcshape;
  cpan[achan]=sfxcpan;
  cfmode[achan]=sfxcfmode;
  coff[achan]=sfxcoff;
  creso[achan]=sfxcreso;
  cmode[achan]=0;
  crm[achan]=0;

  return fxoffset+fxpos+1;
}

int main(int argc, char **argv) {
  ALLEGRO_TRANSFORM tra;
  detunefactor=1;
  chip[0].Init();
  chip[1].Init();
  chip[2].Init();
  chip[3].Init();
  printf("soundtracker (r%d)\n",ver);
  framecounter=0;
  playermode=false;
  comments=new char[65536];
  memset(comments,0,65536);
  int filearg=0;
  if (argc>1) {
    // for each argument
    for (int i=1;i<argc;i++) {
        // is it an option?
        if (argv[i][0]=='-') {
      // is it a multi-char option?
           if (argv[i][1]=='-') {
         if (!strcmp(&argv[i][2],"help")) {
           printf("usage: soundtracker [--help] [-o order] [-f song] [-v] [] [song]\n"); return 0;
         }
       }
       // -n
       if (argv[i][1]=='n') {
         ntsc=true;
       }
       // -o ORDER
       if (argv[i][1]=='o') {
         curpat=atoi(argv[i+1]); i++;
       }
       // -f FILE
       if (argv[i][1]=='f') {
         fileswitch=true;filearg=++i;
       }
    } else {playermode=true;filearg=i;}
    }
    }
      if (ntsc) {
#ifdef FILM
FPS = 24;
tempo=60;
DETUNE_FACTOR_GLOBAL=0.5;
#else
FPS = 60;
tempo=150;
DETUNE_FACTOR_GLOBAL=1.2;
#endif
  }else{
FPS = 50;
tempo=125;
DETUNE_FACTOR_GLOBAL=1;
  }
   ALLEGRO_EVENT_QUEUE *event_queue = NULL;
   ALLEGRO_FILE *texthand;
   filessorted.resize(1024);
   filenames.resize(1024);
   //int success=0;
   bool is_audio_inited=false;
   int helpfilesize;
   scrW=800;
   scrH=450;
   // create memory blocks
   patlength=new unsigned char[256];
   //texthand=al_fopen("help.txt","rb");
  texthand=NULL;
  if (texthand!=NULL) { // read the file
  printf("loading helpfile, ");
    helpfilesize=al_fsize(texthand);
  printf("%d bytes\n",helpfilesize);
    helptext=new char[helpfilesize];
    al_fseek(texthand,0,ALLEGRO_SEEK_SET);
    al_fread(texthand,helptext,helpfilesize);
    al_fclose(texthand);
  } else {
    helptext=new char[18];
    //helptext="help.txt not found";
  }
  // allocate memory for files
  /*for (int ii=0;ii<1024;ii++) {
  filenames[ii].name=new char[257];
  }*/

   printf("initializing allegro\n");
   if (!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }
   
   curdir=new char[4096];
   strcpy(curdir,al_get_current_directory());
   print_entry(curdir);
   unsigned char thenote=0;
   al_init_font_addon();
   al_init_ttf_addon();
   al_init_primitives_addon();

   printf("creating timer\n");
   printf("%f\n",FPS);
   timer = al_create_timer(1.0 / FPS);
   if (!timer) {
      fprintf(stderr, "failed to create timer!\n");
      return -1;
   }
   if (!playermode) {
   printf("creating display\n");
   al_set_new_display_flags(ALLEGRO_WINDOWED|ALLEGRO_RESIZABLE);
#ifndef __APPLE__
#ifndef __MINGW32__
al_set_new_window_title("soundtracker");
#endif
#endif
   dpiScale=getScale();
   display = al_create_display(scrW*dpiScale,scrH*dpiScale);
   //al_set_display_option(display,ALLEGRO_VSYNC,ALLEGRO_REQUIRE);
   if (!display) {
      fprintf(stderr, "failed to create display!\n");
      al_destroy_timer(timer);
      return -1;
   }}

   printf("loading font\n");
   text=al_load_ttf_font("unifont.ttf",16,0);
   if (text==NULL) {
     fprintf(stderr,"unifont.ttf wasn't found - halting");
     return -1;
   }
   g.init(text);
   printf("initializing mouse input\n");
   bool ismouse=al_install_mouse();
   if (!ismouse) {
     fprintf(stderr,"there will be no mouse!");
   }
   printf("initializing keyboard input\n");
   bool iskb=al_install_keyboard();
   if (!iskb) {
     fprintf(stderr,"there will be no keyboard! :(");
   }
   patternbitmap=al_create_bitmap(scrW,scrH);
   piano=al_create_bitmap(700,60);
   pianoroll=al_create_bitmap(700,128);
   pianoroll_temp=al_create_bitmap(700,128);
   mixer=al_create_bitmap(scrW,scrH);
   al_init_image_addon();
   logo=al_load_bitmap("logo.png");
   //logo=al_create_bitmap(360,240);
   //al_set_target_bitmap(logo);
   // <~>'s logo
   //al_draw_rectangle(10,10,350,230,al_map_rgb(255,255,255),5);

   printf("initializing audio system\n");
   is_audio_inited=al_install_audio();
   is_audio_inited&=al_init_acodec_addon();
   if (!is_audio_inited) {
     fprintf(stderr, "unable to init audio -- program might be inaudible\n");
     return -1;
   }
     al_reserve_samples(0);
  printf("cleaning up stuff\n");
  for (int nonsense=0;nonsense<256;nonsense++) {
    patlength[nonsense]=64;
    instrument[nonsense].noteOffset=48;
  }
  for (int nonsense2=0;nonsense2<256;nonsense2++) {
    instrument[nonsense2].noteOffset=48;
  }
   // init colors
   ALLEGRO_COLOR colors[256]={};
   for (int lc=0;lc<256;lc++) {
     colors[lc]=getucol(lc);
   }
   al_set_target_bitmap(patternbitmap);
   al_clear_to_color(al_map_rgb(0,0,0));
   al_set_target_bitmap(pianoroll);
   al_clear_to_color(al_map_rgb(0,0,0));
   al_set_target_bitmap(pianoroll_temp);
   al_clear_to_color(al_map_rgb(0,0,0));
   al_set_target_bitmap(piano);
   al_clear_to_color(al_map_rgb(0,0,0));
   // draw a piano
  for (int ii=0;ii<10;ii++) {
    for (int jj=0; jj<7; jj++) {
      al_draw_filled_rectangle((jj*10)+(ii*70),60-60,((jj+1)*10)+(ii*70),60,al_map_rgb(64,64,64));
      al_draw_filled_rectangle((jj*10)+1+(ii*70),60-59,((jj+1)*10)+(ii*70),60-1,al_map_rgb(224,224,224));
    }
    for (int jj=0; jj<6; jj++) {
      if (jj==2) continue;
      al_draw_filled_rectangle((jj+0.666666667)*10+(ii*70),60-60,((jj+1.36)*10)+(ii*70),60-25,al_map_rgb(0,0,0));
    }
/*
    for (int jj=0;jj<12;jj++) {
    if (jj==0 || jj==2 || jj==4 || jj==5 || jj==7 || jj==9 || jj==11) {
    al_draw_filled_rectangle((jj*6)+(ii*72),60-60,(jj*6)+6+(ii*72),60,al_map_rgb(64,64,64));
    al_draw_filled_rectangle((jj*6)+1+(ii*72),60-59,(jj*6)+5+(ii*72),60-1,al_map_rgb(255,255,255));
    }
    else {
    al_draw_filled_rectangle((jj*6)+(ii*72),60-60,(jj*6)+6+(ii*72),60-29,al_map_rgb(0,0,0));
    al_draw_filled_rectangle((jj*6)+(ii*72),60-29,(jj*6)+6+(ii*72),60,al_map_rgb(64,64,64));
    al_draw_filled_rectangle((jj*6)+1+(ii*72),60-28,(jj*6)+5+(ii*72),60-1,al_map_rgb(255,255,255));
    }
    }
*/
  }
   if (!playermode) {
     al_set_target_bitmap(mixer);
     al_clear_to_color(al_map_rgb(0,0,0));
       drawmixerlayer();
     al_set_target_bitmap(al_get_backbuffer(display));}
   printf("creating event queue\n");
   event_queue = al_create_event_queue();
   if (!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }
   printf("initializing audio channels\n");
   initaudio();
   //success=ImportMOD();
   CleanupPatterns();
   // register_event_sources
  if (!playermode) {al_register_event_source(event_queue, al_get_display_event_source(display));
  al_register_event_source(event_queue, al_get_keyboard_event_source());}
   al_register_event_source(event_queue, al_get_timer_event_source(timer));
   // clear to black
   if (!playermode) {al_clear_to_color(al_map_rgb(0,0,0));
   // flip buffers
   al_flip_display();}
   //printf("%d",argc);
   if (playermode || fileswitch) {
     if (LoadFile(argv[filearg])) {return 1;}
     if (playermode) {Play();
     printf("playing: %s\n",name);}
   }
   // some sound effects
   const char* examplefxerror="\x01\x01\x70\x7f\x02\x00\x02\x00\x02\x00\x02\x00\x02\x00\x02\x00\x02\x00\x02\x00\x02\x00\x01\x00\x00\x00\x00";
   for (int ii=0;ii<27;ii++) {
     sfxdata[1][ii]=examplefxerror[ii];
   }
   // example pauser sound effect
   int uselesspointer=0;
   sfxdata[30][uselesspointer+0]=1; sfxdata[30][uselesspointer+1]=0; sfxdata[30][uselesspointer+2]=226; sfxdata[30][uselesspointer+3]=120;
   sfxdata[30][uselesspointer+4]=1; sfxdata[30][uselesspointer+5]=0; sfxdata[30][uselesspointer+6]=226; sfxdata[30][uselesspointer+7]=112;
   sfxdata[30][uselesspointer+8]=1; sfxdata[30][uselesspointer+9]=0; sfxdata[30][uselesspointer+10]=226; sfxdata[30][uselesspointer+11]=104;
   sfxdata[30][uselesspointer+12]=1; sfxdata[30][uselesspointer+13]=0; sfxdata[30][uselesspointer+14]=226; sfxdata[30][uselesspointer+15]=104;
   sfxdata[30][uselesspointer+16]=1; sfxdata[30][uselesspointer+17]=0; sfxdata[30][uselesspointer+18]=226; sfxdata[30][uselesspointer+19]=96;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=0; sfxdata[30][uselesspointer+22]=226; sfxdata[30][uselesspointer+23]=88;
   uselesspointer=24;
   sfxdata[30][uselesspointer+0]=1; sfxdata[30][uselesspointer+1]=1; sfxdata[30][uselesspointer+2]=26; sfxdata[30][uselesspointer+3]=120;
   sfxdata[30][uselesspointer+4]=1; sfxdata[30][uselesspointer+5]=1; sfxdata[30][uselesspointer+6]=26; sfxdata[30][uselesspointer+7]=112;
   sfxdata[30][uselesspointer+8]=1; sfxdata[30][uselesspointer+9]=1; sfxdata[30][uselesspointer+10]=26; sfxdata[30][uselesspointer+11]=104;
   sfxdata[30][uselesspointer+12]=1; sfxdata[30][uselesspointer+13]=1; sfxdata[30][uselesspointer+14]=26; sfxdata[30][uselesspointer+15]=104;
   sfxdata[30][uselesspointer+16]=1; sfxdata[30][uselesspointer+17]=1; sfxdata[30][uselesspointer+18]=26; sfxdata[30][uselesspointer+19]=96;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=88;
   uselesspointer=48;
   sfxdata[30][uselesspointer+0]=1; sfxdata[30][uselesspointer+1]=0; sfxdata[30][uselesspointer+2]=226; sfxdata[30][uselesspointer+3]=120;
   sfxdata[30][uselesspointer+4]=1; sfxdata[30][uselesspointer+5]=0; sfxdata[30][uselesspointer+6]=226; sfxdata[30][uselesspointer+7]=112;
   sfxdata[30][uselesspointer+8]=1; sfxdata[30][uselesspointer+9]=0; sfxdata[30][uselesspointer+10]=226; sfxdata[30][uselesspointer+11]=104;
   sfxdata[30][uselesspointer+12]=1; sfxdata[30][uselesspointer+13]=0; sfxdata[30][uselesspointer+14]=226; sfxdata[30][uselesspointer+15]=104;
   sfxdata[30][uselesspointer+16]=1; sfxdata[30][uselesspointer+17]=0; sfxdata[30][uselesspointer+18]=226; sfxdata[30][uselesspointer+19]=96;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=0; sfxdata[30][uselesspointer+22]=226; sfxdata[30][uselesspointer+23]=88;
   uselesspointer=72;
   sfxdata[30][uselesspointer+0]=1; sfxdata[30][uselesspointer+1]=1; sfxdata[30][uselesspointer+2]=26; sfxdata[30][uselesspointer+3]=120;
   sfxdata[30][uselesspointer+4]=1; sfxdata[30][uselesspointer+5]=1; sfxdata[30][uselesspointer+6]=26; sfxdata[30][uselesspointer+7]=112;
   sfxdata[30][uselesspointer+8]=1; sfxdata[30][uselesspointer+9]=1; sfxdata[30][uselesspointer+10]=26; sfxdata[30][uselesspointer+11]=104;
   sfxdata[30][uselesspointer+12]=1; sfxdata[30][uselesspointer+13]=1; sfxdata[30][uselesspointer+14]=26; sfxdata[30][uselesspointer+15]=104;
   sfxdata[30][uselesspointer+16]=1; sfxdata[30][uselesspointer+17]=1; sfxdata[30][uselesspointer+18]=26; sfxdata[30][uselesspointer+19]=96;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=88;
   uselesspointer+=4;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=80;
   uselesspointer+=4;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=72;
   uselesspointer+=4;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=72;
   uselesspointer+=4;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=64;
   uselesspointer+=4;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=56;
   uselesspointer+=4;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=48;
   uselesspointer+=4;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=40;
   uselesspointer+=4;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=40;
   uselesspointer+=4;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=32;
   uselesspointer+=4;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=24;
   uselesspointer+=4;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=16;
   uselesspointer+=4;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=8;
   uselesspointer+=4;
   sfxdata[30][uselesspointer+20]=1; sfxdata[30][uselesspointer+21]=1; sfxdata[30][uselesspointer+22]=26; sfxdata[30][uselesspointer+23]=8;
   sfxdata[30][uselesspointer+24]=0;
   printf("run\n");
   // run timer
   al_start_timer(timer);
#ifndef JACK
   SDL_PauseAudioDevice(audioID,0);
#endif
   printf("done\n");
   // MAIN LOOP
   al_identity_transform(&tra);
   al_scale_transform(&tra,dpiScale,dpiScale);
   al_use_transform(&tra);
   if (playermode) {
     al_rest(3600);
   } else {
   while (1)
   {
      ALLEGRO_EVENT ev;
      while (al_get_next_event(event_queue, &ev)) {

      if (ev.type == ALLEGRO_EVENT_TIMER) {
     //al_set_timer_speed(timer,1.0/FPS);
 
     skipframe=true;//!skipframe;
      }
      else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
     printf("close button pressed\n");
     al_stop_timer(timer);
                 quit=true;
         break;
      } else if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
      al_acknowledge_resize(display);
      al_destroy_bitmap(mixer);
      // recreate pattern bitmap
      al_destroy_bitmap(patternbitmap);
      patternbitmap=al_create_bitmap(al_get_display_width(display)/dpiScale,al_get_display_height(display)/dpiScale);
      scrW=al_get_display_width(display)/dpiScale;
      scrH=al_get_display_height(display)/dpiScale;
      mixer=al_create_bitmap(scrW,scrH);
      drawmixerlayer();
      drawpatterns(true);
    } else if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
      //printf("event, %c\n",ev.keyboard.unichar);
      if (inputvar!=NULL) {
      if (ev.keyboard.keycode==ALLEGRO_KEY_BACKSPACE) {inputcurpos--; if (inputcurpos<0) {inputcurpos=0;}; inputvar[inputcurpos]=0;}
      else if (ev.keyboard.keycode==ALLEGRO_KEY_RIGHT) {
        inputcurpos++; if (inputcurpos>(int)strlen(inputvar)) {inputcurpos=strlen(inputvar);}
      } else if (ev.keyboard.keycode==ALLEGRO_KEY_LEFT) {
        inputcurpos--; if (inputcurpos<0) {inputcurpos=0;}
      } else if (ev.keyboard.keycode==ALLEGRO_KEY_HOME) {
        inputcurpos=0;
      } else if (ev.keyboard.keycode==ALLEGRO_KEY_END) {
        inputcurpos=strlen(inputvar);
      } else { if (maxinputsize>inputcurpos) {
      memmove(inputvar+1+inputcurpos,inputvar+inputcurpos,maxinputsize-inputcurpos);
      inputvar[inputcurpos]=ev.keyboard.unichar;
      inputcurpos++;} else {sfxpos=0;}
      //printf("new inputvar value: %s\n",inputvar);
      }
      }
    } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
      if (screen==1) {
        Play(); playmode=2;
        RunTestNote(ev.keyboard.keycode);
      }
    }
      }
      if (true) {
        doframe=1;
  rt3=al_get_time();
      framecounter++;
     if (!playermode) {
     scrW=al_get_display_width(display)/dpiScale;
     scrH=al_get_display_height(display)/dpiScale;
     }

     maxrasterdelta=(maxval(0,raster2-raster1)>maxrasterdelta)?(maxval(0,raster2-raster1)):(maxrasterdelta);
     //printf("%f\n",raster2-raster1);
     ////cout << "\n";
         if (!playermode) {if (skipframe) {al_clear_to_color(al_map_rgb(0,0,0));
     drawdisp();
     if (kb[ALLEGRO_KEY_LSHIFT]) {
     al_draw_filled_rectangle(0,raster1,scrW,(raster1>raster2)?(525):(raster2),al_map_rgba(64,64,128,64));
     if (raster1>raster2) {
       al_draw_filled_rectangle(0,0,scrW,raster2,al_map_rgba(64,64,128,64));
     }
     al_draw_textf(text,al_map_rgb(255,0,0),scrW,0,ALLEGRO_ALIGN_RIGHT,"scantime: %.3x-%.3x %.3x/%.3x",(int)raster1,(int)raster2,(int)(maxval(0,raster2-raster1)),(int)maxrasterdelta);
       if (ntsc) {
    #ifdef FILM
     al_draw_textf(text,al_map_rgb(255,0,0),scrW/2,0,ALLEGRO_ALIGN_CENTER,"FPS: %.1f ASC/i: %x ASC/t: %.5x FILM",FPS,(6180000/(double)ASC::interval)*2.5,ASC::interval,ASC::currentclock);
    #else
     al_draw_textf(text,al_map_rgb(255,0,0),scrW/2,0,ALLEGRO_ALIGN_CENTER,"%.1fHz 525 CLOCK: 6.18MHz i: %x t: %.5x NTSC",FPS,(6180000/(double)ASC::interval)*2.5,ASC::interval,ASC::currentclock);
    #endif
     } else {
     al_draw_textf(text,al_map_rgb(255,0,0),scrW/2,0,ALLEGRO_ALIGN_CENTER,"%.1fHz 625 CLOCK: 5.95MHz i: %x t: %.5x PAL",FPS,(5950000/(double)ASC::interval)*2.5,ASC::interval,ASC::currentclock);
     }
     al_draw_text(text,al_map_rgb(255,0,0),scrW,80,ALLEGRO_ALIGN_RIGHT,"timings:");
     al_draw_textf(text,al_map_rgb(255,0,0),scrW,92,ALLEGRO_ALIGN_RIGHT,"playback: %f",raster2-raster1);
     al_draw_textf(text,al_map_rgb(255,0,0),scrW,104,ALLEGRO_ALIGN_RIGHT,"audio: %.2fms, %3.0f%% load",fabs(rt2-rt1)*1000,fabs(rt2-rt1)*44100);
     rt4=al_get_time();
     al_draw_textf(text,al_map_rgb(255,0,0),scrW,116,ALLEGRO_ALIGN_RIGHT,"display: %.2fms, %.0f (max) FPS, %3.0f%% load",(rt4-rt3)*1000,(1/FPS)/(rt4-rt3)*FPS,(rt4-rt3)/(1/FPS)*100);
     al_draw_textf(text,al_map_rgb(255,0,0),scrW,128,ALLEGRO_ALIGN_RIGHT,"total frame time: %.2fms",(time1-time2)*1000);
     }
     al_flip_display();
     al_wait_for_vsync();
     time2=time1;
     time1=al_get_time();
  }}
     
      }
      if (quit) {
        break;
      }
   }
   }
    printf("destroying audio system\n");
   al_uninstall_audio();
#ifdef JACK
   jack_deactivate(jclient);
#endif
   printf("destroying timer\n");
   al_destroy_timer(timer);
   printf("destroying display\n");
   al_destroy_display(display);
   printf("destroying event queue\n");
   al_destroy_event_queue(event_queue);
   /*for (int destroyid=0;destroyid<32;destroyid++) {
    al_drain_audio_stream(chan[destroyid]);
    al_destroy_audio_stream(chan[destroyid]);
   }*/
   al_shutdown_primitives_addon();
   printf("destroying some buffers\n");
   delete[] patlength;
   delete[] comments;
   printf("finished\n");
   return 0;
}

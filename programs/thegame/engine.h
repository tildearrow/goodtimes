// tildearrow's game engine!
#ifndef header_game
#define header_game
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>

int add(int x, int y);
void setinputpointers(ALLEGRO_MOUSE_STATE* m,ALLEGRO_KEYBOARD_STATE* k);
enum eventnames {evNone=0,evCreate,evDestroy,evCollision,evMouseIn,evMouseLeftPressed,evMouseLeftClick};
enum colmethods {colBoundBox,colRadius,colPrecise};

class entitytype {
  ALLEGRO_BITMAP* spr;
};

class sprite {
  ALLEGRO_BITMAP* bitmap;
  int colmethod;
};

class entity {
    void calculateevents(void);
    entitytype* obj;
  public:
    bool events[7];
    bool exists;
    float x,y;
    float direction,speed;
    float xscale,yscale;
    float angle;
    ALLEGRO_BITMAP* spr;
    bool** colmap;
    void update(void);
    void draw(void);
    void destroy(void);
};

class scene {
  public:
    std::vector<entity> entities;
    std::vector<entity>::iterator ei;
    void update(void);
    void draw(void);
    int spawn(float x, float y);
};

#endif
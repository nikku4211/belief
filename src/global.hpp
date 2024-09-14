#include <stdio.h>
#include <soa.h>

extern unsigned char level_metatile_index;

struct Metatile {
  uint8_t tl;
  uint8_t tr;
  uint8_t bl;
  uint8_t br;
  uint8_t attr;
	uint8_t coll;
};

#define SOA_STRUCT Metatile
#define SOA_MEMBERS MEMBER(tl) MEMBER(tr) MEMBER(bl) MEMBER(br) MEMBER(attr) MEMBER(coll)
#include <soa-struct.inc>

extern const soa::Array<Metatile, 22> global_metatiles;

extern unsigned char collision_map[256];

extern int scroll_x;

extern uint8_t parallax_buf[6 * 16];

extern int16_t old_parallax_scroll;

extern volatile char NAME_UPD_ENABLE;

extern unsigned char collision_L;
extern unsigned char collision_R;
extern unsigned char collision_U;
extern unsigned char collision_D;
extern unsigned char eject_L;   // from the left
extern unsigned char eject_R;   // remember these from the collision sub routine
extern unsigned char eject_D;   // from below
extern unsigned char eject_U;   // from up

struct Player {
  unsigned x; // low byte is sub-pixel
  unsigned y;
  int vel_x; // speed, signed, low byte is sub-pixel
  int vel_y;
  unsigned char direction;
  unsigned char onground;
  unsigned char fall_delay_counter; //delay after a fall before you can't jump
  unsigned char frame_counter;
  unsigned char frame;
  unsigned char const * const * animation;
  unsigned char const * animation_duration;
  unsigned char is_crouching;
  unsigned char is_punching;
  unsigned char punch_counter;
};

extern struct Player Ninj;

extern unsigned char L_R_switch;

extern unsigned char prev_pad_state;
extern unsigned char pad1;
extern unsigned char pad1_new;

#define MAX_ENEMY 8

extern unsigned enemy_x[MAX_ENEMY];
extern unsigned enemy_y[MAX_ENEMY];
extern unsigned char enemy_active[MAX_ENEMY];
extern unsigned char enemy_room[MAX_ENEMY];
extern unsigned enemy_actual_x[MAX_ENEMY];
extern unsigned char enemy_type[MAX_ENEMY];
extern const unsigned char *enemy_anim[MAX_ENEMY];

#define MAX_SHURIK 3

extern unsigned shurik_x[MAX_SHURIK];
extern unsigned shurik_y[MAX_SHURIK];
extern int shurik_vel_x[MAX_SHURIK];
extern int shurik_vel_y[MAX_SHURIK];
extern unsigned char shurik_active[MAX_SHURIK];
extern unsigned char shurik_room[MAX_SHURIK];
extern unsigned shurik_actual_x[MAX_SHURIK];
extern unsigned char shurik_throw_index;

extern unsigned char game_mode;
enum {
  MODE_TITLE,
  MODE_GAME,
  MODE_PAUSE,
  MODE_SWITCH,
  MODE_END,
  MODE_GAME_OVER
};

extern unsigned char level;
extern unsigned char level_up;
extern unsigned char death;
extern unsigned char map_loaded;   // only load it once
extern unsigned char enemy_frames; // in case of skipped frames

void sprite_obj_init(void);

void update_tiles(int8_t diff);

void enemy_moves(void);
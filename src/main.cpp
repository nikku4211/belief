#include <bank.h>
#include <nesdoug.h>
#include <neslib.h>
#include <string.h>
#include <soa.h>

#include "../chr/testbg.h"
#include "../chr/playerspr.h"
#include "../chr/enemyninjspr.h"
#include "../chr/shurikspr.h"

#include "../level/rooms.h"
#include "sprites.h"
#include "anim.h"

#define LEFT 0
#define RIGHT 1
#define ACCEL 0x40
#define GRAVITY 0x60
#define MAX_SPEED 0x320
#define JUMP_VEL (-0x600)
#define JUMP_TERMINAL 0xf00
#define SAFE_JUMP_STEP 0x300
#define FALL_DELAY_FRAMES 6

#define PLAYER_WIDTH 13
#define PLAYER_HEIGHT 26
#define PLAYER_CROUCH_HEIGHT 13

#define MELEE_WIDTH 26
#define MELEE_HEIGHT 26
#define PUNCH_TIME 24

#define MAX_RIGHT 0x8000
#define MAX_LEFT 0x7000

#define COL_DOWN 0x80
#define COL_ALL 0x40

MAPPER_USE_VERTICAL_MIRRORING;

extern "C" void __putchar(char c) { POKE(0x4018, c); }

constexpr unsigned char kScreenWidth = 32;
constexpr unsigned char kScreenHeight = 30;
constexpr int kScreenSize = kScreenWidth * kScreenHeight;

constexpr unsigned char background_pal[] = {
    0x30, 0x15, 0x2b, 0x38, // test0
    0x0f, 0x18, 0x26, 0x05, // test1
    0x0f, 0x3a, 0x2c, 0x35, // test2
    0x0f, 0x01, 0x32, 0x30, // test3
};

constexpr unsigned char sprite_pal[] = {
    0x0f, 0x0f, 0x16, 0x36, // player
    0x0f, 0x0f, 0x11, 0x36, // enemyninj
    0x0f, 0x10, 0x20, 0x30, // unused
    0x0f, 0x10, 0x20, 0x30, // unused
};

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

struct Player Ninj = {0x3000, 0x4000, 0, 0, 1, 0, FALL_DELAY_FRAMES, 10, 0, player_stand_right_anim, player_stand_duration, 0, 0, 0};

//level variables
/*unsigned char level_num = 0;

typedef struct level_t {
    const unsigned char * tile_maps;
    const unsigned char * collision_maps;
    const unsigned char map_width;
    const unsigned char map_height;
    const unsigned char bank_tile;
    const unsigned char bank_collision;
} level_t; 

int entity_screen;

const unsigned char total_levels;*/

static unsigned char collision_map[256];
static unsigned char collision_map2[256];

static unsigned char column_map[30];
static unsigned char column_map2[30];
static unsigned char column_map_atr[8];

static unsigned char prev_pad_state;
static unsigned char pad1;
static unsigned char pad1_new;
static unsigned char collision_L;
static unsigned char collision_R;
static unsigned char collision_U;
static unsigned char collision_D;
static unsigned char eject_L;   // from the left
static unsigned char eject_R;   // remember these from the collision sub routine
static unsigned char eject_D;   // from below
static unsigned char eject_U;   // from up

static int scroll_x;
static unsigned pseudo_scroll_x_right;
static unsigned pseudo_scroll_x_left;
static int scroll_y;
static unsigned char scroll_count;
static unsigned char L_R_switch;

unsigned char const * const * old_animation = player_stand_right_anim;

unsigned char game_mode;
enum { MODE_GAME, MODE_PAUSE };

#define MAX_ENEMY 8
unsigned enemy_x[MAX_ENEMY];
unsigned enemy_y[MAX_ENEMY];
unsigned char enemy_active[MAX_ENEMY];
unsigned char enemy_room[MAX_ENEMY];
unsigned enemy_actual_x[MAX_ENEMY];

#define ENEMY_WIDTH 13
#define ENEMY_HEIGHT 26

#define MAX_SHURIK 3

unsigned shurik_x[MAX_SHURIK];
unsigned shurik_y[MAX_SHURIK];
int shurik_vel_x[MAX_SHURIK];
int shurik_vel_y[MAX_SHURIK];
unsigned char shurik_active[MAX_SHURIK];
unsigned char shurik_room[MAX_SHURIK];
unsigned shurik_actual_x[MAX_SHURIK];
unsigned char shurik_throw_index;

#define SHURIK_WIDTH 7
#define SHURIK_HEIGHT 8

void load_room(void);
void draw_sprites(void);
void flicker_sprites(void);
void animate_player(void);
void movement(void);
void bg_collision(unsigned char x, unsigned char y, unsigned char width, unsigned char height);
void draw_screen_L(void);
void draw_screen_R(void);
void new_cmap(void);
char bg_collision_sub(unsigned x, unsigned char y);
void bg_check_low(unsigned char x, unsigned char y, unsigned char width, unsigned char height);

void enemy_moves(void);
void playenemy_collisions(void);
void playenemy_melee_collisions(void);
void check_spr_objects(void);
void sprite_obj_init(void);

void shurik_moves(void);
void shurenemy_collisions(unsigned char shurik_num);

#define ATTRIBUTE_TOP_LEFT 3
#define ATTRIBUTE_TOP_RIGHT 12
#define ATTRIBUTE_BOTTOM_LEFT 48
#define ATTRIBUTE_BOTTOM_RIGHT 192

struct Metatile {
  uint8_t tl;
  uint8_t tr;
  uint8_t bl;
  uint8_t br;
  uint8_t attr;
};

#define SOA_STRUCT Metatile
#define SOA_MEMBERS MEMBER(tl) MEMBER(tr) MEMBER(bl) MEMBER(br) MEMBER(attr)
#include <soa-struct.inc>
static const soa::Array<Metatile, 12> metatiles = {
  { 0, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 0,},
	{ 2, 2, 2, 2, 0,},
	{ 3, 3, 3, 3, 0,},
	{ 1, 1, 1, 1, 85,},
	{ 2, 2, 2, 2, 85,},
	{ 3, 3, 3, 3, 85,},
	{ 1, 1, 1, 1, 170,},
	{ 2, 2, 2, 2, 170,},
	{ 3, 3, 3, 3, 170,},
	{ 1, 1, 1, 1, 255,},
	{ 2, 2, 2, 2, 255,}
};

static uint8_t shuffle_offset;
static uint8_t sprite_slot;

void init_ppu() {
  // Disable the PPU so we can freely modify its state
  ppu_off();

  // Set up bufferd VRAM operations (see `multi_vram_buffer_horz` below)
  set_vram_buffer();

  // Set the Action 53 to use the chosen CHR bank for the upper half of the PPU
  // pattern table. Do this first thing after NMI finishes so that we are
  // still in VBLANK.
  set_chr_bank(0);

  // Use lower half of PPU memory for background tiles
  bank_bg(0);
  // Copy background tiles to CHR-RAM
  vram_adr(0x0000);
  vram_write(testbg, sizeof(testbg) - 1);

  // Set the background palette
  pal_bg(background_pal);

  // Fill the background with null characters to clear the screen
  vram_adr(NAMETABLE_A);
  vram_fill(0x00, kScreenSize);

  // Use the upper half of PPU memory for sprites
  bank_spr(1);
  // Copy sprite tiles to CHR-RAM
  vram_adr(0x1000);
  vram_write(playerspr, 1376);

  // Copy sprite tiles to CHR-RAM
  vram_adr(0x1000+1376);
  vram_write(enemyninjspr, 1280);
  
  // Copy sprite tiles to CHR-RAM
  vram_adr(0x1e00);
  vram_write(shurikspr, 64);
  
  // Set the sprite palette
  pal_spr(sprite_pal);
  
  oam_size(1);

  set_chr_bank(0);
}

template <size_t Mod>
static uint8_t mod_add(uint8_t l, uint8_t r) {
    uint8_t o = l + r;
    if (o >= Mod) {
        o -= Mod;
    }
    return o;
}

int main() {
  init_ppu();
  
  set_vram_buffer(); // do at least once

  load_room();

  ppu_on_all(); // turn on screen

  // Store pad state across frames to check for changes
  prev_pad_state = 0;
  
  unsigned char old_direction = 0;

  while (1) {
    // infinite loop
    while (game_mode == MODE_GAME) {
      ppu_wait_nmi(); // wait till beginning of the frame

      // set scroll
      set_scroll_x(scroll_x);
      set_scroll_y(scroll_y);
      
      animate_player();
      draw_sprites();
			flicker_sprites();
      //printf("frame change done!\n");
      
      pad1 = pad_poll(0); // read the first controller
      pad1_new = get_pad_new(0);

      movement();
      check_spr_objects(); // see which objects are on screen
      playenemy_collisions();
			enemy_moves();
			
			if (Ninj.is_punching) {
				playenemy_melee_collisions();
				if (Ninj.punch_counter > 0) {
					Ninj.punch_counter--;
				} else {
					Ninj.is_punching = 0;
				}
			}
      
      for (char i = 0; i < MAX_SHURIK ; ++i){
        shurenemy_collisions(i);
      }
      
      shurik_moves();
      
      if (old_direction != Ninj.direction) scroll_count = 0;
      
      if (pad1 & PAD_START && !(prev_pad_state & PAD_START)) {
        game_mode = MODE_PAUSE;
        color_emphasis(COL_EMP_DARK);
      }
      
      prev_pad_state = pad1;
      old_direction = Ninj.direction;
    }
    while (game_mode == MODE_PAUSE) {
      ppu_wait_nmi();

      pad1 = pad_poll(0); // read the first controller
      pad1_new = get_pad_new(0);

      draw_sprites();

      if (pad1 & PAD_START && !(prev_pad_state & PAD_START)) {
        game_mode = MODE_GAME;
        color_emphasis(COL_EMP_NORMAL);
      }
      prev_pad_state = pad1;
    }
  }
}

void load_room(void) {
  set_data_pointer(rooms[0]);
  
  for (char x = 0;; x += 0x10) {
    for (char i = 0; i < 30; i+=2) {
      auto tile = *(rooms[0] + (x & 0xf0) + (i >> 1));
			column_map[i] = metatiles[tile].tl;
			column_map[i+1] = metatiles[tile].tr;
			column_map2[i] = metatiles[tile].bl;
			column_map2[i+1] = metatiles[tile].br;
    }
    
    multi_vram_buffer_vert(column_map, 30, get_ppu_addr(0, x, 0));
    multi_vram_buffer_vert(column_map2, 30, get_ppu_addr(0, x+8, 0));
    
    flush_vram_update2();
    if (x == 0xf0)
      break;
  }
  for (char x = 0;; x += 0x10) {
    for (char i = 0; i < 8; i++) {
			auto tile = rooms[0] + (x & 0xf0) + (i << 1);
			if ((x & 0x10) == 0){
				column_map_atr[i] = 0;
				column_map_atr[i] += (metatiles[*(tile)].attr & ATTRIBUTE_TOP_LEFT);
				column_map_atr[i] += (metatiles[*(tile+1)].attr & ATTRIBUTE_BOTTOM_LEFT);
				//printf("left attribute\n");
			} else {
				column_map_atr[i] += (metatiles[*(tile)].attr & ATTRIBUTE_TOP_RIGHT);
				column_map_atr[i] += (metatiles[*(tile+1)].attr & ATTRIBUTE_BOTTOM_RIGHT);
				//printf("right attribute\n");
			}
    }
		if ((x & 0x10) == 0x10) {
			auto addr = get_at_addr(0,x,0);
			for (auto i = 0; i < 8; ++i) {
				one_vram_buffer(column_map_atr[i], addr);
				addr += 8;
			}
		}
		if (x == 0xf0)
      break;
  }
  

  // a little bit in the next room
  set_data_pointer(rooms[0]+256);
  
  for (char i = 0; i < 30; i+=2) {
		auto tile = *(rooms[0] + 256 + (i >> 1));
		column_map[i] = metatiles[tile].tl;
		column_map[i+1] = metatiles[tile].tr;
		column_map2[i] = metatiles[tile].bl;
		column_map2[i+1] = metatiles[tile].br;
  }
  
  multi_vram_buffer_vert(column_map, 30, get_ppu_addr(1, 0, 0));
  multi_vram_buffer_vert(column_map2, 30, get_ppu_addr(1, 8, 0));

  // copy the room to the collision map
  // the second one should auto-load with the scrolling code
  memcpy(collision_map, rooms[0], 256);
  
  sprite_obj_init();
}

void draw_sprites(void) {
  // clear all sprites from sprite buffer
  oam_clear();

  
  
  for (char i = 0; i < MAX_ENEMY; ++i) {
    if (high_byte(enemy_y[i]) == TURN_OFF)
      continue;
    if (high_byte(enemy_x[i]) > 0xf0)
      continue;
    if (enemy_active[i] && (high_byte(enemy_y[i]) < 0xf0)) {
      oam_meta_spr(high_byte(enemy_x[i])+15, high_byte(enemy_y[i])+19, eninj_standL_data);
    }
  }
  
  for (char i = 0; i < MAX_SHURIK; ++i) {
    if (high_byte(shurik_y[i]) == TURN_OFF)
      continue;
    if (high_byte(shurik_x[i]) > 0xf0)
      continue;
    if (shurik_active[i] && (high_byte(shurik_y[i]) < 0xf0)) {
      oam_meta_spr(high_byte(shurik_x[i])+12, high_byte(shurik_y[i])+19, shurik0_data);
    }
  }
  
}

//Thanks Jroweboy
void flicker_sprites(void) {
        
				sprite_slot = 0;
        
    
        // Draw the player first to reserve their slot... or not
        // Everything else can fight with flickering... or not
        

        // OAM shuffle the rest of the sprites
        shuffle_offset = mod_add<12>(shuffle_offset, 11);
        uint8_t original_offset = shuffle_offset;
        uint8_t count = 12;
        for (uint8_t i = original_offset; count > 0; --count, i = mod_add<12>(i, 7)) {
					if (i == 0) {
						// draw 1 metasprite
  
						oam_meta_spr(high_byte(Ninj.x)+15, high_byte(Ninj.y) + 19, Ninj.animation[Ninj.frame]);
						continue;
          }
            
        }
				
    }

void animate_player(void) {
  if (Ninj.animation != old_animation) {
    Ninj.frame = 0;
  }
  if (Ninj.frame_counter > 0) {
    Ninj.frame_counter--;
  } else {
    if (Ninj.animation_duration[Ninj.frame+1] > 0) {
      Ninj.frame++;
    } else {
      Ninj.frame = 0;
    }
    Ninj.frame_counter = Ninj.animation_duration[Ninj.frame];
    //printf("frame change go!\n");
  }
  old_animation = Ninj.animation;
}

void movement(void) {

  // handle x

  unsigned char old_x = Ninj.x;

  if (!(pad1 & PAD_DOWN) || Ninj.onground == 0){
    Ninj.is_crouching = 0;
    if (pad1 & PAD_LEFT) {
			if (Ninj.animation_duration == player_stand_duration || Ninj.animation_duration == player_crouch_duration ||
          (Ninj.animation_duration == player_throw_duration && Ninj.frame == 1 && Ninj.frame_counter == 0) || Ninj.animation == player_run_right_anim){
				Ninj.animation = player_run_left_anim;
				Ninj.animation_duration = player_run_duration;
			}
			
      Ninj.direction = LEFT;
      Ninj.vel_x -= ACCEL;
      if (Ninj.vel_x < -MAX_SPEED)
        Ninj.vel_x = -MAX_SPEED;
    } else if (pad1 & PAD_RIGHT) {
			if (Ninj.animation_duration == player_stand_duration || Ninj.animation_duration == player_crouch_duration ||
          (Ninj.animation_duration == player_throw_duration && Ninj.frame == 1 && Ninj.frame_counter == 0) || Ninj.animation == player_run_left_anim){
				Ninj.animation = player_run_right_anim;
				Ninj.animation_duration = player_run_duration;
			}
      
      Ninj.direction = RIGHT;
      Ninj.vel_x += ACCEL;
      if (Ninj.vel_x > MAX_SPEED)
        Ninj.vel_x = MAX_SPEED;
    } else { // nothing pressed
      if (Ninj.animation_duration == player_run_duration || Ninj.animation_duration == player_crouch_duration ||
          (Ninj.animation_duration == player_throw_duration && Ninj.frame == 1 && Ninj.frame_counter == 0)){
        if (Ninj.direction == LEFT) {
          Ninj.animation = player_stand_left_anim;
        } else if (Ninj.direction == RIGHT) {
          Ninj.animation = player_stand_right_anim;
        }
        Ninj.animation_duration = player_stand_duration;
      }
      if (Ninj.vel_x >= 0x100)
        Ninj.vel_x -= ACCEL;
      else if (Ninj.vel_x < -0x100)
        Ninj.vel_x += ACCEL;
      else
        Ninj.vel_x = 0;
    } 
  } else {
      Ninj.is_crouching = 1;
			if (Ninj.animation_duration == player_run_duration || Ninj.animation_duration == player_stand_duration ||
			    (Ninj.animation_duration == player_throw_duration && Ninj.frame == 1 && Ninj.frame_counter == 0)) {
				if (Ninj.direction == LEFT) {
					Ninj.animation = player_crouch_left_anim;
				} else if (Ninj.direction == RIGHT) {
					Ninj.animation = player_crouch_right_anim;
				}
				Ninj.animation_duration = player_crouch_duration;
      }
      if (Ninj.vel_x >= 0x100)
        Ninj.vel_x -= ACCEL;
      else if (Ninj.vel_x < -0x100)
        Ninj.vel_x += ACCEL;
      else
        Ninj.vel_x = 0;
  }

  Ninj.x += Ninj.vel_x;

  if (Ninj.x > 0xf800) { // make sure no wrap around to the other side
    Ninj.x = 0x100;
    Ninj.vel_x = 0;
  }

  L_R_switch = 1; // shinks the y values in bg_coll, less problems with head /
                  // feet collisions

  if (!Ninj.is_crouching){
    bg_collision(high_byte(Ninj.x), high_byte(Ninj.y), PLAYER_WIDTH,
                 PLAYER_HEIGHT);
  } else {
    bg_collision(high_byte(Ninj.x), high_byte(Ninj.y)+13, PLAYER_WIDTH,
                 PLAYER_CROUCH_HEIGHT);
  }
  if (collision_R &&
      collision_L) { // if both true, probably half stuck in a wall
    Ninj.x = old_x;
    Ninj.vel_x = 0;
  } else if (collision_L) {
    Ninj.vel_x = 0;
    high_byte(Ninj.x) = high_byte(Ninj.x) - eject_L;

  } else if (collision_R) {
    Ninj.vel_x = 0;
    high_byte(Ninj.x) = high_byte(Ninj.x) - eject_R;
  }

  // handle y

  // gravity

  // Ninj.vel_y is signed
  if (Ninj.vel_y < JUMP_TERMINAL) {
    Ninj.vel_y += GRAVITY;
  } else {
    Ninj.vel_y = JUMP_TERMINAL; // consistent
  }
  
  int y_distance = Ninj.vel_y;
  
  if (y_distance <= SAFE_JUMP_STEP) {
    Ninj.y += Ninj.vel_y;
  }
  
  while (y_distance > SAFE_JUMP_STEP) {
    //printf("falling fast function\n");
    Ninj.y += SAFE_JUMP_STEP;
    
    L_R_switch = 0;
    if (!Ninj.is_crouching){
      bg_collision(high_byte(Ninj.x), high_byte(Ninj.y), PLAYER_WIDTH,
                   PLAYER_HEIGHT);
    } else {
      bg_collision(high_byte(Ninj.x), high_byte(Ninj.y)+13, PLAYER_WIDTH,
                   PLAYER_CROUCH_HEIGHT);
    }

    if (collision_U) {
      high_byte(Ninj.y) -= eject_U;
      Ninj.vel_y = 0;
    } else if (collision_D) {
      high_byte(Ninj.y) -= eject_D;
      Ninj.y &= 0xff00;
      if (Ninj.vel_y > 0) {
        Ninj.vel_y = 0;
      }
    }
    y_distance -= SAFE_JUMP_STEP;
  }
  //printf("falling fast function over\n");
  
  L_R_switch = 0;
  if (!Ninj.is_crouching){
    bg_collision(high_byte(Ninj.x), high_byte(Ninj.y), PLAYER_WIDTH,
                 PLAYER_HEIGHT);
  } else {
    bg_collision(high_byte(Ninj.x), high_byte(Ninj.y)+13, PLAYER_WIDTH,
                 PLAYER_CROUCH_HEIGHT);
  }

  if (collision_U) {
    high_byte(Ninj.y) -= eject_U;
    Ninj.vel_y = 0;
  } else if (collision_D) {
    high_byte(Ninj.y) -= eject_D;
    Ninj.y &= 0xff00;
    if (Ninj.vel_y > 0) {
      Ninj.vel_y = 0;
    }
  }
  
  // check collision down a little lower than player
  if (!Ninj.is_crouching){
    bg_check_low(high_byte(Ninj.x), high_byte(Ninj.y), PLAYER_WIDTH,
               PLAYER_HEIGHT);
  } else {
    bg_check_low(high_byte(Ninj.x), high_byte(Ninj.y)+13, PLAYER_WIDTH,
               PLAYER_CROUCH_HEIGHT);
  }

  if (collision_D) {
    Ninj.onground = 1;
    Ninj.fall_delay_counter = FALL_DELAY_FRAMES;
  } else {
    Ninj.onground = 0;
    if (Ninj.fall_delay_counter > 0) {
      Ninj.fall_delay_counter--;
    }
  }
  
  if (pad1 & PAD_A && Ninj.fall_delay_counter > 0 && !(prev_pad_state & PAD_A)) {
      Ninj.vel_y = JUMP_VEL; // JUMP
      Ninj.onground = 0;
  }
  
  if (pad1 & PAD_SELECT && !(prev_pad_state & PAD_SELECT)) {
    shurik_active[shurik_throw_index] = 1;
    shurik_actual_x[shurik_throw_index] = Ninj.x + (scroll_x << 8);
    shurik_y[shurik_throw_index] = Ninj.y;
    if (Ninj.direction == RIGHT) {
      Ninj.animation = player_throw_right_anim;
      shurik_vel_x[shurik_throw_index] = 0x200;
    } else if (Ninj.direction == LEFT) {
      Ninj.animation = player_throw_left_anim;
      shurik_vel_x[shurik_throw_index] = -0x200;
    }
    shurik_vel_y[shurik_throw_index] = 0;
    if (pad1 & PAD_UP) {
      if (!(pad1 & PAD_LEFT) && !(pad1 & PAD_RIGHT)) {
        shurik_vel_x[shurik_throw_index] = 0;
      }
      shurik_vel_y[shurik_throw_index] = -0x200;
    }
    
    Ninj.animation_duration = player_throw_duration;
    if (shurik_throw_index < (MAX_SHURIK - 1)) {
      shurik_throw_index++;
    } else {
      shurik_throw_index = 0;
    }
  }
	
	if (pad1 & PAD_B && !(prev_pad_state & PAD_B)) {
		if (Ninj.direction == RIGHT) {
      Ninj.animation = player_throw_right_anim;
    } else if (Ninj.direction == LEFT) {
      Ninj.animation = player_throw_left_anim;
    }
		Ninj.animation_duration = player_throw_duration;
		Ninj.is_punching = 1;
		Ninj.punch_counter = PUNCH_TIME;
	}

  // scroll
  unsigned new_x = Ninj.x;
  unsigned char scroll_amt;
  if (Ninj.x > MAX_RIGHT) {
    scroll_amt = (Ninj.x - MAX_RIGHT) >> 8;
    scroll_x += scroll_amt;
    high_byte(Ninj.x) -= scroll_amt;
    draw_screen_R();
    // do we need to load a new collision map? (scrolled into a new room)
    if ((scroll_x & 0x0f) < 4) {
      new_cmap();
    }
  } else if (Ninj.x < MAX_LEFT) {
    scroll_amt = (MAX_LEFT - Ninj.x) >> 8;
    scroll_x -= scroll_amt;
    high_byte(Ninj.x) += scroll_amt;
    draw_screen_L();
    // do we need to load a new collision map? (scrolled into a new room)
    if ((scroll_x & 0x0f) < 4) {
      new_cmap();
    }
  }

  if (scroll_x >= MAX_SCROLL) {
    scroll_x = MAX_SCROLL; // stop scrolling right, end of level
    Ninj.x = new_x;     // but allow the x position to go all the way right
    if (high_byte(Ninj.x) >= 0xf1) {
      Ninj.x = 0xf100;
    }
  } else if (scroll_x <= 0) {
    scroll_x = 0; //stop scrolling left, start of level
    Ninj.x = new_x; //allow the x position to go all the way left
    if (Ninj.x <= 0x100) {
      Ninj.x = 0x100;
    }
  }
}

void shurik_moves(void) {
  for (char i = 0; i < MAX_SHURIK; ++i) {
    if (shurik_active[i] == 1) {
      shurik_actual_x[i] += shurik_vel_x[i];
      shurik_y[i] += shurik_vel_y[i];
      
      bg_collision(high_byte(shurik_x[i]), high_byte(shurik_y[i]), SHURIK_WIDTH, SHURIK_HEIGHT);
      
      if (collision_L || collision_R || collision_D || collision_U || 
          high_byte(shurik_x[i]) > 248 ||
          high_byte(shurik_y[i]) > 232) {
        shurik_y[i] = TURN_OFF << 8;
        shurik_active[i] = 0;
      }
      
    }
  }
}

void enemy_moves(void) {
  if (get_frame_count() & 0x01)
    return; // half speed

  for (char i = 0; i < MAX_ENEMY; ++i) {
    if (enemy_active[i]) {
      if (enemy_x[i] > Ninj.x) {
        if (enemy_actual_x[i] == 0)
          --enemy_room[i];
        enemy_actual_x[i] -= 256;
      } else if (enemy_x[i] < Ninj.x) {
        enemy_actual_x[i] += 256;
        if (enemy_actual_x[i] == 0)
          ++enemy_room[i];
      }
    }
  }
	//printf("enemy movin\n");
}

void bg_collision(unsigned char x, unsigned char y, unsigned char width, unsigned char height) {
  // note, uses bits in the metatile data to determine collision
  // sprite collision with backgrounds

  collision_L = 0;
  collision_R = 0;
  collision_U = 0;
  collision_D = 0;

  if (y >= 0xf0)
    return;

  unsigned x_upper_left = x + scroll_x; // upper left (temp6 = save for reuse)

  eject_L = (x_upper_left & 0xff) | 0xf0;

  unsigned char y_top = y;

  eject_U = y_top | 0xf0;

  if (L_R_switch)
    y_top += 2; // fix bug, walking through walls

  if (bg_collision_sub(x_upper_left,
                       y_top) & COL_ALL) { // find a corner in the collision map
    ++collision_L;
    ++collision_U;
  }

  unsigned x_upper_right = x_upper_left + width;

  eject_R = (x_upper_right + 1) & 0x0f;

  // find a corner in the collision map
  if (bg_collision_sub(x_upper_right, y_top) & COL_ALL) {
    ++collision_R;
    ++collision_U;
  }

  // again, lower

  // bottom right, x hasn't changed

  unsigned char y_bot = y + height; // y bottom
  if (L_R_switch)
    y_bot -= 2; // fix bug, walking through walls
  eject_D = (y_bot + 1) & 0x0f;
  if (y_bot >= 0xf0)
    return;

  unsigned char collision = bg_collision_sub(x_upper_right, y_bot);

  if (collision & COL_ALL) { // find a corner in the collision map
    ++collision_R;
  }
  if (collision & (COL_DOWN | COL_ALL)) { // find a corner in the collision map
    ++collision_D;
  }

  // bottom left
  collision = bg_collision_sub(x_upper_left, y_bot);

  if (collision & COL_ALL) { // find a corner in the collision map
    ++collision_L;
  }
  if (collision & (COL_DOWN | COL_ALL)) { // find a corner in the collision map
    ++collision_D;
  }

  if ((y_bot & 0x0f) > 3)
    collision_D = 0; // for platforms, only collide with the top 3 pixels
}

char bg_collision_sub(unsigned x, unsigned char y) {
  unsigned char upper_left = ((y & 0xff) >> 4) + (x & 0xf0);
  unsigned char typ = (x & 1 << 8 ? collision_map2 : collision_map)[upper_left];
  static const unsigned char is_solid[]={
  0,
  COL_ALL+COL_DOWN,
  COL_DOWN,
  0,
  COL_DOWN,
  COL_DOWN,
  0,
  0,
  0,
  0,
  0,
  0
  };
  return is_solid[typ];
}

void draw_screen_R(void) {
  // scrolling to the right, draw metatiles as we go
  pseudo_scroll_x_right = scroll_x + 0x110;

  unsigned char room = pseudo_scroll_x_right >> 8;
  unsigned char roomp = high_byte(pseudo_scroll_x_right);

  set_data_pointer(rooms[0]+roomp);
  unsigned char nt = room & 1;
  unsigned char x = pseudo_scroll_x_right & 0xf0;

  // important that the main loop clears the vram_buffer
  
  for (char i = 0; i < 30; i+=2) {
		auto tile = *(rooms[0] + (pseudo_scroll_x_right & 0xfff0) + (i >> 1));
		column_map[i] = metatiles[tile].tl;
		column_map[i+1] = metatiles[tile].tr;
		column_map2[i] = metatiles[tile].bl;
		column_map2[i+1] = metatiles[tile].br;
  }

  multi_vram_buffer_vert(column_map, 30, get_ppu_addr(nt, x, 0));
  multi_vram_buffer_vert(column_map2, 30, get_ppu_addr(nt, x+8, 0));
	
	for (char i = 0; i < 8; i++) {
		auto tile = rooms[0] + (pseudo_scroll_x_right & 0xffe0) + (i<<1);
		column_map_atr[i] = 0;
		column_map_atr[i] += (metatiles[*(tile)].attr & ATTRIBUTE_TOP_LEFT);
		column_map_atr[i] += (metatiles[*(tile + 1)].attr & ATTRIBUTE_BOTTOM_LEFT);
		//printf("left attribute\n");
		column_map_atr[i] += (metatiles[*(tile + 16)].attr & ATTRIBUTE_TOP_RIGHT);
		column_map_atr[i] += (metatiles[*(tile + 17)].attr & ATTRIBUTE_BOTTOM_RIGHT);
		//printf("right attribute\n");
		
	}
	
	auto addr = get_at_addr(nt,x,0);
	for (auto i = 0; i < 8; ++i) {
		one_vram_buffer(column_map_atr[i], addr);
		addr += 8;
	}	
}

void draw_screen_L(void) {
  // scrolling to the left, draw metatiles as we go
  pseudo_scroll_x_left = scroll_x - 0x10;

  unsigned char room = pseudo_scroll_x_left >> 8;

  set_data_pointer(rooms[0]+(room << 8));
  unsigned char nt = room & 1;
  unsigned char x = pseudo_scroll_x_left & 0xf0;

  // important that the main loop clears the vram_buffer

  for (char i = 0; i < 30; i+=2) {
    auto tile = *(rooms[0] + (pseudo_scroll_x_left & 0xfff0) + (i >> 1));
		column_map[i] = metatiles[tile].tl;
		column_map[i+1] = metatiles[tile].tr;
		column_map2[i] = metatiles[tile].bl;
		column_map2[i+1] = metatiles[tile].br;
  }
  
  multi_vram_buffer_vert(column_map, 30, get_ppu_addr(nt, x, 0));
  multi_vram_buffer_vert(column_map2, 30, get_ppu_addr(nt, x+8, 0));
	
	for (char i = 0; i < 8; i++) {
		auto tile = rooms[0] + (pseudo_scroll_x_left & 0xffe0) + (i<<1);
		column_map_atr[i] = 0;
		column_map_atr[i] += (metatiles[*(tile)].attr & ATTRIBUTE_TOP_LEFT);
		column_map_atr[i] += (metatiles[*(tile + 1)].attr & ATTRIBUTE_BOTTOM_LEFT);
		//printf("left attribute\n");
		column_map_atr[i] += (metatiles[*(tile + 16)].attr & ATTRIBUTE_TOP_RIGHT);
		column_map_atr[i] += (metatiles[*(tile + 17)].attr & ATTRIBUTE_BOTTOM_RIGHT);
		//printf("right attribute\n");
		
	}
	
	auto addr = get_at_addr(nt,x,0);
	for (auto i = 0; i < 8; ++i) {
		one_vram_buffer(column_map_atr[i], addr);
		addr += 8;
	}	
}

void new_cmap(void) {
  // copy a new collision map to one of the 4 collision_map arrays
  unsigned char room_right = (high_byte(scroll_x) + 1); // high byte = room, one to the right
  
  unsigned char *map;
  if ((room_right & 1) == 0) {
    map = collision_map;
  } else {
    map = collision_map2;
  }
  
  if (Ninj.direction == 1) {
    memcpy(map + (scroll_x & 0xf0), rooms[0]+((scroll_x & 0xfff0) + 256), 16);
  } else {
    memcpy(map + (scroll_x & 0xf0), rooms[0]+((scroll_x & 0xfff0) - 256), 16);
  }
}

void bg_check_low(unsigned char x, unsigned char y, unsigned char width, unsigned char height) {
  // floor collisions
  collision_D = 0;

  unsigned x_left = x + scroll_x;
  unsigned char y_bot = y + height + 1;

  if (y_bot >= 0xf0)
    return;

  // find a corner in the collision map
  if (bg_collision_sub(x_left, y_bot) & (COL_DOWN | COL_ALL)) {
    ++collision_D;
  }

  unsigned x_right = x_left + width;
  // find a corner in the collision map
  if (bg_collision_sub(x_right, y_bot) & (COL_DOWN | COL_ALL)) {
    ++collision_D;
  }

  /*if ((y_bot & 0x0f) > 9)
    collision_D = 0; // for platforms, only collide with the top 9 pixels*/
}

void playenemy_collisions(void) {
  struct Box {
    unsigned char x, y, width, height;
  };

  struct Box ninj_box = {high_byte(Ninj.x), high_byte(Ninj.y),
                            PLAYER_WIDTH, PLAYER_HEIGHT};
  struct Box other_box;

  other_box.width = ENEMY_WIDTH;
  other_box.height = ENEMY_HEIGHT;
  for (unsigned char i = 0; i < MAX_ENEMY; ++i) {
    if (!enemy_active[i])
      continue;

    other_box.x = high_byte(enemy_x[i]);
    other_box.y = high_byte(enemy_y[i]);
    if (!check_collision(&ninj_box, &other_box))
      continue;
  }
}

void playenemy_melee_collisions(void) {
  struct Box {
    unsigned char x, y, width, height;
  };

	int8_t melee_x;
	if (Ninj.direction == 1) {
		melee_x = 10;
	} else {
		melee_x = -24;
	}

  struct Box melee_box = {static_cast<unsigned char>(high_byte(Ninj.x) + melee_x), high_byte(Ninj.y),
                            MELEE_WIDTH, MELEE_HEIGHT};
  struct Box other_box;

  other_box.width = ENEMY_WIDTH;
  other_box.height = ENEMY_HEIGHT;
  for (unsigned char i = 0; i < MAX_ENEMY; ++i) {
    if (!enemy_active[i])
      continue;

    other_box.x = high_byte(enemy_x[i]);
    other_box.y = high_byte(enemy_y[i]);
    if (!check_collision(&melee_box, &other_box))
      continue;
		
		enemy_y[i] = TURN_OFF << 8;
  }
}

void shurenemy_collisions(unsigned char shurik_num) {
  struct Box {
    unsigned char x, y, width, height;
  };

  struct Box shur_box = {high_byte(shurik_x[shurik_num]), high_byte(shurik_y[shurik_num]),
                            SHURIK_WIDTH, SHURIK_HEIGHT};
  struct Box other_box;

  other_box.width = ENEMY_WIDTH;
  other_box.height = ENEMY_HEIGHT;
  for (unsigned char i = 0; i < MAX_ENEMY; ++i) {
    if (!enemy_active[i])
      continue;

    other_box.x = high_byte(enemy_x[i]);
    other_box.y = high_byte(enemy_y[i]);
    if (!check_collision(&shur_box, &other_box))
      continue;

    enemy_y[i] = TURN_OFF << 8;
    shurik_y[shurik_num] = TURN_OFF << 8;
    shurik_active[shurik_num] = 0;
  }
}

void check_spr_objects(void) {
  // mark each object "active" if they are, and get the screen x

  for (unsigned char i = 0; i < MAX_ENEMY; ++i) {
    enemy_active[i] = 0; // default to zero
    if (high_byte(enemy_y[i]) != TURN_OFF) {
      unsigned x = (enemy_room[i] << 8) + high_byte(enemy_actual_x[i]) - scroll_x;
      enemy_active[i] = !high_byte(x);
      enemy_x[i] = (x & 0xff) << 8; // screen x coords
    }
  }
  
  for (unsigned char i = 0; i < MAX_SHURIK; ++i) {
    if (high_byte(shurik_y[i]) != TURN_OFF) {
      unsigned x = (shurik_room[i] << 8) + high_byte(shurik_actual_x[i]) - scroll_x;
      shurik_x[i] = (x & 0xff) << 8; // screen x coords
    }
  }
}

void sprite_obj_init(void) {
  unsigned char i, j;

  for (i = 0, j = 0; i < MAX_ENEMY; ++i) {
    enemy_x[i] = 0;
    enemy_y[i] = level_1_enemies[j++] << 8;
    if (high_byte(enemy_y[i]) == TURN_OFF)
      break;
    enemy_active[i] = 0;
    enemy_room[i] = level_1_enemies[j++];
    enemy_actual_x[i] = (level_1_enemies[j++]) << 8;
  }
  for (++i; i < MAX_ENEMY; ++i)
    enemy_y[i] = TURN_OFF << 8;
  
  for (i = 0, j = 0; i < MAX_SHURIK; ++i) {
    shurik_x[i] = 0;
    shurik_y[i] = TURN_OFF;
    shurik_vel_x[i] = 0;
    shurik_vel_y[i] = 0;
    shurik_active[i] = 0;
    shurik_room[i] = 0;
    shurik_actual_x[i] = 0;
  }
  for (++i; i < MAX_SHURIK; ++i)
    shurik_y[i] = TURN_OFF << 8;
  
}
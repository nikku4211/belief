#include <mapper.h>
#include <nesdoug.h>
#include <neslib.h>
#include <string.h>
#include <soa.h>
#include <stdio.h>

#include "../chr/forestbg.h"
#include "../chr/playerspr.h"
#include "../chr/enemyninjspr.h"
#include "../chr/shurikspr.h"

#include "../level/rooms.h"
#include "sprites.h"
#include "anim.h"
#include "bankprgs.h"
#include "map.hpp"
#include "global.hpp"
#include "collision.hpp"
#include "movement.hpp"

MAPPER_USE_VERTICAL_MIRRORING;

constexpr unsigned char kScreenWidth = 32;
constexpr unsigned char kScreenHeight = 30;
constexpr int kScreenSize = kScreenWidth * kScreenHeight;

constexpr unsigned char hud_pal[] = {
	  0x09, 0x0f, 0x16, 0x36, // hud
};

constexpr unsigned char forest_background_pal[] = {
    0x09, 0x16, 0x27, 0x29, // ground and tree bark
    0x09, 0x0c, 0x1c, 0x21, // far away trees
    0x09, 0x16, 0x29, 0x19, // tree foliage
};

constexpr unsigned char cave_background_pal[] = {
    0x01, 0x10, 0x22, 0x31, // ground
    0x01, 0x0f, 0x25, 0x20, // boss
    0x01, 0x16, 0x29, 0x19, // tree foliage
};

constexpr unsigned char sprite_pal[] = {
    0x0f, 0x0f, 0x16, 0x36, // player
    0x0f, 0x0f, 0x11, 0x36, // enemyninj
    0x0f, 0x10, 0x20, 0x30, // unused
    0x0f, 0x10, 0x20, 0x30, // unused
};

#define FALL_DELAY_FRAMES 6

struct Player Ninj = {0x3000, 0x4000, 0, 0, 1, 0, FALL_DELAY_FRAMES, 10, 0, player_stand_right_anim, player_stand_duration, 0, 0, 0};

unsigned char prev_pad_state;
unsigned char pad1;
unsigned char pad1_new;

int scroll_x;
int scroll_y;
unsigned char scroll_count;
unsigned char L_R_switch;

unsigned char level_metatile_index;

unsigned char const * const * old_animation = player_stand_right_anim;

unsigned char game_mode;

unsigned char level;
unsigned char level_up;
unsigned char death;
unsigned char map_loaded;   // only load it once
unsigned char enemy_timer; // in case of skipped frames

#define MAX_ENEMY 8
unsigned enemy_x[MAX_ENEMY];
unsigned enemy_y[MAX_ENEMY];
int enemy_vel_x[MAX_ENEMY];
int enemy_vel_y[MAX_ENEMY];
unsigned char enemy_active[MAX_ENEMY];
unsigned char enemy_room[MAX_ENEMY];
unsigned enemy_actual_x[MAX_ENEMY];
unsigned char enemy_type[MAX_ENEMY];
unsigned char enemy_frame_counter[MAX_ENEMY];
unsigned char enemy_frame[MAX_ENEMY];
unsigned char const * const * enemy_anim[MAX_ENEMY];
unsigned char const * enemy_anim_duration[MAX_ENEMY];
unsigned char const * const * old_enemy_animation[MAX_ENEMY];

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

uint8_t parallax_buf[6 * 16];

//void draw_sprites(void);
void flicker_sprites(void);
void animate_player(void);
void animate_enemies(void);
void init_level(void);

// 0 = attr 0
// 85 = attr 1
// 170 = attr 2
// 255 = attr 3

#define COL_DOWN 0x80
#define COL_ALL 0x40

constexpr soa::Array<Metatile, 22> global_metatiles = {
	//forest
  { 0, 0, 0, 0, 85, 0},
  { 1, 2, 22, 23, 85, COL_ALL+COL_DOWN},
  { 23, 22, 22, 23, 85, COL_ALL+COL_DOWN},
  { 0, 3, 0, 24, 85,0},
  { 4, 43, 25, 59, 85,0},
  { 5, 44, 26, 60, 85,0},
  { 0, 0, 27, 0, 85,0},
  { 39, 40, 55, 56, 85,0},
  { 0, 50, 41, 42, 85,0},
  { 1, 2, 33, 34, 85,COL_DOWN},
  { 34, 33, 33, 34, 85,0},
  { 35, 36, 51, 52, 85,0},
	{ 37, 38, 53, 54, 85,0},
	{ 4, 5, 25, 26, 85,0},
	{ 9, 10, 11, 12, 170,0},
	{ 15, 16, 19, 20, 170,0},
	{ 0, 50, 49, 49, 85,0},
	{ 0, 0, 49, 0, 85,0},
	{ 32, 7, 48, 28, 255,0},
	{ 8, 32, 29, 7, 255,0},
	{ 16, 0, 50, 0, 255,0},
	{ 13, 14, 13, 14, 170,0},
};

const unsigned char *level_palette;

int16_t old_parallax_scroll;

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
  vram_write(forestbg, sizeof(forestbg) - 1);

	// Set the hud palette
	
	pal_col(0, hud_pal[0]);
	pal_col(1, hud_pal[1]);
	pal_col(2, hud_pal[2]);
	pal_col(3, hud_pal[3]);

  // Set the background palette
	for (unsigned char i = 0; i < 12; i++) {
		pal_col(i+4, level_palette[i]);
	}

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

void init_level(void){
	level_metatile_index = 0; //the metatiles are one global array, but each level has an index to it
	level_palette = forest_background_pal; //set the palette to forest
}

int main() {
	init_level();
	
	init_ppu();

  load_room();

  ppu_on_all(); // turn on screen

  // Store pad state across frames to check for changes
  prev_pad_state = 0;
	
	game_mode = MODE_GAME;
  
  unsigned char old_direction = 0;
	old_parallax_scroll = 0;
	unsigned char bright = 0;
  unsigned char bright_count = 0;
	
	for (auto i = 0; i < MAX_ENEMY; ++i){
		enemy_frame[i] = 0;
		enemy_anim[i] = eninj_stand_left_anim;
		enemy_anim_duration[i] = player_stand_duration;
	}
	
  while (1) {
    // infinite loop
    while (game_mode == MODE_GAME) {
      ppu_wait_nmi(); // wait till beginning of the frame
		  NAME_UPD_ENABLE = 0;

      // set scroll
      set_scroll_x(scroll_x);
      set_scroll_y(scroll_y);
      
      animate_player();
			animate_enemies();
      flicker_sprites();
      //puts("frame change done!\n");
      
      pad1 = pad_poll(0); // read the first controller
      pad1_new = get_pad_new(0);

      movement();
      check_spr_objects(); // see which objects are on screen
      playenemy_collisions();
      
      if (Ninj.is_punching) {
        playenemy_melee_collisions();
        if (Ninj.punch_counter > 0) {
          Ninj.punch_counter--;
        } else {
          Ninj.is_punching = 0;
        }
      }
      
      for (uint8_t i = 0; i < MAX_SHURIK ; ++i){
        shurenemy_collisions(i);
      }
      
      shurik_moves();
      
      if (old_direction != Ninj.direction) scroll_count = 0;
      
      if (pad1 & PAD_START && !(prev_pad_state & PAD_START)) {
        game_mode = MODE_PAUSE;
        color_emphasis(COL_EMP_DARK);
      }
			
			if (level_up) {
        game_mode = MODE_SWITCH;
        level_up = 0;
        bright = 4;
        bright_count = 0;
        ++level;
      } else if (death) {
        game_mode = MODE_SWITCH;
        bright = 4;
        bright_count = 0;
      }
      
      prev_pad_state = pad1;
      old_direction = Ninj.direction;
    }
		// switch rooms, due to level++
    // also, death, restart level (removed feature)
    while (game_mode == MODE_SWITCH) {
      ppu_wait_nmi();
      if (++bright_count >= 0x10) { // fade out
        bright_count = 0;
        if (--bright != 0xff)
          pal_bright(bright); // fade out
      }
      set_scroll_x(scroll_x);

      if (bright == 0xff) { // now switch rooms
        ppu_off();
        oam_clear();
        scroll_x = 0;
        set_scroll_x(scroll_x);
        
					// Set the background palette
					for (unsigned char i = 0; i < 12; i++) {
						pal_col(i+4, level_palette[i]);
					}
          load_room();
          game_mode = MODE_GAME;
          ppu_on_all();
          pal_bright(4); // back to normal brighness
      }
    }
    while (game_mode == MODE_PAUSE) {
      ppu_wait_nmi();

      pad1 = pad_poll(0); // read the first controller
      pad1_new = get_pad_new(0);

      flicker_sprites();

      if (pad1 & PAD_START && !(prev_pad_state & PAD_START)) {
        game_mode = MODE_GAME;
        color_emphasis(COL_EMP_NORMAL);
      }
			
			if (pad1 & (PAD_SELECT) && !(prev_pad_state & (PAD_SELECT))) {
        level_metatile_index++;
				level_palette = cave_background_pal;
				game_mode = MODE_SWITCH;
      }
      prev_pad_state = pad1;
    }
  }
}

//thanks jroweboy
void update_tiles(int8_t diff) {
    if (diff == 0) {
        return;
    }
    if (diff < 0) {
        do {
            // 3 "pairs" of tiles to shift
            uint8_t offset = 0;
            for (uint8_t i=0; i < 3; i++) {
                // 16 bytes per tile
                asm volatile(R"ASM(
                    lda #15
                    clc
                    adc %0
                    tax
                    lda #128
                1:  lda parallax_buf + 16,x
                    asl
                    rol parallax_buf,x
                    rol parallax_buf + 16,x
                    dex
                    cpx %0
                    bpl 1b
                )ASM" : "+r"(offset) : : "a", "x");
                offset += 32;
            }
        }
        while ( ++diff > 0);
    } else {
        do {
            // 3 "pairs" of tiles to shift
            uint8_t offset = 0;
            for (uint8_t i=0; i < 3; i++) {
                // 16 bytes per tile
                asm volatile(R"ASM(
                    lda #15
                    clc
                    adc %0
                    tax
                1:  lda parallax_buf,x
                    lsr
                    ror parallax_buf + 16,x
                    ror parallax_buf,x 
                    dex
                    cpx %0
                    bpl 1b
                )ASM" : "+r"(offset) : : "a", "x");
                offset += 32;
            }
        }
        while ( --diff < 0);
    }
}



void draw_sprites(void) {
  // clear all sprites from sprite buffer
  

  
  /*
  for (uint8_t i = 0; i < MAX_ENEMY; ++i) {
    if (high_byte(enemy_y[i]) == TURN_OFF)
      continue;
    if (high_byte(enemy_x[i]) > 0xf0)
      continue;
    if (enemy_active[i] && (high_byte(enemy_y[i]) < 0xf0)) {
      oam_meta_spr(high_byte(enemy_x[i])+15, high_byte(enemy_y[i])+19, eninj_standL_data);
    }
  }
  
  for (uint8_t i = 0; i < MAX_SHURIK; ++i) {
    if (high_byte(shurik_y[i]) == TURN_OFF)
      continue;
    if (high_byte(shurik_x[i]) > 0xf0)
      continue;
    if (shurik_active[i] && (high_byte(shurik_y[i]) < 0xf0)) {
      oam_meta_spr(high_byte(shurik_x[i])+12, high_byte(shurik_y[i])+19, shurik0_data);
    }
  }*/
  
}

//Thanks Jroweboy
void flicker_sprites(void) {
  oam_clear();
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
    if (i > 0 && i < MAX_ENEMY+1) {
      if (high_byte(enemy_y[i-1]) == TURN_OFF)
        continue;
      if (high_byte(enemy_x[i-1]) > 0xf0)
        continue;
      if (enemy_active[i-1] && (high_byte(enemy_y[i-1]) < 0xf0)) {
        oam_meta_spr(high_byte(enemy_x[i-1])+13, high_byte(enemy_y[i-1])+13, enemy_anim[i-1][enemy_frame[i-1]]);
      }
    }
    if (i > MAX_ENEMY && i < MAX_SHURIK+MAX_ENEMY+1) {
      if (high_byte(shurik_y[i-MAX_ENEMY-1]) == TURN_OFF)
        continue;
      if (high_byte(shurik_x[i-MAX_ENEMY-1]) > 0xf0)
        continue;
      if (shurik_active[i-MAX_ENEMY-1] && (high_byte(shurik_y[i-MAX_ENEMY-1]) < 0xf0)) {
        oam_meta_spr(high_byte(shurik_x[i-MAX_ENEMY-1])+12, high_byte(shurik_y[i-MAX_ENEMY-1])+19, shurik0_data);
      }
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
    //puts("frame change go!\n");
  }
  old_animation = Ninj.animation;
}

void animate_enemies(void) {
	for (auto i = 0; i < MAX_ENEMY; ++i){
		if (enemy_anim[i] != old_enemy_animation[i]) {
			enemy_frame[i] = 0;
		}
		if (enemy_frame_counter[i] > 0) {
			enemy_frame_counter[i]--;
		} else {
			if (enemy_anim_duration[i][enemy_frame[i]+1] > 0) {
				enemy_frame[i]++;
			} else {
				enemy_frame[i] = 0;
			}
			enemy_frame_counter[i] = enemy_anim_duration[i][enemy_frame[i]];
			//puts("frame change go!\n");
		}
		old_enemy_animation[i] = enemy_anim[i];
	}
}
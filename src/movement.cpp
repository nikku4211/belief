#include "movement.hpp"
#include "global.hpp"
#include "collision.hpp"

#include <nesdoug.h>
#include <soa.h>
#include <string.h>
#include <stdio.h>
#include <neslib.h>

#include "sprites.h"
#include "anim.h"
#include "bankprgs.h"

#include "../level/rooms.h"

#define LEFT 0
#define RIGHT 1
#define ACCEL 0x40
#define GRAVITY 0x60
#define MAX_SPEED 0x320
#define JUMP_VEL (-0x600)
#define JUMP_TERMINAL 0xf00
#define ENEMY_JUMP_TERMINAL 0x300
#define SAFE_JUMP_STEP 0x300
#define FALL_DELAY_FRAMES 6

#define PLAYER_WIDTH 13
#define PLAYER_HEIGHT 26
#define PLAYER_CROUCH_TOP 13
#define PLAYER_CROUCH_HEIGHT 13

#define MELEE_WIDTH 26
#define MELEE_HEIGHT 26
#define PUNCH_TIME 24

#define MAX_RIGHT 0x8000
#define MAX_LEFT 0x7000

#define MAX_SHURIK 3

#define SHURIK_WIDTH 7
#define SHURIK_HEIGHT 8

#define ENEMY_WIDTH 13
#define ENEMY_HEIGHT 26

int8_t scroll_diff;

prg_rom_0 void movement(void) {

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
    bg_collision(high_byte(Ninj.x), high_byte(Ninj.y)+PLAYER_CROUCH_TOP, PLAYER_WIDTH,
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
    //puts("falling fast function\n");
    Ninj.y += SAFE_JUMP_STEP;
    
    L_R_switch = 0;
    if (!Ninj.is_crouching){
      bg_collision(high_byte(Ninj.x), high_byte(Ninj.y), PLAYER_WIDTH,
                   PLAYER_HEIGHT);
    } else {
      bg_collision(high_byte(Ninj.x), high_byte(Ninj.y)+PLAYER_CROUCH_TOP, PLAYER_WIDTH,
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
  //puts("falling fast function over\n");
  
  L_R_switch = 0;
  if (!Ninj.is_crouching){
    bg_collision(high_byte(Ninj.x), high_byte(Ninj.y), PLAYER_WIDTH,
                 PLAYER_HEIGHT);
  } else {
    bg_collision(high_byte(Ninj.x), high_byte(Ninj.y)+PLAYER_CROUCH_TOP, PLAYER_WIDTH,
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
    bg_check_low(high_byte(Ninj.x), high_byte(Ninj.y)+PLAYER_CROUCH_TOP, PLAYER_WIDTH,
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
  if (Ninj.x > MAX_RIGHT && scroll_x < MAX_SCROLL) {
    scroll_amt = (Ninj.x - MAX_RIGHT) >> 8;
    scroll_x += scroll_amt;
    high_byte(Ninj.x) -= scroll_amt;
    draw_screen_R();
    // do we need to load a new collision map? (scrolled into a new room)
    if ((scroll_x & 0x0f) < 4) {
      new_cmap();
    }
  } else if (Ninj.x < MAX_LEFT && scroll_x > 0) {
    scroll_amt = (MAX_LEFT - Ninj.x) >> 8;
    scroll_x -= scroll_amt;
    high_byte(Ninj.x) += scroll_amt;
    draw_screen_L();
    // do we need to load a new collision map? (scrolled into a new room)
    if ((scroll_x & 0x0f) < 4) {
      new_cmap();
    }
  }
  
	
	// for every 2 px the screen scrolls, counter scroll by 1px
  int16_t new_parallax_scroll = scroll_x >> 1;
  scroll_diff = new_parallax_scroll - old_parallax_scroll;
  update_tiles(scroll_diff);
	
	old_parallax_scroll = new_parallax_scroll;

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
  for (uint8_t i = 0; i < MAX_TOTAL_SHURIK; ++i) {
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

void enemy_moves(uint8_t index) {
	//thanks wendel
	// puts("enemy ");
	// putchar('0' + index);
	// puts(" be movin\n");
	
	// for bg collisions
    unsigned x = enemy_x[index];
    unsigned y = enemy_y[index];
    uint8_t width = ENEMY_WIDTH;
  if (enemy_type[index] == ENEMY_PATROL) {
		
		if (enemy_vel_x[index] > 0) {
			enemy_anim[index] = eninj_run_right_anim;
			enemy_anim_duration[index] = player_run_duration;
			bg_collision_horizontal(high_byte(x)+(ENEMY_WIDTH<<8), high_byte(y), width);
      if (collision_R) {
				enemy_vel_x[index] *= -1;
				enemy_actual_x[index] -= 256;
				return;
			}
			bg_check_low(high_byte(x)+(ENEMY_WIDTH<<8), high_byte(y), width, ENEMY_HEIGHT);
			if (!collision_D) {
				enemy_vel_x[index] *= -1;
				enemy_actual_x[index] -= 256;
				return;
			}
			if (enemy_actual_x[index] == 0xff00)
        ++enemy_room[index];
			enemy_actual_x[index] += enemy_vel_x[index];
		} else if (enemy_vel_x[index] < 0) {
			enemy_anim[index] = eninj_run_left_anim;
			enemy_anim_duration[index] = player_run_duration;
			bg_collision_horizontal(high_byte(x) - 256, high_byte(y), width);
      if (collision_L) {
				enemy_vel_x[index] *= -1;
				enemy_actual_x[index] += 256;
        return;
			}
			bg_check_low(high_byte(x)-256, high_byte(y), width, ENEMY_HEIGHT);
			if (!collision_D) {
				enemy_vel_x[index] *= -1;
				enemy_actual_x[index] += 256;
				return;
			}
			if (enemy_actual_x[index] == 0)
        --enemy_room[index];
			enemy_actual_x[index] += enemy_vel_x[index];
		}
    // if (enemy_frames & 1)
      // return; // half speed
  } else if (enemy_type[index] == ENEMY_SENTRY) {
		// char anim_timer = (enemy_timer + (index << 3)) & 0x3f;
		if (enemy_vel_y[index] > 0){
			puts("enemy falling\n");
			if ((enemy_timer & 0x07) == 0){
				
				shurik_active[enemy_shurik_throw_index[index]] = 1;
				shurik_actual_x[enemy_shurik_throw_index[index]] = enemy_actual_x[index];
				shurik_y[enemy_shurik_throw_index[index]] = enemy_y[index];
				if (Ninj.x > x) {
					enemy_anim[index] = eninj_throw_right_anim;
					shurik_vel_x[enemy_shurik_throw_index[index]] = 0x200;
				} else {
					enemy_anim[index] = eninj_throw_left_anim;
					shurik_vel_x[enemy_shurik_throw_index[index]] = -0x200;
				}
				shurik_vel_y[enemy_shurik_throw_index[index]] = 0;
				
				if (enemy_shurik_throw_index[index] < (MAX_TOTAL_SHURIK - 1))
					enemy_shurik_throw_index[index]++;
				else
					enemy_shurik_throw_index[index] = MAX_SHURIK;
			}
				
		} else {
			if (Ninj.x > x)
				enemy_anim[index] = eninj_stand_right_anim;
			else
				enemy_anim[index] = eninj_stand_left_anim;
		}
		
		enemy_y[index] += enemy_vel_y[index];
		
		if (enemy_vel_y[index] < ENEMY_JUMP_TERMINAL)
			enemy_vel_y[index] += GRAVITY;
		else
			enemy_vel_y[index] = ENEMY_JUMP_TERMINAL;
		
		bg_check_low(high_byte(x), high_byte(enemy_y[index]), width, ENEMY_HEIGHT);
		if (collision_D){
			enemy_vel_y[index] = 0;
			if ((enemy_timer & 0x0f) == 0){
				enemy_vel_y[index] = JUMP_VEL;
				puts("enemy jump\n");
			}
		}
  }

/*   for (uint8_t i = 0; i < MAX_ENEMY; ++i) {
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
  } */
  //puts("enemy movin\n");
}

void check_spr_objects(void) {
	//puts("checking sprite objects\n");
	++enemy_timer;
  // mark each object "active" if they are, and get the screen x

  for (unsigned char i = 0; i < MAX_ENEMY; ++i) {
    enemy_active[i] = 0; // default to zero
    if (high_byte(enemy_y[i]) != TURN_OFF) {
      unsigned x = ((enemy_room[i] << 8) | high_byte(enemy_actual_x[i])) - scroll_x;
			// puts("enemy check x:");
			// putchar('0'+((x & 0xf0)>>4));
			// putchar('0'+(x & 0x0f));
			// puts("\n");
      enemy_active[i] = !high_byte(x);
			// if (enemy_active[i]){
				// puts("enemy ");
				// putchar('0' + i);
				// puts(" online\n");
			// }
      if (!enemy_active[i])
        continue;
      enemy_x[i] = (x & 0xff) << 8; // screen x coords
			
      enemy_moves(i); // if active, do its moves now
    }
  }
  
  for (unsigned char i = 0; i < MAX_TOTAL_SHURIK; ++i) {
    if (high_byte(shurik_y[i]) != TURN_OFF) {
      unsigned x = high_byte(shurik_actual_x[i]) - scroll_x;
      shurik_x[i] = (x & 0xff) << 8; // screen x coords
    }
  }
}

void sprite_obj_init(void) {
  unsigned char i, j;
	const unsigned char *enemies = Enemy_list[level];
  for (i = 0, j = 0; i < MAX_ENEMY; ++i) {
    enemy_x[i] = 0;
    enemy_y[i] = enemies[j] << 8;
    if (high_byte(enemy_y[i]) == TURN_OFF)
      break;
    enemy_active[i] = 0;
    enemy_room[i] = enemies[++j];
    enemy_actual_x[i] = (enemies[++j]) << 8;
		enemy_type[i] = enemies[++j];
		enemy_vel_x[i] = 0;
		enemy_vel_y[i] = 0;
		if (enemy_type[i] == ENEMY_PATROL)
				enemy_vel_x[i] = -256;
    ++j;
		enemy_shurik_throw_index[i] = MAX_SHURIK;
  }
  for (++i; i < MAX_ENEMY; ++i)
    enemy_y[i] = TURN_OFF << 8;
  
  for (i = 0; i < MAX_TOTAL_SHURIK; ++i) {
    shurik_x[i] = 0;
    shurik_y[i] = TURN_OFF << 8;
    shurik_vel_x[i] = 0;
    shurik_vel_y[i] = 0;
    shurik_active[i] = 0;
    shurik_actual_x[i] = 0;
		if (i < MAX_SHURIK)
			shurik_source[i] = 0;
		else
			shurik_source[i] = 1;
  }
  
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
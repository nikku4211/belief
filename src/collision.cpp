#include "collision.hpp"
#include "global.hpp"

#include "../level/rooms.h"

#include <nesdoug.h>
#include <soa.h>
#include <string.h>
#include <stdio.h>
#include <neslib.h>

unsigned char collision_map[256];
unsigned char collision_map2[256];

#define COL_DOWN 0x80
#define COL_ALL 0x40

unsigned char collision_L;
unsigned char collision_R;
unsigned char collision_U;
unsigned char collision_D;
unsigned char eject_L;   // from the left
unsigned char eject_R;   // remember these from the collision sub routine
unsigned char eject_D;   // from below
unsigned char eject_U;   // from up

void bg_collision_horizontal(char x, char y, char width) {
  // rewrote this for enemies, bg_collision was too slow
  collision_L = 0;
  collision_R = 0;

  if (y >= 0xf0)
    return;

  unsigned upper_left = x + scroll_x; // upper left (temp6 = save for reuse)

  if (bg_collision_sub(upper_left, y) &
      COL_ALL) // find a corner in the collision map
    ++collision_L;

  // upper right
  unsigned upper_right = upper_left + width;

  if (bg_collision_sub(upper_right, y) &
      COL_ALL) // find a corner in the collision map
    ++collision_R;
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
  unsigned char typ = (x & 1 << 8 ? collision_map2 : collision_map)[upper_left]+level_metatile_index;
  return global_metatiles[typ].coll;
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
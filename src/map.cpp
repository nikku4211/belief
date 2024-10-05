#include "map.hpp"
#include "global.hpp"

#include "../chr/forestbg.h"

#include "../level/rooms.h"

#include <nesdoug.h>
#include <soa.h>
#include <string.h>
#include <stdio.h>
#include <neslib.h>

#define ATTRIBUTE_TOP_LEFT 3
#define ATTRIBUTE_TOP_RIGHT 12
#define ATTRIBUTE_BOTTOM_LEFT 48
#define ATTRIBUTE_BOTTOM_RIGHT 192

static unsigned char column_map[24];
static unsigned char column_map2[24];
static unsigned char column_map_atr[7];

static unsigned pseudo_scroll_x_right;
static unsigned pseudo_scroll_x_left;

static unsigned char rendered_x_left;
static unsigned char rendered_x_right;

void load_room(void) {
  set_data_pointer(rooms[0]);
  
	//put tiles of metatiles in a column map
  for (uint8_t x = 0;; x += 0x10) {
    for (uint8_t i = 0; i < 24; i+=2) {
      auto tile = *(rooms[0] + (x & 0xf0) + ((i >> 1)+3))+level_metatile_index;
      column_map[i] = global_metatiles[tile].tl;
      column_map[i+1] = global_metatiles[tile].bl;
      column_map2[i] = global_metatiles[tile].tr;
      column_map2[i+1] = global_metatiles[tile].br;
    }
    
    multi_vram_buffer_vert(column_map, 24, get_ppu_addr(0, x, 48));
    multi_vram_buffer_vert(column_map2, 24, get_ppu_addr(0, x+8, 48));
    
    flush_vram_update2();
    if (x == 0xf0)
      break;
  }
  for (uint8_t x = 0;; x += 0x10) {
    for (uint8_t i = 0; i < 7; i++) {
      auto tile = rooms[0] + (x & 0xf0) + ((i << 1)+2);
      if ((x & 0x10) == 0){
        column_map_atr[i] = 0;
				if(i > 0)
					column_map_atr[i] += (global_metatiles[*(tile)+level_metatile_index].attr & ATTRIBUTE_TOP_LEFT);
        column_map_atr[i] += (global_metatiles[*(tile+1)+level_metatile_index].attr & ATTRIBUTE_BOTTOM_LEFT);
        //puts("left attribute\n");
      } else {
				if(i > 0)
					column_map_atr[i] += (global_metatiles[*(tile)+level_metatile_index].attr & ATTRIBUTE_TOP_RIGHT);
        column_map_atr[i] += (global_metatiles[*(tile+1)+level_metatile_index].attr & ATTRIBUTE_BOTTOM_RIGHT);
        //puts("right attribute\n");
      }
    }
    if ((x & 0x10) == 0x10) {
      auto addr = get_at_addr(0,x,32);
      for (auto i = 0; i < 7; ++i) {
        one_vram_buffer(column_map_atr[i], addr);
        addr += 8;
      }
      flush_vram_update2();
    }
    if (x == 0xf0)
      break;
  }
  

  // a little bit in the next room
  set_data_pointer(rooms[0]+256);
  
  for (uint8_t i = 0; i < 24; i+=2) {
    auto tile = *(rooms[0] + 256 + ((i >> 1)+3))+level_metatile_index;
    column_map[i] = global_metatiles[tile].tl;
    column_map[i+1] = global_metatiles[tile].bl;
    column_map2[i] = global_metatiles[tile].tr;
    column_map2[i+1] = global_metatiles[tile].br;
  }
  
  multi_vram_buffer_vert(column_map, 24, get_ppu_addr(1, 0, 48));
  multi_vram_buffer_vert(column_map2, 24, get_ppu_addr(1, 8, 48));

  // copy the room to the collision map
  // the second one should auto-load with the scrolling code
  memcpy(collision_map, rooms[0], 256);
  
	memcpy(parallax_buf, forestbg+176, 96);
	
  sprite_obj_init();
}

void draw_screen_R(void) {
  // scrolling to the right, draw metatiles as we go
  pseudo_scroll_x_right = scroll_x + 0x110;
  unsigned char x = pseudo_scroll_x_right & 0xf0;

  // If we've already drawn this strip of metatiles, just return
  if (rendered_x_right == x) {
    return;
  }

  rendered_x_right = x;

  unsigned char room = pseudo_scroll_x_right >> 8;
  unsigned char roomp = high_byte(pseudo_scroll_x_right);

  set_data_pointer(rooms[0]+roomp);
  unsigned char nt = room & 1;

  // important that the main loop clears the vram_buffer
  
  for (uint8_t i = 0; i < 24; i+=2) {
    auto tile = *(rooms[0] + (pseudo_scroll_x_right & 0xfff0) + ((i >> 1)+3))+level_metatile_index;
    column_map[i] = global_metatiles[tile].tl;
    column_map[i+1] = global_metatiles[tile].bl;
    column_map2[i] = global_metatiles[tile].tr;
    column_map2[i+1] = global_metatiles[tile].br;
  }

	multi_vram_buffer_vert(column_map, 24, get_ppu_addr(nt, x, 48));
  multi_vram_buffer_vert(column_map2, 24, get_ppu_addr(nt, x+8, 48));
  
  for (uint8_t i = 0; i < 7; i++) {
    auto tile = rooms[0] + (pseudo_scroll_x_right & 0xffe0) + ((i<<1)+2);
    column_map_atr[i] = 0;
		if(i > 0)
			column_map_atr[i] += (global_metatiles[*(tile)+level_metatile_index].attr & ATTRIBUTE_TOP_LEFT);
    column_map_atr[i] += (global_metatiles[*(tile + 1)+level_metatile_index].attr & ATTRIBUTE_BOTTOM_LEFT);
    //puts("left attribute\n");
		if(i > 0)
			column_map_atr[i] += (global_metatiles[*(tile + 16)+level_metatile_index].attr & ATTRIBUTE_TOP_RIGHT);
    column_map_atr[i] += (global_metatiles[*(tile + 17)+level_metatile_index].attr & ATTRIBUTE_BOTTOM_RIGHT);
    //puts("right attribute\n");
    
  }
  
  auto addr = get_at_addr(nt,x,32);
  for (auto i = 0; i < 8; ++i) {
    one_vram_buffer(column_map_atr[i], addr);
    addr += 8;
  }
	
  NAME_UPD_ENABLE = 1;
}

void draw_screen_L(void) {
  // scrolling to the left, draw metatiles as we go
  pseudo_scroll_x_left = scroll_x - 0x10;
  unsigned char x = pseudo_scroll_x_left & 0xf0;

  // If we've already drawn this strip of metatiles, just return
  if (rendered_x_left == x) {
    return;
  }

  rendered_x_left = x;

  unsigned char room = pseudo_scroll_x_left >> 8;

  set_data_pointer(rooms[0]+(room << 8));
  unsigned char nt = room & 1;

  // important that the main loop clears the vram_buffer

  for (uint8_t i = 0; i < 24; i+=2) {
    auto tile = *(rooms[0] + (pseudo_scroll_x_left & 0xfff0) + ((i >> 1)+3))+level_metatile_index;
    column_map[i] = global_metatiles[tile].tl;
    column_map[i+1] = global_metatiles[tile].bl;
    column_map2[i] = global_metatiles[tile].tr;
    column_map2[i+1] = global_metatiles[tile].br;
  }
  
  multi_vram_buffer_vert(column_map, 24, get_ppu_addr(nt, x, 48));
  multi_vram_buffer_vert(column_map2, 24, get_ppu_addr(nt, x+8, 48));
  
  for (uint8_t i = 0; i < 7; i++) {
    auto tile = rooms[0] + (pseudo_scroll_x_left & 0xffe0) + ((i<<1)+2);
    column_map_atr[i] = 0;
		if(i > 0)
			column_map_atr[i] += (global_metatiles[*(tile)+level_metatile_index].attr & ATTRIBUTE_TOP_LEFT);
    column_map_atr[i] += (global_metatiles[*(tile + 1)+level_metatile_index].attr & ATTRIBUTE_BOTTOM_LEFT);
    //puts("left attribute\n");
		if(i > 0)
			column_map_atr[i] += (global_metatiles[*(tile + 16)+level_metatile_index].attr & ATTRIBUTE_TOP_RIGHT);
    column_map_atr[i] += (global_metatiles[*(tile + 17)+level_metatile_index].attr & ATTRIBUTE_BOTTOM_RIGHT);
    //puts("right attribute\n");
    
  }
  
  auto addr = get_at_addr(nt,x,32);
  for (auto i = 0; i < 8; ++i) {
    one_vram_buffer(column_map_atr[i], addr);
    addr += 8;
  }
  NAME_UPD_ENABLE = 1;
}
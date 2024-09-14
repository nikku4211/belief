#include "rooms.h"
#include "../src/bankprgs.h"

extern prg_rom_0 const unsigned char testmap2_tilemap[];

const unsigned char *const rooms[] = {testmap2_tilemap};

//y, room, x, type
//y = TURN_OFF end of list
// clang-format on
static const unsigned char level_1_enemies[] = {
  0x75, 0, 0xf0, ENEMY_CHASE,
  0x85, 1, 0x80, ENEMY_CHASE,
  0x85, 2, 0xc0, ENEMY_CHASE,
  0x85, 3, 0xf0, ENEMY_CHASE,
  TURN_OFF
};
// clang-format off

const unsigned char *const Enemy_list[] = {level_1_enemies};

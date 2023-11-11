#include "rooms.h"

extern const unsigned char testmap1_collision[];
extern const unsigned char testmap2_collision[];

const unsigned char *const rooms[] = {testmap1_collision, testmap1_collision,testmap2_collision};

//y, room, x, type
//y = TURN_OFF end of list
// clang-format on
static const unsigned char level_1_enemies[] = {
  0xb5, 0, 0xb0, ENEMY_CHASE,
  0xb5, 1, 0x80, ENEMY_CHASE,
  0xb5, 2, 0xc0, ENEMY_CHASE,
  0xb5, 3, 0xf0, ENEMY_CHASE,
  TURN_OFF
};
// clang-format off

const unsigned char *const Enemy_list[] = {level_1_enemies, level_1_enemies, level_1_enemies};

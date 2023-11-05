#include "rooms.h"

extern const unsigned char testmap1_collision[];
extern const unsigned char testmap2_collision[];

const unsigned char *const rooms[] = {testmap1_collision, testmap1_collision,testmap2_collision, testmap2_collision,testmap1_collision, testmap2_collision};

//y, room, x
//y = TURN_OFF end of list
// clang-format on
const char level_1_enemies[] = {
  0xb5, 0, 0xb0,
  0xb5, 1, 0x80,
  0xb5, 2, 0xc0,
  0xb5, 3, 0xf0,
  TURN_OFF
};
// clang-format off


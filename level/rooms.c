#include "rooms.h"
#include "../src/bankprgs.h"

extern prg_rom_0 const unsigned char testmap2_tilemap[];
extern prg_rom_0 const unsigned char testmap2_sprites[];

const unsigned char *const rooms[] = {testmap2_tilemap};

//y, room, x, type
//y = TURN_OFF end of list


const unsigned char *const Enemy_list[] = {testmap2_sprites};

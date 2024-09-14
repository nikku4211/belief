#pragma once

#include "bankprgs.h"

void playenemy_collisions(void);
void playenemy_melee_collisions(void);
void shurenemy_collisions(unsigned char shurik_num);

void check_spr_objects(void);
void shurik_moves(void);
prg_rom_0 void movement(void);

void draw_screen_L(void);
void draw_screen_R(void);
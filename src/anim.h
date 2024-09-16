#include <neslib.h>
#include "sprites.h"

#ifndef ANIM_H
#define ANIM_H

// a 24x32 pixel metasprite's animations

extern const unsigned char* const player_stand_right_anim[];
extern const unsigned char* const player_stand_left_anim[];
extern const unsigned char* const player_lose_right_anim[];
extern const unsigned char* const player_lose_left_anim[];
extern const unsigned char* const player_crouch_right_anim[];
extern const unsigned char* const player_crouch_left_anim[];
extern const unsigned char* const player_run_right_anim[];
extern const unsigned char* const player_run_left_anim[];
extern const unsigned char* const player_throw_right_anim[];
extern const unsigned char* const player_throw_left_anim[];

extern const unsigned char* const eninj_stand_right_anim[];
extern const unsigned char* const eninj_stand_left_anim[];
extern const unsigned char* const eninj_lose_right_anim[];
extern const unsigned char* const eninj_lose_left_anim[];
extern const unsigned char* const eninj_run_right_anim[];
extern const unsigned char* const eninj_run_left_anim[];
extern const unsigned char* const eninj_throw_right_anim[];
extern const unsigned char* const eninj_throw_left_anim[];

extern const unsigned char player_stand_duration[];
extern const unsigned char player_lose_duration[];
extern const unsigned char player_crouch_duration[];
extern const unsigned char player_run_duration[];
extern const unsigned char player_throw_duration[];

#endif // ANIM_H
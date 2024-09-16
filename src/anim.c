#include <neslib.h>
#include "sprites.h"

const unsigned char* const player_stand_right_anim[]={
	player_standR_data, player_standR_data
};

const unsigned char* const player_stand_left_anim[]={
	player_standL_data, player_standR_data
};

const unsigned char* const player_lose_right_anim[]={
	player_loseR_data, player_standR_data
};

const unsigned char* const player_lose_left_anim[]={
	player_loseL_data, player_standR_data
};

const unsigned char* const player_crouch_right_anim[]={
	player_crouchR_data, player_standR_data
};

const unsigned char* const player_crouch_left_anim[]={
	player_crouchL_data, player_standR_data
};

const unsigned char* const player_run_right_anim[]={
	player_runR0_data,
	player_runR1_data,
	player_runR2_data,
	player_runR3_data, player_standR_data
};

const unsigned char* const player_run_left_anim[]={
	player_runL0_data,
	player_runL1_data,
	player_runL2_data,
	player_runL3_data, player_standR_data
};

const unsigned char* const player_throw_right_anim[]={
	player_throwR0_data,
	player_throwR1_data, player_standR_data
};

const unsigned char* const player_throw_left_anim[]={
	player_throwL0_data,
	player_throwL1_data, player_standR_data
};

const unsigned char player_stand_duration[]={
	10,
	0
};

const unsigned char player_lose_duration[]={
	10,
	0
};

const unsigned char player_crouch_duration[]={
	10,
	0
};

const unsigned char player_run_duration[]={
	10,
	6,
	10,
	6,
	0
};

const unsigned char player_throw_duration[]={
	12,
	12,
	0
};

const unsigned char* const eninj_stand_right_anim[]={
	eninj_standR_data, eninj_standR_data
};

const unsigned char* const eninj_stand_left_anim[]={
	eninj_standL_data, eninj_standR_data
};

const unsigned char* const eninj_lose_right_anim[]={
	eninj_loseR_data, eninj_standR_data
};

const unsigned char* const eninj_lose_left_anim[]={
	eninj_loseL_data, eninj_standR_data
};

const unsigned char* const eninj_run_right_anim[]={
	eninj_runR0_data,
	eninj_runR1_data,
	eninj_runR2_data,
	eninj_runR3_data, eninj_standR_data
};

const unsigned char* const eninj_run_left_anim[]={
	eninj_runL0_data,
	eninj_runL1_data,
	eninj_runL2_data,
	eninj_runL3_data, eninj_standR_data
};

const unsigned char* const eninj_throw_right_anim[]={
	eninj_throwR0_data,
	eninj_throwR1_data, eninj_standR_data
};

const unsigned char* const eninj_throw_left_anim[]={
	eninj_throwL0_data,
	eninj_throwL1_data, eninj_standR_data
};
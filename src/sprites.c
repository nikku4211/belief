#include <neslib.h>

const unsigned char player_standR_data[]={

	-16,-24,0x01,0,
	- 8,-24,0x03,0,
	-16,- 8,0x21,0,
	- 8,- 8,0x23,0,
	0x80

};

const unsigned char player_throwR0_data[]={

	-15,-24,0x05,0,
	- 7,-24,0x07,0,
	-16,- 8,0x25,0,
	- 8,- 8,0x27,0,
	0x80

};

const unsigned char player_throwR1_data[]={

	-15,-24,0x05,0,
	- 7,-24,0x09,0,
	-16,- 8,0x25,0,
	- 8,- 8,0x29,0,

	  0,- 8,0x2b,0,
	  1,-24,0x0b,0,
	0x80

};

const unsigned char player_throwL0_data[]={

	-17,-24,0x0d,0,
	- 9,-24,0x0f,0,
	-16,- 8,0x2d,0,
	- 8,- 8,0x2f,0,
	0x80

};

const unsigned char player_throwL1_data[]={

	-25,-24,0x11,0,
	-17,-24,0x13,0,
	- 9,-24,0x0f,0,
	-24,- 8,0x31,0,

	-16,- 8,0x33,0,
	- 8,- 8,0x35,0,
	0x80

};

const unsigned char player_loseR_data[]={

	-24,- 8,0x17,0,
	-16,- 8,0x19,0,
	- 8,- 8,0x1b,0,
	0x80

};

const unsigned char player_crouchR_data[]={

	-20,- 8,0x37,0,
	-12,- 8,0x39,0,
	- 4,- 8,0x3b,0,
	-10,-23,0x15,0,
	0x80

};

const unsigned char player_runR1_data[]={

	-16,-24,0x1d,0,
	- 8,-24,0x1f,0,
	-16,- 8,0x3d,0,
	- 8,- 8,0x3f,0,
	0x80

};

const unsigned char player_runR0_data[]={

	-18,-24,0x41,0,
	-10,-24,0x43,0,
	-20,- 8,0x4d,0,
	-12,- 8,0x4f,0,

	- 4,- 8,0x51,0,
	0x80

};

const unsigned char player_runR3_data[]={

	-16,-24,0x1d,0,
	- 8,-24,0x1f,0,
	-16,- 8,0x49,0,
	- 8,- 8,0x4b,0,
	0x80

};

const unsigned char player_runR2_data[]={

	-16,-24,0x45,0,
	- 8,-24,0x47,0,
	-19,- 8,0x53,0,
	-11,- 8,0x55,0,
	0x80

};

const unsigned char player_standL_data[]={

	- 8,-24,0x01,0|OAM_FLIP_H,
	-16,-24,0x03,0|OAM_FLIP_H,
	- 8,- 8,0x21,0|OAM_FLIP_H,
	-16,- 8,0x23,0|OAM_FLIP_H,
	0x80

};

const unsigned char player_loseL_data[]={

	  1,- 8,0x17,0|OAM_FLIP_H,
	- 7,- 8,0x19,0|OAM_FLIP_H,
	-15,- 8,0x1b,0|OAM_FLIP_H,
	0x80

};

const unsigned char player_crouchL_data[]={

	- 4,- 8,0x37,0|OAM_FLIP_H,
	-12,- 8,0x39,0|OAM_FLIP_H,
	-20,- 8,0x3b,0|OAM_FLIP_H,
	-14,-23,0x15,0|OAM_FLIP_H,
	0x80

};

const unsigned char player_runL0_data[]={

	- 6,-24,0x41,0|OAM_FLIP_H,
	-14,-24,0x43,0|OAM_FLIP_H,
	- 4,- 8,0x4d,0|OAM_FLIP_H,
	-12,- 8,0x4f,0|OAM_FLIP_H,

	-20,- 8,0x51,0|OAM_FLIP_H,
	0x80

};

const unsigned char player_runL1_data[]={

	- 8,-24,0x1d,0|OAM_FLIP_H,
	-16,-24,0x1f,0|OAM_FLIP_H,
	- 8,- 8,0x3d,0|OAM_FLIP_H,
	-16,- 8,0x3f,0|OAM_FLIP_H,
	0x80

};

const unsigned char player_runL2_data[]={

	- 8,-24,0x45,0|OAM_FLIP_H,
	-16,-24,0x47,0|OAM_FLIP_H,
	- 5,- 8,0x53,0|OAM_FLIP_H,
	-13,- 8,0x55,0|OAM_FLIP_H,
	0x80

};

const unsigned char player_runL3_data[]={

	- 8,-24,0x1d,0|OAM_FLIP_H,
	-16,-24,0x1f,0|OAM_FLIP_H,
	- 8,- 8,0x49,0|OAM_FLIP_H,
	-16,- 8,0x4b,0|OAM_FLIP_H,
	0x80

};

const unsigned char* const player_list[]={

	player_standR_data,
	player_throwR0_data,
	player_throwR1_data,
	player_throwL0_data,
	player_throwL1_data,
	player_loseR_data,
	player_crouchR_data,
	player_runR1_data,
	player_runR0_data,
	player_runR3_data,
	player_runR2_data,
	player_standL_data,
	player_loseL_data,
	player_crouchL_data,
	player_runL0_data,
	player_runL1_data,
	player_runL2_data,
	player_runL3_data

};

const unsigned char eninj_standR_data[]={

	-16,-24,0x57,1,
	- 8,-24,0x59,1,
	-16,- 8,0x77,1,
	- 8,- 8,0x79,1,
	0x80

};

const unsigned char eninj_throwR0_data[]={

	-15,-24,0x5b,1,
	- 7,-24,0x5d,1,
	-16,- 8,0x7b,1,
	- 8,- 8,0x7d,1,
	0x80

};

const unsigned char eninj_throwR1_data[]={

	-15,-24,0x5f,1,
	- 7,-24,0x5d,1,
	-16,- 8,0x7b,1,
	- 8,- 8,0x7d,1,

	  0,-16,0x61,1,
	0x80

};

const unsigned char eninj_throwL0_data[]={

	-16,- 8,0x7f,1,
	- 8,- 8,0x81,1,
	-16,-24,0x63,1,
	- 8,-24,0x65,1,
	0x80

};

const unsigned char eninj_throwL1_data[]={

	-16,- 8,0x83,1,
	- 8,- 8,0x85,1,
	-16,-24,0x69,1,
	- 8,-24,0x65,1,

	-24,-10,0x67,1,
	0x80

};

const unsigned char eninj_loseR_data[]={

	-20,- 8,0x8f,1,
	-12,- 8,0x91,1,
	- 4,- 8,0x93,1,
	0x80

};

const unsigned char eninj_runR0_data[]={

	-20,-10,0x87,1,
	-12,-10,0x89,1,
	- 4,-10,0x8b,1,
	-12,-26,0x6b,1,

	- 4,-26,0x6d,1,
	0x80

};

const unsigned char eninj_runR1_data[]={

	-12,- 8,0x73,1,
	- 4,- 8,0x75,1,
	-12,-24,0x6f,1,
	- 4,-24,0x71,1,

	-20,- 8,0x8d,1,
	0x80

};

const unsigned char eninj_runR2_data[]={

	-20,-24,0x97,1,
	-12,-24,0x99,1,
	- 4,-24,0x9b,1,
	-20,- 8,0x9f,1,

	-12,- 8,0xa1,1,
	- 4,- 8,0xa3,1,
	0x80

};

const unsigned char eninj_runR3_data[]={

	-12,- 8,0xa5,1,
	- 4,- 8,0x95,1,
	-12,-24,0x9d,1,
	- 4,-24,0x71,1,

	-20,- 8,0x8d,1,
	0x80

};

const unsigned char eninj_standL_data[]={

	- 8,-24,0x57,1|OAM_FLIP_H,
	-16,-24,0x59,1|OAM_FLIP_H,
	- 8,- 8,0x77,1|OAM_FLIP_H,
	-16,- 8,0x79,1|OAM_FLIP_H,
	0x80

};

const unsigned char eninj_loseL_data[]={

	- 4,- 8,0x8f,1|OAM_FLIP_H,
	-12,- 8,0x91,1|OAM_FLIP_H,
	-20,- 8,0x93,1|OAM_FLIP_H,
	0x80

};

const unsigned char eninj_runL0_data[]={

	- 4,-10,0x87,1|OAM_FLIP_H,
	-12,-10,0x89,1|OAM_FLIP_H,
	-20,-10,0x8b,1|OAM_FLIP_H,
	-12,-26,0x6b,1|OAM_FLIP_H,

	-20,-26,0x6d,1|OAM_FLIP_H,
	0x80

};

const unsigned char eninj_runL1_data[]={

	-12,- 8,0x73,1|OAM_FLIP_H,
	-20,- 8,0x75,1|OAM_FLIP_H,
	-12,-24,0x6f,1|OAM_FLIP_H,
	-20,-24,0x71,1|OAM_FLIP_H,

	- 4,- 8,0x8d,1|OAM_FLIP_H,
	0x80

};

const unsigned char eninj_runL2_data[]={

	- 4,-24,0x97,1|OAM_FLIP_H,
	-12,-24,0x99,1|OAM_FLIP_H,
	-20,-24,0x9b,1|OAM_FLIP_H,
	- 4,- 8,0x9f,1|OAM_FLIP_H,

	-12,- 8,0xa1,1|OAM_FLIP_H,
	-20,- 8,0xa3,1|OAM_FLIP_H,
	0x80

};

const unsigned char eninj_runL3_data[]={

	-12,- 8,0xa5,1|OAM_FLIP_H,
	-20,- 8,0x95,1|OAM_FLIP_H,
	-12,-24,0x9d,1|OAM_FLIP_H,
	-20,-24,0x71,1|OAM_FLIP_H,

	- 4,- 8,0x8d,1|OAM_FLIP_H,
	0x80

};

const unsigned char* const eninj_list[]={

	eninj_standR_data,
	eninj_throwR0_data,
	eninj_throwR1_data,
	eninj_throwL0_data,
	eninj_throwL1_data,
	eninj_loseR_data,
	eninj_runR0_data,
	eninj_runR1_data,
	eninj_runR2_data,
	eninj_runR3_data,
	eninj_standL_data,
	eninj_loseL_data,
	eninj_runL0_data,
	eninj_runL1_data,
	eninj_runL2_data,
	eninj_runL3_data

};

const unsigned char shurik0_data[]={

	-12,-12,0xe1,1,
	0x80

};

const unsigned char shurik1_data[]={

	-12,-12,0xe3,1,
	0x80

};

const unsigned char* const shurik_list[]={

	shurik0_data,
	shurik1_data

};
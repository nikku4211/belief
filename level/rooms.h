#ifndef ROOMS_H
#define ROOMS_H

// data is exactly 256 bytes, 16 * 16

extern const unsigned char *const rooms[];

enum { ENEMY_CHASE, ENEMY_BOUNCE };

extern const unsigned char *const Enemy_list[];

#define MAX_ROOMS (5 - 1)
#define MAX_SCROLL (MAX_ROOMS * 0x100 - 1)

#define TURN_OFF 0xff

#endif // ROOMS_H

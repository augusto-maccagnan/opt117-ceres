#ifndef _UTILS_H_
#define _UTILS_H_

#include <genesis.h>
#include <sprite_eng.h>
#include "globals.h"

extern u8 buttons[NUMBER_OF_JOYPADS];
extern u8 buttons_old[NUMBER_OF_JOYPADS];

////////////////////////////////////////////////////////////////////////////
// LOGIC

// #define wrap(X, L, H) ((X < L)? H : ((X > H)? : L: X))

#define WRAP(X, L, H) \
	if (X < L) \
		X = H; \
	else if (X > H) \
		X = L;

// #define clamp(X, L, H)   (min(max((X), (L)), (H)))

inline Box UTILS_get_box(int x, int y, int w, int h) {
	return (Box){x, x+w, y, y+h};
}

// void print_bits(u32 value) {
// 	// HEX -> 1234 1234 1234 1234
// 	u32 mask = 0x00000000000000FF;
// 	char str[33];
// 	for (u8 i = 0; i < 4; ++i) {
// 		u32 nibble = value & mask;
// 		str
// 	}
// 	str[32] = 0;
// 	kprintf("%s");
// }

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

static inline void glow_color(u16 color_index, const u16* const color_vector, u8 n) {
	static u8 idx = 0;
	static u8 inc = 1;
	
	PAL_setColor(color_index, color_vector[idx]);
		
	idx += inc;
	if (idx == 0 || idx == n-1) {
		inc = -inc;
	}
}

inline void rotate_colors(u8 first_index, u8 last_index, s8 direction) {
	u16 last_color = PAL_getColor(first_index);
	for (u8 i = first_index; i != last_index; i += direction) {
		PAL_setColor(i, PAL_getColor(i + direction));
	}
	PAL_setColor(last_index, last_color);
}
//  2 3 4 5 1
//  i        
//  last: 1

inline void rotate_colors_left(u8 left_index, u8 right_index) {
	rotate_colors(left_index, right_index, 1);
}

inline void rotate_colors_right(u8 left_index, u8 right_index) {
	rotate_colors(right_index, left_index, -1);
}

//////////////////////////////////////////////////////////////////////////
// GAMEPAD INPUT

inline void update_input() {
    // update joypad input
    for (int i = NUMBER_OF_JOYPADS-1; i >= 0; --i) {
		buttons_old[i] = buttons[i];
		buttons[i] = JOY_readJoypad(i);
	}
}

inline bool is_bitset(u8 value, u8 bit) {
    return ((value & bit) == bit);
}

inline bool key_down(u8 joy_id, u8 key) {
    return is_bitset(buttons[joy_id], key);
}

inline bool key_pressed(u8 joy_id, u8 key) {
    return is_bitset(buttons[joy_id], key) && !is_bitset(buttons_old[joy_id], key);
}

inline bool key_released(u8 joy_id, u8 key) {
    return !is_bitset(buttons[joy_id], key) && is_bitset(buttons_old[joy_id], key);
}

#endif // _STRUCTS_H_
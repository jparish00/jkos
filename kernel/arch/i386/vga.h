#ifndef _ARCH_I386_VGA_H_
#define _ARCH_I386_VGA_H_

#include <stdint.h>

static size_t const VGA_WIDTH = 80;
static size_t const VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;
static uint8_t const MAX_SCANLINES = 15;

static uint16_t const VGA_INDEX_PORT = 0x3D4;
static uint16_t const VGA_DATA_PORT = 0x3D5;

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

/**************************************************************************//**
 * @brief This function converts specified VGA colors into the correct format.
 *
 * @param fg Foreground color.
 * @param bg Background color.
 * @return Formatted color value.
 *              
 ******************************************************************************/
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

/**************************************************************************//**
 * @brief This function returns the correct character format for VGA text mode.
 *
 * @param uc Character to write.
 * @param color Color of character, preferably derived from vga_entry_color();
 * @param blink Boolean to determine whether written character blinks.
 * @return Formatted character value.
 *              
 ******************************************************************************/
static inline uint16_t vga_entry(unsigned char uc, uint8_t color, bool blink) {
	return (uint16_t) uc | (uint16_t) color << 8 | (uint16_t) blink << 15;
}

#endif // _ARCH_I386_VGA_H_
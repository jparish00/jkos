#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/pio.h>

#include "vga.h"

// state var
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;
static uint8_t terminal_blink;

/**************************************************************************//**
 * @brief Initializes terminal functionality.
 *
 * This function set up default state variables, and writes white space to
 * the VGA text mode address.
 *              
 ******************************************************************************/
void term_init(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
    terminal_blink = 0;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color, false);
		}
	}
}

// void term_setcolor(uint8_t color) {
// 	terminal_color = color;
// }

// void term_setblinkchar(uint8_t blink) {

// }

/**************************************************************************//**
 * @brief Sets character foreground color.
 *
 * This function sets the foreground color for all characters written afterwards.
 * 
 * @param color Color to assign foreground. Restricted to VGA text mode colors.
 *              
 ******************************************************************************/
void term_setfgcolor(uint8_t color) {
    terminal_color &= 0b01110000;
    terminal_color |= (0b1111 & color);
}

/**************************************************************************//**
 * @brief Sets character background color.
 *
 * This function sets the background color for all characters written afterwards.
 * 
 * @param color Color to assign background. Restricted to VGA text mode colors.
 *              
 ******************************************************************************/
void term_setbgcolor(uint8_t color) {
    terminal_color &= 0b1111;
    terminal_color |= ((0b111 & color) << 4);
}

/**************************************************************************//**
 * @brief Local function. Writes character to terminal buffer.
 * 
 * @param c Character to write. Restricted to VGA text mode characters.
 * @param color Color to assign to character.
 * @param x Horizontal location to write character. Index starts at 0.
 * @param y Vertical location to write character. Index starts at 0.
 * 
 ******************************************************************************/
void term_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color, false);
}

/**************************************************************************//**
 * @brief Local function. Scrolls if terminal full after new line.
 * 
 ******************************************************************************/
void term_scroll() {
    terminal_row--;

    // Move everything up 1
    for(size_t y = 0; y < VGA_HEIGHT; y++) {
        for(size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t upper_index = y * VGA_WIDTH + x;
            const size_t lower_index = (y+1) * VGA_WIDTH + x;
            terminal_buffer[upper_index] = terminal_buffer[lower_index];
        }
    }
}

/**************************************************************************//**
 * @brief Writes character to terminal buffer. Prefer to use term_write() or term_writestring().
 * 
 * @param c Character to write. Restricted to VGA text mode characters.
 * 
 ******************************************************************************/
void term_putchar(char c) {
	unsigned char uc = c;

    if (uc == 10) { // Handle newlines
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT)
        term_scroll();
    } else {
        term_putentryat(uc, terminal_color, terminal_column, terminal_row);
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT)
                term_scroll();
        }
    }
}

/**************************************************************************//**
 * @brief Writes characters to terminal, up to specified size.
 * 
 * @param data Character array. Restricted to VGA text mode characters.
 * @param size Number of characters to write.
 * 
 ******************************************************************************/
void term_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		term_putchar(data[i]);
        term_setcursorpos(terminal_row, terminal_column);
}

/**************************************************************************//**
 * @brief Writes string to terminal.
 * 
 * @param data Character array. Restricted to VGA text mode characters.
 * 
 ******************************************************************************/
void term_writestring(const char* data) {
	term_write(data, strlen(data));
}

/**************************************************************************//**
 * @brief Enables cursor.
 * 
 * This function enables the cursor at 0, 0.
 * 
 * @param min Low scanline row. Must be less that max.
 * @param size High scanline row.  Must be greater than min, less than 16.
 * 
 ******************************************************************************/
void term_enablecursor(uint8_t min, uint8_t max) { 
    
    if(max <= min)
        return;
    else if(max > MAX_SCANLINES)
        max = MAX_SCANLINES;
    
    outb(0x0A, VGA_INDEX_PORT);
	outb((inb(VGA_DATA_PORT) & 0xC0) | min, VGA_DATA_PORT);

	outb(0x0B, VGA_INDEX_PORT);
	outb((inb(VGA_DATA_PORT) & 0xE0) | max, VGA_DATA_PORT);
}

/**************************************************************************//**
 * @brief Enables cursor with default(maximum) scanline width.
 * 
 * This function enables the cursor at 0, 0.
 * 
 ******************************************************************************/
void term_enablecursordefault() {
    term_enablecursor(0, 15);
}

/**************************************************************************//**
 * @brief Disables cursor.
 * 
 ******************************************************************************/
void term_disablecursor() {
    outb(0x0A, VGA_INDEX_PORT);
	outb(0x20, VGA_DATA_PORT);
}

/**************************************************************************//**
 * @brief Modifies cursor position to specified location.
 * 
 * @param x Horizontal location. Index starts at 0.
 * @param y Vertical location. Index starts at 0.
 * 
 ******************************************************************************/
void term_setcursorpos(uint8_t x, uint8_t y) {

	uint16_t pos = y * 80 + x;

	outb(0x0F, VGA_INDEX_PORT);
	outb((uint8_t) (pos & 0xFF), VGA_DATA_PORT);
	outb(0x0E, VGA_INDEX_PORT);
	outb((uint8_t) ((pos >> 8) & 0xFF), VGA_DATA_PORT);

}
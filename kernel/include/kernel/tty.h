#ifndef _KERNEL_TTY_H_
#define _KERNEL_TTY_H_

#include <stddef.h>
#include <stdint.h>

void term_init();
void term_setfgcolor(uint8_t color);
void term_setbgcolor(uint8_t color);
void term_putchar(char c);
void term_write(const char* data, size_t size);
void term_writestring(const char* data);
void term_enablecursor(uint8_t min, uint8_t max);
void term_enablecursordefault();
void term_disablecursor();
void term_setcursorpos(uint8_t x, uint8_t y);

#endif // _KERNEL_TTY_H_

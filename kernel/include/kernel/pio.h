#ifndef _KERNEL_IO_H_
#define _KERNEL_IO_H_

#include <stddef.h>
#include <stdint.h>

#define write_byte(x, y) outb(x, y)
#define write_word(x, y) outw(x, y)

#define read_byte(x, y) inb(x, y)
#define read_word(x, y) inw(x, y)

void outb(uint8_t value, unsigned short int port);
void outw(uint16_t value, unsigned short int port);

uint8_t inb(unsigned short int port);
uint16_t inw(unsigned short int port);

#endif // _KERNEL_IO_H_

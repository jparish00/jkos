#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <kernel/pio.h>

/**************************************************************************//**
 * @brief Writes an 8-bit value to the specified port.
 *
 * This function writes an 8-bit value to a hardware port, through
 * a specified 16-bit address.
 *
 * @param value Value to be written.
 * @param port Port to be written to.
 *              
 ******************************************************************************/
void outb(uint8_t value, unsigned short int port) {

    asm("out %0, %1\n\t"
        :
        : "a" (value), "d" (port)
        );

}

/**************************************************************************//**
 * @brief Writes an 16-bit value to the specified port.
 *
 * This function writes an 16-bit word to a hardware port, through
 * a specified 16-bit address.
 *
 * @param value Value to be written.
 * @param port Port to be written to.
 *              
 ******************************************************************************/
void outw(uint16_t value, unsigned short int port) {

    asm("out %0, %1\n\t"
        :
        : "a" (value), "d" (port)
        );
}

/**************************************************************************//**
 * @brief Reads an 8-bit value from a specified port.
 *
 * This function reads an 8-bit value from a port, specified by its address.
 * 
 * @param port Port to be read from.
 * @return Value read from port.
 *              
 ******************************************************************************/
uint8_t inb(unsigned short int port) {
    uint8_t in_byte;
    asm("inb %1, %0\n\t"
        : "=a" (in_byte)
        : "d" (port)
        );
    return in_byte;
}

/**************************************************************************//**
 * @brief Reads a 16-bit value from a specified port.
 *
 * This function reads a 16-bit word from a port, specified by its address.
 * 
 * @param port Port to be read from.
 * @return Value read from port.
 *              
 ******************************************************************************/
uint16_t inw(unsigned short int port) {
    uint16_t in_word;

    asm("inw %1, %0\n\t"
        : "=a" (in_word)
        : "d" (port)
        );

    return in_word;
}
#include <stdint.h>

#include <kernel/pic.h>
#include <kernel/pio.h>

#define PIC_MASTER_CMD 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_MASTER_x86_IRQ_OFFSET 0x20 // 32 
#define PIC_SLAVE_CMD 0xA0
#define PIC_SLAVE_DATA 0xA1
#define PIC_SLAVE_x86_IRQ_OFFSET 0x28 // 40

// http://www.brokenthorn.com/Resources/OSDevPic.html 
/*
 * Initialization Control Word 1
 * 
 * Bit #    Value   Description
 * 0        IC4     If set(1), the PIC expects to recieve IC4 during init
 * 1        SNGL    If set(1), only one PIC in system. If cleared, PIC is cascaded with slave PICs, and ICW3 must be sent to controller
 * 2        ADI     If set(1), CALL address interval is 4, else 8. Usually ignored by x86, and is default to 0
 * 3        LTIM    If set(1), operate in Level Triggered Mode. Otherwise, operate in Edge Triggered Mode
 * 4        1       Initialization bit. Set to 1 if PIC is to be initialized
 * 5        0       MCS-80/85: Interrupt Vector Address. x86 Architecture: Must be 0
 * 6        0       MCS-80/85: Interrupt Vector Address. x86 Architecture: Must be 0
 * 7        0       MCS-80/85: Interrupt Vector Address. x86 Architecture: Must be 0
 * 
 * 
 * Initialization Control Word 2
 * 
 * Bit #    Value                                       Description
 * 0-2      A8/A9/A10                                   Address bits A8-A10 for IVT when in MCS-80/85 mode.
 * 3-7      A11(T3)/A12(T4)/A13(T5)/A14(T6)/A15(T7)     Address bits A8-A10 for IVT when in MCS-80/85 mode. In 80x86 mode, specifies the interrupt vector address. May be set to 0 in x86 mode.
 * 
 * 
 * Initialization Control Word 3 - Primary PIC
 * 
 * Bit #    Value   Description
 * 0-7      S0-S7   Specifies what Interrupt Request(IRQ) is connected to the slave PIC
 * 
 * 
 * Initialization Control Word 3 - Secondary PIC
 * 
 * Bit #    Value   Description
 * 0-2      ID0     IRQ number the master PIC uses to connect to slave PIC(binary notation).
 * 3-7      0       Reserved, must be 0.
 * 
 * Initialization Control Word 4
 * 
 * Bit #    Value   Description
 * 0        uPM     If set(1), 80x86 mode. Otherwise, MCS-80/85 mode.
 * 1        AEOI    If set(1), on the last interrupt acknowledge pulse, controller automatically performs End Of Interrupt (EOI) operation.
 * 2        M/S     Only use if BUF is set. If set(1), selects buffer master. Cleared if buffer slave.
 * 3        BUF     If set(1), controller operates in buffered mode.
 * 4        SFNM    Special Fully Nested Mode. Used in systems with a large amount of cascaded controllers.
 * 5-7      0       Reserved, must be zero.
*/


// PIC command flags, see above table or go to link or consult a psychic
#define PIC_ICW1_ICW4_PRESENT 0x01
#define PIC_ICW1_NO_CASCADE 0x02
#define PIC_ICW1_CALL_ADDR_INTERVAL 0x04
#define PIC_ICW1_LEVEL_TRIG_MODE 0x08
#define PIC_ICW1_INIT 0x10

#define PIC_ICW3_x86_SLAVE_IRQ_LINE 0x04

#define PIC_ICW4_x86_MODE 0x01
#define PIC_ICW4_AUTO_EOI 0x02
#define PIC_ICW4_BUF_MASTER 0x04
#define PIC_ICW4_BUF_MODE 0x08
#define PIC_ICW4_SFNM_ENABLE 0x10 

#define PIC_OCW2_EOI 0x20

#define PIC_OCW3_READ_IRR 0x0A
#define PIC_OCW3_READ_ISR 0x0B

static uint8_t pic_master_data, pic_slave_data;

/**************************************************************************//**
 * @brief Initializes the Programmable Interrupt Controller(PIC) with default 
 * offset values, through pic_initOffset(uint8_t,uint8_t).
 *              
 ******************************************************************************/
void pic_init() {
    pic_initOffset(PIC_MASTER_x86_IRQ_OFFSET, PIC_SLAVE_x86_IRQ_OFFSET);
}

/**************************************************************************//**
 * @brief Initializes the Programmable Interrupt Controller(PIC).
 * 
 * This function takes in an offset for the Master and Slave PICs (i386),
 * and sets up parameters accordingly.
 * 
 * @param offset1 Address offset for the MASTER PIC interrupts.
 * @param offset1 Address offset for the SLAVE PIC interrupts.
 * 
 ******************************************************************************/
void pic_initOffset(uint8_t offset1, uint8_t offset2) {

    pic_master_data = inb(PIC_MASTER_DATA);
    pic_slave_data = inb(PIC_SLAVE_DATA); 

    // Init both PICs
    outb(PIC_ICW1_INIT | PIC_ICW1_ICW4_PRESENT, PIC_MASTER_CMD);
    outb(PIC_ICW1_INIT | PIC_ICW1_ICW4_PRESENT, PIC_SLAVE_CMD);

    // Set IVT offsets
    outb(offset1, PIC_MASTER_DATA);
    outb(offset2, PIC_SLAVE_DATA);

    // Inform master to slave IRQ line
    outb(PIC_ICW3_x86_SLAVE_IRQ_LINE, PIC_MASTER_DATA);
    outb(PIC_ICW3_x86_SLAVE_IRQ_LINE, PIC_SLAVE_DATA);

    // Set operating modes
    outb(PIC_ICW4_x86_MODE, PIC_MASTER_DATA);
    outb(PIC_ICW4_x86_MODE, PIC_SLAVE_DATA);

    outb(pic_master_data, PIC_MASTER_DATA);
    outb(pic_slave_data, PIC_SLAVE_DATA);
}

/**************************************************************************//**
 * @brief Disables the PIC.
 * 
 ******************************************************************************/
void pic_disable() {

    pic_master_data = inb(PIC_MASTER_DATA);
    pic_slave_data = inb(PIC_SLAVE_DATA); 

    outb(0xFF, PIC_MASTER_DATA);
    outb(0xFF, PIC_SLAVE_DATA);

}

/**************************************************************************//**
 * @brief Enables the PIC.
 * 
 * If the PIC was disabled using pic_disable(), the restored data will be written.
 * 
 ******************************************************************************/
void pic_enable() {
    outb(pic_master_data, PIC_MASTER_DATA);
    outb(pic_slave_data, PIC_SLAVE_DATA);
}

/**************************************************************************//**
 * @brief Signals the PIC to end the interrupt through an EOI signal.
 * 
 * @param irq Interrupt line. Necessary if interrupt is sent from SLAVE PIC.
 * 
 ******************************************************************************/
void pic_sendEndOfInterrupt(uint8_t irq) {
    if (irq >= 8) // x86, cascaded PICs
        outb(PIC_OCW2_EOI, PIC_SLAVE_CMD);
    outb(PIC_OCW2_EOI, PIC_MASTER_CMD);
}

/**************************************************************************//**
 * @brief Sets the interrupt mask for the provided interrupt line.
 * 
 * @param irq Interrupt line. Can be MASTER of SLAVE PIC.
 * 
 ******************************************************************************/
void pic_setInterruptMask(uint8_t irq) {
    uint16_t port;
    uint8_t mask;

    if (irq < 8)
        port = PIC_MASTER_DATA;
    else {
        port = PIC_SLAVE_DATA;
        irq = irq - 8;
    }
    
    mask = inb(port) | (1 << irq);
    outb(mask, port);
        
}

/**************************************************************************//**
 * @brief Sets the interrupt mask for the provided interrupt line.
 * 
 * @param irq Interrupt line. Can be MASTER of SLAVE PIC.
 * 
 ******************************************************************************/
void pic_clearInterruptMask(uint8_t irq) {
    uint16_t port;
    uint8_t mask;

    if (irq < 8)
        port = PIC_MASTER_DATA;
    else {
        port = PIC_SLAVE_DATA;
        irq = irq - 8;
    }
    mask = inb(port) & ~(1 << irq);
    outb(mask, port);
}

/**************************************************************************//**
 * @brief Retrieves the IRR from the PIC (obviously). 
 * 
 * @return Interrupt Request Register(IRR).
 * 
 ******************************************************************************/
uint16_t pic_getIRR() {
    outb(PIC_OCW3_READ_IRR, PIC_MASTER_CMD);
    outb(PIC_OCW3_READ_IRR, PIC_SLAVE_CMD);
    return (inb(PIC_SLAVE_CMD) << 8) | inb(PIC_MASTER_CMD);
}

/**************************************************************************//**
 * @brief Retrieves the ISR from the PIC (presumably). 
 * 
 * @return In Service Register(ISR).
 * 
 ******************************************************************************/
uint16_t pic_getISR() {
    outb(PIC_OCW3_READ_ISR, PIC_MASTER_CMD);
    outb(PIC_OCW3_READ_ISR, PIC_SLAVE_CMD);
    return (inb(PIC_SLAVE_CMD) << 8) | inb(PIC_MASTER_CMD);
}
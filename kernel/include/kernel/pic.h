#ifndef _KERNEL_PIC_H_
#define _KERNEL_PIC_H_

void pic_init();
void pic_initOffset(uint8_t offset1, uint8_t offset2);
void pic_disable();
void pic_enable();
void pic_sendEndOfInterrupt(uint8_t irq);
void pic_setInterruptMask(uint8_t irq);
void pic_clearInterruptMask(uint8_t irq);
uint16_t pic_getIRR();
uint16_t pic_getISR();

#endif // _KERNEL_PIC_H_
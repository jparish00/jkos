#ifndef _KERNEL_GDT_H_
#define _KERNEL_GDT_H_

#define GDT_SEGMENT_KERN_CODE 0x08
#define GDT_SEGMENT_KERN_DATA 0x10
#define GDT_SEGMENT_USER_CODE 0x18
#define GDT_SEGMENT_USER_DATA 0x20

void gdt_add_descriptor(uint8_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void gdt_init();

#endif // _KERNEL_GDT_H_
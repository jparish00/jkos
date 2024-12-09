#include <stdint.h>

#include <kernel/gdt.h>
#include <kernel/tty.h>

#define GDT_MAX_ENTRIES 6 // for now?

// Segment Descriptor Flags
#define GDT_FLAG_LONG_MODE (0x01 << 1)
#define GDT_FLAG_DB_SIZE_16 (0x00  << 2)
#define GDT_FLAG_DB_SIZE_32 (0x01  << 2)
#define GDT_FLAG_GRANULARITY_BYTE (0x00 << 3)
#define GDT_FLAG_GRANULARITY_PAGE (0x01 << 3)

// Segment Access Flags (Lower 4 bytes)
#define GDT_ACC_ACCESSED (0x01 << 0)
#define GDT_ACC_READ_WRITE_ALLOW (0x01 << 1)
#define GDT_ACC_DIRECTION_CONFORM (0x01 << 2)
#define GDT_ACC_EXECUTABLE (0x01 << 3)

// System Segment Access Flags (Lower 4 bytes)
#define GDT_ACC_TYPE_16_TSS_AVAIL 0x01
#define GDT_ACC_TYPE_LDT 0x02
#define GDT_ACC_TYPE_16_TSS_BUSY 0x03
#define GDT_ACC_TYPE_32_TSS_AVAIL 0x09
#define GDT_ACC_TYPE_32_TSS_BUSY 0x0B

// Generic Segment Flags (Upper 4 bytes)
#define GDT_ACC_CODE_DATA_SEG (0x01 << 4)
#define GDT_ACC_DPL_PRIVILEGE_0 (0x00 << 5)
#define GDT_ACC_DPL_PRIVILEGE_1 (0x01 << 5)
#define GDT_ACC_DPL_PRIVILEGE_2 (0x02 << 5)
#define GDT_ACC_DPL_PRIVILEGE_3 (0x03 << 5)
#define GDT_ACC_PRESENT (0x01 << 7)


typedef struct GDTSegmentDescriptor {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access_attr; // Accessed [0], Read/Writable [1], DC(Direct/Conforming) [2], Executable [3], Descriptor Type [4], DPL [5:6], Present [7]
    uint8_t limit_upper_and_flags; // upper limit [0:3], flags [4:7]
    uint8_t base_upper;
} GDTDesc __attribute__((packed));

typedef struct GDTDescriptor {
    uint16_t limit;
    uint32_t base_addr;
} GDTPtr __attribute__((packed));

#define GDT_SIZEOF_DESC_BYTES sizeof(GDTDesc)

static GDTDesc gdt_gdt[GDT_MAX_ENTRIES]; 
static GDTPtr gdt_desc_ptr;
static uint8_t gdt_entry_count;

/**************************************************************************//**
 * @brief Adds a descriptor entry to the Global Descriptor Table(GDT).
 * 
 * @param entry Entry in GDT to modify. Avoid values 0-5.
 * @param base Address to beginning of segment.
 * @param limit Maximum addressable unit from base address. 20 bits max.
 * @param access Access byte indicating access parameters.
 * @param flags 4-bit section indicating granularity and size.
 *              
 ******************************************************************************/
void gdt_add_descriptor(uint8_t entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {

    gdt_gdt[entry].base_low = base & 0xFFFF;
    gdt_gdt[entry].base_mid = (base & (0xFF << 16)) >> 16;
    gdt_gdt[entry].base_upper = (base & (0xFF << 24)) >> 24;
    gdt_gdt[entry].limit_low = limit & 0xFFFF;
    gdt_gdt[entry].limit_upper_and_flags = (limit & (0xF << 16)) >> 16;
    gdt_gdt[entry].limit_upper_and_flags = gdt_gdt[entry].limit_upper_and_flags | flags;
    gdt_gdt[entry].access_attr = access;
    gdt_entry_count++;
}

/**************************************************************************//**
 * @brief Adds default descriptor entries to the Global Descriptor Table(GDT).
 * 
 * Adds the following entries:
 * 
 * Null Segment
 * 
 * Kernel Mode Code Segment
 * 
 * Kernel Mode Data Segment
 * 
 * User Mode Code Segment
 * 
 * User Mode Data Segment
 *              
 ******************************************************************************/
void gdt_init_descriptors() {

    // Null
    gdt_add_descriptor(0, 0, 0, 0, 0);
    // Kernel Mode Code Segment
    gdt_add_descriptor(1, 0, 0XFFFFF, GDT_ACC_PRESENT|GDT_ACC_DPL_PRIVILEGE_0|GDT_ACC_CODE_DATA_SEG|GDT_ACC_EXECUTABLE|GDT_ACC_READ_WRITE_ALLOW, 
        GDT_FLAG_DB_SIZE_32|GDT_FLAG_GRANULARITY_PAGE);
    // Kernel Mode Data Segment
    gdt_add_descriptor(2, 0, 0XFFFFF, GDT_ACC_PRESENT|GDT_ACC_DPL_PRIVILEGE_0|GDT_ACC_CODE_DATA_SEG|GDT_ACC_READ_WRITE_ALLOW,
        GDT_FLAG_DB_SIZE_32|GDT_FLAG_GRANULARITY_PAGE);
    // User Mode Code Segment
    gdt_add_descriptor(3, 0, 0XFFFFF, GDT_ACC_PRESENT|GDT_ACC_DPL_PRIVILEGE_3|GDT_ACC_CODE_DATA_SEG|GDT_ACC_EXECUTABLE|GDT_ACC_READ_WRITE_ALLOW, 
        GDT_FLAG_DB_SIZE_32|GDT_FLAG_GRANULARITY_PAGE);
    // User Mode Data Segment
    gdt_add_descriptor(4, 0, 0XFFFFF, GDT_ACC_PRESENT|GDT_ACC_DPL_PRIVILEGE_3|GDT_ACC_CODE_DATA_SEG|GDT_ACC_READ_WRITE_ALLOW,
        GDT_FLAG_DB_SIZE_32|GDT_FLAG_GRANULARITY_PAGE);

}

/**************************************************************************//**
 * @brief Initializes the Global Descriptor Table(GDT).
 * 
 * Initializes the GDT, and adds the default descriptor entries via
 * gdt_init_descriptors().
 *              
 ******************************************************************************/
void gdt_init() {

    gdt_entry_count = 0;

    asm("CLI    \n\t"
        :
        :
        );

    gdt_desc_ptr.limit = (GDT_MAX_ENTRIES * GDT_SIZEOF_DESC_BYTES) - 1; 
    gdt_desc_ptr.base_addr = (uint32_t) &gdt_gdt;

    gdt_init_descriptors();

    asm("LGDT %0\n\t"
        "STI   \n\t"
        :
        : "m" (gdt_desc_ptr)
        );

    term_writestring("\nGDT initialized.");

}

// TODO: Add TSS Descriptor

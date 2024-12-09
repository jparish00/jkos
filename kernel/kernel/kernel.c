#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/pio.h>
#include <kernel/gdt.h>
#include <kernel/pic.h>

void kernel_main(void) {
	term_init();
	term_enablecursordefault();
    printf("Hello, kernel World!\nThis is the second line!\nThis is the third line!\nThis is the fourth line!");
    printf("\nHello, kernel World!\nThis is the second line!\nThis is the third line!\nThis is the fourth line!");
	printf("\nHello, kernel World!\nThis is the second line!\nThis is the third line!\nThis is the fourth line!");
	printf("\nHello, kernel World!\nThis is the second line!\nThis is the third line!\nThis is the fourth line!");
	printf("\nHello, kernel World!\nThis is the second line!\nThis is the third line!\nThis is the fourth line!");
    printf("\nHello, kernel World!\nThis is the second line!\nThis is the third line!\nThis is the fourth line!");
    printf("\nHello, kernel World!\nThis is the second line!\nThis is the third line!\nThis is the fourth line!");
    // term_setbgcolor(2);
    // term_setfgcolor(4);
	printf("\nHello, kernel World!\nThis is the second line!\nThis is the third line!\nThis is the fourth line!");
	printf("\nHello, kernel World!\nThis is the second line!\nThis is the third line!\nThis is the fourth line!");
	printf("\nHello, kernel World!\nThis is the second line!\nThis is the third line!\nThis is the fourth line!");
	gdt_init();
	pic_init();


}

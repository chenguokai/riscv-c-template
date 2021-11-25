#include "load_store.h"
void halt(int code) {
  __asm__ volatile("mv a0, %0; .word 0x0005006b" : :"r"(code));
}

#define REG_TX		0x04
#define REG_RX		0x00
#define REG_STATUS	0x08
#define REG_CONTROL	0x0C

#define UART_TX_FULL    (1<<0x3)
#define UART_RX_VALID   (1<<0x0)
#define UART_BASE       0x40600000

void uart_putc(char ch) {
    volatile char *uart_base = (char *)(UART_BASE);
    while((*(char*)(uart_base + REG_STATUS) & UART_TX_FULL));
    *(char *)(uart_base + REG_TX) = ch;
}

void puts(char *s) {
    int i = 0;
    while (s[i]) {
    	uart_putc(s[i++]);
    }
}

int main() {
    int count = 0;
    int (*load_store_tests[])() = {vle8_test, vlse8_test, vlxei8_test, vle16_test, vlse16_test, vlxei16_test, vle32_test, vlse32_test, vlxei32_test, vle64_test, vlse64_test, vlxei64_test};
    char *load_store_names[] = {"vle8", "vlse8", "vlxei8", "vle16", "vlse16", "vlxei16", "vle32", "vlse32", "vlxei32", "vle64", "vlse64", "vlxei64"};
    int load_store_size = sizeof(load_store_size) / sizeof(void *);

    // we did not clear bss, careful with non-initialized data
    puts("RVV test begin\n");
    for (int i = 0; i < load_store_size; ++i) {
    	int ret = load_store_tests[i]();
	puts(load_store_names);
	if (ret == 0) {
	    puts(" test passed\n");
	    ++count;
	} else {
	    puts(" test failed\n");
	}
    }
    puts("Test finish, spining\n");
    while (1);
}

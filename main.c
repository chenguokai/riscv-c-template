#ifndef NATIVE
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

void uart_puts(char *s) {
    int i = 0;
    while (s[i]) {
    	uart_putc(s[i++]);
    }
}

void uart_putn(int n) {
    char buf[10], buf_r[10];
    int offset = 0;
    do {
        buf[offset++] = n % 10 + '0';
        n = n / 10;
    } while (n);
    for (int i = 0; i < offset; ++i) buf_r[offset-i-1] = buf[i];
    buf_r[offset] = 0;
    uart_puts(buf_r);
}

void data_init() {
    char *data = (char *)(0x90000000UL);
    for (int i = 0; i < 1024 / 8; ++i) {
        data[i] = i;
    }
}


void halt(int code) {
  __asm__ volatile("mv a0, %0; .word 0x0005006b" : :"r"(code));
}

void memcpy(char* dest, const char* source, long unsigned int num) {
    for (int i = 0; i < num; ++i) dest[i] = source[i];
}
#else // NATIVE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int non_main();

int main() {
    return non_main();
}
void halt(int code) {
    exit(code);
}

void uart_putc(char ch) {putchar(ch);}
void uart_puts(char *s) {printf("%s", s);}
#endif // NATIVE
volatile int aa;
void a() {
    ++aa;
}

int non_main() {
    for (int j = 0; j < 100; j++) {
    	a();
    }
    halt(0);
    while (1);
}

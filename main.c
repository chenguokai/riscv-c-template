#define VLEN 256

void data_init() {
    char *data = (char *)(0x90000000UL);
    for (int i = 0; i < VLEN / 8; ++i) {
        data[i] = i;
    }
}

int vsetvl_test() {
    // just execute
    asm volatile(
       "li t1, 12         \n\t\
        vsetvl t1, t1, t1 \n\t\
    "
    :
    :
    : "t1");
    return 0;
}

int vsetvli_test() {
    // just execute
    asm volatile(
       "li t1, 12        \n\t\
        vsetvli t1, t1, e8, m2, tu, mu \n\t\
    "
    :
    :
    : "t1");
    return 0;
}

int vle8_test() {
    data_init();
    // load from 0x90000000
    // store to 0x90000100
    asm volatile(
       "li t1, 0x90000000              \n\t\
        li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        vle8.v v0, (t1)                \n\t\
        addi t1, t1, 0x100            \n\t\
        vse8.v v0, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != i) return 1;
    }
    return 0;
}

int vlse8_test() {
    data_init();
    // load from 0x90000000
    // store to 0x90000100
    asm volatile(
       "li t1, 0x90000000              \n\t\
        li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t0, 1                       \n\t\
        vlse8.v v0, (t1), t0           \n\t\
        addi t1, t1, 0x100             \n\t\
        vse8.v v0, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != i) return 1;
    }
    return 0;
}

int vlxei8_test() {
    return 0;
}

int vle16_test() {
    return 0;
}

int vlse16_test() {
    return 0;
}

int vlxei16_test() {
    return 0;
}

int vle32_test() {
    return 0;
}

int vlse32_test() {
    return 0;
}

int vlxei32_test() {
    return 0;
}

int vle64_test() {
    return 0;
}

int vlse64_test() {
    return 0;
}

int vlxei64_test() {
    return 0;
}




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

void uart_puts(char *s) {
    int i = 0;
    while (s[i]) {
    	uart_putc(s[i++]);
    }
}

void memcpy(char* dest, char* source, int num) {
    for (int i = 0; i < num; ++i) dest[i] = source[i];
}

int non_main() {
    int count = 0;
    int (*vsetvl_tests[])() = {vsetvl_test, vsetvli_test};
    char *vsetvl_names[] = {"vsetvl", "vsetvli"};
    int vsetvl_size = 2;

    int (*load_store_tests[])() = {vle8_test, vlse8_test, vlxei8_test, vle16_test, vlse16_test, vlxei16_test, vle32_test, vlse32_test, vlxei32_test, vle64_test, vlse64_test, vlxei64_test};
    char *load_store_names[] = {"vle8", "vlse8", "vlxei8", "vle16", "vlse16", "vlxei16", "vle32", "vlse32", "vlxei32", "vle64", "vlse64", "vlxei64"};
    int load_store_size = 12;

    uart_puts("RVV vsetvl test begin\n");
    uart_puts("---------------------\n");
    for (int i = 0; i < vsetvl_size; ++i) {
        int ret = vsetvl_tests[i]();
        uart_puts(vsetvl_names[i]);
        if (ret == 0) {
            uart_puts(" test passed\n");
        } else {
            uart_puts(" test failed\n");
        }
    }
    uart_puts("RVV vsetvl test end\n");
    uart_puts("-------------------\n");
    uart_puts("\n\n");
    // we did not clear bss, careful with non-initialized data
    uart_puts("RVV load & store test begin\n");
    uart_puts("---------------------------\n");
    for (int i = 0; i < load_store_size; ++i) {
    	int ret = load_store_tests[i]();
	    uart_puts(load_store_names[i]);
	    if (ret == 0) {
	        uart_puts(" test passed\n");
	        ++count;
	    } else {
	        uart_puts(" test failed\n");
	    }
    }
    uart_puts("RVV load & store test end\n");
    uart_puts("-------------------------\n");
    uart_puts("\n\n");

    uart_puts("Test finish, spining\n");
    while (1);
}

#define VLEN 256
#define REG_TX		0x04
#define REG_RX		0x00
#define REG_STATUS	0x08
#define REG_CONTROL	0x0C

#define UART_TX_FULL    (1<<0x3)
#define UART_RX_VALID   (1<<0x0)
#define UART_BASE       0xe0000000

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
    data_init();
    // load from 0x90000000
    // store to 0x90000100
    asm volatile(
       "li t1, 0x90000000              \n\t\
        li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t0, 1                       \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vxor.vi v0, v0, 1              \n\t\
        vlxei8.v v1, (t1), v0          \n\t\
        addi t1, t1, 0x100             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 1) return 1;
    }
    return 0;
}

int vaddvv_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 1              \n\t\
        vadd.vv v1, v1, v1             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 2) return 1;
    }
    return 0;
}

int vaddvx_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 1              \n\t\
        li t0, 3                       \n\t\
        vadd.vx v1, v1, t0             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 4) return 1;
    }
    return 0;
}

int vaddvi_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 1              \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 1) return 1;
    }
    return 0;
}

int vandvv_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        vadd.vi v2, v0, 7              \n\t\
        vand.vv v1, v1, v2             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 4) return 1;
    }
    return 0;
}

int vandvx_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        li t0, 7                       \n\t\
        vand.vx v1, v1, t0             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 4) return 1;
    }
    return 0;
}

int vandvi_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        vand.vi v1, v1, 7              \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 4) return 1;
    }
    return 0;
}

int vorvv_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        vadd.vi v2, v0, 7              \n\t\
        vor.vv v1, v1, v2             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 7) return 1;
    }
    return 0;
}

int vorvx_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        li t0, 7                       \n\t\
        vor.vx v1, v1, t0              \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 7) return 1;
    }
    return 0;
}

int vorvi_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        vor.vi v1, v1, 7               \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 7) return 1;
    }
    return 0;
}

int vrsubvx_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        li t0, 7                       \n\t\
        vrsub.vx v1, v1, t0            \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 3) return 1;
    }
    return 0;
}

int vrsubvi_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        vrsub.vi v1, v1, 7             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 3) return 1;
    }
    return 0;
}

int vsllvv_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        vadd.vi v2, v0, 1              \n\t\
        vsll.vv v1, v1, v2             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 8) return 1;
    }
    return 0;
}

int vsllvx_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        li t0, 1                       \n\t\
        vsll.vx v1, v1, t0             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 8) return 1;
    }
    return 0;
}

int vsllvi_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        vsll.vi v1, v1, 1              \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 8) return 1;
    }
    return 0;
}

int vsravv_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        vadd.vi v2, v0, 1              \n\t\
        vsra.vv v1, v1, v2             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 2) return 1;
    }
    return 0;
}

int vsravx_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        li t0, 1                       \n\t\
        vsra.vx v1, v1, t0             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 2) return 1;
    }
    return 0;
}

int vsravi_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        vsra.vi v1, v1, 1              \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 2) return 1;
    }
    return 0;
}

int vsrlvv_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        vadd.vi v2, v0, 1              \n\t\
        vsrl.vv v1, v1, v2             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 2) return 1;
    }
    return 0;
}

int vsrlvx_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        li t0, 1                       \n\t\
        vsrl.vx v1, v1, t0             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 2) return 1;
    }
    return 0;
}

int vsrlvi_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        vsrl.vi v1, v1, 1              \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 2) return 1;
    }
    return 0;
}

int vsubvv_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        vadd.vi v2, v0, 3              \n\t\
        vsub.vv v1, v1, v2             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 1) return 1;
    }
    return 0;
}

int vsubvx_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        li t0, 3                       \n\t\
        vsub.vx v1, v1, t0             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 1) return 1;
    }
    return 0;
}

int vsubvi_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        li t0, 3                       \n\t\
        vsub.vx v1, v1, t0             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 1) return 1;
    }
    return 0;
}

int vxorvv_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        li t0, 3                       \n\t\
        vsub.vx v1, v1, t0             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 1) return 1;
    }
    return 0;
}

int vxorvx_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        li t0, 4                       \n\t\
        vxor.vx v1, v1, t0             \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 0) return 1;
    }
    return 0;
}

int vxorvi_test() {
    data_init();
    asm volatile(
       "li t0, 16                      \n\t\
        vsetvli t0, t0, e8, m1, tu, mu \n\t\
        li t1, 0x90000100              \n\t\
        vxor.vv v0, v0, v0             \n\t\
        vadd.vi v1, v0, 4              \n\t\
        vxor.vi v1, v1, 4              \n\t\
        vse8.v v1, (t1)                \n\t\
    "
    :
    :
    : "t0", "t1"
    );
    char *verify = (char *)(0x90000100UL);
    for (int i = 0; i <16; ++i) {
        if (verify[i] != 0) return 1;
    }
    return 0;
}

void halt(int code) {
  __asm__ volatile("mv a0, %0; .word 0x0005006b" : :"r"(code));
}





void memcpy(char* dest, char* source, int num) {
    for (int i = 0; i < num; ++i) dest[i] = source[i];
}

int non_main() {
    int count = 0;
    int (*vsetvl_tests[])() = {vsetvl_test, vsetvli_test};
    char *vsetvl_names[] ={"vsetvl", "vsetvli"};
    int vsetvl_size = 2;

    int (*load_store_tests[])() = {vle8_test, vlse8_test, vlxei8_test};
    char *load_store_names[] = {"vle8", "vlse8", "vlxei8"};
    int load_store_size = 3;

    int (*calc_tests[])() = {vaddvv_test, vaddvx_test, vaddvi_test, vandvv_test, vandvx_test, vandvi_test,
                             vorvv_test, vorvx_test, vorvi_test, vrsubvx_test, vrsubvi_test, 
                             vsllvv_test, vsllvx_test, vsllvi_test, vsravv_test, vsravx_test, vsravi_test, 
                             vsrlvv_test, vsrlvx_test, vsrlvi_test, vsubvv_test, vsubvx_test, vsubvi_test, 
                             vxorvv_test, vxorvx_test, vxorvi_test
    };
    char *calc_names[] = {"vaddvv", "vaddvx", "vaddvi", "vandvv", "vandvx", "vandvi", 
                          "vorvv", "vorvx", "vorvi", "vrsubvx", "vrsubvi", 
                          "vsllvv", "vsllvx", "vsllvi", "vsravv", "vsravx", "vsravi", 
                          "vsrlvv", "vsrlvx", "vsrlvi", "vsrlvv", "vsubvv", "vsubvx", "vsubvi",
                          "vxorvv", "vxorvx", "vxorvi"
    };
    int calc_size = 26;

    uart_puts("RVV vsetvl test begin\n");
    uart_puts("---------------------\n");
    for (int i = 0; i < vsetvl_size; ++i) {
        int ret = vsetvl_tests[i]();
        uart_puts(vsetvl_names[i]);
        if (ret == 0) {
            uart_puts(" test passed\n");
            ++count;
        } else {
            uart_puts(" test failed\n");
        }
    }
    uart_putn(count);
    uart_puts(" test point passed\n");
    uart_puts("RVV vsetvl test end\n");
    uart_puts("-------------------\n");
    uart_puts("\n\n");
    // we did not clear bss, careful with non-initialized data
    count = 0;
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
    uart_putn(count);
    uart_puts(" test point passed\n");
    uart_puts("RVV load & store test end\n");
    uart_puts("-------------------------\n");
    uart_puts("\n\n");

    count = 0;
    uart_puts("RVV arithmetic test begin\n");
    uart_puts("---------------------------\n");
    for (int i = 0; i < calc_size; ++i) {
    	int ret = calc_tests[i]();
	    uart_puts(calc_names[i]);
	    if (ret == 0) {
	        uart_puts(" test passed\n");
	        ++count;
	    } else {
	        uart_puts(" test failed\n");
	    }
    }
    uart_putn(count);
    uart_puts(" test point passed\n");
    uart_puts("RVV arithmetic test end\n");
    uart_puts("-------------------------\n");
    uart_puts("\n\n");

    uart_puts("Test finish, spining\n");
    while (1);
}

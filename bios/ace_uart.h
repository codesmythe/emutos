#ifndef ACE_UART_H
#define ACE_UART_H

struct ACE_UART {
    /* 1 */    UBYTE rbr_thr_divlsb; // UART RBR (read), THR (write) and DIVLSB registers
    UBYTE dummy02;
    /* 3 */    UBYTE ier_divmsb;     // UART IER (read/write) and DIVMSB registers
    UBYTE dummy04;
    /* 5 */    UBYTE fifo_iir;       // UART Interrupt Identification Register (read) or FIFO CR (write).
    UBYTE dummy06;
    /* 7 */    UBYTE lcr;            // UART Line Control Register
    UBYTE dummy08;
    /* 9 */    UBYTE mcr;            // UART Modem Control Register
    UBYTE dummy10;
    /*11 */    UBYTE lsr;            // UART Line Status Register
    UBYTE dummy12;
    /*13 */    UBYTE msr;            // UART Modem Status Register
    UBYTE dummy14;
    /*15 */    UBYTE scr;            // UART Scratch Register
};

#define ACE_LSR_DR 0x1
#define ACE_LSR_THRE 0x20

#define ikbd_ace (*(volatile struct ACE_UART *) ACE_UART_BASE)

#endif //ACE_UART_H

#ifndef SK_H
#define SK_H

// for detect_UART

#define UART_8250   1
#define UART_16450  2
#define UART_16550  3
#define UART_16550A 4

// registers in UART

#define RBF   0   // the read buffer
#define THR   0   // the write buffer
#define IER   1   // the int. enable register
#define IIR   2   // the int. identification register
#define FCR   2   // FIFO control register
#define LCR   3   // control data config. and divisor latch
#define MCR   4   // modem control reg.
#define LSR   5   // line status reg.
#define MSR   6   // modem status of cts, ring etc.
#define SCR   7   // the scratch register
#define DLL   0   // the low byte of baud rate divisor
#define DLH   1   // the hi byte of divisor latch

// bit patterns for control registers

#define BAUD_1200       96      // baud rate divisors
#define BAUD_2400       48
#define BAUD_9600       12
#define BAUD_19200      6
#define BAUD_28400      3
#define BAUD_57600      2
#define BAUD_115200     1

#define COM1            0x3F8   // base port address of port 0
#define COM2            0x2F8   // base port address of port 1
#define COM3            0x3E8   // base port address of port 2
#define COM4            0x2E8   // base port address of port 3

#define STOP_1          0       // 1 stop bit per character
#define STOP_2          4       // 2 stop bits per character

#define BITS_5          0       // send 5 bit characters
#define BITS_6          1       // send 6 bit characters
#define BITS_7          2       // send 7 bit characters
#define BITS_8          3       // send 8 bit characters

#define PARITY_NONE     0       // no parity
#define PARITY_ODD      8       // odd parity
#define PARITY_EVEN     24      // even parity

#define DTR             0x01    // for skHand()
#define RTS             0x02
#define USER            0x04
#define LOOPBACK        0x10

#define GP02          8       // enable interrupt

#define DIV_LATCH_ON  128     // used to turn reg 0,1 into divisor latch

#define PIC_IMR       0x21    // pic's interrupt ma reg.
#define PIC_ICR       0x20    // pic's interupt control reg.

#define INT_SER_PORT_0  0x0C  // port 0 interrupt com 1 & 3
#define INT_SER_PORT_1  0x0B  // port 0 interrupt com 2 & 4


/* the important stuff */

extern char sk_desc[];

int     skReady     ();
int     skRecv      ();
void    skRead      (unsigned char *, int);
void    skPutback   ();
void    skClear     ();

int     skWaiting   ();
void    skSend      (unsigned char);
void    skSendString(unsigned char *);
void    skWrite     (unsigned char *, int);
void    skFlush     ();
void    skHand      (unsigned int);

int     detect_UART (unsigned);
int     skOpen      (int, int, int);
int     skEnableFIFO();
void    skClose     ();

#endif
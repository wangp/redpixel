#ifndef SK_H
#define SK_H

// registers in UART

#define skRBF   0   // the read buffer
#define skTHR   0   // the write buffer
#define skIER   1   // the int. enable register
#define skIIR   2   // the int. identification register
#define skLCR   3   // control data config. and divisor latch
#define skMCR   4   // modem control reg.
#define skLSR   5   // line status reg.
#define skMSR   6   // modem status of cts, ring etc.
#define skDLL   0   // the low byte of baud rate divisor
#define skDLH   1   // the hi byte of divisor latch

// bit patterns for control registers

#define BAUD_1200       96      // baud rate divisors for 1200 baud -> 19200
#define BAUD_2400       48
#define BAUD_9600       12
#define BAUD_19200      6

#define skGP02          8       // enable interrupt

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

#define skDIV_LATCH_ON  128     // used to turn reg 0,1 into divisor latch

#define skPIC_IMR       0x21    // pic's interrupt mask reg.
#define skPIC_ICR       0x20    // pic's interupt control reg.

#define skINT_SER_PORT_0  0x0C  // port 0 interrupt com 1 & 3
#define skINT_SER_PORT_1  0x0B  // port 0 interrupt com 2 & 4

#define skBUFFER_SIZE   128


int     skReady     ();
int     skRecv      ();
void    skSend      (unsigned char);
void    skClear     ();

void    skOpen      (int, int, int);
void    skClose     ();

#endif
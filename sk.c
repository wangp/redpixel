/*  SK - Serial Communications
 *      by Peter Wang  (tjaden@alphalink.com.au -or- tjaden@psynet.net)
 *
 *  These routines were originally basically just a port of Andre' LaMothes
 *  routines from 'Tricks of the Game Programming Gurus'. 
 *
 *  Extra thanks:
 *
 *  Chris Blums for 'The Serial Port'.
 *  Dim Zegebart for DZComm, from which I copy and pasted a bit.
 *  Sam Vincent, from which I stole a routine.
 *  Shawn Hargreaves, for I ripped out a few macros from Allegro.
 */

/*  Started:    14 March 1998
 *  Modified:   15 March 1998   0.51á   skClear() more efficient
 *                                      added #define DEBUGME
 *  Modified:   13 June 1998    0.51c   skPutback()
 *                                      removed DEBUGME
 *  Modified:   14 June 1998    0.51d   skSendString() skWrite()
 *                                      open_port = 0 in Close()
 *                                      boosted BUFFER_SIZE to 2K
 *  Modified:   15 June 1998    x.xx    FIFO used if possible [unfinished]
 *                                      started send buffer
 *  Modified:   16 June 1998    0.7     FIFO works! (i think) + better ISR
 *                                      send buffer finished
 *  Modified:   17 June 1998    0.7a    fixed small bug in detect_UART()
 *  Modified:   18 June 1998    0.7á    fixed the fix in detect_UART()
 *  Modified:   14 July 1998    0.7c    added skHand()
 *  Modified:   25 July 1998    0.7d    added skRead()
 */

#include <dpmi.h>
#include <go32.h>
#include <dos.h>
#include <pc.h>
#include <sys/segments.h>
#include "sk.h"



char sk_desc[] = "SK 0.7d by Peter Wang, July 1998.";



/*  These macros are taken from Allegro 3.0 (allegro.h)
 *  They are only defined if not already defined.
 */
#ifndef ALLEGRO_H
#define END_OF_FUNCTION(x)    void x##_end() { }
#define LOCK_VARIABLE(x)      _go32_dpmi_lock_data((void *)&x, sizeof(x))
#define LOCK_FUNCTION(x)      _go32_dpmi_lock_code(x, (long)x##_end - (long)x)
#endif



/*  too lazy */
#define DISABLE()   asm("cli")
#define ENABLE()    asm("sti")



/* a heap of globals */

#define BUFFER_SIZE     2048    // resize if required

static volatile unsigned char recv_buf[BUFFER_SIZE];    // the receive buffer
static volatile int recv_head, recv_tail;               //  and indexes

static volatile unsigned char send_buf[BUFFER_SIZE];    // the send buffer
static volatile int send_head, send_tail;               //  and indexes

static _go32_dpmi_seginfo old_vector;
static _go32_dpmi_seginfo new_vector;

static int old_int_mask;        // the old interrupt mask on the PIC

static int open_port = 0;       // the currently open port
static int fifo_enabled = 0;    // whether 16550A FIFO is enabled

static int virgin = 1;



/*  These are pinched from DZComm, renamed of course. :)
 */
#define THREINT 0x02
#define RDAINT  0x04

static inline void enable_interrupt(unsigned char i)
{
    unsigned char ch = inportb(open_port + IER);
    if (!(ch & i)) outportb(open_port + IER, ch | i);
}

END_OF_FUNCTION(enable_interrupt)

static inline void disable_interrupt(unsigned char i)
{
    unsigned char ch = inportb(open_port + IER);
    if (ch & i) outportb(open_port + IER, ch & ~i);
}

END_OF_FUNCTION(disable_interrupt);



/*  This is the ISR for the COM port.  It is very simple.  When it gets 
 *  called, it gets the next character out of the recieve buffer register 0 
 *  and places it into the software buffer.
 */
void skISR()
{
    int cause, chars;

    // loop till all interrupts handled
    for (;;)
    {
	cause = inportb(open_port + IIR) & 0x07;    // only use lower 3 bits

	if ((cause & 0x01)==1)  // no interrupt
	{
	    // re-enable interrupts
	    outportb(PIC_ICR, 0x20);
	    return;
	}

	switch (cause)
	{
	    case 0x06:  // read the LSR and discard
		inportb(open_port + LSR);
		break;

	    case 0x04:  // receive character
		// wrap buffer index around
		if (++recv_head == BUFFER_SIZE)
		    recv_head = 0;

		// move character into recv_buf
		recv_buf[recv_head] = inportb(open_port + RBF);
		break;

	    case 0x02:  // send chars in queue
		if (send_head == send_tail) 
		{
		    // nothing in queue, disable THRE interrupt
		    disable_interrupt(THREINT);
		}
		else
		{
		    if (fifo_enabled) 
			chars = 16;
		    else 
			chars = 1;

		    while (send_head != send_tail && chars)
		    {
			outportb(open_port + THR, send_buf[send_tail]);
			if (++send_tail == BUFFER_SIZE)
			    send_tail = 0;
			chars--;
		    }
		}
		break;

	    case 0x00:  // read MSR and discard
		inportb(open_port + MSR);
		break;
	}
    }
}

END_OF_FUNCTION(skISR)



/*  This functions returns the number of characters waiting 
 *  in the receive buffer.
 */
int skReady()
{
    if (recv_head >= recv_tail)
	return recv_head - recv_tail;
    else
	return recv_head - recv_tail + BUFFER_SIZE;
}


/*  This function reads a character from the receive buffer 
 *  and returns it to the caller.
 */
int skRecv()
{ 
    int ch;

    if (recv_tail == recv_head)
	return 0;

    DISABLE();

    // wrap buffer index if needed 
    if (++recv_tail == BUFFER_SIZE)
       recv_tail = 0;

    // get the character out of buffer
    ch = recv_buf[recv_tail];

    ENABLE();

    // send data back to caller
    return ch;
}


/*  This function reads num chars and sticks them in buf.
 *  Make sure you have enough characters ready before calling this.
 */
void skRead(unsigned char *dest, int num)
{
    int i = 0;

    DISABLE();

    while (num--)
    { 
	// wrap buffer index if needed 
	if (++recv_tail == BUFFER_SIZE)
	   recv_tail = 0;

	// get the character out of buffer
	dest[i++] = recv_buf[recv_tail];
    }

    ENABLE();
}


/*  This function puts the last retrieved character back into the buffer.
 */
void skPutback()
{
    DISABLE();

    if (--recv_tail < 0)
	recv_tail = BUFFER_SIZE - 1;

    ENABLE();
}


/*  This function simplys clears the receive buffer.
 */
void skClear()
{
    DISABLE();

    recv_tail = recv_head = 0;

    ENABLE();
}



/*  This functions returns the number of characters waiting 
 *  in the send buffer.
 */
int skWaiting()
{
    if (send_head >= send_tail)
	return send_head - send_tail;
    else
	return send_head - send_tail + BUFFER_SIZE;
}


/*  This function puts a character into the send buffer. 
 */
void skSend(unsigned char ch)
{
    DISABLE();

    send_buf[send_head] = ch;

    if (++send_head == BUFFER_SIZE)
	send_head = 0;

    // enable THRE int
    enable_interrupt(THREINT);

    ENABLE();
}


/*  Send a NULL terminated string.
 */
void skSendString(unsigned char *str)
{
    DISABLE();

    // while not terminated
    while (*str)
    {
	send_buf[send_head] = *str++;
	if (++send_head == BUFFER_SIZE)
	    send_head = 0;
    }

    // enable THRE int
    enable_interrupt(THREINT);

    ENABLE();
}


/*  Sends len bytes.
 */
void skWrite(unsigned char *str, int len)
{
    DISABLE();

    // send len bytes
    while (len--)
    {
	send_buf[send_head] = *str++;
	if (++send_head == BUFFER_SIZE)
	    send_head = 0;
    }

    // enable THRE int
    enable_interrupt(THREINT);

    ENABLE();
}


/*  This function writes all characters waiting to be sent into the
 *  transmit buffer.
 */
void skFlush()
{
    DISABLE();

    while (send_head != send_tail)
    {
	// wait for transmit buffer to be empty 
	while (!(inportb(open_port + LSR) & 0x20));

	// send the character
	outportb(open_port + THR, send_buf[send_tail]);

	// next character please
	if (++send_tail == BUFFER_SIZE)
	    send_tail = 0;
    }

    ENABLE();
}


/*  Sets various handshaking lines.
 *  Ripped from SVAsync
 */
void skHand(unsigned int hand)
{
    outportb(open_port + MCR, hand | 0x08); /* Keep interrupt enable ON */
}


/*  Ripped from ser_port.txt by Chris Blum.
 */
int detect_UART(unsigned baseaddr)
{
    // this function returns 0 if no UART is installed.
    // 1: 8250, 2: 16450 or 8250 with scratch reg., 3: 16550, 4: 16550A
    int x, olddata;

    // check if a UART is present anyway
    // if port is already opened, then we know it exists anyway...
    if (open_port != baseaddr)
    {
	olddata = inportb(baseaddr + MCR);

	// for some reason, this bit doesn't like the ISR being installed
	// so we skip all of this
	outportb(baseaddr + MCR, 0x10);
	if ((inportb(baseaddr + MSR) & 0xf0)) return 0;

	outportb(baseaddr + MCR, 0x1f);
	if ((inportb(baseaddr + MSR) & 0xf0) != 0xf0) return 0;
	outportb(baseaddr + MCR, olddata);
    }

    // next thing to do is look for the scratch register
    olddata = inportb(baseaddr + SCR);
    outportb(baseaddr + SCR, 0x55);
    if (inportb(baseaddr + SCR) != 0x55) return UART_8250;
    outportb(baseaddr + SCR, 0xAA);
    if (inportb(baseaddr + SCR) != 0xAA) return UART_8250;
    outportb(baseaddr + SCR, olddata); // we don't need to restore it if it's not there

    // then check if there's a FIFO
    outportb(baseaddr + FCR, 1);
    x = inportb(baseaddr + IIR);
    // some old-fashioned software relies on this!
    outportb(baseaddr + IIR, 0x0);
    if ((x & 0x80)==0) return UART_16450;
    if ((x & 0x40)==0) return UART_16550;
    return UART_16550A;
}


/*  This function will open up the serial port, set it's configuration, turn
 *  on all the little flags and bits to make interrupts happen and load the
 *  ISR.
 */
int skOpen(int port_base, int baud, int config)
{
    // lock down ISR and variables?
    if (virgin)
    {
	LOCK_VARIABLE(recv_buf);
	LOCK_VARIABLE(recv_head);
	LOCK_VARIABLE(recv_tail);
	LOCK_VARIABLE(send_buf);
	LOCK_VARIABLE(send_head);
	LOCK_VARIABLE(send_tail);
	LOCK_VARIABLE(open_port);
	LOCK_VARIABLE(fifo_enabled);
	LOCK_FUNCTION(skISR);
	LOCK_FUNCTION(enable_interrupt);
	LOCK_FUNCTION(disable_interrupt);
	virgin = 0; 
    }

    if (!detect_UART(port_base))
	return 0;

    // clear buffers
    recv_head = recv_tail = 0;
    send_head = send_tail = 0;

    // save the port for other functions
    open_port = port_base;

    // first set the baud rate

    // turn on divisor latch registers
    outportb(port_base + LCR, DIV_LATCH_ON);

    // send low and high bytes to divisor latches
    outportb(port_base + DLL, baud);
    outportb(port_base + DLH, 0);

    // set the configuration for the port
    outportb(port_base + LCR, config);

    // enable the interrupts
    outportb(port_base + MCR, GP02);
    outportb(port_base + IER, 1);

    // disable THRE interrupt 
    disable_interrupt(THREINT);

    // install ISR
    new_vector.pm_selector = _go32_my_cs();
    new_vector.pm_offset = (int)skISR;
    _go32_dpmi_allocate_iret_wrapper(&new_vector);

    if (port_base == COM1 || port_base == COM3)
    {
	_go32_dpmi_get_protected_mode_interrupt_vector(INT_SER_PORT_0, &old_vector);
	_go32_dpmi_set_protected_mode_interrupt_vector(INT_SER_PORT_0, &new_vector);
    }
    else    /* COM2 || COM4 */
    {
	_go32_dpmi_get_protected_mode_interrupt_vector(INT_SER_PORT_1, &old_vector);
	_go32_dpmi_set_protected_mode_interrupt_vector(INT_SER_PORT_1, &new_vector);
    }

    // enable interrupt on PIC
    old_int_mask = inportb(PIC_IMR);
    outportb(PIC_IMR, (port_base==COM1 || port_base==COM3) ? (old_int_mask & 0xEF) : (old_int_mask & 0xF7));

    return 1;
}


/*  Enables FIFO if possible.
 */
int skEnableFIFO()
{
    if (!open_port)
	return 0;

    // 16550 has a FIFO, but doesn't work!  Go figure...
    // 16550A is ok tho...
    if (detect_UART(open_port) == UART_16550A)
    {
	outportb(open_port + FCR, 0x87); // 8 byte trigger level should be good
					 // use C7h for 14 byte trigger level
	fifo_enabled = 1;
	return 1;
    } 

    // otherwise disable
    outportb(open_port + FCR, 0);
    fifo_enabled = 0;
    return 0;
}


/*  This function closes the port.
 */
void skClose()
{
    if (open_port)
    {
	// disable FIFOs
	outportb(open_port + FCR, 0);
	fifo_enabled = 0;

	// disable the interrupts
	outportb(open_port + MCR, 0);
	outportb(open_port + IER, 0);
	outportb(PIC_IMR, old_int_mask);

	_go32_dpmi_free_iret_wrapper(&new_vector);

	// reset old ISR handler
	if (open_port == COM1 || open_port == COM3)
	    _go32_dpmi_set_protected_mode_interrupt_vector(INT_SER_PORT_0, &old_vector);
	else    /* COM2 || COM4 */
	    _go32_dpmi_set_protected_mode_interrupt_vector(INT_SER_PORT_1, &old_vector);

	open_port = 0;
    }
}
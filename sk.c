/*  SK 0.51d - Serial Communications
 *      by Peter Wang  (tjaden@alphalink.com.au -or- tjaden@psynet.net)
 *
 *  These routines are based (heavily) on Andre' LaMothes routines featured
 *  in 'Tricks of the Game Programming Gurus'.  Most of the work on my part
 *  was in porting the real mode ISR to protected mode.
 *
 *  Recently they have moved further and further away from LaMothe's code.
 *
 *  Also thanks to Shawn Hargreaves, for I ripped out a few macros from
 *  Allegro.
 */

/*  Started:    14 March 1998
 *  Modified:   15 March 1998   0.51á   skClear() more efficient
 *                                      added #define DEBUGME
 *  Modified:   13 June 1998    0.51c   skPutback()
 *                                      removed DEBUGME
 *  Modified:   14 June 1998    0.51d   skSendString() skWrite()
 *                                      open_port = 0 in skClose()
 *                                      boosted skBUFFER_SIZE to 2K
 */

#include <dpmi.h>
#include <go32.h>
#include <dos.h>
#include <pc.h>
#include <sys/segments.h>
#include "sk.h"



char sk_desc[] = "SK 0.51d";



/*  These macros are taken from Allegro 3.0 (allegro.h)
 *  They are only defined if not already defined.
 */
#ifndef ALLEGRO_H
#define END_OF_FUNCTION(x)    void x##_end() { }
#define LOCK_VARIABLE(x)      _go32_dpmi_lock_data((void *)&x, sizeof(x))
#define LOCK_FUNCTION(x)      _go32_dpmi_lock_code(x, (long)x##_end - (long)x)
#endif


static unsigned char buffer[skBUFFER_SIZE];  // the receive buffer
static volatile int  buf_head,          // index to where the data is written to
		     buf_tail;          // index to where the data is read from
static volatile int  isr_ch;            // current char (used by ISR)

static int old_int_mask;                // the old interrupt mask on the PIC
static int open_port = 0;               // the currently open port

static int virgin = 1;

static _go32_dpmi_seginfo old_vector;
static _go32_dpmi_seginfo new_vector;



/*  This is the ISR for the COM port.  It is very simple.  When it gets 
 *  called, it gets the next character out of the recieve buffer register 0 
 *  and places it into the software buffer.
 */
void skISR()
{
    // place character into next position in buffer
    isr_ch = inportb(open_port + skRBF);

    // wrap buffer index around
    if (++buf_head > skBUFFER_SIZE - 1)
	buf_head = 0;

    // move character into buffer
    buffer[buf_head] = isr_ch;

    // re-enable interrupts
    outportb(skPIC_ICR, 0x20);
}

END_OF_FUNCTION(skISR)


/*  This functions returns the number of characters waiting
 *  in the receive buffer.
 */
int skReady()
{
    if (buf_head >= buf_tail)
	return buf_head - buf_tail;
    else
	return buf_head - buf_tail + skBUFFER_SIZE;
}


/*  This function reads a character from the FIFO
 *  buffer and returns it to the caller.
 */
int skRecv()
{ 
    int ch;

    if (buf_tail == buf_head)
	return 0;

    disable();

    // wrap buffer index if needed 
    if (++buf_tail > skBUFFER_SIZE - 1)
       buf_tail = 0;

    // get the character out of buffer
    ch = buffer[buf_tail];

    enable();

    // send data back to caller
    return ch;
}


/*  This function puts the last retrieved character back into the buffer.
 */
void skPutback()
{
    disable();
    if (--buf_tail < 0)
	buf_tail = skBUFFER_SIZE - 1;
    enable();
}


/*  This function writes a character to the transmit buffer, but first it
 *  waits for the transmit buffer to be empty.  Note: It is not interrupt
 *  driven and it turns off interrupts while it's working.
 */
void skSend(unsigned char ch)
{
    // wait for transmit buffer to be empty 
    while (!(inportb(open_port + skLSR) & 0x20));

    // send the character
    outportb(open_port + skTHR, ch);
}


/*  Send a NULL terminated string.
 */
void skSendString(unsigned char *str)
{
    // while not terminated
    while (*str)
    {
	// do as in skSend()
	while (!(inportb(open_port + skLSR) & 0x20));
	outportb(open_port + skTHR, *str++);
    }
}


/*  Sends len bytes.
 */
void skWrite(unsigned char *str, int len)
{
    // send len bytes
    while (len--)
    {
	// do as in skSend()
	while (!(inportb(open_port + skLSR) & 0x20));
	outportb(open_port + skTHR, *str++);
    }
}


/*  This function simplys clears the buffer.
 */
void skClear()
{
    disable();
    buf_tail = buf_head = 0;
    enable();
}


/*  This function will open up the serial port, set it's configuration, turn
 *  on all the little flags and bits to make interrupts happen and load the
 *  ISR.
 */
void skOpen(int port_base, int baud, int config)
{
    // lock down ISR and variables?
    if (virgin)
    {
	LOCK_FUNCTION(skISR);
	LOCK_VARIABLE(buffer);
	LOCK_VARIABLE(buf_head);
	LOCK_VARIABLE(buf_tail);
	LOCK_VARIABLE(isr_ch);
	virgin = 0; 
    }

    // save the port for other functions
    open_port = port_base;

    // first set the baud rate

    // turn on divisor latch registers
    outportb(port_base + skLCR, skDIV_LATCH_ON);

    // send low and high bytes to divsor latches
    outportb(port_base + skDLL, baud);
    outportb(port_base + skDLH, 0);

    // set the configuration for the port
    outportb(port_base + skLCR, config);

    // enable the interrupts
    outportb(port_base + skMCR, skGP02);
    outportb(port_base + skIER, 1);

    // hold off on enabling PIC until we have the ISR installed
    new_vector.pm_selector = _go32_my_cs();
    new_vector.pm_offset = (int)skISR;
    _go32_dpmi_allocate_iret_wrapper(&new_vector);

    if (port_base == COM1 || port_base == COM3)
    {
	_go32_dpmi_get_protected_mode_interrupt_vector(skINT_SER_PORT_0, &old_vector);
	_go32_dpmi_set_protected_mode_interrupt_vector(skINT_SER_PORT_0, &new_vector);
    }
    else    /* COM2 || COM4 */
    {
	_go32_dpmi_get_protected_mode_interrupt_vector(skINT_SER_PORT_1, &old_vector);
	_go32_dpmi_set_protected_mode_interrupt_vector(skINT_SER_PORT_1, &new_vector);
    }

    // enable interrupt on PIC
    old_int_mask = inportb(skPIC_IMR);
    outportb(skPIC_IMR, (port_base==COM1 || port_base==COM3) ? (old_int_mask & 0xEF) : (old_int_mask & 0xF7));

    // clear it, just in case
    skClear();
}


/*  This function closes the port which entails turning off interrupts and
 *  restoring the old interrupt vector.
 */
void skClose()
{
    if (open_port)
    {
	// disable the interrupts
	outportb(open_port + skMCR, 0);
	outportb(open_port + skIER, 0);
	outportb(skPIC_IMR, old_int_mask);

	_go32_dpmi_free_iret_wrapper(&new_vector);

	// reset old ISR handler
	if (open_port == COM1 || open_port == COM3)
	    _go32_dpmi_set_protected_mode_interrupt_vector(skINT_SER_PORT_0, &old_vector);
	else    /* COM2 || COM4 */
	    _go32_dpmi_set_protected_mode_interrupt_vector(skINT_SER_PORT_1, &old_vector);

	open_port = 0;
    }
}
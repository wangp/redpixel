/*  SK 0.51á - Serial Communications
 *      by Peter Wang  (tjaden@alphalink.com.au -or- tjaden@psynet.net)
 *
 *  These routines are based (heavily) on Andre' LaMothes routines featured
 *  in 'Tricks of the Game Programming Gurus'.  Most of the work on my part
 *  was in porting the real mode ISR to protected mode.
 *
 *  Also thanks to Shawn Hargreaves, for I ripped out a few macros from
 *  Allegro.
 */

/*  Started:    14 March 1998
 *  Modified:   15 March 1998   skClear() more efficient
 *                              added #define DEBUGME
 */

//#define DEBUGME     // if #define'd, does print outs

#include <dpmi.h>
#include <go32.h>
#include <pc.h>
#include <sys/segments.h>
//#include <stdlib.h>
#include "sk.h"


/*  These macros are taken from Allegro 3.0 (allegro.h)
 *  They are only defined if not already defined.
 */
#ifndef ALLEGRO_H
#define END_OF_FUNCTION(x)    void x##_end() { }
#define LOCK_VARIABLE(x)      _go32_dpmi_lock_data((void *)&x, sizeof(x))
#define LOCK_FUNCTION(x)      _go32_dpmi_lock_code(x, (long)x##_end - (long)x)
#endif


static unsigned char sk_buffer[skBUFFER_SIZE];  // the receive buffer
static volatile int  buff_end    = -1,  // indexes into receive buffer 
		     buff_start  = -1; 
static volatile int  num_chars   = 0;   // ready flag
static volatile int  isr_ch;            // current char (used by ISR)

static volatile int  locked = 0; 

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
    // lock out any other functions so the buffer doesn't get corrupted
    locked = 1;

    // place character into next position in buffer
    isr_ch = inportb(open_port + skRBF);

    // wrap buffer index around
    if (++buff_end > skBUFFER_SIZE - 1)
	buff_end = 0;

    // move character into buffer
    sk_buffer[buff_end] = isr_ch;
    ++num_chars;

    // re-enable interrupts
    outportb(skPIC_ICR, 0x20);

    // unlock
    locked = 0;
}

END_OF_FUNCTION(skISR)



/*  This functions returns non-zero if there are any 
 *  characters waiting and 0 if the buffer is empty.
 */
int skReady()
{
    return num_chars;
}


/*  This function reads a character from the circulating 
 *  buffer and returns it to the caller.
 */
int skRecv()
{ 
    int ch;

    // wait for ISR to end
    while (locked);

    // character(s) ready in buffer?
    if (num_chars)
    { 
	// wrap buffer index if needed 
	if (++buff_start > skBUFFER_SIZE - 1)
	   buff_start = 0;

	// get the character out of buffer
	ch = sk_buffer[buff_start];

	// one less character in buffer now
	if (num_chars)
	    num_chars--;

#ifdef DEBUGME
	printf("skRecv()\t%3d %c\n", ch, ch);
#endif

	// send data back to caller
	return ch;
    }

    // buffer was empty return 0
    return 0;
}


/*  This function writes a character to the transmit buffer, but first it
 *  waits for the transmit buffer to be empty.  Note: It is not interrupt
 *  driven and it turns off interrupts while it's working.
 */
void skSend(unsigned char ch)
{
    // wait for transmit buffer to be empty 
    while (!(inportb(open_port + skLSR) & 0x20));

    // turn off interrupts for a while
    asm ("cli");

    // send the character
    outportb(open_port + skTHR, ch);

    // turn interrupts back on
    asm ("sti");

#ifdef DEBUGME
    printf("skSend()\t%3d %c\n", ch, ch);
#endif
}


/*  This function simplys clears the buffer.
 */
void skClear()
{
    while (locked);
    buff_start = buff_end = -1;
#ifdef DEBUGME
    printf("skClear()\tCleared %d chars\n", num_chars);
#endif
    num_chars = 0;
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
	LOCK_VARIABLE(sk_buffer);
	LOCK_VARIABLE(buff_end);
	LOCK_VARIABLE(buff_start);
	LOCK_VARIABLE(isr_ch);
	LOCK_VARIABLE(num_chars);
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
    }
}
/*
 * SK module for Linux
 * 
 * Code taken from my Libnet serial driver.
 */

#ifdef TARGET_LINUX


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "sk.h"


char sk_desc[] = "SK module (linux)";

static int fd = -1;
static struct termios oldtio;

/* would be unneeded if we didn't have to satisfy skReady() */

#define BUFFER_SIZE     2048    

static int recv_head, recv_tail;
static unsigned char recv_buf[BUFFER_SIZE];

static void poll_read()
{
    char buf[256];
    int size, i;
    
    /* should use `select' if this is a hog */
    size = read(fd, buf, sizeof buf);
    for (i = 0; i < size; i++) {
	if (++recv_head == BUFFER_SIZE)
	    recv_head = 0;

	recv_buf[recv_head] = buf[i];
    }
}


/*  This functions returns the number of characters waiting 
 *  in the receive buffer.
 */
int skReady()
{
    poll_read();
    
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
    poll_read();

    if (recv_tail == recv_head)
	return 0;

    if (++recv_tail == BUFFER_SIZE)
       recv_tail = 0;

    return recv_buf[recv_tail];
}


/*  This function reads num chars and sticks them in buf.
 *  Make sure you have enough characters ready before calling this.
 */
void skRead(unsigned char *dest, int num)
{
    int i = 0;
    
    poll_read();

    while (num--) {
	if (++recv_tail == BUFFER_SIZE)
	    recv_tail = 0;

	dest[i++] = recv_buf[recv_tail];
    }
}


/*  This function puts the last retrieved character back into the buffer.
 */
void skPutback()
{
    if (--recv_tail < 0)
	recv_tail = BUFFER_SIZE - 1;
}


/*  This function simplys clears the receive buffer.
 */
void skClear()
{
    tcflush(fd, TCIFLUSH);
    recv_head = recv_tail = 0;
}



/*  This functions returns the number of characters waiting 
 *  in the send buffer.
 */
int skWaiting()
{
    /* This function is unused by the game.  */
    exit(1);
}


/*  This function puts a character into the send buffer. 
 */
void skSend(unsigned char ch)
{
    write(fd, &ch, 1);
}


/*  Send a NULL terminated string.
 */
void skSendString(unsigned char *str)
{
    int len = strlen(str);
    write(fd, str, len);
}


/*  Sends len bytes.
 */
void skWrite(unsigned char *str, int len)
{
    write(fd, str, len);
}


/*  This function writes all characters waiting to be sent into the
 *  transmit buffer.
 */
void skFlush()
{
    tcdrain(fd);
}


/*  This function will open up the serial port, set it's configuration, turn
 *  on all the little flags and bits to make interrupts happen and load the
 *  ISR.
 */
int skOpen(int port_base, int baud, int config)
{
    char device[20];
    struct termios tio;
    int num; 
    
    switch (port_base) {
	case COM1: num = 0; break; 
	case COM2: num = 1; break;
	case COM3: num = 2; break;
	case COM4: num = 3; break;
	default: return 0;
    }

    sprintf(device, "/dev/ttyS%d", num);
    fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) 
	return 0;

    tcgetattr(fd, &oldtio);

    memset(&tio, 0, sizeof tio);

    /* FIXME: this stuff should be extracted from the config variable
       (DOS code left-over cruft) */
    tio.c_cflag = CLOCAL | CREAD
			| B19200    /* baud rate */
			| CS8 	    /* databits */
			| 0	    /* no parity */
			| 0;	    /* one stop bit */
		    /* CRTSCTS should be added if the DOS code 
		     * ever gets hardware flow control. */
    
    tio.c_iflag = IGNPAR;
    tio.c_oflag = 0;
    tio.c_lflag = 0;
    tio.c_cc[VTIME] = 0;
    tio.c_cc[VMIN] = 0;
    
    tcflush(fd, TCIOFLUSH);
    tcsetattr(fd, TCSANOW, &tio);
    
    recv_head = recv_tail = 0;

    return 1;
}


/*  Enables FIFO if possible.
 */
int skEnableFIFO()
{
    return 1;
}


/*  This function closes the port.
 */
void skClose()
{
    if (fd < 0) 
	return;
    
    tcdrain(fd);
    tcsetattr(fd, TCSANOW, &oldtio);
    close(fd);
    fd = -1;
}


#endif /* TARGET_LINUX */

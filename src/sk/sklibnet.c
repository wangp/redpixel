 /*
 * Hack!  Libnet pretending to be serial 
 * 
 * Currently hard-wired to use socket drivers.
 */

#ifdef LIBNET_CODE


#include <libnet.h>
#include "sk.h"
#include "skintern.h"


static NET_CONN *conn;


#define BUFFER_SIZE     2048    

static int recv_head, recv_tail;
static unsigned char recv_buf[BUFFER_SIZE];

static void poll_read()
{
    char buf[256];
    int size, i;
    
    /* should use `select' if this is a hog */
    size = net_receive_rdm(conn, buf, sizeof buf);
    for (i = 0; i < size; i++) {
	if (++recv_head == BUFFER_SIZE)
	    recv_head = 0;
	
	recv_buf[recv_head] = buf[i];
    }
}


static int libnet_ready()
{
    if (!conn) return 0;

    poll_read();
    
    if (recv_head >= recv_tail)
	return recv_head - recv_tail;
    else
	return recv_head - recv_tail + BUFFER_SIZE;
}


static int libnet_recv()
{
    if (!conn) return 0;
    
    poll_read();

    if (recv_tail == recv_head)
	return 0;

    if (++recv_tail == BUFFER_SIZE)
       recv_tail = 0;

    return recv_buf[recv_tail];
}


static void libnet_read(unsigned char *dest, int num)
{
    int i = 0;

    if (!conn) return;

    poll_read();
    
    while (num--) {
	if (++recv_tail == BUFFER_SIZE)
	    recv_tail = 0;

	dest[i++] = recv_buf[recv_tail];
    }
}


static void libnet_putback()
{
    if (--recv_tail < 0)
	recv_tail = BUFFER_SIZE - 1;
}


static void libnet_clear()
{
    if (conn) while (net_ignore_rdm(conn));
    recv_head = recv_tail = 0;
}



static void libnet_send(unsigned char ch)
{
    if (conn) net_send_rdm(conn, &ch, 1);
}


static void libnet_send_string(unsigned char *str)
{
    if (conn) {
	int len = strlen(str);
	net_send_rdm(conn, str, len);
    }
}


static void libnet_write(unsigned char *str, int len)
{
    if (conn)
	net_send_rdm(conn, str, len);
}


static void libnet_flush() 
{
    int out; 
    
    if (!conn)
	while (!((net_conn_stats(conn, 0, &out) != 0) || (out == 0))) 
	    ;
}


int (*_sk_libnet_open_callback)();


static int do_listen(int drv)
{
    NET_CONN *listen;
    
    listen = net_openconn(drv, "");
    conn = 0;

    if (listen && !net_listen(listen)) {
	while (1) {
	    conn = net_poll_listen(listen);
	    if (conn) break;
	    
	    if (_sk_libnet_open_callback)
		if (_sk_libnet_open_callback())
		    break;
	}
    }

    if (listen)
	net_closeconn(listen);
    
    return (conn) ? 1 : 0;
}

static int do_connect(int drv, char *addr)
{
    conn = net_openconn(drv, NULL);
    if (conn) {
	if (net_connect(conn, addr) == 0) {
	    while (1) {
		int x = net_poll_connect(conn);
		if (x > 0) 
		    return 1;
		else if (x < 0)
		    break;

		if (_sk_libnet_open_callback)
		    if (_sk_libnet_open_callback()) 
			break;
	    }
	}

    	net_closeconn(conn);
	conn = 0;
    }
    
    return 0;
}


static int libnet_open(int listen, char *info)
{
    int drv;
    
    net_init();
    net_loadconfig(NULL);

    /* We're not supposed to use these constants, but the net driver
     * classes interface just seems too tedious right now.  */
    drv = -1;
    
#ifdef TARGET_LINUX
    drv = NET_DRIVER_SOCKETS;
#endif
#ifdef TARGET_DJGPP
    drv = NET_DRIVER_WSOCK_DOS;
#endif
#ifdef TARGET_WINDOWS
    drv = NET_DRIVER_WSOCK_WIN;
#endif

    if (drv < 0)
	return 0;
    
    if (!net_initdriver(drv))
	return 0;
    
    if (listen)
	return do_listen(drv);
    else
	return do_connect(drv, info);
}


static void libnet_close()
{
    net_shutdown();
    conn = 0;
}


SK_DRIVER __sk__libnet = {
    	"Libnet driver",
	
	libnet_ready,
	libnet_recv,
	libnet_read,
	libnet_putback,
	libnet_clear,
	
	libnet_send,
	libnet_send_string,
	libnet_write,
	libnet_flush,
	
	libnet_open,
	libnet_close
};


#endif

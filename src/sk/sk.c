/*
 * SK high level interface
 *
 */

#include <string.h>
#include "sk.h"
#include "skintern.h"


static SK_DRIVER *drv;
static int open;


int skReady()
{
    if (!open) return 0;
    return drv->ready();
}

int skRecv()
{
    if (!open) return 0;
    return drv->recv();
}
    
    
void skRead(unsigned char *dest, int num)
{
    if (open) drv->read(dest, num);
}

void skPutback()
{
    if (open) drv->putback();
}

void skClear()
{
    if (open) drv->clear();
}

void skSend(unsigned char c)
{
    if (open) drv->send(c);
}

void skSendString(unsigned char *s)
{
    if (open) drv->send_string(s);
}

void skWrite(unsigned char *p, int n)
{
    if (open) drv->write(p, n);
}

void skFlush()
{
    if (open) drv->flush();
}

int skOpen(int x, char *y)
{
    if (!drv) return 0;

    open = drv->open(x, y);
    return open;
}

void skClose()
{
    if (drv && open) {
	drv->close();
	open = 0;
    }
}

/*------------------------------------------------------------*/

extern SK_DRIVER __sk__serial;
extern SK_DRIVER __sk__libnet;

struct drv {
    int id;
    SK_DRIVER *drv;
};

static struct drv table[] = {
    { SK_SERIAL, &__sk__serial },
#ifndef NO_LIBNET_CODE
    { SK_LIBNET, &__sk__libnet },
#endif
    { 0, 0 }
};


int skSetDriver(int driver)
{
    int i;
    
    for (i = 0; table[i].drv; i++) {
	if (table[i].id == driver) {
	    drv = table[i].drv;
	    return 1;
	}
    }
    
    return 0;
}

/*------------------------------------------------------------*/

char __sk__config_path[1024];

void skSetConfigPath(const char *path)
{
    strcpy(__sk__config_path, path);
}

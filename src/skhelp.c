/* skhelp.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include "sk.h"
#include "skhelp.h"


void send_long(long val)
{
    skSend(val & 0xff);
    skSend((val >> 8) & 0xff);
    skSend((val >> 16) & 0xff);
    skSend((val >> 24) & 0xff);
}


long recv_long()
{
    while (skReady() < 4) ;
    return skRecv() | (skRecv() << 8) | (skRecv() << 16) | (skRecv() << 24);
}

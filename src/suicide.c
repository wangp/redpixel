/* suicide.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "suicide.h"
#include "main.h"
#include "sk.h"


void suicide(char *s)
{
    shutdown();
    skClose();
    allegro_message("%s\n", s);
    exit(1);
}

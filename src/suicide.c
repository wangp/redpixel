/*
 *  Red Pixel, a violent game.
 *  Copyright (C) 1999 Psyk Software.
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

#ifndef SKMOD_H
#define SKMOD_H

#include "sk.h"

// word responses from the modem
#define OK          "OK"                // 0
#define CONNECT     "CONNECT"           // 1 used for all flavours of CONNECT*
#define RING        "RING"              // 2 should ever get this
#define NOCARRIER   "NO CARRIER"        // 3
#define ERROR       "ERROR"             // 4 command line error
#define NODIALTONE  "NO DIALTONE"       // 6
#define BUSY        "BUSY"              // 7
#define NOANSWER    "NO ANSWER"         // 8

// errors
#define ERR_NOCARRIER   3
#define ERR_ERROR       4
#define ERR_NODIALTONE  6
#define ERR_BUSY        7
#define ERR_NOANSWER    8
#define ERR_USERABORT   666

// globals

extern char skm_desc[];

extern void (*skm_delay)(); // IMPORTANT!! this callback must delay 
			    // the program for 50 milliseconds or more

extern char init_string[];
extern char dial_prefix[];
extern char autoanswer_count[];

// prototypes
int     skmModemPresent();
void    skmInit();
int     skmAwaitConnect(int (*callback)());
int     skmDial(char *no, int (*callback)());
void    skmHangup();
void    skmEnableAutoAnswer();
void    skmDisableAutoAnswer();
void    skmCommandMode();
void    skmDataMode();

#endif
/*  SKM - Modem Extension for SK 
 *      by Peter Wang   (tjaden@alphalink.com.au -or- tjaden@psynet.net)
 */

/*  Started:    17-9 July 1998
    Modified:   26 July 1998
 */

#include <string.h>
#include <mss.h>
#include "skmod.h"


//------------------------------------------------------------ globals ------

char skm_desc[] = "SKM 1998.07.26";

void (*skm_delay)() = NULL;     // important!! see skmod.h

//char init_string[80] = "AT&C1&D2EX4 S7=55S11=60 \\N6\\Q3\\J\\V1%C1";
char init_string[80] = "ATZ";
char dial_prefix[20] = "ATDT";
char autoanswer_count[5] = "2";

//------------------------------------------------------------ macros -------

#define CRSTR   "\13\0"
#define CR      13

#define modem_cmd(x) { skSendString(x); skSend(CR); skFlush(); }


//------------------------------------------------------------ helpers ------

static char rbuf[80];

#define responded(x) (strcmp(rbuf, x)==0)

static char *get_response(int (*callback)())
{
    char *p, ch;

    p = rbuf; 
    do
    {
	while (!skReady()) 
	{
	    // allow user to abort
	    if (callback)
		if (callback())
		{
		    skSend(' ');
		    return NULL;
		}

	    skm_delay();
	}

	ch = skRecv();

	if (ch == CR && p == rbuf)
	    ch = 0;
	else if (ch != CR && ch != '\n')
	    *p++ = ch;
    } while (ch != CR);
    *p = 0;

    return rbuf;
}

static void wait_response(char *x)
{
    do { 
	get_response(NULL); 
    } while (!responded(x));
}


//------------------------------------------------------------ routines -----

/* these must be run in Command Mode ONLY! */

int skmModemPresent()
{
    int i;

    // must have this callback
    if (skm_delay==NULL) return 0;

    // anybody home?
    modem_cmd("AT"); 

    for (i=0; i<7; i++)
    {
	// wait 50 msec or more
	skm_delay();

	// modem answered yet?
	if (skReady())
	{
	    get_response(NULL);
	    if (responded(OK))  // got it.. bye bye
		return 1;
	}
    }

    // nothing, no modem
    return 0;
}

void skmInit()
{
    // DTR on
    skHand(DTR);

    modem_cmd(init_string);
    wait_response(OK); 

    // turn off echo and autoanswer
    modem_cmd("ATE&S0=0");
    wait_response(OK);
}

int skmAwaitConnect(int (*callback)())
{
    do
    {
	if (!get_response(callback))
	    return ERR_USERABORT;
    } while (strncmp(rbuf, CONNECT, strlen(CONNECT)) != 0);
    return 0;
}

// callback: return zero for dialling to continue
//           return non-zero to abort
// use NULL for no callback

int skmDial(char *no, int (*callback)())
{
    // dial
    skSendString(dial_prefix);
    skSendString(no);
    skSend(CR);
    skFlush();

    if (!get_response(callback))
    {
	wait_response(NOCARRIER);
	return ERR_USERABORT;
    }

    // various forms of CONNECT
    if (strncmp(rbuf, CONNECT, strlen(CONNECT))==0) 
	return 0;

    // uh oh go here:
    if (responded(NOCARRIER))   return ERR_NOCARRIER;
    if (responded(ERROR))       return ERR_ERROR;
    if (responded(NODIALTONE))  return ERR_NODIALTONE;
    if (responded(BUSY))        return ERR_BUSY;
    if (responded(NOANSWER))    return ERR_NOANSWER; 

    // please don't come here!
    return -1;
}

void skmEnableAutoAnswer()
{
    char tmp[40] = "ATS0=";
    strcat(tmp, autoanswer_count);
    modem_cmd(tmp);
    wait_response(OK);
}

void skmDisableAutoAnswer()
{
    modem_cmd("ATS0=0"); 
    wait_response(OK);
}

void skmHangup()
{
    // drop DTR to hangup
    skHand(0);
}


/* for switching between command and data mode 
 * will not work in synchronous mode
 */

void skmCommandMode()
{
    skSendString("+++");
    skFlush();
}

void skmDataMode()
{
    modem_cmd("ATO");
    // don't need to wait for OK I assume
}
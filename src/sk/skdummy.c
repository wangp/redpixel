/*
 * Dummy SK module
 */

#include "sk.h"
#include "skintern.h"

static int dummy_open(int num, char *dummy) { return 0; }

SK_DRIVER __sk__serial = {
    	"Dummy serial driver",
	0, 0, 0, 0, 0,
	0, 0, 0, 0,
	dummy_open, 0
};

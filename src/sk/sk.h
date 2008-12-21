#ifndef __included_sk_h
#define __included_sk_h


#define SK_NONE  	0
#define SK_SERIAL	1
#define SK_LIBNET	2


int skReady (void);
int skRecv (void);
void skRead (unsigned char *, int);
void skPutback (void);
void skClear (void);

void skSend (unsigned char);
void skSendString (unsigned char *);
void skWrite (unsigned char *, int);
void skFlush (void);
void skHand (unsigned int);

int skOpen (int, char *);
void skClose (void);

int skSetDriver (int);
void skSetConfigPath (const char *);


#ifndef NO_LIBNET_CODE
extern int (*_sk_libnet_open_callback) (void);
#endif


#endif

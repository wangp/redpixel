#ifndef __included_skintern_h
#define __included_skintern_h


typedef struct {
    char *desc;
    
    int (*ready)();
    int (*recv)();
    void (*read)(unsigned char *, int);
    void (*putback)();
    void (*clear)();
    
    void (*send)(unsigned char);
    void (*send_string)(unsigned char *);
    void (*write)(unsigned char *, int);
    void (*flush)();
    
    int (*open)(int, char *);
    void (*close)();
    
} SK_DRIVER;


extern char __sk__config_path[];


#endif

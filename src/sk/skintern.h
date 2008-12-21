#ifndef __included_skintern_h
#define __included_skintern_h


typedef struct {
    char *desc;
    
    int (*ready)(void);
    int (*recv)(void);
    void (*read)(unsigned char *, int);
    void (*putback)(void);
    void (*clear)(void);
    
    void (*send)(unsigned char);
    void (*send_string)(unsigned char *);
    void (*write)(unsigned char *, int);
    void (*flush)(void);
    
    int (*open)(int, char *);
    void (*close)(void);
    
} SK_DRIVER;


extern char __sk__config_path[];


#endif

#include <allegro.h>
#include <stdio.h>


void bin_print(int val)
{
    char buf[12];
    int i;
    sprintf(buf, "%x", val);
    for (i=0; i<strlen(buf); i++)
    {
	switch (buf[i])
	{
	    case '0': printf("0000 "); break;
	    case '1': printf("0001 "); break;
	    case '2': printf("0010 "); break;
	    case '3': printf("0011 "); break;
	    case '4': printf("0100 "); break;
	    case '5': printf("0101 "); break;
	    case '6': printf("0110 "); break;
	    case '7': printf("0111 "); break;
	    case '8': printf("1000 "); break;
	    case '9': printf("1001 "); break;
	    case 'a': printf("1010 "); break;
	    case 'b': printf("1011 "); break;
	    case 'c': printf("1100 "); break;
	    case 'd': printf("1101 "); break;
	    case 'e': printf("1110 "); break;
	    case 'f': printf("1111 "); break;
	}
    }
    printf("\n");
}

int main()
{
    fixed angle = 1234567812;
    fixed fit;
    unsigned char i1, i2, i3, i4;

    i1 = angle & 0xff;
    i2 = (angle>>8) & 0xff;
    i3 = (angle>>16) & 0xff;
    i4 = (angle>>24) & 0xff;

    fit = ;
    bin_print(angle);
    bin_print(i1);
    bin_print(i2);
    bin_print(i3);
    bin_print(i4);

    return 0;
}
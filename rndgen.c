#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main()
{
    time_t a;
    int i, j;

    a = time(NULL);
    srandom(a);
    printf("/* created by rndgen  seed = %d */\n", a);
    printf("\nint rnd[600] = {");
    for (i=0; i<600; i+=5) {
	printf("\n  ");
	for (j=0; j<5; j++)
	{
	    printf("%11d,", random());
	}
    }
    printf("\n};");

    return 0;
}
// quick benchmarking util by Peter Wang 3 august 1998

#include <stdio.h>
#include <conio.h>
#include <pc.h>
#include <math.h>
#include <allegro.h>    // :-)
#include "fastsqrt.h"

volatile int last_count = 0, counter = 0;

void persec()
{
    last_count = counter;
    counter = 0;
};
END_OF_FUNCTION(persec);

int main()
{
    int old = 0, anum = 0;

    allegro_init();
    LOCK_FUNCTION(persec);
    LOCK_VARIABLE(last_count);
    LOCK_VARIABLE(counter); 
    install_timer();
    install_int(persec, 1000);

    build_sqrt_table();

    printf("\nStandard C sqrt():\n");

    while (!kbhit())
    {
	sqrt(anum++);
	counter++;

	if (old != last_count)
	{
	    printf("%d ", old = last_count);
	    fflush(stdout);
	}
    }
    getch();

    printf("\n\nfast_sqrt():\n");

    while (!kbhit())
    {
	fast_fsqrt(anum++);
	counter++;

	if (old != last_count)
	{
	    printf("%d ", old = last_count);
	    fflush(stdout);
	}
    }
    getch();

    printf("\n\nfast_sqrt_asm():\n");

    while (!kbhit())
    {
	fast_fsqrt_asm(anum++);
	counter++;

	if (old != last_count)
	{
	    printf("%d ", old = last_count);
	    fflush(stdout);
	}
    }
    getch();

    return 0;
}
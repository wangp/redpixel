#include "sk.c"

int main()
{
    skOpen(COM2, BAUD_115200, BITS_8 | PARITY_NONE | STOP_1);
    if (skEnableFIFO())
	printf("FIFO enabled\n");
    else
	printf("FIFO not enabled\n");
    skClose();
    return 0;
}
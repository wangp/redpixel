#include <stdio.h>
#include <conio.h>
#include "sk.c"

int main()
{
    char ch;

    switch (detect_UART(COM2))
    {
	case UART_8250:
	    printf("COM2 detected as 8250 UART\n");
	    break;
	case UART_16450:
	    printf("COM2 detected as 16450 UART\n");
	    break;
	case UART_16550:
	    printf("COM2 detected as 16550 UART\n");
	    break;
	case UART_16550A:
	    printf("COM2 detected as 16550A UART\n");
	    break;
	default:
	    printf("COM2 not detect\n");
	    break;
    }

    if (!skOpen(COM2, BAUD_19200, BITS_8 | PARITY_NONE | STOP_1))
    {
	printf("COM2 not opened\n");
	return 1;
    }

    printf("COM2 opened\n");

    if (skEnableFIFO())
	printf("FIFO enabled\n");
    else
	printf("FIFO not enabled\n");

    for (;;)
    {
	if (kbhit())
	{
	    ch = getche();
	    if (ch==27)
		break;

	    if (ch=='1')
		skSendString("This is a really long string I hope it doesn't overflow the buffer cause the buffer's only 2K big so you know, if its over 2k then it will overflow but this string isn't that long I hope i wish.");
	    else
		skSend(ch);

	    if (ch==13) {
		putch('\n');
		skSend('\n');
	    }
	}

	while (skReady())
	{
	    ch = skRecv();
	    putch(ch);
	}
    }

    skClose();
    return 0;
}
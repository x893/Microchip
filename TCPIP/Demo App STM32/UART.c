#define __UART_C

#include "Compiler.h"
#include "TCPIPConfig.h"

#if defined(STACK_USE_UART)

#include "TCPIP Stack/TCPIP.h"

char BusyUSART(void)
{
	return ((STACK_USE_UART_PORT->SR & USART_SR_TXE) ? 0 : 1);
}
char DataRdyUSART(void)
{
	return (STACK_USE_UART_PORT->SR & USART_SR_RXNE) ? 1 : 0;
}
char ReadUSART(void)
{
	return (char)STACK_USE_UART_PORT->DR;
}
void WriteUSART(char data)
{
	STACK_USE_UART_PORT->DR = data;
}

void getsUSART(char *buffer, unsigned char len)
{
	char i;	// Length counter
	unsigned char data;

	for(i = 0; i < len; i++)	// Only retrieve len characters
	{
		while(!DataRdyUSART());	// Wait for data to be received

		data = getcUART();		// Get a character from the USART
								// and save in the string
		*buffer = data;
		buffer++;				// Increment the string pointer
	}
}

void putsUSART(char *data) __attribute((alias("putrsUSART")));
void putrsUSART(const char *data)
{
	do
	{  // Transmit a byte
		while (BusyUSART());
		putcUART(*data);
	} while( *data++ );
}

BYTE ReadStringUART(BYTE *Dest, BYTE BufferLen)
{
	BYTE c;
	BYTE count = 0;

	while(BufferLen--)
	{
		*Dest = '\0';

		while(!DataRdyUART());
		c = ReadUART();

		if(c == '\r' || c == '\n')
			break;

		count++;
		*Dest++ = c;
	}
	return count;
}

#endif

# uart

code interface between std function and user for Windows and Linux

## Warning:
After many docs read, i think you should never read msdn, and never do something with Windows, but i did it.

## Example:
```C
#include "lib/uart/uart.h"

int main ( void )
{
	uartHandler uart = 0; // uart handler
	char busName[ 64 ] = { 0 }; // bus name /dev/ttyACMx or COMx
	uint8_t buffer[  1024 ] = { 0 }; // buffer for data

	...
		
	uart = uartOpen ( busName, O_RDWR );

	if ( !uartValide( uart ) )
	{
		printf ( "can't open %s\n", busName );
		return ( __LINE__ );
	}

	uartWrite ( uart, buffer, bufferSizeMax );

	uartRead ( uart, buffer, bufferSizeMax );

	uartClose ( uart );

	return ( 0 );
}
```

## Polling read:
```C
#include "lib/uart/uart.h"

int main ( void )
{
	uint8_t buf[ 256 ] = { 0 };
	char busName[ 64 ] = { 0 }; // bus name /dev/ttyACMx or COMx

	...

	uartHandler uart = uartOpen ( busName, O_RDWR );

	if ( !uartValide ( uart ) )
	{
		return ( __LINE__ );
	}

	if ( uartInit ( uart, 115200, UART_DATA_8 | UART_PARITY_N | UART_STOP_1 ) )
	{
		return( __LINE__ );
	}

	// set non blocking read mode
	uartSetReadTimeout ( uart, 0, 0 );

	do
	{
		if ( uartRead ( uart, buf, 1 ) )
		{ // if read a byte
			printf ( "%x\n", *buf );
		}

		Sleep ( 10 );
	}
	while ( 1 );

	uartClose ( uart );

	return ( 0 );
}
```

For Windows in the function `int uartSetReadTimeout ( uartHandler bus, uint8_t time, uint8_t min );` 
 - if you want a non blocking mode: set time and min to zero,
 - if you want a timed mode: set time upper than zero and min to zero, 
 - if you want a blocking mode: set time to zero and min to 1
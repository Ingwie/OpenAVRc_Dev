////////////////////////////////////////////////////////////////////////////////
/// \copiright ox223252, 2017
///
/// This program is free software: you can redistribute it and/or modify it
///     under the terms of the GNU General Public License published by the Free
///     Software Foundation, either version 2 of the License, or (at your
///     option) any later version.
///
/// This program is distributed in the hope that it will be useful, but WITHOUT
///     ANY WARRANTY; without even the implied of MERCHANTABILITY or FITNESS FOR
///     A PARTICULAR PURPOSE. See the GNU General Public License for more
///     details.
///
/// You should have received a copy of the GNU General Public License along with
///     this program. If not, see <http://www.gnu.org/licenses/>
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdbool.h>

#include "uart.h"

#ifdef __linux__
#include <sys/ioctl.h>
#include <asm/termbits.h>
// ressources :
// http://manpagesfr.free.fr/man/man3/termios.3.html
// https://stackoverflow.com/questions/12646324/how-to-set-a-custom-baud-rate-on-linux
#elif _WIN32
#include <windows.h>
#define RX_SIZE         4096    /* taille tampon d'entrée  */
#define TX_SIZE         4096    /* taille tampon de sortie */
// ressources :
// https://msdn.microsoft.com/en-us/library/ff802693.aspx
// https://code.msdn.microsoft.com/windowsdesktop/Serial-Port-Sample-e8accf30/sourcecode?fileId=67164&pathId=1394200469
#endif

#ifdef __linux__
uartHandler uartInit ( uartHandler bus, uint32_t speed, const int flags )
{
	uint16_t i = 0;
	struct termios2 uart;

	// get old mask
	if ( ioctl ( bus, TCGETS2, &uart ) )
	{
		return ( __LINE__ );
	}

	// set speed
	uart.c_cflag &= ~CBAUD;
	uart.c_cflag |= BOTHER | CREAD;
	uart.c_ispeed = speed;
	uart.c_ospeed = speed;

	// set config
	for ( i = 1; i != 0; i <<= 1 )
	{
		switch ( flags & i )
		{
			case UART_DATA_5:
			{
				uart.c_cflag &= ~CSIZE;
				uart.c_cflag |= CS5;
				break;
			}
			case UART_DATA_6:
			{
				uart.c_cflag &= ~CSIZE;
				uart.c_cflag |= CS6;
				break;
			}
			case UART_DATA_7:
			{
				uart.c_cflag &= ~CSIZE;
				uart.c_cflag |= CS7;
				break;
			}
			case UART_DATA_8:
			{
				uart.c_cflag &= ~CSIZE;
				uart.c_cflag |= CS8;
				break;
			}
			case UART_PARITY_N:
			{
				uart.c_cflag &= ~PARENB;
				break;
			}
			case UART_PARITY_O:
			{
				uart.c_cflag |= PARENB;
				uart.c_cflag |= PARODD;
				break;
			}
			case UART_PARITY_E:
			{
				uart.c_cflag |= PARENB;
				uart.c_cflag &= ~PARODD;
				break;
			}
			case UART_STOP_1:
			{
				uart.c_cflag &= ~CSTOPB;
				break;
			}
			case UART_STOP_2:
			{
				uart.c_cflag |= CSTOPB;
				break;
			}
			default:
			{
				break;
			}
		}
	}

	// clean rest of struct
	uart.c_iflag = 0;
	uart.c_lflag = 0;
	uart.c_oflag = 0;

	return ( ioctl ( bus, TCSETS2, &uart ) );
}

int uartSetReadTimeout ( uartHandler bus, uint8_t time, uint8_t min )
{
	static struct termios uart;

	if ( ioctl ( bus, TCGETS2, &uart ) )
	{
		return  ( __LINE__ );
	}

	uart.c_cc[ VMIN ] = min;
	uart.c_cc[ VTIME ] = time;

	return ( ioctl ( bus, TCSETS2, &uart ) );
}
#elif _WIN32
uartHandler uartOpen ( const char * const busName, const int flags )
{
	DWORD dwDesiredAccess = 0;

	switch ( flags & ( O_RDONLY | O_WRONLY | O_RDWR ) )
	{
		case O_RDONLY:
		{
			dwDesiredAccess = GENERIC_READ;
			break;
		}
		case O_WRONLY:
		{
			dwDesiredAccess = GENERIC_WRITE;
			break;
		}
		case O_RDWR:
		{
			dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
			break;
		}
		default:
		{
			return ( INVALID_HANDLE_VALUE );
		}
	}
	// fd = CreateFile ( busName, dwDesiredAccess, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL );
	return ( CreateFile ( busName, dwDesiredAccess, 0, NULL, OPEN_EXISTING, 0, NULL ) );
}

int uartInit ( uartHandler bus, uint32_t speed, const int flags )
{
	uint16_t i = 0;
	DCB dcb;

	FillMemory ( &dcb, sizeof ( dcb ), 0 );
	dcb.DCBlength = sizeof(dcb);

	// set speed
	dcb.BaudRate = speed ;

	// set config
	for ( i = 1; i != 0; i <<= 1 )
	{
		switch ( flags & i )
		{
			case UART_DATA_5:
			{
				dcb.ByteSize = 5;
				break;
			}
			case UART_DATA_6:
			{
				dcb.ByteSize = 6;
				break;
			}
			case UART_DATA_7:
			{
				dcb.ByteSize = 7;
				break;
			}
			case UART_DATA_8:
			{
				dcb.ByteSize = 8;
				break;
			}
			case UART_PARITY_N:
			{
				dcb.fParity = 0;
				dcb.Parity = NOPARITY;
				break;
			}
			case UART_PARITY_O:
			{
				dcb.fParity = 1;
				dcb.Parity = ODDPARITY;
				break;
			}
			case UART_PARITY_E:
			{
				dcb.fParity = 1;
				dcb.Parity = EVENPARITY;
				break;
			}
			case UART_STOP_1:
			{
				dcb.StopBits = ONESTOPBIT;
				break;
			}
			case UART_STOP_2:
			{
				dcb.StopBits = TWOSTOPBITS;
				break;
			}
			default:
			{
				break;
			}
		}
	}

	dcb.fBinary = TRUE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fTXContinueOnXoff = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;

	dcb.fAbortOnError = FALSE;
	dcb.fDummy2 = 0;
	dcb.wReserved = 0;
	dcb.XonLim = 0x100;
	dcb.XoffLim = 0x100;
	dcb.XonChar = 0x11;
	dcb.XoffChar = 0x13;
	dcb.ErrorChar = '?';
	dcb.EofChar = 0x1A;
	dcb.EvtChar = 0x10;

	if ( ( i = SetCommState ( bus, &dcb ) ) == 0 )
	{
		printf ( "i %d\"n", i );
		return ( __LINE__ );
	}
	return ( 0 );
}

int uartSetReadTimeout ( uartHandler bus, uint8_t time, uint8_t min )
{
	COMMTIMEOUTS cto;

	if ( ( time == 0 ) &&
		( min == 0 ) )
	{
		cto.ReadIntervalTimeout = MAXDWORD;
	}
	else
	{
    	cto.ReadIntervalTimeout = time * 100;
	}

    cto.ReadTotalTimeoutConstant = time * 100;
    cto.ReadTotalTimeoutMultiplier = 0;
    cto.WriteTotalTimeoutMultiplier = 0;
    cto.WriteTotalTimeoutConstant = 0;

    return ( SetCommTimeouts ( bus, &cto ) );
}

void uartWrite ( uartHandler bus, uint8_t * buf, size_t size )
{
	DWORD bWr = 0;
	if ( !buf ||
		!size ||
		!bus )
	{
		return;
	}

	OVERLAPPED overlapWrite;
	overlapWrite.Internal = 0;
	overlapWrite.InternalHigh = 0;
	overlapWrite.Offset = 0;
	overlapWrite.OffsetHigh = 0;
	overlapWrite.hEvent = CreateEvent ( NULL, true, false, NULL );

	WriteFile ( bus, buf, size, &bWr, &overlapWrite );    // Always false because : asynchronous
	ResetEvent ( overlapWrite.hEvent );
	CloseHandle ( overlapWrite.hEvent );
}

int uartRead ( uartHandler bus, uint8_t * buf, size_t size )
{
	DWORD bRd = 0;
	bool r = false;

	if ( !buf ||
		!size ||
		!bus )
	{
		return ( 0 );
	}

	OVERLAPPED overlapRead;
	overlapRead.Internal = 0;
	overlapRead.InternalHigh = 0;
	overlapRead.Offset = 0;
	overlapRead.OffsetHigh = 0;
	overlapRead.hEvent = CreateEvent ( NULL, true, false, NULL );

	r = ReadFile ( bus, buf, size, &bRd, &overlapRead );
	ResetEvent ( overlapRead.hEvent ); // Supression de l'évènement erreur sur la lecture
	CloseHandle ( overlapRead.hEvent );

	return ( (int)( (r)?bRd : 0 ) );
}

void uartClose ( uartHandler bus )
{
	CloseHandle ( bus );
}
int uartValide ( uartHandler bus )
{
	return ( bus != INVALID_HANDLE_VALUE );
}
#endif


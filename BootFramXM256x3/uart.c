/************************************************************************/
/* XBoot Extensible AVR Bootloader                                      */
/*                                                                      */
/* UART Module                                                          */
/*                                                                      */
/* uart.c                                                               */
/*                                                                      */
/* Alex Forencich <alex@alexforencich.com>                              */
/*                                                                      */
/* Copyright (c) 2010 Alex Forencich                                    */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files(the "Software"), to deal in the Software without restriction,  */
/* including without limitation the rights to use, copy, modify, merge, */
/* publish, distribute, sublicense, and/or sell copies of the Software, */
/* and to permit persons to whom the Software is furnished to do so,    */
/* subject to the following conditions:                                 */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  */
/* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   */
/* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    */
/* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     */
/* SOFTWARE.                                                            */
/*                                                                      */
/************************************************************************/

#include "uart.h"


// Initialize UART
void uart_init(void)
{
#ifdef __AVR_XMEGA__
        UART_PORT.DIRSET = (1 << UART_TX_PIN);
        #ifdef UART_REMAP
        UART_PORT.REMAP |= PORT_USART0_bm;
        #endif // UART_REMAP
        UART_DEVICE.BAUDCTRLA = (UART_BSEL_VALUE & USART_BSEL_gm);
        UART_DEVICE.BAUDCTRLB = ((UART_BSCALE_VALUE << USART_BSCALE_gp) & USART_BSCALE_gm) | ((UART_BSEL_VALUE >> 8) & ~USART_BSCALE_gm);
        #if UART_CLK2X
        UART_DEVICE.CTRLB = USART_RXEN_bm | USART_CLK2X_bm | USART_TXEN_bm;
        #else
        UART_DEVICE.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
        #endif // UART_CLK2X
        #ifdef USE_INTERRUPTS
        UART_DEVICE.CTRLA = USART_RXCINTLVL0_bm | USART_TXCINTLVL0_bm;
        #endif // USE_INTERRUPTS
#else // __AVR_XMEGA__
        UART_UBRR = UART_BRV;
        #ifdef UART_U2X
        UART_UCSRA = _BV(U2X0);
        #else
        UART_UCSRA = 0;
        #endif
        UART_UCSRB = _BV(RXEN0) | _BV(TXEN0);
        UART_UCSRC = _BV(UCSZ01) | _BV(UCSZ00);
#endif // __AVR_XMEGA__
}

// Shut down UART
void uart_deinit(void)
{
#ifdef __AVR_XMEGA__
        UART_DEVICE.CTRLB = 0;
        #ifdef USE_INTERRUPTS
        UART_DEVICE.CTRLA = 0;
        #endif // USE_INTERRUPTS
        UART_DEVICE.BAUDCTRLA = 0;
        UART_DEVICE.BAUDCTRLB = 0;
        #ifdef UART_REMAP
        UART_PORT.REMAP &= ~PORT_USART0_bm;
        #endif // UART_REMAP
        UART_PORT.DIRCLR = (1 << UART_TX_PIN);
#else // __AVR_XMEGA__
        UART_UCSRA = 0;
        UART_UCSRB = 0;
        UART_UCSRC = _BV(UCSZ01) | _BV(UCSZ00);
        UART_UBRR = 0;
#endif // __AVR_XMEGA__
}


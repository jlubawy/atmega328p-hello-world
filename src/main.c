/**
 * main.c - Atmega328p Hello World Example
 * Copyright (C) 2016 Josh Lubawy <jlubawy@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <avr/io.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static FILE g_uart_stream = {0};

static char line[256];

static void
uart_init( uint32_t baud )
{
    /* See table 20-1 for baud rate calculations */
    uint16_t ubrr = (F_CPU / (16*baud)) - 1;

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    /* Disable 2x TX speed */
    UCSR0A &= ~_BV(U2X0);

    /* Enable RX and TX */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);

    /* Set 8-N-1 frame */
    UCSR0C = (3 << UCSZ00);
}


static int
uart_getc( FILE* stream )
{
    while ( (UCSR0A & _BV(RXC0)) == 0 ); /* wait for a character to be received */
    return UDR0;
}


static int
uart_putc( char c, FILE* stream )
{
    while ( (UCSR0A & _BV(UDRE0)) == 0 ); /* wait for the TX buffer to be ready */
    UDR0 = c; /* put char into TX buffer */
    return 0;
}


int
main( void )
{
    DDRB |= _BV(5);

    uart_init( 9600 );
    fdev_setup_stream( &g_uart_stream, uart_putc, uart_getc, _FDEV_SETUP_RW );

    stdin  = &g_uart_stream;
    stdout = &g_uart_stream;
    stderr = &g_uart_stream;

    for (;;)
    {
        int i;
        int c;
        int empty;

        /* Read in a line from stdin */
        i = 0;
        empty = 1;
        while( i < sizeof(line) - 1 )
        {
            c = getchar();

            /* Look for EOT (CTRL-D) char */
            if ( c == 4 ) {
                goto EXIT;
            }

            line[i++] = (char)c;

            if ( (char)c == '\n' ) {
                break;
            } else if ( (char)c == '\r' ) {
                continue; /* ignore carriage returns */
            }

            empty = 0;
        }
        line[i++] = '\0';

        /* Skip empty lines */
        if ( empty ) {
            putchar( '\n' );
            continue;
        }

        size_t len = strlen( line );
        for ( i = 0; i < len; i++ )
        {
            if ( line[i] == 'E' || line[i] == 'e' ) {
                line[i] = '3';
            }
        }

        /* Send the entire line over UART */
        fputs( line, stdout );
        fflush( stdout );
    }

EXIT:
    return 0;
}

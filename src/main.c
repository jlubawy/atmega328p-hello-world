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
#include <stdlib.h>
#include <string.h>

static void
serial_init( uint32_t baud )
{
    /* See table 20-1 for baud rate calculations */
    uint16_t ubrr = (F_CPU / (16*baud)) - 1;

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    /* Disable 2x TX speed */
    UCSR0A &= ~_BV(U2X0);

    /* Enable TX */
    UCSR0B = _BV(TXEN0);

    /* Set 8-N-1 frame */
    UCSR0C = (3 << UCSZ00);
}


static void
serial_putc( char c )
{
    while ( (UCSR0A & _BV(UDRE0)) == 0 ); /* wait for the TX buffer to be ready */
    UDR0 = c; /* put char into TX buffer */
}


static void
serial_println( const char* str )
{
    size_t i;
    size_t len = strlen( str );

    for ( i = 0; i < len; i++ )
    {
        serial_putc( str[i] );
    }

    serial_putc( '\r' );
    serial_putc( '\n' );
}


int
main( void )
{
    serial_init( 9600 );
    serial_println( "Hello World" );
    return 0;
}

Sample UART Client Code
=======================

```c
#include "signal.h"
#include "stdio.h"
#include "string.h"

#define COMPORT "COM3"

static char fName[] = "\\\\.\\"COMPORT;

static char line[256];


int main( void )
{
    FILE* f;

    f = fopen( fName, "r+b" );
    if ( f == NULL )
    {
        fprintf( stderr, "could not open serial port '%s'\n", fName );
        perror( NULL );
        return 1;
    }

    for (;;)
    {
        int i;
        int c;
        int empty;

        fputs( "> ", stdout );
        fflush( stdout );

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

        /* Send the entire line over UART */
        fputs( line, f );
        fflush( f );

        /* Receive a line back over UART */
        i = 0;
        while ( i < sizeof(line) - 1 )
        {
            c = getc( f );

            /* Keep reading even if EOF reached */
            if ( c == EOF ) {
                continue;
            }

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
        }
        line[i++] = '\0';

        fputs( line, stdout );
        fflush( stdout );
    }

EXIT:
    fprintf( stdout, "Closing serial port\n" );
    fflush( stdout );

    fclose( f );
    return 0;
}
```
//v.01
#include "main.h"


int main() {
    int server_error = 0;

    if ( ( server_error = server_init( PORT ) ) ) {
        server_error_print( server_error );
        return 1;
    }

    while ( 1 ) {

        if ( ( server_error = server_loop() ) ) {
            server_error_print( server_error );
            return 2;
        }

    }

    if ( ( server_error = server_del() ) ) {
        server_error_print( server_error );
        return 3;
    }

    return 0;

}

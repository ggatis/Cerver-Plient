#ifndef SERVER_H
#define SERVER_H

#include "messages_A.h"
#include "unpackers.h"
#include "woodpeckers.h"


//structures


//external variables
//extern int global_variable;


//function declarations
void server_error_print( int err_code );
int server_init( uint16_t port );
//int server_init( void );
int server_del( void );
int server_loop( void );


#endif //SERVER_H

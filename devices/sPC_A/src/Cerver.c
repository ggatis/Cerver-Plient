#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <zlib.h>                   //include the zlib library for crc32
#include <sys/time.h>               //include the <sys/time.h> header for gettimeofday()
#include "Cerver.h"


#define PORT 12345


static const char   sexit[] = "EXIT";
unsigned            times;          //atkaartoshanas skaits
char                proc;           //proceduura, A > 0, B > 1

struct timeval      start_time,
                    finnish_time;


int evaluate( char proc ) {
    return 0;
}


void send_time( struct timeval* start_time, struct timeval* finnish_time ) {
    //in ms
    long elapsed_time_ms = 1000 * ( finnish_time->tv_sec - start_time->tv_sec ) +
        0.001 * ( finnish_time->tv_usec - start_time->tv_usec );

    printf("Time elapsed: %ld milliseconds\n", elapsed_time_ms );

}


int validate_command( char* buffer, size_t size ) {
    uint32_t CRC, rCRC;
    
    unsigned len = size - SIZE_OF_UINT32;

    if ( ( SIZE_OF_BUFFER - SIZE_OF_UINT32 ) < len ) {
        return -1;  //no place for CRC
    }

    //const char *data = "Hello, CRC!";
    CRC = crc32( 0L, (const Bytef*)buffer, (uInt)len );

    printf("cCRC32 value: 0x%08X\n", CRC );

    memcpy( &rCRC, buffer + len, sizeof( rCRC ) );
    //ensure proper byte order (if needed)
    rCRC = be32toh( rCRC );

    printf("rCRC32 value: 0x%08X\n", rCRC );

    return ( CRC == rCRC ) ? 0 : -2;	//invalid incoming CRC

}


void unpack_command( char* proc, unsigned* times ) {
    *proc = '@';
    *times = 0;
}


int main() {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_len = sizeof( clientAddr );
    char buffer[SIZE_OF_BUFFER];

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( sockfd < 0 ) {
        perror("Error in socket");
        exit( 1 );
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons( PORT );
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if ( bind( sockfd, (struct sockaddr *)&serverAddr, sizeof( serverAddr ) ) < 0 ) {
        perror("Error in binding");
        exit( 1 );
    }

    printf("UDP server is listening on port %d...\n", PORT );

    while ( 1 ) {

        memset( buffer, 0, SIZE_OF_BUFFER );

        // Receive data from the client
        int bytesReceived = recvfrom( sockfd, buffer, SIZE_OF_BUFFER, 0, (struct sockaddr *)&clientAddr, &addr_len );

        if ( bytesReceived < 0 ) {
            perror("Error in recvfrom");
            exit( 1 );
        }

        //printf("Received from client: %d, %s\r\n", bytesReceived, buffer );
        printf("Received from client: %d, ", bytesReceived );
	for ( int i = 0; i < ( bytesReceived - SIZE_OF_UINT32 ); i++ ) {
		printf("%c",  buffer[i] );
	}
        printf("\r\n");

        // You can process the received data here

        // Send a response back to the client (optional)
        //sendto( sockfd, "Message received", strlen("Message received"), 0, (struct sockaddr *)&clientAddr, addr_len);
        sendto( sockfd, buffer, bytesReceived, 0, (struct sockaddr *)&clientAddr, addr_len);

        if ( validate_command( buffer, bytesReceived ) ) continue;

        int fexit = 1;
        for ( int i = 0; i < strlen( (const char*)&sexit ); i++ ) {
            if ( sexit[i] != buffer[i] ) {
                fexit = 0;
                break;
            }
        }
        if ( fexit ) exit(0);

        unpack_command( &proc, &times );

        gettimeofday( &start_time, NULL );
        for ( unsigned u = 0; u < times; u++ ) {
            evaluate( proc );
        }
        gettimeofday( &finnish_time, NULL );
        send_time( &start_time, &finnish_time );

    }

    close( sockfd );

    return 0;

}

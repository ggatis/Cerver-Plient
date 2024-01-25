//v.01
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <zlib.h>                   //include the zlib library for crc32
#include <sys/time.h>               //include the <sys/time.h> header for gettimeofday()
#include "server.h"


//default port
//#define PORT 12345


char                buffer[SIZE_OF_BUFFER];

struct timeval      start_time,
                    finnish_time;

int                 comm_error    = 0;
int                 bytesReceived = 0;
int                 bytesToSend   = 0;
uint32_t            transaction   = 0;
uint8_t             done          = 0;
uint8_t             msg_topic     = 0;
uint32_t            msg_crc       = 0;
uint16_t            main_arg_index;
uint32_t            main_chunk_index;
uint8_t*            Incoming      = NULL;
uint8_t*            inc_data      = NULL;
size_t              dataindex;
size_t              msg_total_payload_size;

//1st kind of messages. msg_crc is hanging!
msg_topic_init_t*   pmsg_header;
//2nd kind of messages. msg_crc is hanging! what if the size is not exactly the size to fit
msg_topic_data_t*   pmsg_data;
//3rd kind of messages. msg_crc is hanging!
msg_topic_done_t*   pmsg_done;
//1st kind of messages. msg_crc is hanging!
msg_topic_init_t    rl_header;
//2nd kind of messages. msg_crc is hanging! what if the size is not exactly the size to fit
msg_topic_data_t    rl_data;
//3rd kind of messages. msg_crc is hanging!
msg_topic_done_t    rl_done;


//functional part

//analizeet atbildi!
void* preallocate_Dspace( size_t size ) {
    if ( NULL != Incoming ) {
        TEST_MATRIX_A = NULL;
        TEST_MATRIX_B = NULL;
        free( Incoming );
    }
    Incoming = (uint8_t*)malloc( size );
    return (void*)Incoming;
}


//analizeet atbildi!
void* preallocate_Cspace( size_t size ) {
    if ( NULL != TEST_MATRIX_C ) free( TEST_MATRIX_C );
    TEST_MATRIX_C = (int32_t*)malloc( size );
    return (void*)TEST_MATRIX_C;
}


void deallocate_space( void ) {
    if ( NULL != Incoming ) free( Incoming );
    if ( NULL != TEST_MATRIX_C ) free( TEST_MATRIX_C );
}


//and extract the vital data
int validate_command( char* buffer, size_t size ) {
    uint32_t CRC, rCRC;

    unsigned len = size - SIZE_OF_UINT32;
    printf("validate_command len: %d\r\n", len );

    if ( ( SIZE_OF_BUFFER - SIZE_OF_UINT32 ) < len ) {
        return -1;  //no place for CRC
    }

    memcpy( &rCRC, buffer + len, sizeof( rCRC ) );
    //ensure proper byte order (if needed)
    //rCRC = be32toh( rCRC );
    msg_crc = rCRC;
    printf("rCRC32 value: 0x%08X\n", rCRC );

    //const char *data = "Hello, CRC!";
    CRC = crc32( 0L, (const Bytef*)buffer, (uInt)len );
    if ( CRC != rCRC ) {
        printf("cCRC32 value: 0x%08X\n", CRC );
        return -2;      //invalid incoming CRC
    }

    msg_topic = buffer[0];
    switch ( msg_topic ) {
    case TOPIC_INIT:
        printf("validate_command TOPIC_INIT\r\n");
        transaction = rCRC;
        pmsg_header = (msg_topic_init_t*)buffer;

        rl_header.msg_topic             = pmsg_header->msg_topic;
        rl_header.msg_kind              = pmsg_header->msg_kind;
        rl_header.repeats               = pmsg_header->repeats;
        rl_header.payload_parm_cnt      = pmsg_header->payload_parm_cnt;
        rl_header.total_payload_size    = pmsg_header->total_payload_size;

        printf("msg_topic:              %d\r\n", rl_header.msg_topic );
        printf("msg_kind:               %d\r\n", rl_header.msg_kind );
        printf("repeats:                %d\r\n", rl_header.repeats );
        printf("payload_parm_cnt:       %d\r\n", rl_header.payload_parm_cnt );
        printf("total_payload_size:     %lu\r\n", (unsigned long)rl_header.total_payload_size );
        printf("msg_crc:                %08X\r\n", msg_crc );

        msg_total_payload_size          = rl_header.total_payload_size;
        break;

    case TOPIC_DATA:
        printf("validate_command TOPIC_DATA\r\n");
        pmsg_data = (msg_topic_data_t*)buffer;

        rl_data.msg_topic               = pmsg_data->msg_topic;
        rl_data.response                = pmsg_data->response;
        rl_data.current_payload_size    = pmsg_data->current_payload_size;
        rl_data.origin_crc              = pmsg_data->origin_crc;
        rl_data.arg_index               = pmsg_data->arg_index;
        rl_data.chunk_index             = pmsg_data->chunk_index;
        //apspeeleet!!!
        inc_data                        = (uint8_t*)( buffer + sizeof( msg_topic_data_t ) );

        printf("msg_topic:             %d\r\n",   rl_data.msg_topic );
        printf("msg_response:          %d\r\n",   rl_data.response );
        printf("current_payload_size:  %d\r\n",   rl_data.current_payload_size );
        printf("origin_crc:            %08X\r\n", rl_data.origin_crc );
        printf("arg_index:             %d\r\n",   rl_data.arg_index );
        printf("chunk_index:           %d\r\n",   rl_data.chunk_index );
        printf("msg_crc:               %08X\r\n", msg_crc );
        printf("inc_data:              %p\r\n",   (void*)inc_data );

        break;

    case TOPIC_DONE:    //data transfer done
        printf("validate_command TOPIC_DONE\r\n");
        //serveris pats shos suuta, nevajadzeetu sanjemt
        break;

    case TOPIC_BUSY:    //server is busy processing data, a lot of log info: freeram, temp, voltages, frequencies
        printf("validate_command TOPIC_BUSY\r\n");
        //serveris pats shos suuta, nevajadzeetu sanjemt

        break;

    default:
        printf("Complete invalid option\r\n");
        break;
    }

    return 0;

}


void make_data_request( void ) {
    pmsg_data = (msg_topic_data_t*)buffer;

    pmsg_data->msg_topic             = TOPIC_DATA;
    pmsg_data->response              = 0;
    pmsg_data->current_payload_size  = sizeof( msg_topic_data_t ) + sizeof( msg_crc );
    pmsg_data->origin_crc            = transaction;
    pmsg_data->arg_index             = main_arg_index;
    pmsg_data->chunk_index           = main_chunk_index;
    msg_crc                          = crc32( 0L, (const Bytef*)buffer, (uInt)sizeof( msg_topic_data_t ) );

    memcpy( buffer + sizeof( msg_topic_data_t ), &msg_crc, sizeof( msg_crc ) );

    bytesToSend                      = pmsg_data->current_payload_size;

    printf("TX make_data_request\r\n");
    printf("msg_topic:              %d\r\n",   pmsg_data->msg_topic );
    printf("msg_response:           %d\r\n",   pmsg_data->response );
    printf("current_payload_size:   %d\r\n",   pmsg_data->current_payload_size );
    printf("origin_crc:             %08X\r\n", pmsg_data->origin_crc );
    printf("arg_index:              %d\r\n",   pmsg_data->arg_index );
    printf("chunk_index:            %d\r\n",   pmsg_data->chunk_index );
    printf("msg_crc:                %08X\r\n", msg_crc );
}


void make_data_transmission( void ) {
    pmsg_data = (msg_topic_data_t*)buffer;

    pmsg_data->msg_topic             = TOPIC_DATA;

    uint32_t chunks = rl_header.total_payload_size / SIZE_OF_DATA;
    chunks += ( rl_header.total_payload_size >  chunks * (uint64_t)SIZE_OF_DATA ) ? 1 : 0;
    //printf("chunks:                 %d\r\n", chunks );

    pmsg_data->chunk_index = ( rl_data.chunk_index >= chunks ) ? ( chunks - 1 ) : rl_data.chunk_index;

    pmsg_data->response              = ( ( chunks - 1 ) == pmsg_data->chunk_index ) ? 1 : 0;
    pmsg_data->arg_index             = 0;

    size_t offset = pmsg_data->chunk_index * SIZE_OF_DATA;
    uint16_t size = ( pmsg_data->response ) ? ( rl_header.total_payload_size - offset ) : SIZE_OF_DATA;
    //printf("offset:                 %lu\r\n", offset );
    //printf("size:                   %d\r\n",  size );

    pmsg_data->current_payload_size  = sizeof( msg_topic_data_t ) + size + sizeof( msg_crc );
    pmsg_data->origin_crc            = transaction;

    memcpy( buffer + sizeof( msg_topic_data_t ), &((uint8_t*)TEST_MATRIX_C)[offset], size );

    msg_crc                          = crc32( 0L, (const Bytef*)buffer, (uInt)( sizeof( msg_topic_data_t ) + size ) );

    memcpy( buffer + sizeof( msg_topic_data_t ) + size, &msg_crc, sizeof( msg_crc ) );

    bytesToSend                      = pmsg_data->current_payload_size;

    printf("TX make_data_transmission\r\n");
    printf("msg_topic:              %d\r\n",   pmsg_data->msg_topic );
    printf("msg_response:           %d\r\n",   pmsg_data->response );
    printf("current_payload_size:   %d\r\n",   pmsg_data->current_payload_size );
    printf("origin_crc:             %08X\r\n", pmsg_data->origin_crc );
    printf("arg_index:              %d\r\n",   pmsg_data->arg_index );
    printf("chunk_index:            %d\r\n",   pmsg_data->chunk_index );
    printf("msg_crc:                %08X\r\n", msg_crc );
}


//make_error_response( ERR_UNPACK, result );
void make_error_response( uint8_t err_kind, uint8_t result ) {
    pmsg_data = (msg_topic_data_t*)buffer;

    pmsg_data->msg_topic             = err_kind;
    pmsg_data->response              = result;
    pmsg_data->current_payload_size  = sizeof( msg_topic_data_t ) + sizeof( msg_crc );
    pmsg_data->origin_crc            = transaction;
    pmsg_data->arg_index             = main_arg_index;
    pmsg_data->chunk_index           = main_chunk_index;
    msg_crc                          = crc32( 0L, (const Bytef*)buffer, (uInt)sizeof( msg_topic_data_t ) );
    memcpy( buffer + sizeof( msg_topic_data_t ), &msg_crc, sizeof( msg_crc ) );

    bytesToSend                      = pmsg_data->current_payload_size;

    printf("TX make_error_response\r\n");
    printf("msg_topic:              %d\r\n",   pmsg_data->msg_topic );
    printf("msg_response:           %d\r\n",   pmsg_data->response );
    printf("current_payload_size:   %d\r\n",   pmsg_data->current_payload_size );
    printf("origin_crc:             %08X\r\n", pmsg_data->origin_crc );
    printf("arg_index:              %d\r\n",   pmsg_data->arg_index );
    printf("chunk_index:            %d\r\n",   pmsg_data->chunk_index );
    printf("msg_crc:                %08X\r\n", msg_crc );
}


//make_done_response();
void make_done_response( uint64_t time_us, size_t size ) {
    pmsg_done = (msg_topic_done_t*)buffer;

    pmsg_done->msg_topic             = TOPIC_DONE;
    pmsg_done->time_us               = time_us;
    pmsg_done->total_payload_size    = size;
    pmsg_done->origin_crc            = transaction;
    msg_crc                          = crc32( 0L, (const Bytef*)buffer, (uInt)sizeof( msg_topic_done_t ) );
    memcpy( buffer + sizeof( msg_topic_done_t ), &msg_crc, sizeof( msg_crc ) );

    bytesToSend                      = sizeof( msg_topic_done_t ) + sizeof( msg_crc );

    printf("TX make_done_response\r\n");
    printf("msg_topic:              %d\r\n",   pmsg_done->msg_topic );
    printf("time_us     :           %lu\r\n",  pmsg_done->time_us );
    printf("total_payload_size:     %lu\r\n",  pmsg_done->total_payload_size );
    printf("origin_crc:             %08X\r\n", pmsg_done->origin_crc );
    printf("msg_crc:                %08X\r\n", msg_crc );

    rl_header.total_payload_size     = pmsg_done->total_payload_size;
}


//send_results( result, &start_time, &finnish_time );
void send_results( uint8_t result ) {
//void send_results( struct timeval* start_time, struct timeval* finnish_time ) {
    //globaalie start_time, finnish_time
    //in ms
    uint64_t elapsed_time_us = 1000000UL * (uint64_t )( finnish_time.tv_sec - start_time.tv_sec ) +
        (uint64_t)( finnish_time.tv_usec - start_time.tv_usec );

    printf("Time elapsed: %lu microseconds\n", elapsed_time_us );
    //peec header paariet uz TOPIC_DONE!!!
    
    //debug C
    if ( TEST_MATRIX_C ) {
        for ( uint32_t i = 0; i < TEST_MATRIX_rowsC; i++ ) {
            for ( uint32_t j = 0; j < TEST_MATRIX_columnsC; j++ ) {
                printf("%6d\t", TEST_MATRIX_C[ j +  i * TEST_MATRIX_columnsC ] );
            }
            printf("\r\n");
        }
    } else {
        printf("TEST_MATRIX_C: %p\r\n", (void*)TEST_MATRIX_C );
    }
    //debug

    if ( done ) {
        make_done_response( elapsed_time_us,
            ((uint64_t)TEST_MATRIX_rowsC)*((uint64_t)TEST_MATRIX_columnsC)*((uint64_t)sizeof(int32_t)) );

        dataindex = 0;
        main_arg_index = 0;
        main_chunk_index = 0;

    } else {
        printf("Data processing problem: %d\r\n", result );
        make_error_response( ERR_PROC, result );
    }
}

/*************************************************************
**                      INTERFACE                           **
*************************************************************/

void server_error_print( int err_code ) {
    printf( "Server error: %d!", err_code );
}


int                 sockfd;
struct sockaddr_in  serverAddr, clientAddr;
socklen_t           addr_len;


int server_init( uint16_t port ) {

    addr_len = sizeof( clientAddr );

    //Create an UDP socket
    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
    if ( sockfd < 0 ) {
        perror("Error in socket");
        return 1;
    }

    //Configure server address
    serverAddr.sin_family       = AF_INET;
    serverAddr.sin_port         = htons( port );
    serverAddr.sin_addr.s_addr  = INADDR_ANY;

    //Bind the socket to the server address
    if ( bind( sockfd, (struct sockaddr *)&serverAddr, sizeof( serverAddr ) ) < 0 ) {
        perror("Error in binding");
        return 2;
    }

    printf("UDP server is listening on port %d...\n", port );

    return 0;
}


//int server_init( void ) {
//  return server_init( PORT );
//}


int server_del( void ) {
    deallocate_space();
    close( sockfd );
    return 0;
}


int server_loop( void ) {
    //memset( buffer, 0, SIZE_OF_BUFFER );  //?

    //pielikt noildzi gadiijumaa, ja pieprasa datus no klienta
    bytesReceived = recvfrom( sockfd, buffer, SIZE_OF_BUFFER, 0, (struct sockaddr *)&clientAddr, &addr_len );

    if ( bytesReceived < 0 ) {
        perror("Error in recvfrom");
        return 1;
    }
    //printf("Received from client: %d, %s\r\n", bytesReceived, buffer );
    printf("Received: %d", bytesReceived );
    for ( int i = 0; i < ( bytesReceived - SIZE_OF_UINT32 ); i++ ) {
        if ( 0 == ( i % 4 ) ) printf(" ");
        if ( 0 == ( i % 32 ) ) printf("\r\n");
        printf("%02X", (uint8_t)buffer[i] );
    }
    printf("\r\n");

    //WTH?
    if ( ( comm_error = validate_command( buffer, bytesReceived ) ) ) {
        //something weird
        printf("validate_command: %d\r\n", comm_error );
        return 2;
    }

    //analyse this!
    //iestatiit buffer un bytesReceived
    switch ( msg_topic ) {
    case TOPIC_INIT:
        printf("main TOPIC_INIT\r\n");
        done = 0;

        if ( preallocate_Dspace( msg_total_payload_size ) ) {
            dataindex = 0;
            main_arg_index = 0;
            main_chunk_index = 0;
            make_data_request();
        } else {
            //nav vietas - nosuutiit sorry
            bytesToSend = 0;
        }
        
        break;

    case TOPIC_DATA:
        printf("main TOPIC_DATA\r\n");
        //printf("msg_topic:              %d\r\n", rl_data.msg_topic );
        //printf("msg_response:           %d\r\n", rl_data.response );
        //printf("current_payload_size:   %d\r\n", rl_data.current_payload_size );
        //printf("origin_crc:             %08X\r\n", rl_data.origin_crc );
        //printf("arg_index:              %d\r\n", rl_data.arg_index );
        //printf("chunk_index:            %d\r\n", rl_data.chunk_index );
        //printf("msg_crc:                %08X\r\n", msg_crc );
        //printf("inc_data:               %p\r\n", (void*)inc_data );

        if ( done ) {
            //ienaakosho infu uzskata par ticamu - ko prasa, to suuta prom
            make_data_transmission();
        } else {
            //vai tas ir tas, kas prasiits
            if ( ( main_arg_index == rl_data.arg_index ) || ( main_chunk_index == rl_data.chunk_index ) ) {
                //savaakt datus, advanceeties
                //nolikt datus
                //printf("rl_data.current_payload_size: %d\r\n", rl_data.current_payload_size );
                //sizeof( msg_crc ) nav payloadaa
                for ( uint16_t i = 0;
                    i < ( rl_data.current_payload_size - sizeof( msg_topic_data_t ) ); )
                    Incoming[dataindex++] = inc_data[i++];

                if ( rl_data.response ) {
                    //peedeejais gabals tekoshajam argumentam - ir veel argumenti?
                    ++main_arg_index;
                    if ( main_arg_index < rl_header.payload_parm_cnt ) {
                        //next pls
                        main_chunk_index = 0;
                        make_data_request();
                    } else {
                        //dati sanjemti: izpakot, uzzinaat lielumum, izdaliit un palaist
                        //typedef int (*unpacker_func_t)( uint8_t* buffer, size_t size ); //jaatgriezh 0, ja OK
                        //extern const unpacker_func_t      unpackers[PROCEDURE_COUNT];       //in unpackers.c
                    
                        //printf("Incoming: %lu\r\n", (long unsigned int)dataindex );
                        //for ( int i = 0; i < dataindex; i++ ) {
                        //    if ( 0 == ( i % 4 ) ) printf(" ");
                        //    if ( 0 == ( i % 32 ) ) printf("\r\n");
                        //    printf("%02X", Incoming[i] );
                        //}
                        //printf("\r\n");
                        //int result = unpackers[rl_header.msg_kind]( &Incoming[0],

                        int result = unpackers[rl_header.msg_kind]( &Incoming[0],
                        rl_header.total_payload_size );
                        if ( result ) {
                            //nebuus - noformeet sorry paku un palaist atpakalj
                            printf("Data unpacking problem: %d\r\n", result );
                            make_error_response( ERR_UNPACK, result );
                            bytesToSend = 0;
                        } else {
                            //analizeet atbildi
                            if ( preallocate_Cspace( TEST_MATRIX_rows * TEST_MATRIX_columnsC * sizeof( int32_t ) ) ) {

                                gettimeofday( &start_time, NULL );
                                for ( unsigned u = 0; u < rl_header.repeats; u++ ) {
                                    //typedef int (*woodpecker_func_t)( void );                       //jaatgriezh 0, ja OK
                                    //extern const woodpecker_func_t    woodpeckers[PROCEDURE_COUNT];     //in woodpeckers.c
                                    result = woodpeckers[rl_header.msg_kind]();
                                    if ( result ) {
                                        //nebuus - noformeet sorry paku un palaist atpakalj
                                        break;
                                    }

                                }
                                gettimeofday( &finnish_time, NULL );
                                done = result ? 0 : 255;
                                send_results( result ); //noformeet INIT paku un paarsleegties uz DONE
                                printf("bytesToSend: %d\r\n", bytesToSend );

                            } else {
                                //nebuus - noformeet sorry paku un palaist atpakalj
                                bytesToSend = 0;
                            }
                        }
                    }
                } else {
                    ++main_chunk_index;
                    make_data_request();
                }
            
            } else {
                make_data_request();
            }
        }

        break;

    case TOPIC_DONE:    //data transfer done
        printf("TOPIC_DONE\r\n");
        break;

    case TOPIC_BUSY:    //server is busy processing data, a lot of log info: freeram, temp, voltages, frequencies
        printf("TOPIC_BUSY\r\n");
        break;

    default:
        printf("Complete invalid option\r\n");
        break;
    }

    if ( bytesToSend )
        sendto( sockfd, buffer, bytesToSend, 0, (struct sockaddr *)&clientAddr, addr_len );

    return 0;

}

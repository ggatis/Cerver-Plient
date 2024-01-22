#ifndef MESSAGES_A_H
#define MESSAGES_A_H


#include <stdint.h>
#include <stddef.h>

#define SIZE_OF_UINT8   sizeof( uint8_t )
#define SIZE_OF_UINT16  sizeof( uint16_t )
#define SIZE_OF_UINT32  sizeof( uint32_t )
#define SIZE_OF_SIZE    sizeof( size_t )
#define SIZE_OF_DATA    1024
#define SIZE_OF_SDATA   ( SIZE_OF_UINT8 + SIZE_OF_UINT8 + SIZE_OF_UINT16 + SIZE_OF_UINT32 + SIZE_OF_UINT16 + SIZE_OF_UINT16 + SIZE_OF_UINT32 )
#define SIZE_OF_BUFFER  ( SIZE_OF_DATA + SIZE_OF_SDATA )


typedef enum {
    TOPIC_INIT              //initialise the conversation
    ,TOPIC_DATA             //data transfer specific for INIT origin_crc
    ,TOPIC_DONE             //data transfer done
    ,TOPIC_BUSY             //server is busy processing data, a lot of log info: freeram, temp, voltages, frequencies
    ,ERR_UNPACK
    ,ERR_PROC
    //,TOPIC_STOP = 255     //terminate server - is it a good idea?
} msg_topic_t;

typedef enum {
    TEST_MATRIX_SUM_CPU,    //check A+B on CPU
    TEST_MATRIX_SUM_PPU,    //check A+B on PPU
    TEST_VECTOR_SUM_CPU,    //check A+B on CPU
    TEST_VECTOR_SUM_PPU,    //check A+B on PPU
    TEST_CNN1_CPU,          //check CNN on CPU
    TEST_CNN1_PPU,          //check CNN on PPU
    PROCEDURE_COUNT         //count; kill also
} msg_kind_t;


//client structures
//msg_topic, msg_kind aka function, repeats, payload_parm_cnt, total_payload_size, crc
typedef struct __attribute__((__packed__)) {
    uint8_t     msg_topic;
    uint8_t     msg_kind;
    uint32_t    repeats;
    uint8_t     payload_parm_cnt;
    size_t      total_payload_size;
//    uint32_t    crc;
} msg_header_t;     //0 == msg_topic

//msg_topic, response, current_payload_size, origin_crc, arg_index, chunk_index, data, crc
typedef struct __attribute__((__packed__)) {
    uint8_t      msg_topic;
    uint8_t      response;  //not used at a time
    uint16_t     current_payload_size;
    uint32_t     origin_crc;
    uint16_t     arg_index;
    uint16_t     chunk_index;
//    uint8_t      data[];
//    uint32_t     crc;     //crc buus pakaljaa
} msg_data_t;       //0 != msg_topic and server replies



#define SIZE_OF_MSG_HEADER sizeof( msg_header_t );

typedef int (*unpacker_func_t)( uint8_t* buffer, size_t size ); //jaatgriezh 0, ja OK
typedef int (*woodpecker_func_t)( void );                       //jaatgriezh 0, ja OK


//external variables
extern const unpacker_func_t      unpackers[PROCEDURE_COUNT];       //in unpackers.c
extern const woodpecker_func_t    woodpeckers[PROCEDURE_COUNT];     //in woodpeckers.c


//function declarations


#endif //MESSAGES_A_H

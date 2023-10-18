#ifndef MESSAGES_H
#define MESSAGES_H


#include <stddef.h>             //size_t


typedef enum {
    TEST_MATRIX_MUL_CPU,
    TEST_MATRIX_MUL_PPU,
    TEST_CNN1_CPU,
    TEST_CNN1_PPU,
    PROCEDURE_COUNT
} msg_kind_t;


typedef struct __attribute__((__packed__)) {
    char        msg_kind_c;
    char        repeats[3];
    char        payload_size[8];
    char        zero;
    uint32_t    crc
} msg_header_t;


typedef int (*unpacker_func_t)( uint8_t* buffer, size_t size ); //jaatgriezh 0, ja OK
typedef int (*woodpecker_func_t)( void );                       //jaatgriezh 0, ja OK


extern unpacker_func_t      unpackers[PROCEDURE_COUNT];
extern woodpecker_func_t    woodpeckers[PROCEDURE_COUNT];


//external variables


//function declarations


#endif //MESSAGES_H

//a link between two worlds
//#include <stdio.h>          //tikai debagam!!!
#include "unpackers.h"
#include "woodpeckers.h"    


//messages_A.h
/* what we have *
typedef enum {
    TEST_MATRIX_SUM_CPU,    //check A+B on CPU
    TEST_MATRIX_SUM_PPU,        //check A+B on PPU
    TEST_VECTOR_SUM_CPU,    //check A+B on CPU
    TEST_VECTOR_SUM_PPU,        //check A+B on PPU
    TEST_CNN1_CPU,      //check CNN on CPU
    TEST_CNN1_PPU,      //check CNN on PPU
    PROCEDURE_COUNT     //count; kill also
} msg_kind_t;

typedef int (*unpacker_func_t)( uint8_t* buffer, size_t size ); //jaatgriezh 0, ja OK
*/

//unpackers.h
/*
typedef struct __attribute__((__packed__)) {
    uint32_t     matrix_rows;
    uint32_t     matrix_cols;
    uint8_t      data[];
} msg_data_header_RCAA_t;    //0 != msg_topic and server replies
*/


//for the greater good
size_t AI_size( uint64_t rows, uint64_t columns, uint32_t element_size ) {
    return rows * columns * element_size;
}


//here we proudly present our unpackers
//do not try to access while woodpecker is working!

//TEST_MATRIX_SUM_CPU,  //check A+B on CPU
int uTEST_MATRIX_SUM_CPU( uint8_t* buffer, size_t size ) {

    msg_data_header_RCAAI_t* msg_data_header_RCAAI = (msg_data_header_RCAAI_t*)buffer;

    if ( size < ( 2 * SIZE_OF_UINT32 + 2 * sizeof( int32_t ) ) )
        //sizeof( msg_data_header_RCAAI_t ) + 2 * AI_size( 1, 1, sizeof( int32_t ) )
        return -1;  //size too small

    TEST_MATRIX_rows    = msg_data_header_RCAAI->matrix_rows;
    TEST_MATRIX_columns = msg_data_header_RCAAI->matrix_cols;

//    printf("OP buffer:            %p\r\n", (void*)buffer );
//    printf("TEST_MATRIX_rows:     %u\r\n", TEST_MATRIX_rows );
//    printf("TEST_MATRIX_columns:  %u\r\n", TEST_MATRIX_columns );

    //R, C, A, A
    if ( size != ( SIZE_OF_UINT32 + SIZE_OF_UINT32 +
        2 * AI_size( TEST_MATRIX_rows, TEST_MATRIX_columns, sizeof( int32_t ) ) ) )
        return -2;  //size differs from expected

    TEST_MATRIX_rowsC    = TEST_MATRIX_rows;
    TEST_MATRIX_columnsC = TEST_MATRIX_columns;

    //A
    TEST_MATRIX_A = (int32_t*)&msg_data_header_RCAAI->data[0];
    //skip A
    TEST_MATRIX_B = (int32_t*)&msg_data_header_RCAAI->data[
        AI_size( TEST_MATRIX_rows, TEST_MATRIX_columns, sizeof( int32_t ) ) ];
    return 0;
}

//TEST_MATRIX_SUM_PPU,        //check A+B on PPU
int uTEST_MATRIX_SUM_PPU( uint8_t* buffer, size_t size ) {
//actually the same; left unique since it could demand different handling of data

    msg_data_header_RCAAI_t* msg_data_header_RCAAI = (msg_data_header_RCAAI_t*)buffer;

    if ( size < ( 2 * SIZE_OF_UINT32 + 2 * sizeof( int32_t ) ) )
        //sizeof( msg_data_header_RCAAI_t ) + 2 * AI_size( 1, 1, sizeof( int32_t ) )
        return -1;  //size too small

    TEST_MATRIX_rows     = msg_data_header_RCAAI->matrix_rows;
    TEST_MATRIX_columns  = msg_data_header_RCAAI->matrix_cols;

    //R, C, A, A
    if ( size != ( SIZE_OF_UINT32 + SIZE_OF_UINT32 +
        2 * AI_size( TEST_MATRIX_rows, TEST_MATRIX_columns, sizeof( int32_t ) ) ) )
        return -2;  //size differs from expected

    TEST_MATRIX_rowsC    = TEST_MATRIX_rows;
    TEST_MATRIX_columnsC = TEST_MATRIX_columnsC;

    //A
    TEST_MATRIX_A = (int32_t*)&msg_data_header_RCAAI->data[0];
    //skip A
    TEST_MATRIX_B = (int32_t*)&msg_data_header_RCAAI->data[
        AI_size( TEST_MATRIX_rows, TEST_MATRIX_columns, sizeof( int32_t ) ) ];
    return 0;
}

//TEST_VECTOR_SUM_CPU,  //check A+B on CPU
int uTEST_VECTOR_SUM_CPU( uint8_t* buffer, size_t size ) {

    msg_data_header_RVVI_t* msg_data_header_RVVI = (msg_data_header_RVVI_t*)buffer;

    if ( size < ( SIZE_OF_UINT32 + 2 * sizeof( int32_t ) ) )
        //sizeof( msg_data_header_RVVI_t ) + 2 * AI_size( 1, 1, sizeof( int32_t ) )
        return -1;  //size too small

    TEST_MATRIX_rows     = msg_data_header_RVVI->vector_elements;
    TEST_MATRIX_columns  = 1;

    //R, V, V
    if ( size != ( SIZE_OF_UINT32 +
        2 * AI_size( TEST_MATRIX_rows, 1, sizeof( int32_t ) ) ) )
        return -2;  //size differs from expected

    TEST_MATRIX_rowsC    = TEST_MATRIX_rows;
    TEST_MATRIX_columnsC = TEST_MATRIX_columnsC;

    //A
    TEST_MATRIX_A = (int32_t*)&msg_data_header_RVVI->data[0];
    //Skip A
    TEST_MATRIX_B = (int32_t*)&msg_data_header_RVVI->data[
        AI_size( TEST_MATRIX_rows, 1, sizeof( int32_t ) ) ];
    return 0;
}


//TEST_VECTOR_SUM_PPU,        //check A+B on PPU
int uTEST_VECTOR_SUM_PPU( uint8_t* buffer, size_t size ) {
//actually the same; left unique since it could demand different handling of data

    msg_data_header_RVVI_t* msg_data_header_RVVI = (msg_data_header_RVVI_t*)buffer;

    if ( size < ( SIZE_OF_UINT32 + 2 * sizeof( int32_t ) ) )
        //sizeof( msg_data_header_RVVI_t ) + 2 * AI_size( 1, 1, sizeof( int32_t ) )
        return -1;  //size too small

    TEST_MATRIX_rows     = msg_data_header_RVVI->vector_elements;
    TEST_MATRIX_columns  = 1;

    //R, V, V
    if ( size != ( SIZE_OF_UINT32 +
        2 * AI_size( TEST_MATRIX_rows, 1, sizeof( int32_t ) ) ) )
        return -2;  //size differs from expected

    TEST_MATRIX_rowsC    = TEST_MATRIX_rows;
    TEST_MATRIX_columnsC = TEST_MATRIX_columnsC;

    //A
    TEST_MATRIX_A = (int32_t*)&msg_data_header_RVVI->data[0];
    //Skip A
    TEST_MATRIX_B = (int32_t*)&msg_data_header_RVVI->data[
        AI_size( TEST_MATRIX_rows, 1, sizeof( int32_t ) ) ];
    return 0;
}

//TEST_CNN1_CPU,        //check CNN on CPU
int uTEST_CNN1_CPU( uint8_t* buffer, size_t size ) {
    return 0;
}

//TEST_CNN1_PPU,        //check CNN on PPU
int uTEST_CNN1_PPU( uint8_t* buffer, size_t size ) {
    return 0;
}

//the solt of the file
const unpacker_func_t unpackers[PROCEDURE_COUNT] = {
    uTEST_MATRIX_SUM_CPU,
    uTEST_MATRIX_SUM_PPU,
    uTEST_VECTOR_SUM_CPU,
    uTEST_VECTOR_SUM_PPU,
    uTEST_CNN1_CPU,
    uTEST_CNN1_PPU
};

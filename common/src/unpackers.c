//a link between two worlds
//#include <stdio.h>          //tikai debagam!!!
#include "unpackers.h"
#include "woodpeckers.h"    


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


//here we proudly present our unpackers
//try to access while woodpecker is working and you will enter wonderland!

//TEST_MATRIX_SUM_CPU,  //check A+B on CPU
int uTEST_MATRIX_SUM_CPU( uint8_t* buffer, size_t size ) {
    if ( size < ( 2 * SIZE_OF_UINT32 + 2 * sizeof( int32_t ) ) )
        return -1;  //size too small
    TEST_MATRIX_rows = *(uint32_t*)&buffer[0];
    TEST_MATRIX_columns = *(uint32_t*)&buffer[SIZE_OF_UINT32];

//    printf("OP buffer:            %p\r\n", (void*)buffer );
//    printf("TEST_MATRIX_rows:     %u\r\n", TEST_MATRIX_rows );
//    printf("TEST_MATRIX_columns:  %u\r\n", TEST_MATRIX_columns );

    if ( size != ( 2 * ( SIZE_OF_UINT32 + TEST_MATRIX_rows * TEST_MATRIX_columns * sizeof( int32_t ) ) ) )
        return -2;  //size differs from expected

    TEST_MATRIX_rowsC = TEST_MATRIX_rows;
    TEST_MATRIX_columnsC = TEST_MATRIX_columns;

    TEST_MATRIX_A = (int32_t*)&buffer[2*SIZE_OF_UINT32];
    TEST_MATRIX_B = (int32_t*)&buffer[2*SIZE_OF_UINT32+TEST_MATRIX_rows*TEST_MATRIX_columns*sizeof(int32_t)];
    return 0;
}

//TEST_MATRIX_SUM_PPU,        //check A+B on PPU
int uTEST_MATRIX_SUM_PPU( uint8_t* buffer, size_t size ) {
//actually the same; left unique since it could demand different handling of data
    if ( size < ( 2 * SIZE_OF_UINT32 + 2 * sizeof( int32_t ) ) )
        return -1;  //size too small
    TEST_MATRIX_rows = *(uint32_t*)&buffer[0];
    TEST_MATRIX_columns = *(uint32_t*)&buffer[SIZE_OF_UINT32];
    if ( size != ( 2 * ( SIZE_OF_UINT32 + TEST_MATRIX_rows * TEST_MATRIX_columns * sizeof( int32_t ) ) ) )
        return -2;  //size differs from expected

    TEST_MATRIX_rowsC = TEST_MATRIX_rows;
    TEST_MATRIX_columnsC = TEST_MATRIX_columnsC;

    TEST_MATRIX_A = (int32_t*)&buffer[2*SIZE_OF_UINT32];
    TEST_MATRIX_B = (int32_t*)&buffer[2*SIZE_OF_UINT32+TEST_MATRIX_rows*TEST_MATRIX_columns*sizeof(int32_t)];
    return 0;
}

//TEST_VECTOR_SUM_CPU,  //check A+B on CPU
int uTEST_VECTOR_SUM_CPU( uint8_t* buffer, size_t size ) {
    if ( size < ( SIZE_OF_UINT32 + 2 * sizeof( int32_t ) ) )
        return -1;  //size too small
    TEST_MATRIX_rows = *(uint32_t*)&buffer[0];
    TEST_MATRIX_columns = 1;
    if ( size != ( SIZE_OF_UINT32 + 2 * ( TEST_MATRIX_rows * sizeof( int32_t ) ) ) )
        return -2;  //size differs from expected

    TEST_MATRIX_rowsC = TEST_MATRIX_rows;
    TEST_MATRIX_columnsC = TEST_MATRIX_columnsC;

    TEST_MATRIX_A = (int32_t*)&buffer[SIZE_OF_UINT32];
    TEST_MATRIX_B = (int32_t*)&buffer[SIZE_OF_UINT32+TEST_MATRIX_rows*sizeof(int32_t)];
    return 0;
}

//TEST_VECTOR_SUM_PPU,        //check A+B on PPU
int uTEST_VECTOR_SUM_PPU( uint8_t* buffer, size_t size ) {
//actually the same; left unique since it could demand different handling of data
    if ( size < ( SIZE_OF_UINT32 + 2 * sizeof( int32_t ) ) )
        return -1;  //size too small
    TEST_MATRIX_rows = *(uint32_t*)&buffer[0];
    TEST_MATRIX_columns = 1;
    if ( size != ( SIZE_OF_UINT32 + 2 * ( TEST_MATRIX_rows * sizeof( int32_t ) ) ) )
        return -2;  //size differs from expected

    TEST_MATRIX_rowsC = TEST_MATRIX_rows;
    TEST_MATRIX_columnsC = TEST_MATRIX_columnsC;

    TEST_MATRIX_A = (int32_t*)&buffer[SIZE_OF_UINT32];
    TEST_MATRIX_B = (int32_t*)&buffer[SIZE_OF_UINT32+TEST_MATRIX_rows*sizeof(int32_t)];
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

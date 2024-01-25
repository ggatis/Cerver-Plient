//no wonders, plain prose
//#include <stdio.h>          //tikai debagam!!!
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

typedef int (*woodpecker_func_t)( void );                       //jaatgriezh 0, ja OK
*/

//globally accessible things for following functions
//TEST_MATRIX_SUM_CPU,  //check A+B on CPU
//TEST_MATRIX_SUM_PPU,  //check A+B on PPU
//TEST_VECTOR_SUM_CPU,  //check A+B on CPU
//TEST_VECTOR_SUM_PPU,  //check A+B on PPU
unsigned TEST_MATRIX_rows;
unsigned TEST_MATRIX_columns;
unsigned TEST_MATRIX_rowsC;
unsigned TEST_MATRIX_columnsC;
int32_t* TEST_MATRIX_A = NULL;
int32_t* TEST_MATRIX_B = NULL;
int32_t* TEST_MATRIX_C = NULL;

//TEST_CNN1_CPU,    //check CNN on CPU
//no idae here

//TEST_CNN1_PPU,    //check CNN on PPU
//even no clue


//here are non reentrant woodpeckers

//TEST_MATRIX_SUM_CPU,  //check A+B on CPU
int wTEST_MATRIX_SUM_CPU( void ) {
    if ( NULL == TEST_MATRIX_A ) return -1;
    if ( NULL == TEST_MATRIX_B ) return -2;
    if ( NULL == TEST_MATRIX_C ) return -3;
    uint64_t k = 0;

//    printf("RES buffer:           %p\r\n", (void*)TEST_MATRIX_C );
//    printf("TEST_MATRIX_rowsC:    %u\r\n", TEST_MATRIX_rowsC );
//    printf("TEST_MATRIX_columnsC: %u\r\n", TEST_MATRIX_columnsC );

    for ( uint32_t i = 0; i < TEST_MATRIX_rows; i++ ) {
        for ( uint32_t j = 0; j < TEST_MATRIX_columns; j++ ) {
            TEST_MATRIX_C[k] = TEST_MATRIX_A[k] + TEST_MATRIX_B[k];
            k++;
        }
    }
    return 0;
}


//TEST_MATRIX_SUM_PPU,        //check A+B on PPU
int wTEST_MATRIX_SUM_PPU( void ) {
    //?
    return 0;
}


//TEST_VECTOR_SUM_CPU,  //check A+B on CPU
int wTEST_VECTOR_SUM_CPU( void ) {
    if ( NULL == TEST_MATRIX_A ) return -1;
    if ( NULL == TEST_MATRIX_B ) return -2;
    if ( NULL == TEST_MATRIX_C ) return -3;
    for ( uint32_t i = 0; i < TEST_MATRIX_rows; i++ ) {
        TEST_MATRIX_C[i] = TEST_MATRIX_A[i] + TEST_MATRIX_B[i];
    }
    return 0;
}


//TEST_VECTOR_SUM_PPU,        //check A+B on PPU
int wTEST_VECTOR_SUM_PPU( void ) {
    return 0;
}


//TEST_CNN1_CPU,        //check CNN on CPU
int wTEST_CNN1_CPU( void ) {
    return 0;
}


//TEST_CNN1_PPU,        //check CNN on PPU
int wTEST_CNN1_PPU( void ) {
    return 0;
}


//the solt of the file
const woodpecker_func_t woodpeckers[PROCEDURE_COUNT] = {
    wTEST_MATRIX_SUM_CPU,
    wTEST_MATRIX_SUM_PPU,
    wTEST_VECTOR_SUM_CPU,
    wTEST_VECTOR_SUM_PPU,
    wTEST_CNN1_CPU,
    wTEST_CNN1_PPU
};

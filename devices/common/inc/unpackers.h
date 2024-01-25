#ifndef UNPACKERS_H
#define UNPACKERS_H


#include <stddef.h>             //size_t
#include "messages_A.h"


typedef struct __attribute__((__packed__)) {
    uint32_t     vector_elements;
    uint8_t      data[];	//automatically includes the CRC32 at the end
} msg_data_header_RVVI_t;


typedef struct __attribute__((__packed__)) {
    uint32_t     matrix_rows;
    uint32_t     matrix_cols;
    uint8_t      data[];	//automatically includes the CRC32 at the end
} msg_data_header_RCAAI_t;


//external variables


//function declarations


#endif //UNPACKERS_H


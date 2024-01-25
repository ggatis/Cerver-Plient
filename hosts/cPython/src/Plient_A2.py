#!/usr/bin/env python3
#v.01
"""
Module: Plient_A2.py
Author: Gatis Gaigals, EDI
Date:   12.12.2023.

Description:
Second experiment in data exchange. Everything is included, very plain.

Usage:
Please, do no use this. But should work with the second verion of the server.

Example:
Please see the main text.

Notes:
-
"""
import socket
import binascii
import struct
import ctypes
import numpy as np


SERVER_IP = '127.0.0.1'  # Change this to the server's IP address
SERVER_PORT = 12346


#C data sizes
SIZE_OF_UINT8  = ctypes.sizeof( ctypes.c_uint8(  0 ) )
SIZE_OF_UINT16 = ctypes.sizeof( ctypes.c_uint16( 0 ) )
SIZE_OF_UINT32 = ctypes.sizeof( ctypes.c_uint32( 0 ) )
SIZE_OF_UINT64 = ctypes.sizeof( ctypes.c_uint64( 0 ) )
SIZE_OF_SIZE   = ctypes.sizeof( ctypes.c_size_t( 0 ) )
SIZE_OF_CRC    = ctypes.sizeof( ctypes.c_uint32( 0 ) )
#print( SIZE_OF_UINT8, SIZE_OF_UINT16, SIZE_OF_UINT32, SIZE_OF_SIZE )

SIZE_OF_DATA   = 1024

#contact of 1st kind
#iekljauj msg_topic, msg_kind, repeats, payload_parm_cnt, total_payload_size
SIZE_OF_MSG_TOPIC_INIT          = \
    SIZE_OF_UINT8 + SIZE_OF_UINT8 + SIZE_OF_UINT32 + SIZE_OF_UINT8 + SIZE_OF_SIZE
#iekljauj msg_topic, msg_kind, repeats, payload_parm_cnt, total_payload_size, crc
SIZE_OF_MSG_TOPIC_INIT_CRC      = \
    SIZE_OF_MSG_TOPIC_INIT + SIZE_OF_UINT32

#contact of second kind
#iekljauj msg_topic, response, current_payload_size, origin_crc, arg_index, chunk_index
SIZE_OF_MSG_TOPIC_DATA          = \
    SIZE_OF_UINT8 + SIZE_OF_UINT8 + SIZE_OF_UINT16 + SIZE_OF_UINT32 + SIZE_OF_UINT16 + SIZE_OF_UINT32
#iekljauj msg_topic, response, current_payload_size, origin_crc, arg_index, chunk_index, crc
SIZE_OF_MSG_TOPIC_DATA_NO_DATA  = \
    SIZE_OF_MSG_TOPIC_DATA + SIZE_OF_UINT32
#iekljauj msg_topic, response, current_payload_size, origin_crc, arg_index, chunk_index, data, crc
SIZE_OF_BUFFER                  = \
    SIZE_OF_MSG_TOPIC_DATA_NO_DATA + SIZE_OF_DATA

#contact of 3rd kind
#iekljauj msg_topic, msg_time_us, total_payload_size, origin_crc
SIZE_OF_MSG_TOPIC_DONE          = \
    SIZE_OF_UINT8 + SIZE_OF_UINT64 + SIZE_OF_SIZE + SIZE_OF_UINT32
#iekljauj msg_topic, msg_time_us, total_payload_size, origin_crc, crc
SIZE_OF_MSG_TOPIC_DONE_CRC      = \
    SIZE_OF_MSG_TOPIC_DONE + SIZE_OF_UINT32



#uint16_t
TOPIC_INIT    =   0    #initialise the conversation
TOPIC_DATA    =   1    #data transfer specific for INIT origin_crc
TOPIC_DONE    =   2    #data transfer done
TOPIC_BUSY    =   3    #server is busy processing data, a lot of log info: freeram, temp, voltages, frequencies
#TOPIC_STOP    = 255    #terminate server - is it a good idea?

'''
#Unavoidable
#define SIZE_OF_BUFFER  1024
#define SIZE_OF_UINT32  sizeof( uint32_t )
#define SIZE_OF_SIZE    sizeof( size_t )

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
//    uint32_t    crc;          //crc beigaas jaahendlo
} msg_topic_init_t;      //0 == msg_topic

//msg_topic, response, current_payload_size, origin_crc, arg_index, chunk_index, data, crc
typedef struct __attribute__((__packed__)) {
    uint8_t      msg_topic;
    uint8_t      response;    //not used at a time
    uint16_t     current_payload_size;
    uint32_t     origin_crc;
    uint16_t     arg_index;
    uint32_t     chunk_index;
    uint8_t      data[];
//    uint32_t     crc;         //crc beigaas jaahendlo
} msg_topic_data_t;

//msg_topic, time_us, total_payload_size, origin_crc, chunk_index, crc
typedef struct __attribute__((__packed__)) {
    uint8_t      msg_topic;
    uint64_t     time_us;
    size_t       total_payload_size;
    uint32_t     origin_crc;
//    uint32_t     crc;         //crc beigaas jaahendlo
} msg_topic_done_t;

#define SIZE_OF_MSG_TOPIC_INIT sizeof( msg_topic_init_t );

typedef int (*unpacker_func_t)( uint8_t* buffer, size_t size ); //jaatgriezh 0, ja OK
typedef int (*woodpecker_func_t)( void );                       //jaatgriezh 0, ja OK

//external variables
extern const unpacker_func_t      unpackers[PROCEDURE_COUNT];   //in unpackers.c
extern const woodpecker_func_t    woodpeckers[PROCEDURE_COUNT]; //in woodpeckers.c
'''

TEST_MATRIX_SUM_CPU = 0
TEST_MATRIX_SUM_PPU = 1
TEST_VECTOR_SUM_CPU = 2
TEST_VECTOR_SUM_PPU = 3
TEST_CNN1_CPU       = 4
TEST_CNN1_PPU       = 5
PROCEDURE_COUNT     = 6

#errors
#pakotaaji, kljuudas zem -0
ER_SIZE1_MISMATCH    = -1
ER_SIZE2_MISMATCH    = -2
ER_SIZE3_MISMATCH    = -3
#izshkjirot!!!
ER_INCORRECT_CRC     = -4   #todo pieshkjirt zemaak
ER_INCOMPATIBLE_REP  = -5
ER_WRONG_TRANSACT    = -6
#transmit, kljuudas zem -10

#receive, kljuudas zem  -20

#atpakotaaji, kljuudas zem -30
ER_BUF_SIZE_MISMATCH = -31


#global error
comm_error               = 0
transaction              = ''
#global dynamic buffer
message                  = ''
#globals
msg_topic                = 0
#0 == msg_topic
msg_kind                 = 0
msg_repeats              = 0
msg_payload_parm_cnt     = 0
msg_total_payload_size   = 0
#1 == msg_topic
msg_response             = 0
msg_current_payload_size = 0
msg_origin_crc           = 0
msg_arg_index            = 0
msg_chunk_index          = 0
msg_data                 = ''
#
msg_time_us              = 0
#
msg_crc                  = 0
#
byte_buffer              = bytearray(b'')

#pakotaaji
def pTEST_MATRIX_SUM_CPU( A, B ):   #check A+B on CPU
    global comm_error
    #print("pTEST_MATRIX_SUM_CPU:", A, B )
    #print( A.shape, B.shape )
    size_of_Ax, size_of_Ay = A.shape
    size_of_Bx, size_of_By = B.shape
    #print( size_of_Ax, size_of_Ay, size_of_Bx, size_of_By )
    if ( size_of_Ax != size_of_Bx ):
        comm_error = ER_SIZE1_MISMATCH        #x size mismatch
        return []
    if ( size_of_Ay != size_of_By ):
        comm_error = ER_SIZE2_MISMATCH        #y size mismatch
        return []
    return [ struct.pack( "<I", size_of_Ax ),
        struct.pack( "<I", size_of_Ay ),
        A.tobytes(), B.tobytes() ]      #sheit var buut gljuks, ja cl/se baitu svariigums atshkjiraas
                                        #tad sanjmeemeejaa jaamaina

#citi pakotaaji liidziigi
packers = [ pTEST_MATRIX_SUM_CPU ]


#atpakotaaji, kljuudas zem -40
def uTEST_MATRIX_SUM_CPU( A, B ):
    global comm_error, byte_buffer
    size_of_Ax, size_of_Ay = A.shape
    #size_of_Bx, size_of_By = B.shape
    if ( len( byte_buffer ) != ( size_of_Ax * size_of_Ay * ctypes.sizeof( ctypes.c_int32( 0 ) ) ) ):
        comm_error = ER_BUF_SIZE_MISMATCH
        return 0
    array = np.frombuffer( byte_buffer, dtype = np.int32 ).reshape( ( size_of_Ax, size_of_Ay ) )
    byte_buff = b''
    return array


#citi atpakotaaji liidziigi
unpackers = [ uTEST_MATRIX_SUM_CPU ]


def compose_header( all_data ):
#sets also global msg_crc
    global transaction, msg_topic, msg_kind, msg_repeats
    global msg_payload_parm_cnt, msg_total_payload_size, msg_crc

    msg_topic = TOPIC_INIT
    msg_kind  = 0
    msg_payload_parm_cnt   = len( all_data );
    msg_total_payload_size = 0
    for i in range( msg_payload_parm_cnt ):
        msg_total_payload_size += len( all_data[i] )
    print("msg_total_payload_size:", msg_total_payload_size )

    message = \
        struct.pack( 'B', msg_topic ) + \
        struct.pack( 'B', msg_kind ) + \
        struct.pack('<I', msg_repeats ) + \
        struct.pack( 'B', msg_payload_parm_cnt ) + \
        struct.pack( 'N', msg_total_payload_size )   #'>'?

    msg_crc = binascii.crc32( message ) & 0xFFFFFFFF
    print(f" CRC32 value: {hex( msg_crc ).upper()}")

    transaction = msg_crc
    return message


def compose_data_message( chunk ):
    global transaction
    global msg_response, msg_current_payload_size, msg_origin_crc
    global msg_arg_index, msg_chunk_index, msg_data, msg_crc

    #print("chunk: ", len( chunk ), chunk )
    msg_topic = TOPIC_DATA
    message = \
        struct.pack( 'B', msg_topic ) + \
        struct.pack( 'B', msg_response ) + \
        struct.pack('<H', len( chunk ) + SIZE_OF_MSG_TOPIC_DATA ) + \
        struct.pack('<I', transaction ) + \
        struct.pack('<H', msg_arg_index ) + \
        struct.pack('<I', msg_chunk_index ) + \
        ( struct.pack( 'B' * len(chunk), *chunk ) if len( chunk ) else b'')

    msg_crc = binascii.crc32( message ) & 0xFFFFFFFF
    print("TX message: ", message )
    print(f" CRC32 value: {hex( msg_crc ).upper()}\r\n")

    return message


def validate_packet( reply ):
#return
#-1    #incorrect CRC
#-2    #incompatible server_reply
#-3    #not a current transaction
    global transaction, msg_topic, msg_kind, msg_repeats
    global msg_payload_parm_cnt, msg_total_payload_size, msg_crc
    global msg_response, msg_current_payload_size, msg_origin_crc
    global msg_arg_index, msg_chunk_index, msg_data, msg_time_us

    size_of_reply = len( reply )
    print(f"RX size: { size_of_reply }")
    print("RX: ", reply )

    #msg_topic = struct.unpack('B', reply[0] )[0]
    msg_topic = reply[0]
    print("RX msg_topic: ", msg_topic )

    rCRC32  = struct.unpack('<I', reply[-SIZE_OF_UINT32:])[0]
    msg_crc = rCRC32
    cCRC32  = binascii.crc32( reply[:-SIZE_OF_UINT32] ) & 0xFFFFFFFF
    if ( cCRC32 ^ rCRC32 ):
        print(f"rCRC32: {hex( rCRC32 ).upper()}")
        print(f"cCRC32: {hex( cCRC32 ).upper()}")
        return -41          #incorrect CRC

    if ( msg_topic ):
        if ( TOPIC_DATA == msg_topic ):
            #data exchange

            #dati nav prom suutot, jo atnaak datu pieprasiijumi
            size_of_data = size_of_reply - SIZE_OF_MSG_TOPIC_DATA_NO_DATA
            if ( 0 > size_of_data ):
                return -42  #incorrect server_reply
            #dati var ienaakt, kad ir done
            #if ( 0 < size_of_data ):
            #    print(f"size_of_data: {size_of_data}")
            #    #tas nebuus transmittaa!
            #    return -43  #data exchange unsync

            '''
            typedef struct __attribute__((__packed__)) {
                uint8_t      msg_topic;            //0
                uint8_t      response;             //1
                uint16_t     current_payload_size; //2
                uint32_t     origin_crc;           //4
                uint16_t     arg_index;            //8
                uint32_t     chunk_index;          //10
                uint8_t      data[];               //14
            //    uint32_t     crc;        //crc buus pakaljaa
            } msg_topic_data_t;        //0 != msg_topic and server replies
            '''
            msg_response             = struct.unpack_from( 'B', reply, 1 )[0]
            msg_current_payload_size = struct.unpack_from('<H', reply, 2 )[0]
            msg_origin_crc           = struct.unpack_from('<I', reply, 4 )[0]
            msg_arg_index            = struct.unpack_from('<H', reply, 8 )[0]
            msg_chunk_index          = struct.unpack_from('<I', reply,10 )[0]
            msg_data                 = reply[SIZE_OF_MSG_TOPIC_DATA:-SIZE_OF_CRC]

            print(f"msg_topic:                {msg_topic}")
            print(f"msg_response:             {msg_response}")
            print(f"msg_current_payload_size: {msg_current_payload_size}")
            print(f"msg_origin_crc:           {hex( msg_origin_crc ).upper()}")
            print(f"msg_arg_index:            {msg_arg_index}")
            print(f"msg_chunk_index:          {msg_chunk_index}")
            print(f"msg_data len:             {len( msg_data )}")
            print(f"msg_crc:                  {hex( msg_crc ).upper()}")

            if ( transaction != msg_origin_crc ):
                return -44  #not a current transaction

        elif ( TOPIC_DONE == msg_topic ):
            #nevajadzeetu buut ar datiem, jo tos sanjem DONE sadaljaa
            #data exchange

            size_of_data = size_of_reply - SIZE_OF_MSG_TOPIC_DONE_CRC
            if ( 0 > size_of_data ):
                return -45  #incorrect server_reply size
            #principaa, virzienaa uz klientu pagaidaam 0 == size_of_data!
            if ( 0 < size_of_data ):
                print(f"size_of_data: {size_of_data}")
                return -46  #incorrect server_reply
            '''
            //msg_topic, time_us, total_payload_size, origin_crc, chunk_index, crc
            typedef struct __attribute__((__packed__)) {
                uint8_t      msg_topic;
                uint64_t     time_us;
                size_t       total_payload_size;
                uint32_t     origin_crc;
            //    uint32_t     crc;         //crc beigaas jaahendlo
            } msg_topic_done_t;    //0 != msg_topic and server replies
            '''
            msg_time_us              = struct.unpack_from('<Q', reply, 1 )[0]
            msg_total_payload_size   = struct.unpack_from('<Q', reply, 9 )[0]
            msg_origin_crc           = struct.unpack_from('<I', reply,17 )[0]

            print(f"msg_topic:                {msg_topic}")
            print(f"time_us:                  {msg_time_us}")
            print(f"msg_total_payload_size:   {msg_total_payload_size}")
            print(f"msg_origin_crc:           {hex( msg_origin_crc ).upper()}")
            print(f"msg_crc:                  {hex( msg_crc ).upper()}")

            if ( transaction != msg_origin_crc ):
                return -47  #not a current transaction

        else:   #TOPIC_BUSY
            pass

    else:
        #header - vispjau diivaini, ka clients hederi sanjem!
        #izmantot atbildes masiivam!!!
        if ( SIZE_OF_MSG_TOPIC_INIT_CRC != size_of_reply ):
            return -48      #incompatible server_reply (sizeof(size_t) != calcsize('N')
        '''
        typedef struct __attribute__((__packed__)) {
            uint8_t     msg_topic;        //0
            uint8_t     msg_kind;           //1
            uint32_t    repeats;            //2
            uint8_t     payload_parm_cnt;   //6
            size_t      total_payload_size; //7
        //    uint32_t    crc;              //7+SIZE_OF_SIZE
        } msg_topic_init_t;                 //0 == msg_topic
        '''
        msg_kind               = struct.unpack_from( 'B', reply, 1 )[0]
        msg_repeats            = struct.unpack_from('<I', reply, 2 )[0]
        msg_payload_parm_cnt   = struct.unpack_from( 'B', reply, 6 )[0]
        #taa kaa augshaa paarbaude ir veikta un atmesti potenciaalie gljuki
        #minor todo: pielikt klaat analiizi, lai var izmantot arii ne natiivos formaatus, ja arhitektuuras atshkjiraas
        #vai arii paariet uz tipu, kas size_t iekljauj, bet lielumaa nemainaas
        msg_total_payload_size = struct.unpack_from( 'N', reply, 7 )[0] #'>'?

        print(f"msg_topic:              {msg_topic}")
        print(f"msg_kind:               {msg_kind}")
        print(f"msg_payload_parm_cnt:   {msg_payload_parm_cnt}")
        print(f"msg_total_payload_size: {msg_total_payload_size}")
        print(f"msg_crc:                {hex( msg_crc ).upper()}")

    return 0


#suuta operandus
def transmit( socket, ip, port, Data ):
#return codes:
#-1    incorrect data size for data packet
#-2    #incorrect argument index
#-3    #incorrect chunk index
#-254    transmission aborted by host
    global transaction, msg_topic, msg_kind, msg_repeats
    global msg_payload_parm_cnt, msg_total_payload_size, msg_crc
    global msg_response, msg_current_payload_size, msg_origin_crc
    global msg_arg_index, msg_chunk_index, msg_data

    #sets also global msg_crc
    message = compose_header( Data )
    socket.sendto( message + struct.pack( "<I", msg_crc ), ( ip, port ) )

    while True:
        server_reply, server_address = socket.recvfrom( SIZE_OF_BUFFER )
    
        #sets also global msg_xxx
        check_result = validate_packet( server_reply )
        if ( check_result ):
            return check_result

        if ( TOPIC_DATA == msg_topic ):
            #msg_arg_index, msg_chunk_index
            arg_cnt = len( Data )
            if ( ( arg_cnt <= msg_arg_index ) or ( 0 > msg_arg_index ) ):
                print("Err:-2 arg_cnt, msg_arg_index:", arg_cnt, msg_arg_index )
                return -11  #incorrect argument index - ielikt responsee!!!
            else:
                arg_size = len( Data[msg_arg_index] )
                #print("msg_arg_index, arg_size: ", msg_arg_index, arg_size, Data[msg_arg_index] )
                max_chunks = arg_size // SIZE_OF_DATA
                max_chunks = max_chunks + 1 if arg_size > max_chunks * SIZE_OF_DATA else max_chunks

                #print("msg_arg_index, msg_chunk_index, max_chunks: ", msg_arg_index, msg_chunk_index, max_chunks )

                if ( ( max_chunks <= msg_chunk_index ) or ( 0 > msg_chunk_index ) ):
                    return -12  #incorrect chunk index - ielikt responsee!!!
                else:
                    #suutiit beidzot!
                    offset = msg_chunk_index * SIZE_OF_DATA
                    size_of_chunk = SIZE_OF_DATA
                    if ( ( max_chunks - 1 ) == msg_chunk_index ):
                        size_of_chunk = arg_size - offset
                        msg_response = 1    #peedeejais gabals
                        #print("Last one; msg_response: ", msg_response )
                    else: 
                        msg_response = 0    #nepeedeejais gabals
                    offset = msg_chunk_index * SIZE_OF_DATA
                    #iestata arii msg_crc
                    message = compose_data_message( Data[msg_arg_index][offset:(offset+size_of_chunk)] )
                    socket.sendto( message + struct.pack( "<I", msg_crc ), ( ip, port ) )

        elif ( TOPIC_DONE == msg_topic ):
            print("TOPIC_DONE arrived!")
            return 0
        else:
            return -254

#pingo un sanjem rezultaatu
def receive( socket, ip, port ):
    global comm_error, byte_buffer, transaction
    global msg_topic, msg_origin_crc, msg_total_payload_size, msg_response
    global msg_current_payload_size, msg_arg_index, msg_chunk_index, msg_data, msg_crc

    #dabuut ciparus! should we trust remote values?
    byte_buffer = bytearray( msg_total_payload_size )

    #uztveereejmashiinu!!!
    max_chunks = msg_total_payload_size // SIZE_OF_DATA
    max_chunks = max_chunks + 1 if msg_total_payload_size > max_chunks * SIZE_OF_DATA else max_chunks

    #set globals for 1st request
    lcl_chunk_index = 0

    while True:
        #print("msg_arg_index, arg_size: ", msg_arg_index, arg_size, Data[msg_arg_index] )
        msg_response = 0
        msg_arg_index = 0
        msg_chunk_index = lcl_chunk_index

        message = compose_data_message(b'')     #data requests
        socket.sendto( message + struct.pack( "<I", msg_crc ), ( ip, port ) )

        server_reply, server_address = socket.recvfrom( SIZE_OF_BUFFER )

        #sets also global msg_xxx
        check_result = validate_packet( server_reply )
        if ( check_result ):
            return check_result

        if ( TOPIC_DATA == msg_topic ):
            #msg_arg_index, msg_chunk_index
            arg_cnt = 1
            if ( ( arg_cnt <= msg_arg_index ) or ( 0 > msg_arg_index ) ):
                print("Err:-2 arg_cnt, msg_arg_index:", arg_cnt, msg_arg_index )
                return -21  #incorrect argument index - ielikt responsee!!!
            else:
                if ( ( max_chunks <= msg_chunk_index ) or ( 0 > msg_chunk_index ) ):
                    return -12  #incorrect chunk index - ielikt responsee!!!
                else:
                    if ( lcl_chunk_index == msg_chunk_index ):
                        #novietot beidzot!
                        offset = lcl_chunk_index * SIZE_OF_DATA
                        size_of_chunk = SIZE_OF_DATA
                        if ( ( max_chunks - 1 ) == lcl_chunk_index ):
                            size_of_chunk = msg_total_payload_size - offset

                        byte_buffer[offset:offset+size_of_chunk] = msg_data

                    if ( msg_response ):    #peedeejais gabals
                        return 0
                    else:
                        lcl_chunk_index += 1

        elif ( TOPIC_DONE == msg_topic ):
            print("TOPIC_DONE arrived!")
            return 0

        else:
            return -254

    return 0


def remote( socket, ip, port, op, A, B, repeats = 1000 ):
    global comm_error, msg_repeats, byte_buffer
    msg_repeats = repeats
    comm_error = 0
    data = packers[op]( A, B )
    if ( comm_error ):
        print("ERR in packer")
        return 0
    #print("data", data )
    comm_error = transmit( socket, ip, port, data )
    if ( comm_error ):
        print("ERR in transmit")
        return 0
    comm_error = receive( socket, ip, port )
    print("byte_buffer", byte_buffer )
    if ( comm_error ):
        print("ERR in receive")
        return 0
    result = unpackers[op]( A, B )
    if ( comm_error ):
        print("ERR in unpacker")
        return 0
    return result


#tests
#1.Init
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
while True:
    message = input("Are you ready to continue? ('YES' or 'NO'): ")
    if ( 'YES' == message or 'NO' == message ):
        break
    print("Please enter exactly 'YES' or 'NO' to be sure!")
print("You are ", end='')
if ( 'NO' == message ):
    print("not ", end='')
print("ready to continue. Let's go!")

#2.Real stuff
iterations = 1000
A  = np.array( np.mat('1 2; 3 4; 5 6'), dtype = '<i4')
B  = np.array( np.mat('7 8; 9 4; 5 6'), dtype = '<i4')
Cl = A + B
Cr = remote( client_socket, SERVER_IP, SERVER_PORT, TEST_MATRIX_SUM_CPU, A, B, iterations )
if ( comm_error ):
    print(f"Communication error: {comm_error}")
else:
    if ( np.array_equal( Cl, Cr ) ):
        print("Local and remote results are equal.")
    else:
        print("Local and remote results are not equal!")
        print("Local result:")
        print( Cl )
        print("Remote result:")
        print( Cr )
    print(f"Done in {msg_time_us} us; {msg_time_us/iterations} us per iteration")


#3.Deinit
client_socket.close()

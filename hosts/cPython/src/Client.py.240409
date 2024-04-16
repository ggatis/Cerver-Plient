#!/usr/bin/env python3
#v.02 
"""
Module: Client.py
Author: Gatis Gaigals, EDI
Date:   19.12.2023.

Description:
This module defines a Python class named `Client`.
It provides a class for creating Teststand client objects.

Usage:
Instantiate an object of Client by providing required parameters or use class defaults.
Perform actions in order.

Example:
see below

Notes:
-
"""
import socket
import binascii
import struct
import ctypes
import numpy as np


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


#pakotaaji, ar laiku savaa failaa?
def pTEST_MATRIX_SUM_CPU( A, B ):   #check A+B on CPU
    #print("pTEST_MATRIX_SUM_CPU:", A, B )
    #print( A.shape, B.shape )
    size_of_Ax, size_of_Ay = A.shape
    size_of_Bx, size_of_By = B.shape
    #print( size_of_Ax, size_of_Ay, size_of_Bx, size_of_By )
    if ( size_of_Ax != size_of_Bx ):
        return ER_SIZE1_MISMATCH, []        #x size mismatch
    if ( size_of_Ay != size_of_By ):
        return ER_SIZE2_MISMATCH, []        #y size mismatch
    return 0, [ struct.pack( "<I", size_of_Ax ),
        struct.pack( "<I", size_of_Ay ),
        A.tobytes(), B.tobytes() ]      #sheit var buut gljuks, ja cl/se baitu svariigums atshkjiraas
                                        #tad sanjmeemeejaa jaamaina

#citi pakotaaji liidziigi
packers = [ pTEST_MATRIX_SUM_CPU ]


#atpakotaaji, kljuudas zem -40, ar laiku savaa failaa?
def uTEST_MATRIX_SUM_CPU( A, B, byte_buffer ):
    size_of_Ax, size_of_Ay = A.shape
    #size_of_Bx, size_of_By = B.shape
    if ( len( byte_buffer ) != ( size_of_Ax * size_of_Ay * ctypes.sizeof( ctypes.c_int32( 0 ) ) ) ):
        return ER_BUF_SIZE_MISMATCH, 0
    array = np.frombuffer( byte_buffer, dtype = np.int32 ).reshape( ( size_of_Ax, size_of_Ay ) )
    byte_buff = b''
    return 0, array


#citi atpakotaaji liidziigi
unpackers = [ uTEST_MATRIX_SUM_CPU ]


class Client:
    def __init__( self, ip_address = '127.0.0.1', server_port = 12345 ):
        """
        Constructor for Client.

        Parameters:
        - ip_address  (str): Server IP address. Default: '127.0.0.1'.
        - server_port (int): Server port. Default: 12345.
        """
        
        #turpinaat!!!
        self.ip_address               = ip_address
        self.server_port              = server_port
        self._com_error                  = 0   #comm error

        self.transaction              = ''
        #global dynamic buffer
        self.message                  = ''
        #globals
        self.msg_topic                = 0
        #0 == msg_topic
        self.msg_kind                 = 0
        self.msg_repeats              = 0
        self.msg_payload_parm_cnt     = 0
        self.msg_total_payload_size   = 0
        #1 == msg_topic
        self.msg_response             = 0
        self.msg_current_payload_size = 0
        self.msg_origin_crc           = 0
        self.msg_arg_index            = 0
        self.msg_chunk_index          = 0
        self.msg_data                 = ''
        #
        self._msg_time_us             = 0
        #
        self.msg_crc                  = 0
        #
        self._byte_buffer             = bytearray(b'')

        self.client_socket            = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )


    def __del__( self ):
        """Destructor of Client."""
        self.client_socket.close()
        

    def __str__( self ):
        return f"Remote function call client attached to {self.ip_address}:{self.server_port}"

    
    def __repr__( self ):
        return f"Please be patient: currently there is no way to represent all internals of the client."


    @property
    def com_error( self ):
        """Returns the last communication error if the value is not 0."""
        return self._com_error


    @property
    def byte_buffer( self ):
        """Returns the byte_buffer for data exchange."""
        return self._byte_buffer


    @property
    def msg_time_us( self ):
        """Returns the time spent in execution loops."""
        return self._msg_time_us


    def compose_topic_init( self, all_data ):
        """
        Composes TOPIC_INIT message body.
        Besides the self header fields and self.message sets self.msg_crc also.

        Returns implicitly:
        - self TOPIC_INIT self header fields
        - self.message
        - self.msg_crc
        """
    
        self.msg_topic              = TOPIC_INIT
        self.msg_kind               = 0                 #not used in this
        self.msg_payload_parm_cnt   = len( all_data );
        self.msg_total_payload_size = 0
        for i in range( self.msg_payload_parm_cnt ):
            self.msg_total_payload_size += len( all_data[i] )
        print("msg_total_payload_size(data size):", self.msg_total_payload_size )
    
        self.message = \
            struct.pack( 'B', self.msg_topic ) + \
            struct.pack( 'B', self.msg_kind ) + \
            struct.pack('<I', self.msg_repeats ) + \
            struct.pack( 'B', self.msg_payload_parm_cnt ) + \
            struct.pack( 'N', self.msg_total_payload_size )   #'>'?
    
        self.msg_crc = binascii.crc32( self.message ) & 0xFFFFFFFF
        #print(f" CRC32 value: {hex( self.msg_crc ).upper()}")

        self.transaction = self.msg_crc
        self.message += struct.pack( "<I", self.msg_crc )

        print(f"TX TOPIC_INIT message[{len(self.message)}]: ", self.message )
        print( binascii.hexlify( self.message ) )
        print(f"TX CRC32 value: {hex( self.msg_crc ).upper()}\r\n")


    def compose_topic_data( self, chunk ):
        """
        Composes TOPIC_DATA message body.
        Besides the self header fields and self.message sets self.msg_crc also.

        Returns implicitly:
        - self TOPIC_DATA self header fields
        - self.message
        - self.msg_crc
        """

        #print("chunk: ", len( chunk ), chunk )
        self.msg_topic = TOPIC_DATA
        self.message = \
            struct.pack( 'B', self.msg_topic ) + \
            struct.pack( 'B', self.msg_response ) + \
            struct.pack('<H', len( chunk ) + SIZE_OF_MSG_TOPIC_DATA ) + \
            struct.pack('<I', self.transaction ) + \
            struct.pack('<H', self.msg_arg_index ) + \
            struct.pack('<I', self.msg_chunk_index ) + \
            ( struct.pack( 'B' * len(chunk), *chunk ) if len( chunk ) else b'')

        self.msg_crc = binascii.crc32( self.message ) & 0xFFFFFFFF
        self.message += struct.pack( "<I", self.msg_crc )

        print("TX TOPIC_DATA message: ", self.message )
        print( binascii.hexlify( self.message ) )
        print(f"TX CRC32 value: {hex( self.msg_crc ).upper()}\r\n")


    def validate_packet( self, reply ):
        """
        Decomposes incoming message into fields checking the internals.

        Returns:
        -0  OK
        -1  incorrect CRC
        -2  incompatible server_reply
        -3  not a current self.transaction
        """

        size_of_reply = len( reply )
        print( f"RX size: { size_of_reply }")
        print("RX: ", reply )
    
        #self.msg_topic = struct.unpack('B', reply[0] )[0]
        self.msg_topic = reply[0]
        print("RX msg_topic: ", self.msg_topic )
    
        rCRC32  = struct.unpack('<I', reply[-SIZE_OF_CRC:])[0]
        self.msg_crc = rCRC32
        cCRC32  = binascii.crc32( reply[:-SIZE_OF_CRC] ) & 0xFFFFFFFF
        if ( cCRC32 ^ rCRC32 ):
            print(f"rCRC32: {hex( rCRC32 ).upper()}")
            print(f"cCRC32: {hex( cCRC32 ).upper()}")
            return -41          #incorrect CRC

        if ( self.msg_topic ):
            if ( TOPIC_DATA == self.msg_topic ):
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
                self.msg_response               = struct.unpack_from( 'B', reply, 1 )[0]
                self.msg_current_payload_size   = struct.unpack_from('<H', reply, 2 )[0]
                self.msg_origin_crc             = struct.unpack_from('<I', reply, 4 )[0]
                self.msg_arg_index              = struct.unpack_from('<H', reply, 8 )[0]
                self.msg_chunk_index            = struct.unpack_from('<I', reply,10 )[0]
                self.msg_data                   = reply[SIZE_OF_MSG_TOPIC_DATA:-SIZE_OF_CRC]

                print(f"msg_topic:                {self.msg_topic}")
                print(f"msg_response:             {self.msg_response}")
                print(f"msg_current_payload_size: {self.msg_current_payload_size}")
                print(f"msg_origin_crc:           {hex( self.msg_origin_crc ).upper()}")
                print(f"msg_arg_index:            {self.msg_arg_index}")
                print(f"msg_chunk_index:          {self.msg_chunk_index}")
                print(f"msg_data len:             {len( self.msg_data )}")
                print(f"msg_crc:                  {hex( self.msg_crc ).upper()}")
    
                if ( self.transaction != self.msg_origin_crc ):
                    return -44  #not a current self.transaction
    
            elif ( TOPIC_DONE == self.msg_topic ):
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
                self._msg_time_us                = struct.unpack_from('<Q', reply, 1 )[0]
                self.msg_total_payload_size     = struct.unpack_from('<Q', reply, 9 )[0]
                self.msg_origin_crc             = struct.unpack_from('<I', reply,17 )[0]
    
                print(f"msg_topic:                {self.msg_topic}")
                print(f"time_us:                  {self.msg_time_us}")
                print(f"msg_total_payload_size:   {self.msg_total_payload_size}")
                print(f"msg_origin_crc:           {hex( self.msg_origin_crc ).upper()}")
                print(f"msg_crc:                  {hex( self.msg_crc ).upper()}")
    
                if ( self.transaction != self.msg_origin_crc ):
                    return -47  #not a current self.transaction
    
            else:   #TOPIC_BUSY
                pass
    
        else:
            #izmanto atbildes masiivam
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
            self.msg_kind                   = struct.unpack_from( 'B', reply, 1 )[0]
            self.msg_repeats                = struct.unpack_from('<I', reply, 2 )[0]
            self.msg_payload_parm_cnt       = struct.unpack_from( 'B', reply, 6 )[0]
            #taa kaa augshaa paarbaude ir veikta un atmesti potenciaalie gljuki
            #minor todo: pielikt klaat analiizi, lai var izmantot arii ne natiivos formaatus, ja arhitektuuras atshkjiraas
            #vai arii paariet uz tipu, kas size_t iekljauj, bet lielumaa nemainaas
            self.msg_total_payload_size = struct.unpack_from( 'N', reply, 7 )[0] #'>'?
    
            print(f"msg_topic:                {self.msg_topic}")
            print(f"msg_kind:                 {self.msg_kind}")
            print(f"msg_payload_parm_cnt:     {self.msg_payload_parm_cnt}")
            print(f"msg_total_payload_size:   {self.msg_total_payload_size}")
            print(f"msg_crc:                  {hex( self.msg_crc ).upper()}")
    
        return 0
    
    
    #suuta operandus
    def transmit( self, Data ):
        """
        Transmits data.

        Returns:
        -0      OK
        -1      incorrect data size for data packet
        -2      incorrect argument index
        -3      incorrect chunk index
        -254    transmission aborted by host
        """
        phase = 0
        self.client_socket.settimeout( 1 )

        self.compose_topic_init( Data )


        print(f"self.client_socket.sendto {self.ip_address}:{self.server_port}")
        while True:
            if 0 == phase:
                self.client_socket.sendto( self.message, ( self.ip_address, self.server_port ) )
                try:
                    server_reply, server_address = self.client_socket.recvfrom( SIZE_OF_BUFFER )
                except socket.timeout:
                    print("No answer from server!")
                    continue

                self.client_socket.settimeout( None )
                phase = 1

            else:
            
                print("self.client_socket.recvfrom")
                server_reply, server_address = self.client_socket.recvfrom( SIZE_OF_BUFFER )
        
            #sets also global msg_xxx
            check_result = self.validate_packet( server_reply )
            if ( check_result ):
                return check_result
    
            if ( TOPIC_DATA == self.msg_topic ):
                #self.msg_arg_index, self.msg_chunk_index
                arg_cnt = len( Data )
                if ( ( arg_cnt <= self.msg_arg_index ) or ( 0 > self.msg_arg_index ) ):
                    print("Err:-2 arg_cnt, msg_arg_index:", arg_cnt, self.msg_arg_index )
                    return -11  #incorrect argument index - ielikt responsee!!!
                else:
                    arg_size = len( Data[self.msg_arg_index] )
                    #print("msg_arg_index, arg_size: ", self.msg_arg_index, arg_size, Data[self.msg_arg_index] )
                    max_chunks = arg_size // SIZE_OF_DATA
                    max_chunks = max_chunks + 1 if arg_size > max_chunks * SIZE_OF_DATA else max_chunks
    
                    #print("msg_arg_index, msg_chunk_index, max_chunks: ", self.msg_arg_index, self.msg_chunk_index, max_chunks )
    
                    if ( ( max_chunks <= self.msg_chunk_index ) or ( 0 > self.msg_chunk_index ) ):
                        return -12  #incorrect chunk index - ielikt responsee!!!
                    else:
                        #suutiit beidzot!
                        offset = self.msg_chunk_index * SIZE_OF_DATA
                        size_of_chunk = SIZE_OF_DATA
                        if ( ( max_chunks - 1 ) == self.msg_chunk_index ):
                            size_of_chunk = arg_size - offset
                            self.msg_response = 1    #peedeejais gabals
                            #print("Last one; self.msg_response: ", self.msg_response )
                        else: 
                            self.msg_response = 0    #nepeedeejais gabals
                        offset = self.msg_chunk_index * SIZE_OF_DATA
                        #iestata arii self.msg_crc
                        self.compose_topic_data( Data[self.msg_arg_index][offset:(offset+size_of_chunk)] )
                        self.client_socket.sendto( self.message, ( self.ip_address, self.server_port ) )
    
            elif ( TOPIC_DONE == self.msg_topic ):
                print("TOPIC_DONE arrived!")
                return 0
            else:
                return -254
    

    #pingo un sanjem rezultaatu
    def receive( self ):
        """
        Receives data.

        Returns:
        -0      OK
        check_result from message validation
        -254    transmission aborted
        other codes
        """
        #dabuut ciparus! should we trust remote values?
        self._byte_buffer = bytearray( self.msg_total_payload_size )
    
        #uztveereejmashiinu!!!
        max_chunks = self.msg_total_payload_size // SIZE_OF_DATA
        max_chunks = max_chunks + 1 if self.msg_total_payload_size > max_chunks * SIZE_OF_DATA else max_chunks
    
        #set globals for 1st request
        lcl_chunk_index = 0
    
        while True:
            #print("msg_arg_index, arg_size: ", self.msg_arg_index, arg_size, Data[self.msg_arg_index] )
            self.msg_response = 0
            self.msg_arg_index = 0
            self.msg_chunk_index = lcl_chunk_index
    
            self.compose_topic_data(b'')     #data requests
            self.client_socket.sendto( self.message, ( self.ip_address, self.server_port ) )
    
            server_reply, server_address = self.client_socket.recvfrom( SIZE_OF_BUFFER )
    
            #sets also global msg_xxx
            check_result = self.validate_packet( server_reply )
            if ( check_result ):
                return check_result
    
            if ( TOPIC_DATA == self.msg_topic ):
                #self.msg_arg_index, self.msg_chunk_index
                arg_cnt = 1
                if ( ( arg_cnt <= self.msg_arg_index ) or ( 0 > self.msg_arg_index ) ):
                    print("Err:-2 arg_cnt, msg_arg_index:", arg_cnt, self.msg_arg_index )
                    return -21  #incorrect argument index - ielikt responsee!!!
                else:
                    if ( ( max_chunks <= self.msg_chunk_index ) or ( 0 > self.msg_chunk_index ) ):
                        return -12  #incorrect chunk index - ielikt responsee!!!
                    else:
                        if ( lcl_chunk_index == self.msg_chunk_index ):
                            #novietot beidzot!
                            offset = lcl_chunk_index * SIZE_OF_DATA
                            size_of_chunk = SIZE_OF_DATA
                            if ( ( max_chunks - 1 ) == lcl_chunk_index ):
                                size_of_chunk = self.msg_total_payload_size - offset
    
                            self._byte_buffer[offset:offset+size_of_chunk] = self.msg_data
    
                        if ( self.msg_response ):    #peedeejais gabals
                            return 0
                        else:
                            lcl_chunk_index += 1
    
            elif ( TOPIC_DONE == self.msg_topic ):
                print("TOPIC_DONE arrived!")
                return 0
    
            else:
                return -254
    
        return 0
    
    
    def execute( self, op, A, B, repeats = 1000 ):
        """
        Transfers data, executes remotely and receives results back.

        Returns:
        - 0 and ( 0 != client.com_error ) in the case of an error
        - result if there were no errors ( 0 == client.com_error )
        """
        self.msg_repeats = repeats
        self._com_error, data = packers[op]( A, B )
        if ( self._com_error ):
            print("ERR in packer")
            return 0
        #print("data", data )
        self._com_error = self.transmit( data )
        if ( self._com_error ):
            print("ERR in transmit")
            return 0
        self._com_error = self.receive()
        print("byte_buffer: ", self._byte_buffer )
        if ( self._com_error ):
            print("ERR in receive")
            return 0
        self._com_error, result = unpackers[op]( A, B, self._byte_buffer )
        if ( self._com_error ):
            print("ERR in unpacker")
            return 0
        return result
    

if __name__ == "__main__":
    print("This is the main program")
    Client = Client( '169.254.11.200', 15000 )

    print( Client )

    #wait for a server
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
    #local result
    Cl = A + B
    #remote result
    Cr = Client.execute( TEST_MATRIX_SUM_CPU, A, B, iterations )
    if ( Client.com_error ):
        print(f"Communication error: {Client.com_error}")
    else:
        if ( np.array_equal( Cl, Cr ) ):
            print("Local and remote results are equal.")
        else:
            print("Local and remote results are not equal!")
            print("Local result:")
            print( Cl )
            print("Remote result:")
            print( Cr )
        print(f"Done in {Client.msg_time_us} us; {Client.msg_time_us/iterations} us per iteration")


    #3.Deinit
    Client.__del__()

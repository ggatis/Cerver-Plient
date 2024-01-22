#!/usr/bin/env python3
import socket
import binascii
import struct

SERVER_IP = '127.0.0.1'  # Change this to the server's IP address
SERVER_PORT = 12345

# Create a UDP socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    message = input("Enter a message to send to the server (or 'EXIT' to quit): ")

    if ( ( 1024 - 4 ) < len( message ) ):
        print("Invalid message size!")
        continue

    #calculate CRC-32 and ensure it's a 32-bit unsigned integer
    crc32_value = binascii.crc32( message.encode('utf-8') ) & 0xFFFFFFFF
    print(f" CRC32 value: {hex( crc32_value ).upper()}")

    #convert the CRC-32 value to a four-byte string
    crc32_bytes = struct.pack( ">I", crc32_value )

    # Send data to the server
    client_socket.sendto( message.encode('utf-8') + crc32_bytes, ( SERVER_IP, SERVER_PORT ) )

    # Receive a response from the server
    data, server_address = client_socket.recvfrom( 1024 )
    nbytes = len( data )

    print(f"Received from server: {nbytes}, ", end="")
    print( data[:nbytes-4].decode('utf-8') )

    if message == 'EXIT':
        break

client_socket.close()

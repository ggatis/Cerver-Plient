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

    if ( ( 1024 - 5 ) < len( message ) ):
        print("Invalid message size!")
        continue

    #calculate CRC-32 and ensure it's a 32-bit unsigned integer
    crc32_value = binascii.crc32( message.encode('utf-8') ) & 0xFFFFFFFF
    #convert the CRC-32 value to a four-byte string
    crc32_bytes = struct.pack( ">I", crc32_value )
    print(f"crc32_bytes: {crc32_bytes}")

    # Send data to the server
    client_socket.sendto( ( message + '\x00' ).encode('utf-8') + crc32_bytes, ( SERVER_IP, SERVER_PORT ) )

    # Receive a response from the server
    data, server_address = client_socket.recvfrom(1024)
    print(f"Received from server: {len( data )}, {data.decode('utf-8')}")

    if message == 'EXIT':
        break

client_socket.close()

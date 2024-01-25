#!/usr/bin/env python3
#v0.02
"""
Module: Plient_A3.py
Author: Gatis Gaigals, EDI
Date:   20.12.2023.

Description:
This module shows, how to work with the remote function call client module Client.py.

Usage:
See in main text.

Example:
See below.

Notes:
-
"""
import numpy as np
import Client

client = Client.Client( '127.0.0.1', 12346 )
print( client )

#1. Wait for a server
while True:
    message = input("Are you ready to continue? ('YES' or 'NO'): ")
    if ( 'YES' == message or 'NO' == message ):
        break
    print("Please enter exactly 'YES' or 'NO' to be sure!")
print("You are ", end='')
if ( 'NO' == message ):
    print("not ", end='')
print("ready to continue. Let's go!")

#2. Real stuff
#setup
iterations = 1000
A  = np.array( np.mat('1 2; 3 4; 5 6'), dtype = '<i4')
B  = np.array( np.mat('7 8; 9 4; 5 6'), dtype = '<i4')

#results
Cl = A + B                                                          #local
Cr = client.execute( Client.TEST_MATRIX_SUM_CPU, A, B, iterations ) #remote

#3. Dissemination
if ( client.com_error ):
    print(f"Communication error: {client.com_error}")
else:
    if ( np.array_equal( Cl, Cr ) ):
        print("Local and remote results are equal.")
    else:
        print("Local and remote results are not equal!")
        print("Local result:")
        print( Cl )
        print("Remote result:")
        print( Cr )
    print(f"Done in {client.msg_time_us} us; {client.msg_time_us/iterations} us per iteration")

#3. Quit
client.__del__()

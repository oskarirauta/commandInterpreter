commandInterpreter
==================

commandInterpreter library for Arduino(due) and LeafLabs' Maple. 
A protocol for sending messages and arguments over serial line or rs-485. 
Supports device address ID's (designed for 32, but supports more). 

Some features:
 - Uses linked list to store received command and arguments. 
 - Supports cheksuming of messages.
 - Supports also replying to message with simple answers like ACK, NAK, CAN or UNK..

Warning: Maple docs say that using mallocs is unwise. This program currently is using mallocs to initialize the linked list. This might change in future.

Note: Docs missing. Check provided examples. Master program that can be run on PC/Mac/etc can be found from example's master directory with proper Makefile for GNU's g++..
Note2: commandInterpreter.vCompose wants arguments in va_list commandInterpreter->valist, commandInterpreter.compose accepts variadic arguments.

License: expat ( check copying.txt )

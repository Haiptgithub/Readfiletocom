#ifndef MY_FUNCTIONS_H
#define MY_FUNCTIONS_H

#include <windows.h>
#include <stdio.h>

void sendByBlock(FILE *file, HANDLE hComm);
void sendByCharacter(FILE *file, HANDLE hComm);

#endif

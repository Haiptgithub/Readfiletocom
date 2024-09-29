#ifndef MY_FUNCTIONS_H
#define MY_FUNCTIONS_H

#include <windows.h>
#include <cstdio>  

void sendByBlock(FILE *file, HANDLE hComm);
void sendByCharacter(FILE *file, HANDLE hComm);

#endif

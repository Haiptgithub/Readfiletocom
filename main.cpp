#include <iostream>
#include <windows.h>  
#include <cstdio>     
#include "my_functions.h"  

#define BLOCK_SIZE 128

void sendByBlock(FILE *file, HANDLE hComm) {
    char buffer[BLOCK_SIZE];
    DWORD bytesWritten;
    size_t bytesRead;

    std::cout << "\nTruyen va in theo block 128 ky tu:\n";
    while ((bytesRead = fread(buffer, 1, BLOCK_SIZE, file)) > 0) {
        // In toan bo block ra man hinh
        std::cout.write(buffer, bytesRead);

        // Truyen block qua cong COM
        if (!WriteFile(hComm, buffer, bytesRead, &bytesWritten, NULL)) {
            std::cerr << "Loi khi gui du lieu qua cong COM\n";
            break;
        }
        Sleep(1000); 
    }
    std::cout << "\nDa hoan thanh viec truyen theo block\n";
}

void sendByCharacter(FILE *file, HANDLE hComm) {
    char ch;
    DWORD bytesWritten;

    std::cout << "\nTruyen va in tung ky tu:\n";
    while ((ch = fgetc(file)) != EOF) {
        std::cout << ch;

        if (!WriteFile(hComm, &ch, 1, &bytesWritten, NULL)) {
            std::cerr << "Loi khi gui du lieu qua cong COM\n";
            break;
        }
        Sleep(100);  
    }
    std::cout << "\nDa hoan thanh viec truyen tung ky tu\n";
}

int main() {
    // Mo file txt trong o E voi tên 'input.txt'
    FILE *file = fopen("E:\\input.txt", "r");
    if (!file) {
        std::cerr << "Khong the mo file 'input.txt' trong o E\n";
        return 1;
    }

    // Mo cong COM9 (PL2303) de truyen du lieu
    HANDLE hComm = CreateFile("\\\\.\\COM9", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hComm == INVALID_HANDLE_VALUE) {
        std::cerr << "Khong the mo cong COM9\n";
        fclose(file);
        return 1;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hComm, &dcbSerialParams)) {
        std::cerr << "Khong the lay trang thai cong COM\n";
        fclose(file);
        CloseHandle(hComm);
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_9600;  // Baud rate (9600)
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hComm, &dcbSerialParams)) {
        std::cerr << "Khong the thiet lap cong COM\n";
        fclose(file);
        CloseHandle(hComm);
        return 1;
    }

    fseek(file, 0, SEEK_SET); 
    sendByCharacter(file, hComm);

    fseek(file, 0, SEEK_SET);
    sendByBlock(file, hComm);

    fclose(file);
    CloseHandle(hComm);

    std::cout << "\nDa hoan thanh viec doc file và truyen qua COM9\n";
    return 0;
}

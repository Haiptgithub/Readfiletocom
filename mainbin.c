#include "my_functions.h"  

#define BLOCK_SIZE 128 

void sendByBlock(FILE *file, HANDLE hComm) {
    char buffer[BLOCK_SIZE];
    DWORD bytesWritten;
    size_t bytesRead;

    printf("\nTruyen va in theo block 128 ky tu (hoac byte):\n");
    while ((bytesRead = fread(buffer, 1, BLOCK_SIZE, file)) > 0) {
        // In toan bo block ra man hinh
        printf("%.*s", (int)bytesRead, buffer); // In block voi kich thuoc thuc te

        // Truyen block qua cong COM
        if (!WriteFile(hComm, buffer, bytesRead, &bytesWritten, NULL)) {
            printf("Loi khi gui du lieu qua cong COM\n");
            break;
        }
        Sleep(1000); 
    }
    printf("\nDa hoan thanh viec truyen theo block\n");
}

void sendByCharacter(FILE *file, HANDLE hComm) {
    char ch;
    DWORD bytesWritten;

    printf("\nTruyen va in tung ky tu (hoac byte):\n");
    while ((ch = fgetc(file)) != EOF) {
        printf("%c", ch);  

        if (!WriteFile(hComm, &ch, 1, &bytesWritten, NULL)) {
            printf("Loi khi gui du lieu qua cong COM\n");
            break;
        }
        Sleep(100); 
    }
    printf("\nDa hoan thanh viec truyen tung ky tu\n");
}


int main() {
    // Mo file bin trong o E voi tên 'input.bin'
    FILE *file = fopen("D:\\filebin.bin", "rb"); 
    if (!file) {
        printf("Khong the mo file 'input.bin' trong o D\n");
        return 1;
    }

    // Mo cong COM9 (PL2303) de truyen du lieu
    HANDLE hComm = CreateFile("\\\\.\\COM1", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hComm == INVALID_HANDLE_VALUE) {
        printf("Khong the mo cong COM1\n");
        fclose(file);
        return 1;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hComm, &dcbSerialParams)) {
        printf("Khong the lay trang thai cong COM\n");
        fclose(file);
        CloseHandle(hComm);
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_9600;  // Baud rate (9600)
    dcbSerialParams.ByteSize = 8;         
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;  
    if (!SetCommState(hComm, &dcbSerialParams)) {
        printf("Khong the thiet lap cong COM\n");
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

    printf("\nDa hoan thanh viec doc file và truyen qua COM9\n");
    return 0;
} 

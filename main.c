#include "my_functions.h"

#define BLOCK_SIZE 128

void sendByBlock(FILE *file, HANDLE hComm) {
    char buffer[BLOCK_SIZE];
    DWORD bytesWritten;
    size_t bytesRead;

    printf("\nTruyen và in theo block 128 ky tu:\n");
    while ((bytesRead = fread(buffer, 1, BLOCK_SIZE, file)) > 0) {
        // In toàn bộ block ra màn hình
        printf("%.*s", (int)bytesRead, buffer);  // In block với kích thước thực tế

        // Kiểm tra kết nối COM trước khi gửi dữ liệu
        if (hComm != INVALID_HANDLE_VALUE) {
            // Truyền block qua cổng COM
            if (!WriteFile(hComm, buffer, bytesRead, &bytesWritten, NULL)) {
                printf("Loi khi gui du lieu qua cong COM\n");
                break;
            }
            Sleep(1000);
        } else {
            printf("\nKhong gui duoc du lieu qua cong COM vi mat ket noi\n");
        }
    }
    printf("\nDa hoan thanh viec truyen theo block\n");
}

void sendByCharacter(FILE *file, HANDLE hComm) {
    char ch;
    DWORD bytesWritten;

    printf("\nTruyen va in tung ky tu:\n");
    while ((ch = fgetc(file)) != EOF) {
        printf("%c", ch);  // In ký tự ra màn hình

        // Kiểm tra kết nối COM trước khi gửi dữ liệu
        if (hComm != INVALID_HANDLE_VALUE) {
            if (!WriteFile(hComm, &ch, 1, &bytesWritten, NULL)) {
                printf("Loi khi gui du lieu qua cong COM\n");
                break;
            }
            Sleep(100);  // Dừng 100ms giữa mỗi ký tự
        } else {
            printf("\nKhong gui duoc du lieu qua cong COM vi mat ket noi\n");
        }
    }
    printf("\nDa hoan thanh viec truyen tung ky tu\n");
}

HANDLE openSerialPort() {
    // Mo cong COM1 để truyền dữ liệu (hoặc thay đổi theo cổng RS232 của bạn)
    HANDLE hComm = CreateFile("\\\\.\\COM1", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hComm == INVALID_HANDLE_VALUE) {
        printf("Khong the mo cong COM1\n");
        return INVALID_HANDLE_VALUE;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hComm, &dcbSerialParams)) {
        printf("Khong the lay trang thai cong COM\n");
        CloseHandle(hComm);
        return INVALID_HANDLE_VALUE;
    }

    // Thiet lap baudrate và các tham số RS232
    dcbSerialParams.BaudRate = CBR_9600;  // Baud rate (9600)
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    // Kích hoạt handshake phần cứng
    dcbSerialParams.fOutxCtsFlow = TRUE;  // Bật CTS flow control
    dcbSerialParams.fRtsControl = RTS_CONTROL_HANDSHAKE;  // Bật RTS handshake

    if (!SetCommState(hComm, &dcbSerialParams)) {
        printf("Khong the thiet lap cong COM\n");
        CloseHandle(hComm);
        return INVALID_HANDLE_VALUE;
    }

    // Thiết lập timeout cho giao tiếp COM
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(hComm, &timeouts);

    return hComm;
}

int main() {
    // Mo file txt trong o D với tên 'input.txt'
    FILE *file = fopen("D:\\input.txt", "r");
    if (!file) {
        printf("Khong the mo file 'input.txt' trong o D\n");
        return 1;
    }

    // Mo cong COM1 để truyền dữ liệu
    HANDLE hComm = openSerialPort();

    // Gửi từng ký tự qua COM
    fseek(file, 0, SEEK_SET);
    sendByCharacter(file, hComm);

    // Gửi theo block 128 ký tự qua COM
    fseek(file, 0, SEEK_SET);
    sendByBlock(file, hComm);

    fclose(file);
    if (hComm != INVALID_HANDLE_VALUE) {
        CloseHandle(hComm);
    }

    printf("\nDa hoan thanh viec doc file va truyen qua COM1\n");
    return 0;
}

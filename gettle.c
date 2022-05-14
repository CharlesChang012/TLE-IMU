#include<stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>

void errExit(char *reason);

int main() {

    char *host = "celestrak.com"; // 目標 URI
    char *PORT_NUM = "80"; // HTTP port

    char request[0xfff], response[0xfff]; // 請求 與 回應訊息
    char *requestLine = "GET /NORAD/elements/gp.php?CATNR=37606 HTTP/1.1\r\n"; // 請求行
    char *headerFmt = "Host: celestrak.com\r\n"; // Host 表頭欄位
    char *CRLF = "\r\n";  // 表頭後的 CRLF

    int cfd; // Socket 檔案描述符 (File Descriptor)
    int gaiStatus; // getaddrinfo 狀態碼
    struct addrinfo hints; // hints 參數，設定 getaddrinfo() 的回傳方式
    struct addrinfo *result; // getaddrinfo() 執行結果的 addrinfo 結構指標

    // 動態配置記憶體，以決定 表頭緩衝區 (Header Buffer) 長度
    size_t bufferLen = strlen(headerFmt) + strlen(host) + 1;
    char *buffer = (char *) malloc(bufferLen); // 表頭緩衝區

    //組裝請求訊息
    strcpy(request, requestLine);
    snprintf(buffer, bufferLen, headerFmt, host);
    strcat(request, buffer);
    strcat(request, CRLF);

    // 釋放緩衝區記憶體
    free(buffer);
    buffer = NULL;

    // 以 memset 清空 hints 結構
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; // 使用 IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // 串流 Socket
    hints.ai_flags = AI_NUMERICSERV; // 將 getaddrinfo() 第 2 參數 (PORT_NUM) 視為數字

    // 以 getaddrinfo 透過 DNS，取得 addrinfo 鏈結串列 (Linked List)
    // 以從中取得 Host 的 IP 位址
    if ((gaiStatus = getaddrinfo(host, PORT_NUM, &hints, &result)) != 0)
        errExit((char *) gai_strerror(gaiStatus));


    // 分別以 domain, type, protocol 建立 socket 檔案描述符
    cfd = socket(result->ai_family, result->ai_socktype, 0);

    // 以 socket 檔案描述符 (cfd), addr, addrlen 進行連線
    // 其中，result->ai_addr 為 gai 取得之 通用 socket 位址結構 -- sockaddr
    if (connect(cfd, result->ai_addr, result->ai_addrlen) < 0)
        errExit("Connect");


    // 釋放 getaddrinfo (Linked List) 記憶體空間
    freeaddrinfo(result);
    result = NULL;

    // 格式化輸出請求訊息
    printf("----------\nRequest:\n----------\n%s\n", request);

    // 發送請求
    if (send(cfd, request, strlen(request), 0) < 0)
        errExit("Send");

    // 接收回應
    if (recv(cfd, response, 0xfff, 0) < 0)
        errExit("Receive");

    // 格式化輸出回應訊息
    printf("----------\nResponse:\n----------\n%s\n", response);

    shutdown(cfd, SHUT_WR);

    char substr[] = "ST-2";
    char TLE1[95];
    char TLE2[95];
    int index = -1;
    for (int i = 0; response[i] != '\0'; i++) {
        index = -1;
        for (int j = 0; substr[j] != '\0'; j++) {
            if (response[i + j] != substr[j]) {
                index = -1;
                break;
            }
            index = i;
        }
        if (index != -1) {
            strncpy(TLE1, &response[index], 95);
            break;
        }
    }

    index += 97;
    printf("%s\n", TLE1);
    strncpy(TLE2, &response[index], 95);
    printf("%s\n", TLE2);
    /*printf("debug");

    char *ST2;
    ST2 = strstr(response, "ST-2");
    int pos = ST2 - response;
	printf("%d\n", pos);
    int TLE1_start_index = pos + 5;
    printf("%d\n", pos+5);

    char TLE1[80];
    strncpy(TLE1, response[TLE1_start_index], 69);
   
    char TLE2[80];*/

    
    // 半雙工關閉 TCP Socket 連線
    // (i.e., 關閉寫入)
    

    return 0;
}

void errExit(char *reason) {
    char *buff = reason ? reason : strerror(errno);
    printf("Error: %s", buff);
    exit(EXIT_FAILURE);
}

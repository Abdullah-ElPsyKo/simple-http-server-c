#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

// Function prototype for cleanup
int cleanUpSocket(SOCKET* sock);

int main() {
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    int status;

    status = WSAStartup(wVersionRequested, &wsaData);
    if (status != 0) {
        printf("[ERROR] WSAStartup failed with error: %d\n", status);
        return 1;
    }

    // Initialize a socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("[ERROR] socket function failed with error = %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    else {
        printf("[INFO] socket function succeeded\n");
    }

    // Configure service structure
    struct sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(27015);

    // Bind socket
    status = bind(sock, (SOCKADDR*)&service, sizeof(service));
    if (status == SOCKET_ERROR) {
        printf("[ERROR] bind failed with error = %d\n", WSAGetLastError());
        cleanUpSocket(&sock);
        WSACleanup();
        return 1;
    }
    else {
        printf("[INFO] bind succeeded\n");
    }

    // Listen for connections
    if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
        printf("[ERROR] listen function failed with error = %d\n", WSAGetLastError());
        cleanUpSocket(&sock);
        WSACleanup();
        return 1;
    }

    printf("[ACTIVE] Listening on socket\n");

    // Accept client connection
    SOCKET acceptSocket;
    printf("[ACTIVE] Waiting for client to connect...\n");

    acceptSocket = accept(sock, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET) {
        printf("[ERROR] accept failed with error = %d\n", WSAGetLastError());
        cleanUpSocket(&sock);
        WSACleanup();
        return 1;
    }

    printf("[INFO] Client connected successfully\n");

    // Receive data from client
    char recvBuffer[1024] = {0};
    int bytesReceived = recv(acceptSocket, recvBuffer, sizeof(recvBuffer) - 1, 0);
    if (bytesReceived > 0) {
        printf("[INFO] Bytes received: %d\n", bytesReceived);
    }
    else if (bytesReceived == 0) {
        printf("[INFO] Connection closed\n");
    }
    else {
        printf("[ERROR] recv failed: %d\n", WSAGetLastError());
        cleanUpSocket(&acceptSocket);
        cleanUpSocket(&sock);
        WSACleanup();
        return 1;
    }

    // Send response to the client
    const char* httpResponse =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, world!";

    int bytesSent = send(acceptSocket, httpResponse, strlen(httpResponse), 0);
    if (bytesSent == SOCKET_ERROR) {
        printf("[ERROR] send failed with error = %d\n", WSAGetLastError());
        cleanUpSocket(&acceptSocket);
        cleanUpSocket(&sock);
        WSACleanup();
        return 1;
    }
    else {
        printf("[INFO] Sent response to client (%d bytes)\n", bytesSent);
    }
    // Clean up sockets
    cleanUpSocket(&acceptSocket);
    cleanUpSocket(&sock);

    // Cleanup Winsock
    WSACleanup();
    return 0;
}

// Function to clean up a socket
int cleanUpSocket(SOCKET* sock) {
    int status;

    // Clean up the socket
    status = closesocket(*sock);
    if (status == SOCKET_ERROR) {
        printf("[ERROR] closesocket failed with error = %d\n", WSAGetLastError());
        return 1;
    }

    return 0;
}

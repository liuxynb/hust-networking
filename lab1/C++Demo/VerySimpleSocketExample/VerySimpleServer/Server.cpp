#pragma once
#include "winsock2.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

void sendData(const char* fileName, SOCKET clientSocket) {
    ifstream file(fileName, ios::binary | ios::ate); // Open the file in binary mode and seek to the end to get the file size.
    if (!file.is_open()) {
        cout << "File not found: " << fileName << endl;
        char notFoundResponse[] = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        send(clientSocket, notFoundResponse, strlen(notFoundResponse), 0);
        return;
    }

    streampos fileSize = file.tellg(); // Get the file size.
    file.seekg(0, ios::beg); // Reset the file pointer to the beginning.

    char* buffer = new char[fileSize];
    file.read(buffer, fileSize); // Read the entire file into the buffer.

    string contentType;
    if (strstr(fileName, ".html") || strstr(fileName, ".htm")) {
        contentType = "text/html";
    }
    else if (strstr(fileName, ".jpg") || strstr(fileName, ".jpeg")) {
        contentType = "image/jpeg";
    }
    else if (strstr(fileName, ".gif")) {
        contentType = "image/gif";
    }
    else if (strstr(fileName, ".png")) {
        contentType = "image/png";
    }
    else if (strstr(fileName, ".ico")) {
        contentType = "image/x-icon";
    }
    else if (strstr(fileName, ".txt")) {
        contentType = "text/plain";
    }
    else {
        contentType = "application/octet-stream";
    }

    char responseHeader[1024] = "";
    sprintf(responseHeader, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %lld\r\n\r\n", contentType.c_str(), fileSize);// Construct the response header.
    printf(responseHeader);
    printf("---------------------------------------\n");
    printf("\n\n\n");
    send(clientSocket, responseHeader, strlen(responseHeader), 0);
    send(clientSocket, buffer, fileSize, 0);
    delete[] buffer;
}


int main() {
    WSADATA wsaData;
    // Initialize Winsock
    if (WSAStartup(0x0202, &wsaData) != 0) {
        cout << "Winsock startup failed with error!" << endl;
        return 1;
    }
    else if (wsaData.wVersion != 0x0202) {
        cout << "Winsock version is not correct!" << endl;
        WSACleanup();
        return 1;
    }
    else {
        cout << "Winsock startup Ok!" << endl;
    }
    printf("---------------------------------------\n");

    // Create socket
    SOCKET srvSocket;
    sockaddr_in srvaddr;
    srvSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (srvSocket == INVALID_SOCKET) {
        cout << "Socket create failed!" << endl;
        WSACleanup();
        return 1;
    }
    else {
        cout << "Socket create Ok!" << endl;
    }
    printf("---------------------------------------\n");

    // Bind socket
    int srvPort = 5050;
    char srvIP[20] = "127.0.0.1";
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(srvPort);
    srvaddr.sin_addr.S_un.S_addr = inet_addr(srvIP);

    int rtn = bind(srvSocket, (LPSOCKADDR)&srvaddr, sizeof(srvaddr));
    if (rtn == SOCKET_ERROR) {
        cout << "Socket bind false!" << endl;
        closesocket(srvSocket);
        WSACleanup();
        return 1;
    }
    else {
        cout << "Socket bind Ok!" << endl;
    }
    printf("---------------------------------------\n");

    // Listen socket
    rtn = listen(srvSocket, 5);
    if (rtn == SOCKET_ERROR) {
        cout << "Socket listen false!" << endl;
        closesocket(srvSocket);
        WSACleanup();
        return 1;
    }
    else {
        cout << "Socket listen Ok!" << endl;
    }
    printf("---------------------------------------\n");

    
    sockaddr_in clientaddr;
    int addrlen = sizeof(clientaddr);

    while (1) {
        // Accept socket
        SOCKET clientSocket = accept(srvSocket, (sockaddr*)&clientaddr, &addrlen);
        if (clientSocket == INVALID_SOCKET) {
            cout << "Socket accept false!" << endl;
            closesocket(srvSocket);
            WSACleanup();
            return 1;
        }
        else {
            cout << "Socket accept Ok!" << endl;
            cout << "Client IP: " << inet_ntoa(clientaddr.sin_addr) << endl; //inet_ntoa: Convert IP address to string
            cout << "Client Port: " << ntohs(clientaddr.sin_port) << endl;
        }
        printf("---------------------------------------\n");
        // Recv socket
        char recvData[2000] = "";
        int nRc = recv(clientSocket, recvData, sizeof(recvData), 0);
        if (nRc == SOCKET_ERROR) {
            cout << "Socket recv false!" << endl;
            closesocket(srvSocket);
            WSACleanup();
            return 1;
        }
        else {
            cout << "Socket recv Ok!" << endl;
        }
        printf("---------------------------------------\n");

        // Parse the requested resource name and extension name
        char reqName[200] = "";
        char extName[100] = "";
        printf(recvData);
        for (int i = 0; i < nRc; i++) {
            if (recvData[i] == '/') {
                for (int j = 0; j < nRc - i; j++) {
                    if (recvData[i] != ' ') {
                        reqName[j] = recvData[i];
                        i++;
                    }
                    else {
                        reqName[j + 1] = '\0';
                        break;
                    }
                }
                break;
            }
        }
        for (int k = 0; k < nRc; k++) {
            if (recvData[k] == '.') {
                for (int j = 0; j < nRc - k; j++) {
                    if (recvData[k + 1] != ' ') {
                        extName[j] = recvData[k + 1];
                        k++;
                    }
                    else {
                        extName[j + 1] = '\0';
                        break;
                    }
                }
                break;
            }
        }

        cout << "extension name:" << extName << endl; // Print the extension name

        char fileName[1000] = "";
        strcat(fileName, "C:\\Users\\20963\\Desktop\\Grade3\\¼ÆËã»úÍøÂç\\lab\\socket-programming\\"); // Base path

        // Append the requested resource name to the base path
        strcat(fileName, reqName);

        cout << "reqName:" << reqName << endl;
        cout << "fileName:" << fileName << endl;

        sendData(fileName, clientSocket);
        cout << "----------------------------------------------" << endl << endl << endl;
        closesocket(clientSocket);// Close the client socket
    }

    closesocket(srvSocket);
    WSACleanup();
    return 0;
}

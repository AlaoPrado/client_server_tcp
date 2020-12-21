#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

int serverSocketHandle = -1;

void socketOperationCheck(int returnValue, const char *message){
    if(returnValue < 0){
        printf("%s\n", message);
        exit(1);
    }
}

void finishServer(int signalNumber){
    if(serverSocketHandle != -1){
        close(serverSocketHandle);
    }
    exit(0);
}

int isUpperCase(char c){ return (c >= 65 && c <= 90); }

int isLowercase(char c){ return (c >= 97 && c <= 122); }

char revertCaption(char c){
    char revertedC;
    revertedC = c;
    if(isUpperCase(c))
        revertedC += 32;
    else if(isLowercase(c))
        revertedC -= 32;
    return revertedC;
}

void revertString(char *revertedText, char *text, int length){
    int i;
    for(i = 0; i < length; i++){
        revertedText[i] = revertCaption(text[length - 1 - i]);
    }
}

void handleRequest(int clientSocketHandle){
    int bufferSize = 80;
    char buffer[bufferSize + 1], revertedText[bufferSize + 1];
    ssize_t numberOfBytesRecv, numberOfBytesSend;

    numberOfBytesRecv = recv(clientSocketHandle, buffer, bufferSize, 0);
    socketOperationCheck(numberOfBytesRecv, "Receive error");
    buffer[numberOfBytesRecv] = '\0';
    revertString(revertedText, buffer, numberOfBytesRecv);
    numberOfBytesSend = send(clientSocketHandle, revertedText, numberOfBytesRecv, 0);
    socketOperationCheck(numberOfBytesSend, "Send error");
    close(clientSocketHandle);
}


int main(int argc, char *argv[]){
    if(argc != 2){
        printf("A porta do servidor deve ser informada como o primeiro argumento do programa\n");
        exit(1);
    }

    signal(SIGINT, finishServer);

    int clientConnectionLimit;
    in_port_t serverPort;
    struct sockaddr_in serverSocketAddress, clientSocketAddress;

    serverPort = atoi(argv[1]);

    serverSocketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    socketOperationCheck(serverSocketHandle, "Server socket creation error\n");

    memset(&serverSocketAddress, 0, sizeof(struct sockaddr_in));
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverSocketAddress.sin_port = htons(serverPort);

    socketOperationCheck(bind(serverSocketHandle, (struct sockaddr*) &serverSocketAddress, sizeof(struct sockaddr_in)), "Bind error\n");
    clientConnectionLimit = 10;
    socketOperationCheck(listen(serverSocketHandle, clientConnectionLimit), "listen error\n");

    while(1){
        int clientSocketHandle = 0;
        int clientSocketAddressLen;
        clientSocketAddressLen = sizeof(struct sockaddr_in);
        clientSocketHandle = accept(serverSocketHandle, (struct sockaddr*) &clientSocketAddress, &clientSocketAddressLen);
        socketOperationCheck(clientSocketHandle, "Server socket creation error\n");
        handleRequest(clientSocketHandle);
    }
    return 0;
}

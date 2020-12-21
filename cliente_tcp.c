#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int serverSocketHandle = -1;

void socketOperationCheck(int returnValue, const char *message){
    if(returnValue < 0){
        printf("%s\n", message);
        exit(1);
    }
}

void revertString(char *buffer, int bufferSize, int clientSocketHandle, char *text, int length){
    int numberOfBytesSend, numberOfBytesRecv;

    numberOfBytesSend = send(clientSocketHandle, text, length, 0);
    socketOperationCheck(numberOfBytesSend, "Send error");
    numberOfBytesRecv = recv(clientSocketHandle, buffer, bufferSize, 0);
    socketOperationCheck(numberOfBytesRecv, "Receive error");
    buffer[numberOfBytesRecv] = '\0';
}

void getLineText(char *text, char *line, int length){
    int i;
    for(i = 0; i < length && line[i] != '\t' && line[i] != '\n'; i++){
        text[i] = line[i];
    }
    text[i] = '\0';
}

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Numero de argumentos invalido\n");
        exit(1);
    }

    int clientSocketHandle, bufferSize = 80;
    char *serverIP, text[bufferSize + 1], revertedText[bufferSize + 1];
    in_port_t serverPort;
    struct sockaddr_in serverSocketAddress;

    serverIP = argv[1];
    serverPort = atoi(argv[2]);

    fgets(text, bufferSize + 1, stdin);
    getLineText(text, text, strlen(text));

    clientSocketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    socketOperationCheck(clientSocketHandle, "Client socket creation error\n");

    memset(&serverSocketAddress, 0, sizeof(struct sockaddr_in));
    serverSocketAddress.sin_family = AF_INET;
    if(inet_pton(AF_INET, serverIP, &serverSocketAddress.sin_addr.s_addr) <= 0){
        printf("IP address convertion error\n");
        exit(1);
    }
    serverSocketAddress.sin_port = htons(serverPort);

    socketOperationCheck(connect(clientSocketHandle, (struct sockaddr *) &serverSocketAddress, sizeof(struct sockaddr_in)), "Connect error\n");

    revertString(revertedText, bufferSize, clientSocketHandle, text, strlen(text));
    printf("%s\n", revertedText);

    close(clientSocketHandle);
    return 0;
}

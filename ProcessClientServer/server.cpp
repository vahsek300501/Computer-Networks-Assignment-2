#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <fstream>
#include <sys/file.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8000
using namespace std;

int writeFileDescriptor;

struct SocketDescriptor {
    int socketFileDescriptor;
    string clientIP;
    string clientPort;
};

long long int getFactorialOfNumber(int num) {
    if(num == 0)
        return 1;
    long long int factorial = 1;
    for(int i = 1;i<=num;i++) {
        factorial = factorial * i;
    }
    return factorial;
}

void printData(struct SocketDescriptor & cntSocket) {
    sleep(0.001);
    int* cntSocketFileDesc = &cntSocket.socketFileDescriptor;
    string clientIP = cntSocket.clientIP;
    string clientPort = cntSocket.clientPort;
    char buffer[1024];
    int lastNum = 0;
    while(true) {
        if(lastNum == 20)
            break;
        bzero(buffer,1024);
        read(*cntSocketFileDesc,buffer,1024);
        int num = atoi(buffer);
        long long int factorial = getFactorialOfNumber(num);
        string factString = to_string(factorial);
        const char * factCharArr = factString.c_str();
        cout<<"[ClientIP: "<<clientIP<<" ClientPort:"<<clientPort<<"]   Number:"<<num<<"  Factorial: "<<factorial<<endl;
        flock(writeFileDescriptor,LOCK_EX);
        string finalOutputString = "[ClientIP: "+clientIP+" ClientPort:"+clientPort+"]   Number:"+to_string(num)+"  Factorial: "+to_string(factorial)+"\n";
        const char * finalOutputCharArr = finalOutputString.c_str();
        write(writeFileDescriptor,finalOutputCharArr,strlen(finalOutputCharArr));
        flock(writeFileDescriptor,LOCK_UN);
        int bytesSent = send(*cntSocketFileDesc,factCharArr,strlen(factCharArr),0);
        lastNum++;
    }
    close(*cntSocketFileDesc);
}

int main() {
    struct sockaddr_in serverAddress;
    socklen_t clientLength;
    int cntClientCount = 0;

    struct SocketDescriptor * newSocketDescriptor;
    bzero((char *)&serverAddress,sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

//  Socket System Call
    int serverSocketFileDescriptor = socket(AF_INET,SOCK_STREAM,0);
    if(serverSocketFileDescriptor < 0) {
        cout<<"Error in creating main socket"<<endl;
        return -1;
    }

//  Bind System Call
    int socketBind = bind(serverSocketFileDescriptor,(struct sockaddr *)&serverAddress,sizeof(serverAddress));
    if(socketBind < 0) {
        cout<<"Error in binding to address"<<endl;
        return -1;
    }

//  Listen System Call
    listen(serverSocketFileDescriptor,20);

    pid_t processID[10];

    for(int i =0;i<10;i++) {
        struct sockaddr_in clientAddress; 
        clientLength = sizeof(clientAddress);
        newSocketDescriptor = new SocketDescriptor();
        newSocketDescriptor->socketFileDescriptor = accept(serverSocketFileDescriptor,(struct sockaddr *)&clientAddress,&clientLength);
        if(newSocketDescriptor->socketFileDescriptor < 0) {
            cout<<"error in accepting connection"<<endl;
            return -1;
        }
        newSocketDescriptor->clientIP = to_string(clientAddress.sin_addr.s_addr);
        newSocketDescriptor->clientPort = to_string(clientAddress.sin_port);
        if(i == 0) {
            writeFileDescriptor = open("clientServerFile.txt",O_WRONLY | O_CREAT);
            if(writeFileDescriptor < 0) {
                cout<<"Error in opening file"<<endl;
                return -1;
            }
        }

        if((processID[i] = fork()) == 0) {
            printData(*newSocketDescriptor);
            exit(0);
        } else if(processID[i] < 0) {
            cout<<"error in forking process"<<endl;
            abort();
        }
    }

    int status = 0;
    pid_t wpid;
    while((wpid = wait(&status)) > 0);
    close(writeFileDescriptor);
    flock(writeFileDescriptor,LOCK_UN);
    shutdown(serverSocketFileDescriptor,SHUT_RDWR);
    return 0;
}
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <fstream>
#include <pthread.h>
#include <sys/file.h>
#include <chrono>
#include <ctime> 

#define PORT 8000
using namespace std;

using namespace std::chrono;

bool succesfullExec = true;
bool unsuccesfullExec = false;

struct SocketDescriptor {
    int socketFileDescriptor;
    string clientIP;
    string clientPort;
};

pthread_mutex_t mutexLock;
fstream outFile;

long long int getFactorialOfNumber(int num) {
    if(num == 0)
        return 1;
    long long int factorial = 1;
    for(int i = 1;i<=num;i++) {
        factorial = factorial * i;
    }
    return factorial;
}


void * printData(void * args) {
    sleep(0.001);
    int* cntSocketFileDesc = &((struct SocketDescriptor*) args)->socketFileDescriptor;
    string clientIP = ((struct SocketDescriptor*) args)->clientIP;
    string clientPort = ((struct SocketDescriptor*) args)->clientPort;
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
        pthread_mutex_lock(&mutexLock);
        string finalOutputStr = "[ClientIP: "+clientIP+"  ClientPort: "+clientPort+"]   Number: "+to_string(num)+"   Factorial: "+factString+"\n";
        cout<<"[ClientIP: "<<clientIP<<"  ClientPort: "<<clientPort<<"]   Number: "<<num<<"   Factorial: "<<factorial<<endl;
        outFile << finalOutputStr;
        pthread_mutex_unlock(&mutexLock);
        int bytesSent = send(*cntSocketFileDesc,factCharArr,strlen(factCharArr),0);
        lastNum++;
    }
    close(*cntSocketFileDesc);
    return (void*)&succesfullExec;
}

int main() {
    struct sockaddr_in serverAddress;
    socklen_t clientLength;
    int cntClientCount = 0;

    struct SocketDescriptor * newSocketDescriptor;
    pthread_mutex_init(&mutexLock,nullptr);

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
    pthread_t threadArr[10];
    // milliseconds startTime = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    for(int i = 0;i<10;i++) {
        struct sockaddr_in clientAddress; 
        clientLength = sizeof(clientAddress);
        newSocketDescriptor = new SocketDescriptor();
        newSocketDescriptor->socketFileDescriptor = accept(serverSocketFileDescriptor,(struct sockaddr *)&clientAddress,&clientLength);
        if(newSocketDescriptor->socketFileDescriptor < 0) {
            cout<<"error in accepting connection"<<endl;
            return -1;
        }
        if(i == 0 && newSocketDescriptor->socketFileDescriptor >= 0) {
            outFile.open("serverClientFile.txt",ios::out);
            if(!outFile.is_open()) {
                cout<<"Unable to open file"<<endl;
                return -1;
            }
        }
        newSocketDescriptor->clientIP = to_string(clientAddress.sin_addr.s_addr);
        newSocketDescriptor->clientPort = to_string(clientAddress.sin_port);
        pthread_create(&threadArr[i],NULL,printData,newSocketDescriptor);
    }
    // milliseconds endTime = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    // cout<<"Time Elapsed to handle all clients are: "<<endTime.count() - startTime.count()<<endl;
    for(int i = 0;i<10;i++) {
        pthread_join(threadArr[i],nullptr);
    }
    outFile.close();
    shutdown(serverSocketFileDescriptor,SHUT_RDWR);
    return 0;
}



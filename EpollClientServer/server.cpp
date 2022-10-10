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
#include <sys/time.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#define PORT 8080
using namespace std;

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

int main() {
    struct sockaddr_in serverAddress;
    socklen_t clientLength;
    int cntClientCount = 0;
    int maxClients = 0;
    unordered_map <int,struct SocketDescriptor *> clientDescMap;
    fstream outFile;
    struct epoll_event fileDescriptors[20];
    struct epoll_event cntEventAdd;
    struct epoll_event cntEventClose;
    int epollFileDescriptor = epoll_create1(0);
    int eventAdd = 0;
    
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

    cntEventAdd.data.fd = serverSocketFileDescriptor;
    cntEventAdd.events = POLL_IN;

    eventAdd = epoll_ctl(epollFileDescriptor,EPOLL_CTL_ADD,serverSocketFileDescriptor,&cntEventAdd);
    if(eventAdd < 0) {
        cout<<"error in adding event"<<endl;
        return -1;
    }

    for(int i = 0;i<20;i++) {
        fileDescriptors[i].data.fd = 0;
        fileDescriptors[i].events = POLL_IN;
    }

    while(true) {
        if(maxClients == 10 && cntClientCount <= 0)
            break;
        int cntActiveFileDesc = epoll_wait(epollFileDescriptor,fileDescriptors,20,0);
        if(cntActiveFileDesc < 0) {
            cout<<"error in polling"<<endl;
            return -1;
        }
        for(int i =0;i<cntActiveFileDesc;i++) {
            if(fileDescriptors[i].data.fd == serverSocketFileDescriptor) {
                struct sockaddr_in clientAddress;
                clientLength = sizeof(clientAddress);
                struct SocketDescriptor * newSocketDescriptor;
                newSocketDescriptor = new SocketDescriptor();
                newSocketDescriptor->socketFileDescriptor = accept(serverSocketFileDescriptor,(struct sockaddr *)&clientAddress,&clientLength);
                if(newSocketDescriptor->socketFileDescriptor < 0) {
                    cout<<"error in creating new client"<<endl;
                    return -1;
                }
                newSocketDescriptor->clientIP = to_string(clientAddress.sin_addr.s_addr);
                newSocketDescriptor->clientPort = to_string(clientAddress.sin_port);
                clientDescMap[newSocketDescriptor->socketFileDescriptor] = newSocketDescriptor;
                cntEventAdd.data.fd = newSocketDescriptor->socketFileDescriptor;
                cntEventAdd.events = POLL_IN;
                eventAdd = epoll_ctl(epollFileDescriptor,EPOLL_CTL_ADD,newSocketDescriptor->socketFileDescriptor,&cntEventAdd);
                cntClientCount++;
                maxClients++;
                if(maxClients == 1) {
                    outFile.open("clientServerFile.txt",ios::out);
                }
            }

            if(fileDescriptors[i].data.fd != serverSocketFileDescriptor) {
                char buffer[1024];
                memset(buffer,0,1024);
                int bytesRead = read(fileDescriptors[i].data.fd,buffer,1024);
                if(bytesRead <= 0) {
                    int eventRem = epoll_ctl(epollFileDescriptor,EPOLL_CTL_DEL,fileDescriptors[i].data.fd,NULL);
                    if(eventRem < 0) {
                        cout<<"error in removing file descriptor:  "<<endl;
                        return -1;
                    }
                    close(fileDescriptors[i].data.fd);
                    cntClientCount--;
                    continue;
                }
                int num = atoi(buffer);
                long long int factorial = getFactorialOfNumber(num);
                string factString = to_string(factorial);
                const char *factCharArr = factString.c_str();
                cout<<"[ClientIP: "<<clientDescMap[fileDescriptors[i].data.fd]->clientIP<<"   ClientPort: "<<clientDescMap[fileDescriptors[i].data.fd]->clientPort<<"]   Number: "<<num<<"   Factorial: "<<factorial<<endl;
                string finalFileWriteString =  "[ClientIP: "+clientDescMap[fileDescriptors[i].data.fd]->clientIP+"   ClientPort: "+clientDescMap[fileDescriptors[i].data.fd]->clientPort+"]   Number: "+to_string(num)+"   Factorial: "+to_string(factorial)+"\n";
                outFile<<finalFileWriteString;
                send(fileDescriptors[i].data.fd,factCharArr,strlen(factCharArr),0);
            }
        }
    }
    outFile.close();
    close(epollFileDescriptor);
    shutdown(serverSocketFileDescriptor,SHUT_RDWR);
    return 0;
}
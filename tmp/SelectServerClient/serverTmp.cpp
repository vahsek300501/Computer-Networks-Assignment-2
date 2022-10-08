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

#define PORT 8000
using namespace std;

int main() {
    struct sockaddr_in serverAddress;
    socklen_t clientLength;
    int cntClientCount = 0;
    vector<int> clientSockets;

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

    fd_set fileDescriptors, readFileDescriptors;
    FD_ZERO(&fileDescriptors);
    FD_SET(serverSocketFileDescriptor,&fileDescriptors);

    while(true) {
        readFileDescriptors = fileDescriptors;
        int cntActiveReadFileDesc = select(FD_SETSIZE,&readFileDescriptors,NULL,NULL,NULL);
        if(cntActiveReadFileDesc < 0) {
            cout<<"Error in reading read file descriptors"<<endl;
            return -1;
        }

        for(int i = 0;i<FD_SETSIZE;i++) {
            if(FD_ISSET(i,&readFileDescriptors) && i == serverSocketFileDescriptor) {
                struct sockaddr_in clientAddress;
                clientLength = sizeof(clientAddress);
                int newSocketDescriptorTmp = accept(serverSocketFileDescriptor,(struct sockaddr *)&clientAddress,&clientLength);
                if(newSocketDescriptorTmp < 0) {
                    cout<<"error in accepting new connection"<<endl;
                    return -1;
                }
                clientSockets.push_back(newSocketDescriptorTmp);
                FD_SET(newSocketDescriptorTmp,&fileDescriptors);
                cntClientCount++;
            }
            char * responseMsg = "MessageReceived\0";
            if(FD_ISSET(i,&readFileDescriptors) && i != serverSocketFileDescriptor) {
                char buffer[1024];
                bzero(buffer,1024);
                read(i,buffer,1024);
                cout<<"[Client FD:"<<i<<"]  "<<buffer<<endl;
                send(i,responseMsg,strlen(responseMsg),0);
            }
            
        }
    }

    return 0;
}
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
#include <chrono>
#include <ctime> 

#define PORT 8000
using namespace std;

using namespace std::chrono;

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
    fd_set fileDescriptors, readFileDescriptors;
    unordered_map <int,struct SocketDescriptor *> clientDescMap;
    fstream outFile;

    
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
    // milliseconds startTime = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    
    FD_ZERO(&fileDescriptors);
    FD_SET(serverSocketFileDescriptor,&fileDescriptors);

    while(true) {
        if(maxClients == 10 && cntClientCount <= 0) {
            break;
        }
        readFileDescriptors = fileDescriptors;
        int cntActiveReadFileDescCount = select(FD_SETSIZE,&readFileDescriptors,NULL,NULL,NULL);
        if(cntActiveReadFileDescCount < 0) {
            cout<<"Error in reading read file descriptors"<<endl;
            return -1;
        }

        for(int i = 0;i<FD_SETSIZE;i++) {
            if(FD_ISSET(i,&readFileDescriptors) && i == serverSocketFileDescriptor) {
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
                FD_SET(newSocketDescriptor->socketFileDescriptor,&fileDescriptors);
                cntClientCount++;
                maxClients++;
                if(maxClients == 1) {
                    outFile.open("clientServerFile.txt",ios::out);
                }
            }

            if(FD_ISSET(i,&readFileDescriptors) && i != serverSocketFileDescriptor) {
                char buffer[1024];
                bzero(buffer,1024);
                int bytesRead = read(i,buffer,1024);
                if(bytesRead <= 0) {
                    FD_CLR(i,&fileDescriptors);
                    close(i);
                    cntClientCount--;
                    cout<<"Client with File Descriptor "<<i<<" is closed"<<endl;
                    continue;
                }
                int num = atoi(buffer);
                long long int factorial = getFactorialOfNumber(num);
                string factString = to_string(factorial);
                const char *factCharArr = factString.c_str();
                cout<<"[ClientIP: "<<clientDescMap[i]->clientIP<<"   ClientPort: "<<clientDescMap[i]->clientPort<<"]   Number: "<<num<<"   Factorial: "<<factorial<<endl;
                string finalFileWriteString = "[ClientIP: "+clientDescMap[i]->clientIP+"   ClientPort: "+clientDescMap[i]->clientPort+"]   Number: "+to_string(num)+"   Factorial: "+to_string(factorial)+"\n";
                outFile<<finalFileWriteString;
                send(i,factCharArr,strlen(factCharArr),0);
            }
        }
    }
    // milliseconds endTime = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    // cout<<"Time Elapsed to handle all clients are: "<<endTime.count() - startTime.count()<<endl;
    outFile.close();
    shutdown(serverSocketFileDescriptor,SHUT_RDWR);
    return 0;
}
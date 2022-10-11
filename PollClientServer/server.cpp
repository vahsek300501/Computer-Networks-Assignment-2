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
#include <chrono>
#include <ctime> 

#define PORT 8080
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

void printFileDescArray(struct pollfd * fds,int totalCount) {
    for(int i = 0;i<totalCount;i++) {
        cout<<fds[i].fd<<"   "<<fds[i].events<<"   "<<fds[i].revents<<endl;
    }
}

int main() {
    struct sockaddr_in serverAddress;
    socklen_t clientLength;
    int cntClientCount = 0;
    int maxClients = 0;
    unordered_map <int,struct SocketDescriptor *> clientDescMap;
    fstream outFile;
    struct pollfd fileDescriptors[20];
    // int cntFileDescCount = 0;
    
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

    for(int i = 0;i<20;i++) {
        fileDescriptors[i].fd = 0;
        fileDescriptors[i].events = POLL_IN;
    }

    fileDescriptors[0].fd = serverSocketFileDescriptor;
    fileDescriptors[0].events = POLL_IN;
    cntClientCount++;
    maxClients++;
    // milliseconds startTime = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    while(true) {
        if(maxClients == 11 && cntClientCount == 1)
            break;
        int cntActiveFileDesc = poll(fileDescriptors,cntClientCount,0);
        if(cntActiveFileDesc < 0) {
            cout<<"Error in polling"<<endl;
            return -1;
        }
        int activeFileDesc = cntClientCount;
        for(int i = 0;i<activeFileDesc;) {
            if(i == 0 && (fileDescriptors[i].revents & POLL_IN)) {
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
                fileDescriptors[cntClientCount].fd = newSocketDescriptor->socketFileDescriptor;
                fileDescriptors[cntClientCount].events = POLL_IN;
                cntClientCount++;
                maxClients++;

                if(maxClients == 2) {
                    outFile.open("clientServerFile.txt",ios::out);
                }
            }

            if(i != 0 && (fileDescriptors[i].revents & POLL_IN)) {
                char buffer[1024];
                memset(buffer,0,1024);
                int bytesRead = read(fileDescriptors[i].fd,buffer,1024);
                if(bytesRead <= 0) {
                    cout<<"closing socket"<<endl;
                    close(fileDescriptors[i].fd);
                    for(int j = i+1;j<cntClientCount;j++) {
                        fileDescriptors[j-1].fd = fileDescriptors[j].fd;
                        fileDescriptors[j-1].events = fileDescriptors[j].events;
                        fileDescriptors[j-1].revents = fileDescriptors[j].revents;
                    }
                    cntClientCount--;
                    activeFileDesc--;
                    continue;
                }
                int num = atoi(buffer);
                long long int factorial = getFactorialOfNumber(num);
                string factString = to_string(factorial);
                const char *factCharArr = factString.c_str();
                cout<<"[ClientIP: "<<clientDescMap[fileDescriptors[i].fd]->clientIP<<"   ClientPort: "<<clientDescMap[fileDescriptors[i].fd]->clientPort<<"]   Number: "<<num<<"   Factorial: "<<factorial<<endl;
                string finalFileWriteString =  "[ClientIP: "+clientDescMap[fileDescriptors[i].fd]->clientIP+"   ClientPort: "+clientDescMap[fileDescriptors[i].fd]->clientPort+"]   Number: "+to_string(num)+"   Factorial: "+to_string(factorial)+"\n";
                outFile<<finalFileWriteString;
                send(fileDescriptors[i].fd,factCharArr,strlen(factCharArr),0);
            }
            i++;
        }
    }
    // milliseconds endTime = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    // cout<<"Time Elapsed to handle all clients are: "<<endTime.count() - startTime.count()<<endl;
    outFile.close();
    shutdown(serverSocketFileDescriptor,SHUT_RDWR);
    return 0;
}
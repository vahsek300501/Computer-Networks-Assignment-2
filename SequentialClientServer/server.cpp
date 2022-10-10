#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <fstream>
#include <arpa/inet.h>

#define PORT 8000
using namespace std;

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
    struct sockaddr_in serverAddress,clientAddress;
    socklen_t clientLength;
    int cntClientCount = 0;
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
    clientLength = sizeof(clientAddress);


    outFile.open("serverClientFile.txt",ios::out);
    if (!outFile.is_open()) {
        cout << "error opening file";
        return -1;
    }

    while(true) {

//      Accept system call
        int newClient = accept(serverSocketFileDescriptor,(struct sockaddr *)&clientAddress,&clientLength);
        if(newClient < 0) {
            cout << "Error in accepting client request"<<endl;
            return -1;
        } else {
            cntClientCount++;
            cout<<"Client: "<<cntClientCount<<" connected successfully"<<endl;
            cout<<"Client IP Address: "<<clientAddress.sin_addr.s_addr<<endl;
            cout<<"Clinet Port: "<<clientAddress.sin_port<<endl;
            string connStr = "Client: "+to_string(cntClientCount)+" connected successfully\n";
            string clientIP = "Client IP Address:  "+to_string(clientAddress.sin_addr.s_addr)+"\n";
            string clientPort = "Client Port:  "+to_string(clientAddress.sin_port)+"\n";
            outFile<<connStr;
            outFile<<clientIP;
            outFile<<clientPort;
        }

        char clientMsgBuffer[1024];
        
        for(int i = 0;i<20;i++) {
            bzero(clientMsgBuffer,1024);
            read(newClient,clientMsgBuffer,1024);
            int num = atoi(clientMsgBuffer);
            cout<<"[Client:"<<cntClientCount<<"]  "<<num<<" "<<endl;
            long long int factorial = getFactorialOfNumber(num);
            string factString = to_string(factorial);
            const char * factCharArr = factString.c_str();
            int bytesSent = send(newClient,factCharArr,strlen(factCharArr),0);
            string queryStr = "[Client"+to_string(cntClientCount)+":]   "+to_string(num)+"   [Server:]   "+to_string(factorial)+"\n";
            outFile<<queryStr;
        }
        close(newClient);
        string spaceString = "\n\n";
        outFile<<spaceString;
        cout<<endl<<endl;
        if(cntClientCount >= 10)
            break;
    }
    outFile.close();
    shutdown(serverSocketFileDescriptor,SHUT_RDWR);

    return 0;
}

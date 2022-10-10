#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <pthread.h>
#define PORT 8080

using namespace std;

int main() {
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    inet_pton(AF_INET,"127.0.0.1",&serverAddress.sin_addr);
    serverAddress.sin_port = htons(PORT);
    int clientSocketFileDescriptor = socket(AF_INET,SOCK_STREAM,0);
    if (clientSocketFileDescriptor < 0)
    {
        cout << "error creating socket" << endl;
        return -1;
    }

    int connectionStatus = connect(clientSocketFileDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectionStatus < 0) {
        cout << "error in connecting to server" << endl;
        return -1;
    }
    else {
        cout << "[Client:] connected successfully" << endl;
    }

    char buffer[1024];
    char buffer2[1024];
    int i = 0;
    while(i < 10) {
        bzero(buffer,1024);
        bzero(buffer2,1024);
        cin>>buffer;
        send(clientSocketFileDescriptor,buffer,strlen(buffer),0);
        i++;
        int tmp = read(clientSocketFileDescriptor,buffer2,1024);
        cout<<"bytes Read:  "<<tmp<<"    "<<buffer2<<endl;
    }
    close(clientSocketFileDescriptor);
    return 0;
}
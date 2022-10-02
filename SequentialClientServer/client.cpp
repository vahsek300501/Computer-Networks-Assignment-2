#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>

#define PORT 8000

using namespace std;

int main() {
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    inet_pton(AF_INET,"127.0.0.1",&serverAddress.sin_addr);
    serverAddress.sin_port = htons(PORT);


    for(int i = 1; i <= 10; i++) {
        int clientSocketFileDescriptor = socket(AF_INET,SOCK_STREAM,0);
        if (clientSocketFileDescriptor < 0) {
            cout << "error creating socket" << endl;
            return -1;
        }

        int connectionStatus = connect(clientSocketFileDescriptor,(struct sockaddr *)&serverAddress,sizeof(serverAddress));
        if(connectionStatus < 0) {
            cout << "error in connecting to server" << endl;
            return -1;
        } else {
            cout<< "[Client "<<i<<"] connected successfully"<<endl;
        }
        for(int i = 1; i <= 20; i++) {
            char buffer[1024];
            bzero(buffer, 1024);
            const char * cntClientMsg = to_string(i).c_str();
            int bytesSent = send(clientSocketFileDescriptor,cntClientMsg,strlen(cntClientMsg),0);
            read(clientSocketFileDescriptor,buffer,1024);
            cout<<"[Server]  factorial: "<<buffer<<endl;
            // sleep(0.001); 
        }
        close(clientSocketFileDescriptor);
    }
    return 0;
}
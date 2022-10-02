#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <pthread.h>
#define PORT 8000

using namespace std;

pthread_mutex_t mutexLock;
vector<vector<long long int>> ansList;
bool succesfullExec = true;
bool unsuccesfullExec = false;

void * createClient(void * args) {
    vector<long long int> ans;
    sleep(0.001);
    int clientNum = *((int *)args);
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    inet_pton(AF_INET,"127.0.0.1",&serverAddress.sin_addr);
    serverAddress.sin_port = htons(PORT);
    int clientSocketFileDescriptor = socket(AF_INET,SOCK_STREAM,0);
    if (clientSocketFileDescriptor < 0)
    {
        cout << "error creating socket" << endl;
        return nullptr;
    }

    int connectionStatus = connect(clientSocketFileDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectionStatus < 0) {
        cout << "error in connecting to server" << endl;
        return nullptr;
    }
    else {
        cout << "[Client:"<<clientNum<<"] connected successfully" << endl;
    }

    char buffer[1024];

    for (int i = 1; i <= 20; i++) {
        bzero(buffer, 1024);
        const char *num = to_string(i).c_str();
        int bytesSent = send(clientSocketFileDescriptor, num, strlen(num), 0);
        read(clientSocketFileDescriptor, buffer, 1024);
        pthread_mutex_lock(&mutexLock);
        cout << "[Client:" << clientNum << "] number: " << i << " factorial: " << buffer << endl;
        long long tmp = atoll(buffer);
        ans.push_back(tmp);
        pthread_mutex_unlock(&mutexLock);
    }
    pthread_mutex_lock(&mutexLock);
    ansList.push_back(ans);
    pthread_mutex_unlock(&mutexLock);
    return nullptr;
}

int main() {
    pthread_mutex_init(&mutexLock,nullptr);
    int * clientCount = new int;
    *clientCount = 1;
    pthread_t threadArr[10];
    for(int i = 1;i<=10;i++) {
        pthread_create(&threadArr[i-1],NULL,createClient,clientCount);
        (*clientCount)++;
        char * tmp;
    }
    for(int i = 0;i<10;i++) {
        pthread_join(threadArr[i],NULL);
    }

    cout<<endl<<endl<<endl;
    for(int i = 0;i<ansList.size();i++) {
        for(long long int val : ansList[i]) {
            cout<<val<<" ";
        }
        cout<<endl;
    }
    return 0;
}
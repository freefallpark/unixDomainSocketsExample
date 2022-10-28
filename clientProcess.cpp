#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SERVER_PATH "tpf_unix_sock.server"
#define CLIENT_PATH "tpf_unix_sock.client"
#define DATA "Hello from client"

using namespace std;
int main() {
    // Var Instances:
    int client_sock;
    ssize_t rc;
    socklen_t len;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    char buf[256];

    //Memsetting 0's
    memset(&server_sockaddr,0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr,0, sizeof(struct sockaddr_un));

    // Create Socket Stream:
    client_sock = socket(AF_UNIX,SOCK_STREAM,0);
    if(client_sock == -1){
        cout << "Failed to create Client Socket" << endl;
        return 1;
    }

    //Setup client Socket Addr struct
    client_sockaddr.sun_family = AF_UNIX;
    strcpy(client_sockaddr.sun_path, CLIENT_PATH);
    len = sizeof(client_sockaddr);

    // Unlink File:
    unlink(CLIENT_PATH);

    // Bind Path:
    rc = bind(client_sock, (struct sockaddr*)&client_sockaddr,len);
    if(rc == -1){
        cout << "Failed to Bind Client Socket"<<endl;
        close(client_sock);
    }

    //Setup Sockaddr struct for server:
    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, SERVER_PATH);

    //Connect to Server
    rc = connect(client_sock, (struct sockaddr*)&server_sockaddr,len);
    if(rc == -1){
        cout << "Error Connecting to Server"<<endl;
        close(client_sock);
        return 1;
    }

    // Send data to server:
    strcpy(buf,DATA);
    cout << "Sending Data to Server ..."<<endl;
    rc = send(client_sock,buf, strlen(buf),0);
    if(rc == -1){
        cout << "Send Error..."<<endl;
        close(client_sock);
        return 1;
    }
    else{
        cout<<"Data Sent!"<<endl;
    }

    //Read Data from server:
    cout << "Waiting to receive Data from server..."<<endl;
    rc = recv(client_sock, buf,sizeof(buf),0);
    if(rc == -1){
        cout << "Error Receiving Data..."<<endl;
        close(client_sock);
        return 1;
    }
    else{
        cout<< "Data Recieved from Server: "<< buf<<endl;
    }


    //Terminate
    close(client_sock);

    return 0;
}

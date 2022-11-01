#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "converters.h"
#include <csignal>

#define SERVER_PATH "tpf_unix_sock.server"
#define CLIENT_PATH "tpf_unix_sock.client"

using namespace std;
volatile sig_atomic_t stop;
void inthand(int signum){
    stop = 1;
}
int main() {
    signal(SIGINT, inthand);

    // Var Instances:
    int client_sock;
    ssize_t rc;
    socklen_t len;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;

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

    //Prepare data for send:
    unsigned char errorCode = 0x00;
    unsigned char tmpBuf[3];
    unsigned char buf[256];
    memset(buf, '\0', sizeof(buf));
    memset(tmpBuf, '\0', sizeof(tmpBuf));
    float x, y, z, r, p, yaw;
    int converter = 1000;
    int intArray[6]; // x = [0], y = [1] z = [2], r = [3], p = [4], yaw = [5]
    // Normally these values are determined by the Face Pose Client:
    x = 12.3456; y =  5.4321; z   = 123.4567;
    r = 23.4567; p = 10.9876; yaw =   2.3456;
    float floatArray[6] = {x,y,z,r,p,yaw};
    buf[0] = errorCode;
    int count = 0;
    while(!stop){
        //Package data into one buffer and send:
        for(int k = 1; k<=16;k=k+3){
            floatArray[((k+2)/3)-1] = floatArray[((k+2)/3)-1]*(float)converter;
            intArray[((k+2)/3)-1] = (int)floatArray[((k+2)/3)-1];
            rightShift24(intArray[((k+2)/3)-1],tmpBuf);
            for(int j = 0; j<3; j++){
                buf[k+j]= tmpBuf[j];
            }
        }

        // Send data to server:
        rc = send(client_sock,buf, sizeof(buf),0);
        cout << '\r' << count << flush;
        if(rc == -1){
            cout << "Send Error..."<<endl;
            close(client_sock);
            return 1;
        }
        else{
            rc = recv(client_sock,buf,sizeof(buf), 0);
        }
        count++;
        usleep(500000);
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

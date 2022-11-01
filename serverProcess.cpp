// Standard Includes:
#include <iostream>
#include <unistd.h>
#include "converters.h"
#include <csignal>


// UDS Includes:
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <mutex>

// Defines:
#define PATH "server"

using namespace std;

volatile sig_atomic_t stop;
void inthand(int signum){
    stop =1;
}
int main() {
    // setup ctrl + C stop in while loop
    signal(SIGINT,inthand);


    //create UDP Socket:
    int sockfd;
    char buffer[256];
        memset(buffer, '\0', sizeof(buffer));
    sockaddr_un server{};
    memset(&server,0, sizeof(server));

    // Get File Descriptor...
    if((sockfd = socket(AF_UNIX, SOCK_DGRAM,0))<0){
        perror("Socket Creation Failed");
        exit(EXIT_FAILURE);
    }

    //Prep server sockaddr_un
    server.sun_family    = AF_UNIX;
    strcpy(server.sun_path, PATH);


    //Bind the socket to the server address:
    if(bind(sockfd,(const struct sockaddr*)&server,sizeof(struct sockaddr_un))<0){
        perror("Bind Failure");
        exit(EXIT_FAILURE);
    }
    printf("socket-->%s\n",PATH);

    //Enter While loop:
    size_t n;

    cout << "Entering server reading loop..."<<endl;
    while(!stop){
        //wait until the datagram packet arrives from the client:
        if(read(sockfd,buffer,256)<0){
            perror("Receiving datagram packet");
        }
        //Process the datagram packet
        printf("Client: %d\t", buffer[0]);
        usleep(500000);
    }

    //Terminate ???
    close(sockfd);
    unlink(PATH);

    return 0;
}
//
//// Var Instances:
//int server_sock, client_sock;
//ssize_t rc, bytes_rec = 0;
//socklen_t len;
//struct sockaddr_un server_sockaddr;
//struct sockaddr_un client_sockaddr;
//int backlog = 10;
//// Memsetting 0's
//memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
//memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
//
//// Create Server Socket:
//server_sock = socket(AF_UNIX, SOCK_STREAM,0);
//if(server_sock == -1){
//cout << "Error Creating Server Socket!"<<endl;
//return 1;
//}
//
//// Setup UNIX sockaddr structure with AF_UNIX
//server_sockaddr.sun_family = AF_UNIX;
//
////Give filepath to bind to
//strcpy(server_sockaddr.sun_path,SOCK_PATH);
//len = sizeof(server_sockaddr);
//
////Unlink file so bind will succeed:
//unlink(SOCK_PATH);
//
////Bind to the file:
//rc = bind(server_sock,(struct sockaddr*)&server_sockaddr,len);
//if(rc==-1){
//cout<< "Couldn't Bind File!"<<endl;
//close(server_sock);
//return 1;
//}
//
//
////Listen for Clients:
//rc = listen(server_sock, backlog);
//if(rc==-1){
//cout<< "Couldn't Bind File!"<<endl;
//close(server_sock);
//return 1;
//}
//cout << "Server Listening for Clients..."<<endl;
//
//
////Accept an incomming Connection
//
//client_sock = accept(server_sock, (struct sockaddr *)&client_sockaddr,&len);
//if(client_sock == -1){
//cout << "Error Accepting Client Socket"<<endl;
//close(server_sock);
//close(client_sock);
//return 1;
//}
//
////Get Client Peer name
//len = sizeof(client_sock);
//rc = getpeername(client_sock,(struct sockaddr*)&client_sockaddr,&len);
//if(rc == -1){
//cout << "Error Getting Peer Name!"<<endl;
//close(server_sock);
//close(client_sock);
//return 1;
//}
//else{
//cout << "Client socket filepath: " << client_sockaddr.sun_path <<endl;
//}
//
//
//// Read Client Data:
//unsigned char buf[256];
//int iVal = 0;
//float fVal = 0;
//int converter = 1000;
//int intArray[6];
//unsigned char tmpBuf[3];
//memset(tmpBuf,'\0',sizeof(tmpBuf));
//int fpcError;
//
//while(!stop){
//// poll the FD to wait for the correct number of bytes:
//
////
////        select(client_sock,)
//bytes_rec = recv(client_sock,buf, 256,0);
//
//if(bytes_rec == -1){
//cout << "Error Reading Client"<<endl;
//close(server_sock);
//close(client_sock);
//return 1;
//}
//else{
//if (!buf[0]){ // No Errors recieved!
////We expect buffer: [char errorCode[1], char x[3], char y[3], char z[3], char r[3], char p[3], char yaw[3]]
//for(int i = 1; i<=16;i= i+3){
//for(int j = 0; j<3; j++){
//tmpBuf[j] = buf[i+j];
//}
//intArray[(i+3)/3-1] = leftShift24(tmpBuf);
//cout << intArray[(i+3)/3-1]<< '\t';
//}
//memset(buf,'\0',sizeof(buf));
//rc = send(client_sock, buf, 256,0);
//}
//else{// errors comming from facePoseClient
//cout << "Error from facePoseClient"<<endl;
//};
//cout << endl;
//}
//usleep(500000);
//}
//
//
//
//
////Terminate:
//close(client_sock);
//close(server_sock);

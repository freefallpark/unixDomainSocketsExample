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
#include "connection.h"

using namespace std;

volatile sig_atomic_t stop;
void inthand(int signum){
    stop =1;
}
int main() {
    signal(SIGINT, inthand);
    sockaddr_un name{};
    int down_flag = 0;
    size_t ret = -1;
    int connection_socket,data_socket, result;
    char buffer[BUFFSIZE];

    // Create Local Socket...
    if((connection_socket = socket(AF_UNIX,SOCK_SEQPACKET,0))<0){
        perror("Error Creating Connection Socket");
        exit(EXIT_FAILURE);
    }

    //For Portability, clear whole sockaddr_un structure:
    memset(&name,0,sizeof(name));

    //Bind Socket to socket name:
    unlink(PATH);
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path,PATH,sizeof(name.sun_path)-1);
    ret = bind(connection_socket,(const struct sockaddr*)&name,sizeof(name));
    if(ret < 0){
        perror("Error binding Connection Socket");
        exit(EXIT_FAILURE);
    }

    //Prepare for Accepting new connections,
    // backlog set to 10 so while one request is being processed, others can wait... probably don't need this
    printf("Server Started and Listening for Clients...\n");
    if((ret = listen(connection_socket,10))<0){
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    //Wait for incoming connections:
    // This was in an infinite for loop on the man7.org example, but we only need one connection right now...
    if((data_socket = accept(connection_socket, nullptr, nullptr))<0){
        perror("Error during acceptance");
        exit(EXIT_FAILURE);
    }

    result = 0;
    while(!stop){
        // Wait for next Data Packet...
        if((ret = read(data_socket,buffer,sizeof(buffer)))<0){
            perror("Error during Read");
            exit(EXIT_FAILURE);
        }

        //ensure Buffer is 0 terminated:
        buffer[sizeof(buffer)-1] = 0;

        //Check if we need to leave loop:
        if(!strncmp(buffer, "END",sizeof(buffer))){
            break;
        }
        for(int i:buffer){
            printf("%d\t",(int)buffer[i]);
        }
        cout << endl;
    }

    //close socket
    close(data_socket);
    close(connection_socket);
    unlink(PATH);
    printf("Server Shutdown Complete\n");
    exit(EXIT_SUCCESS);
}


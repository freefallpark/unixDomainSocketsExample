// Standard Includes:
#include <iostream>
#include <unistd.h>
#include <csignal>


// UDS Includes:
#include <sys/socket.h>
#include <sys/un.h>
#include <mutex>
#include "connection.h"

using namespace std;
//TODO: I should probably change it so the SERVER sends the raw data while the CLIENT reads it right now the client generates the raw data... clearly i have no idea what I'm doing...

int main() {
    //Variable Instance Creation:
    sockaddr_un name{}; // Stores PATH Name and connection Family
    int connection_socket,data_socket; // These are the file descriptors for the connection socket and the client socket
    char buffer[BUFFSIZE]; // created in 'connection.h'
    bool downFlag = false; // This will help us shut down the server. its trigger by the client sending "DOWN" across the Socket.

    // Create Local Socket...
    if((connection_socket = socket(AF_UNIX,SOCK_SEQPACKET,0))<0){
        perror("Error Creating Connection Socket");
        exit(EXIT_FAILURE);
    }

    //For Portability, set entire structure to 0's
    memset(&name,0,sizeof(name));

    //Update sockaddr_un structure with PATH to socket and family type
    unlink(PATH); // Unlink first incase this path is already linked...
    name.sun_family = AF_UNIX; // Setting up a Unix Domain Socket
    strncpy(name.sun_path,PATH,sizeof(name.sun_path)-1); // PATH is created in 'connection.h'

    //Bind the FD and the PATH.
    if( bind(connection_socket,(const struct sockaddr*)&name,sizeof(name))< 0){
        perror("Error binding Connection Socket");
        exit(EXIT_FAILURE);
    }

    //Listen for new connections
    int backlog = 10;   // I think this determines how many clients it can listen for... I only  need one.
    printf("Server Started and Listening for Clients...\n");
    if(listen(connection_socket,10)<0){
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    while(true){
        //Wait for incoming connections:
        if((data_socket = accept(connection_socket, nullptr, nullptr))<0){
            perror("Error during acceptance");
            exit(EXIT_FAILURE);
        }

        //Once we've found a connection: In the future. might be a good idea to handle this with threading.. i *think* we'll need a new thread for each client
        while(true){
            // Wait for next Data Packet...
            if(read(data_socket,buffer,sizeof(buffer))<0){
                perror("Error during Read");
                exit(EXIT_FAILURE);
            }

            //ensure Buffer is 0 terminated:
            buffer[sizeof(buffer)-1] = 0;

            //Check if we need to leave loop:
            if(!strncmp(buffer, "END",sizeof(buffer))){
                break;
            }
            if(!strncmp(buffer,"DOWN",sizeof(buffer))){
                downFlag = true;
                break;
            }
            for(unsigned char i:buffer){
                printf("%d\t",(int)buffer[i]);
            }
            cout << endl;
        }
        if(downFlag){
            break;
        }
    }


    //close socket
    close(data_socket);
    close(connection_socket);
    unlink(PATH);
    printf("Server Shutdown Complete\n");
    exit(EXIT_SUCCESS);
}


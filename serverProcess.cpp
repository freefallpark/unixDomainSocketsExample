// Standard Includes:
#include <iostream>
#include <unistd.h>
#include <csignal>


// UDS Includes:
#include <sys/socket.h>
#include <sys/un.h>
#include <mutex>
#include "connection.h"
#include <poll.h>

using namespace std;
volatile sig_atomic_t stop = 0;
void inthand(int signum){
    stop = 1;
}
//TODO: I should probably change it so the SERVER sends the raw data while the CLIENT reads it right now the client generates the raw data... clearly i have no idea what I'm doing...

int main() {
    //Setup ctrl + c Stop in while loop...
    signal(SIGINT, inthand);

    //Variable Instance Creation:
    sockaddr_un name{}; // Stores PATH Name and connection Family
    int connection_socket,data_socket; // These are the file descriptors for the connection socket and the client socket
    char buffer[BUFFSIZE]; // created in 'connection.h'
    int downFlag = 0; // This will help us shut down the server. its trigger by the client sending "DOWN" across the Socket.
            // 0 indicates client is NOT connected.
            // 1 indicates client IS connected
            // 2 indicates client is telling server to shutdown
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
    pollfd poll4Client{};
    poll4Client.fd = connection_socket;
    poll4Client.events = POLL_IN;
    while(!stop){
        //Wait for incoming connections:
        downFlag = 0; // no clients connected.
        //Polling allows us to keep looping through (and checking for !stop)
        if(poll(&poll4Client,(nfds_t)1,(int)500)>0){
            if((data_socket = accept(connection_socket, nullptr, nullptr))<0){
                perror("Error during acceptance");
                exit(EXIT_FAILURE);
            }
            cout << "Client Found and Connection Accepted"<<endl;
            //Once we've found a connection: In the future. might be a good idea to handle this with threading.. i *think* we'll need a new thread for each client
            downFlag =1; //Client is connected!
            srand(time(nullptr));
            while(!stop){
                //Generate Fake Data to Send across the Socket
                char bufferSent[BUFFSIZE]; int bufCheck;
                buffer[0] = 0; // first byte is Client/Server communication
                for(int i = 1; i<sizeof(buffer)-1;i++){
                    int num = rand()%10+1;
                    buffer[i] = (char)num;
                }//
                memcpy(bufferSent, buffer, sizeof(bufferSent));
                //Write Data Across Socket:
                if(write(data_socket,buffer, sizeof(buffer))<0){
                    perror("Error during Write");
                }
                //Read Response from Client:
                bzero(buffer,sizeof(buffer));
                if(read(data_socket,buffer,sizeof(buffer))<0){
                    perror("Error during read");
                }
                //Check for shutdown or Close client command:
                if(buffer[0]==2){
                    downFlag = 2; //client is telling us to turn off!
                    break;
                }
                else if(buffer[0] ==1){
                    cout << "Listening for Clients again..." <<endl;
                    break;
                }
                else{
                    //CheckData:
                    // Confirm what we sent is what they got...
                    for(int i = 0; i<sizeof(buffer);i++){
                        bufCheck = buffer[i]-bufferSent[i];
                        if(bufCheck!=0){
                            printf("Difference: %d\tIndex:%d\n",bufCheck,i);
                            cout <<"\nWARNING, data sent to Client was different than data recieved!"<<endl;
                            stop = 1;
                        }
                    }
                }

            }
        }

        if(downFlag == 2){
            //if downflag is raised. the client is telling the server to shutdown...
            //becaues the client is aware already, we shouldn't write to the client.
            //i.e. do nothing!
            break;
        }
    }

    if(downFlag == 1 && stop){
        //if the down flag isn't raised the server needs to tell the client to shutdown!
        bzero(buffer,sizeof(buffer));
        buffer[0] = 2; //inform client to turn off.
        if(write(data_socket,buffer, sizeof(buffer))<0){
            perror("Error during Write");
        }

    }

    //close socket
    close(data_socket);
    close(connection_socket);
    unlink(PATH);
    printf("\nServer Shutdown Complete\n");
    exit(EXIT_SUCCESS);
}


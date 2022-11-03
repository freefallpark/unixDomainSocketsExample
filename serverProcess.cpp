// Standard Includes:
#include <iostream>
#include <unistd.h>
#include <csignal>


// UDS Includes:
#include <sys/socket.h>
#include <sys/un.h>
#include <mutex>
#include "connection.h"
#include "converters.h"
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
    unsigned char bufOut[BUFFSIZE]; // Buffer for outgoing data
    unsigned char  bufIn[BUFFSIZE]; // Buffer for incoming data. I decided to separate these because it makes it easy to
                                    // debug because I can zero the bufIn before reading watch the data as its filled.
                                    // Zeroring the buffer was causing some issues because it would alter the settings
                                    // (eg indicate to the client that the surver turned off when it didn't.
    int dsm, gpu, hand; // Variables used to bring in settings. from the client.
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
    //Create necessary var for polling the connection_socket (accept is blocking so i hid it behind an if statement poll() call)
    pollfd poll4Client{};
    poll4Client.fd = connection_socket;
    poll4Client.events = POLL_IN;
    while(!stop){
        //Wait for incoming connections:
        //Polling allows us to keep looping through (and checking for !stop)
        if(poll(&poll4Client,(nfds_t)1,(int)500)>0){
            if((data_socket = accept(connection_socket, nullptr, nullptr))<0){
                perror("Error during acceptance");
                exit(EXIT_FAILURE);
            }
            cout << "Client Found and Connection Accepted"<<endl;
            cout << "Reading in Settings from Client"<<endl;
            bzero(bufIn,sizeof(bufIn));
            if(read(data_socket, bufIn, BUFFSIZE) < 0){
                perror("Error during read");
            }
            // Read in settings for FPC:
            dsm  = bufIn[2];
            gpu  = bufIn[3];
            hand = bufIn[4];
            cout << "Settings Read from Client..."<< endl;
            cout << "Connecting to FPC and sending data across the wire!"<<endl;
            //Initialize FPC with settings given to by client:
            //TODO: FPC FUNCTION Setup goes here
            srand(time(nullptr)); // TEMPORARY
            //Send FPC Data across the wire in this loop!
            while(!stop){
                // Get New Pose data:
                int rNum = 1; //rand() % 10;
                //TODO: FPC if (data ready){update Pose Array... and Send across the wire}
                // else {do nothing until data is ready...}
                float x = (float)-69.4200*(float)rNum; float y = (float)69.4200 * (float)rNum; float z = (float)69.4200 * (float)rNum;
                float r = (float)4.2069*(float)rNum; float p = (float)4.2069*(float)rNum; float yaw = (float)4.2069*(float)rNum;
                float pose[6] = {x, y, z, r, p, yaw};
                // Covert to Int but preserve decimals:
                for(int i = 0; i<3;i++){
                    pose[i] = pose[i] * XYZ_CONVERT;
                    pose[i + 3] = pose[i + 3] * RPY_CONVERT;
                }
                // Shift right to create 3Byte Char Array
                for (int i = 1; i<7; i++ ) {// ASSUMES 24BIT signals!
                    int j = 3*i+2;
                    rightShift((int) pose[i - 1], &bufOut[j], twentyFour);
                }
                //Write Data Across Socket:
                if(write(data_socket, bufOut, BUFFSIZE) < 0){
                    perror("Error during Write");
                }
                //Read Response from Client:
                bzero(bufIn, sizeof(bufIn));
                if(read(data_socket, bufIn, BUFFSIZE) < 0){
                    perror("Error during read");
                }
                //Check to see if client closed down!
                if(!bufIn[0]){ // Client Turned off disconnect from client!
                    cout << "Client powered off. Stopping Data Transfer..."<<endl;
                    break;
                }
                //Check to see if client had any errors!
                if(!bufIn[1]){
                    switch(bufOut[1]){
                        case(1):
                            printf("Error Code 1: Client experienced *this* error");
                            //TODO: need to think about potential errors client would need to inform server about
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
    //IF Server is shutting down Inform the Client (if its on!)...
    if(bufIn[0]){ //IF client is on we need to turn it off.
        printf("\nServer Powering Off. Informing Client...\n");
        bzero(bufOut, sizeof(bufOut));
        bufOut[0] = 0; //inform client to turn off.
        if(write(data_socket, bufOut, BUFFSIZE) < 0){
                perror("Error during Write");
            }
    }

    //close sockets
    close(data_socket);
    close(connection_socket);
    //Unlink Path so we don't run into any binding issues in future.
    unlink(PATH);
    printf("\nServer Shutdown Complete\n");
    exit(EXIT_SUCCESS);
}


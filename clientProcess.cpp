#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <csignal>

#include "connection.h"

using namespace std;
volatile sig_atomic_t stop;
void inthand(int signum){
    stop = 1;
}
//
int main(int argc, char *argv[]) {
    // if Argument passed in is "1" we need to shutdown...git
    //TODO: need to atually figure out how to do this... this is a cheap go around... I'll just check to see if argument is '49' (if this is true, shut down the server...
    char argument[256];
    memset(argument,0,sizeof(argument));
    for (int i = 1; i < argc; i++){
        strcat(argument,argv[i]);
    }
    //Setup ctrl + c Stop in while loop...
    signal(SIGINT, inthand);
    sockaddr_un  addr{};
    size_t ret;
    int data_socket;
    char buffer[BUFFSIZE];
    bzero(buffer, BUFFSIZE);
    //Create local socket:
    if((data_socket = socket(AF_UNIX,SOCK_SEQPACKET,0))<0){
        perror("Error during Data_socket Creation");
        exit(EXIT_FAILURE);
    }

    //memset 0's for portability:
    memset(&addr,0,sizeof(addr));

    //Connect Socket to address:
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path,PATH, sizeof(addr.sun_path)-1);
    if(connect(data_socket,(const struct sockaddr*) &addr, sizeof(addr))<0){
        fprintf(stderr,"The Server is down.\n");
        exit(EXIT_FAILURE);
    }
    srand(time(nullptr));
    // Send Arguments:
    if(argument[0] == 49){
        strcpy(buffer,"DOWN");
        cout << "Sending Server Shutdown command"<<endl;
        if(write(data_socket,buffer, sizeof(buffer)+1)<0){
            perror("Error during Write");
        }
        stop = 1;
    }
    while(!stop){
        //Generate Fake Data to Send across the Socket
        for(int i = 0; i<sizeof(buffer)-1;i++){
            int num = rand()%10+1;
            buffer[i] = (char)num;
        }
        //Write Data Across
        if(write(data_socket,buffer, sizeof(buffer)+1)<0){
            perror("Error during Write");
        }

        //Namaste, give that a break
        usleep(1000);
    }

    //Request Send end command to server:
    bzero(buffer,BUFFSIZE);

    strcpy(buffer,"END");
    if(write(data_socket,buffer, strlen(buffer)+1)<0){
        perror("Error during Write");
        exit(EXIT_FAILURE);
    }

    close(data_socket);
    exit(EXIT_SUCCESS);
}

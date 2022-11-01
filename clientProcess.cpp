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

#include "connection.h"

using namespace std;
volatile sig_atomic_t stop;
void inthand(int signum){
    stop = 1;
}
int main() {
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
    while(!stop){
        for(int i = 0; i<sizeof(buffer)-1;i++){
            int num = rand()%10+1;
            buffer[i] = (char)num;
        }
        if(write(data_socket,buffer, sizeof(buffer)+1)<0){
            perror("Error during Write");
        }
        usleep(1000);
    }

    //Request Send end command to server:
    bzero(buffer,BUFFSIZE);

    strcpy(buffer,"END");
    if(write(data_socket,buffer, strlen(buffer)+1)<0){
        perror("Error during Write");
        exit(EXIT_FAILURE);
    }

    //Receieve Result:
    if(read(data_socket,buffer,sizeof(buffer))<0){
        perror("Error during read");
        exit(EXIT_FAILURE);
    }

    buffer[sizeof(buffer)-1] = 0;

    printf("Result = %s\n",buffer);

    close(data_socket);
    exit(EXIT_SUCCESS);
}

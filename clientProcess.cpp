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
    //Setup ctrl + c Stop in while loop...
    signal(SIGINT, inthand);
    // if Argument passed in is "1" we need to shutdown...git
    //TODO: need to atually figure out how to do this... this is a cheap go around... I'll just check to see if argument is '49' (if this is true, shut down the server...
    char argument[256];
    memset(argument,0,sizeof(argument));
    for (int i = 1; i < argc; i++){
        strcat(argument,argv[i]);
    }
    sockaddr_un  addr{};
    size_t ret;
    int data_socket,downFlag;
    char buffer[BUFFSIZE];
    char bufferSent[BUFFSIZE];
    int bufCheck;
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
    // Shut Down Server Command!
    if(argument[0] == 49){
        buffer[0] = 2;
        cout << "Sending Server Shutdown command"<<endl;
        if(write(data_socket,buffer, sizeof(buffer)+1)<0){
            perror("Error during Write");
        }
        stop = 1;
        usleep(1000);
        close(data_socket);
        exit(EXIT_SUCCESS);
    }
    while(!stop){
        //Read Data from Server:
        bzero(buffer,sizeof(buffer));
        if(read(data_socket,buffer,sizeof(buffer))<0){
        perror("Error during read");
    }
        if(buffer[0] == 2){//shutdown dawg!
            downFlag = 2;
            break;
        }
        //Check Data (for now that's just print):
        for(char i:buffer){
            printf("%d\t",(int)buffer[i]);
        }
        cout << endl;

        //Write data back to server so it can validate it
        if(write(data_socket,buffer, sizeof(buffer))<0){
            perror("Error during Write");
        }


        //Namaste, give that a break
        usleep(1000);
    }
    cout <<'\n';
    if(downFlag !=2){
        //Inform Server, client is shutting down (but the server should stay up)
        bzero(buffer,BUFFSIZE);
        buffer[0] = 1;
        if(write(data_socket,buffer, strlen(buffer)+1)<0){
            perror("Error during Write");
            exit(EXIT_FAILURE);
        }
        usleep(1000);
    }


    close(data_socket);
    exit(EXIT_SUCCESS);
}

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <csignal>
#include <iomanip>

#include "connection.h"
#include "converters.h"

using namespace std;
volatile sig_atomic_t stop;
void inthand(int signum){
    stop = 1;
}
//
int main() {
    //Setup ctrl + c Stop in while loop...
    signal(SIGINT, inthand);
    // if Argument passed in is "1" we need to shutdown...git
    //TODO: need to atually figure out how to do this... this is a cheap go around... I'll just check to see if argument is '49' (if this is true, shut down the server...
    sockaddr_un  addr{};
    size_t ret;
    int data_socket,downFlag, bufCheck;
    //Buffers:
    unsigned char  buffIn[BUFFSIZE];
    unsigned char buffOut[BUFFSIZE];
    unsigned char bufferSent[BUFFSIZE];
    unsigned char tmpBuf[2]; // Used to extract each indivdual data from pose.
    bzero(buffIn, BUFFSIZE);
    bzero(buffOut,BUFFSIZE);

    //Pose Variables:
    int poseInt[6];
    float pose[6];
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
    usleep(1000);
    //Pass settings over to server:
    buffOut[0] = 1; // the Client is On!
    buffOut[1] = 0; // There are no errors!
    buffOut[2] = 1; // we wan't RGB+D!
    buffOut[3] = 0; // we wan't GPU OFF!
    buffOut[4] = 0; // Hand Detect off!
    //Write data back to server so it can validate it
    if(write(data_socket, buffOut, BUFFSIZE) < 0){
        perror("Error during Write");
    }

    while(!stop){
        //Read Data from Server:
        bzero(buffIn, sizeof(buffIn)); // Zero buffer first.. keep it clean boys
        if(read(data_socket, buffIn, BUFFSIZE) < 0){
            perror("Error during read");
        }
        //Check Server if server is turning off:
        if(!buffIn[0]){//Server is turning off. Destroy Client!
            // Update Pose data being sent to controller with some nominal value...probably 0's

            //Close down Client Process:
            printf("\nServer turned off. Turning off Client!\n");
            close(data_socket);
            exit(EXIT_SUCCESS);
        }
        //Check Server for Errors:
        if(!buffIn[1]){
            //There was an error
            switch(buffIn[1]){
                case 1:
                    printf("Error Code 1: Description!");
                    break;
                default:
                    break;
                    //TODO: Need to figure out what errors can occur and consequently how to handle them:
            }
        }
        //Convert 3Bytes Chars to ints:
        for(int i = 5; i<=20;i=i+3){ //indexing by 3 is because of the 24 bit signal
            for(int j = 0; j<3; j++){
                int k = i+j;
                tmpBuf[j] = buffIn[k];
            }
            int iPose = ((i+3)/3)-2;
            poseInt[iPose] = leftShift(tmpBuf, twentyFour);
        }
        //Convert ints to float
        for (int i=0; i<3; i++){
            pose[i] = (float)poseInt[i]/XYZ_CONVERT;
            pose[i+3] = (float)poseInt[i+3]/RPY_CONVERT;
        }
        //Display Floats:
        cout <<'\r'<<fixed<<setprecision(3)<<"x = "<<pose[0]<< "\ty = "<<pose[1]<< "\tz = "<<pose[2]<< "\tr = "<<pose[3]<< "\tp = "<<pose[4]<< "\tyaw = "<<pose[5]<<flush;
        //Write data back to server so it can validate it
        if(write(data_socket, buffOut, BUFFSIZE) < 0){
            perror("Error during Write");
        }
        //Namaste, give that a break
        usleep(1000);
    }
    cout <<'\n';
    //Inform Server, client is shutting down (but the server should stay up)
    bzero(buffOut, BUFFSIZE);
    buffOut[0] = 0;
    if(write(data_socket, buffOut, BUFFSIZE) < 0){
        perror("Error during Write");
        exit(EXIT_FAILURE);
    }
    usleep(1000);


    close(data_socket);
    exit(EXIT_SUCCESS);
}

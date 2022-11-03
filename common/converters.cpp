//
// Created by pkyle on 10/31/22.
//

#include <cstring>
#include "converters.h"
#include <cstdlib>
#include <cstdio>
#include <string>

void rightShift(int argIn, char *argOut, numBits bits){
    unsigned char bytes[bits/8];
    int maxSignedInt = ((2^bits)/2)-1;
    //Saturate incoming Values that are too big and warn the user
    if(abs(argIn) > maxSignedInt){
        if(argIn < 0){
            argIn = -1*maxSignedInt;
        }
        else{
            argIn = maxSignedInt;
        }
        printf("rightShift(): Warning surpassed boundary of %d bit signal... Saturating to %d", bits, argIn);
    }
    switch(bits){
        case twentyFour:
            bytes[0] = (argIn >> 16) & 0xFF;
            bytes[1] = (argIn >>  8) & 0xFF;
            bytes[2] = (argIn >>  0) & 0xFF;
            break;
        case thirtyTwo:
            bytes[0] = (argIn >> 24) & 0xFF;
            bytes[1] = (argIn >> 16) & 0xFF;
            bytes[2] = (argIn >>  8) & 0xFF;
            bytes[3] = (argIn >>  0) & 0xFF;
            break;
    }
    memcpy(argOut, bytes, sizeof(bytes));
}

int leftShift(const char *argIn, numBits bits){
    int argOut;
    switch(bits){                       //Assumes Positive... we'll negate if needed in the next step.
        case twentyFour:
            argOut = (argIn[0] << 16) | (argIn[1] << 8) | (argIn[2]);
            break;
        case thirtyTwo:
            argOut = (argIn[0] << 24) | (argIn[1] << 16) | (argIn[2]) << 8 | (argIn[3]);
            break;
    }
    if((int)argIn[0] > 127){            // Number will be negative!
        argOut = argOut - 16777216;
    }
    return argOut;
}

void floatToSignal(float pose[6], char buffer[25]){
    int bytesPerSig = 3;
    for(int i = 1; i < bytesPerSig; i++){
        int j = bytesPerSig*i-bytesPerSig + 5;
        rightShift((int)pose[i-1], &buffer[j], twentyFour);
    }
}





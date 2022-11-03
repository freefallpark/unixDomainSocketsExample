//
// Created by pkyle on 10/31/22.
//

#include <cstring>
#include "converters.h"
#include <cstdlib>
#include <cstdio>
#include <string>
#include <valarray>

void rightShift(int argIn, unsigned char *argOut, numBits bits){
    unsigned char bytes[bits/8];
    int maxSignedInt = ((int)pow(2,bits)/2)-1;
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
    // Probably don't need this but I wanted to see if I could do it.
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
    // Copy data to argOut.
    memcpy(argOut, bytes, sizeof(bytes));
}

int leftShift(const unsigned char *argIn, numBits bits){
    int argOut;
    //Probably ddon't need to do this but I wanted to see if I could...
    //Assumes Positive... we'll negate if needed in the next step.
    switch(bits){
        case twentyFour:
            argOut = (argIn[0] << 16) | (argIn[1] << 8) | (argIn[2]);
            break;
        case thirtyTwo:
            argOut = (argIn[0] << 24) | (argIn[1] << 16) | (argIn[2]) << 8 | (argIn[3]);
            break;
    }
    // Number will turn negative if it enters this loop (i.e. the first bit is a 1)
    if((int)argIn[0] > 127){
        argOut = argOut - 16777216;
    }
    return argOut;
}







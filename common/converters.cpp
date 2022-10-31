//
// Created by pkyle on 10/31/22.
//

#include <cstring>
#include "converters.h"
#include <cstdlib>
#include <cstdio>

/// Splits 24bit Int into x3 8 bits
void rightShift24(int argIn, unsigned char *buffer){
    unsigned char bytes[3];
    if(abs(argIn) > 8388607){
        printf("Warning surpassed boundary of 24bit signal... Saturating to max allowed value");
        if(argIn < 0){
            argIn = -8388607;
        }
        else{
            argIn = 8388607;
        }
    }
    bytes[0] = (argIn >> 16) & 0xFF;
    bytes[1] = (argIn >> 8) & 0xFF;
    bytes[2] = (argIn >>  0) & 0xFF;
    memcpy(buffer, bytes, sizeof(bytes));
}
/// turns 3 byte signal into int
int leftShift24(const unsigned char *buffer){
    int argOut;
    if((int)buffer[0]>127){
        argOut = ((buffer[0] << 16) | (buffer[1] << 8) | (buffer[2]))-16777216;

    }
    else{
        argOut = ((buffer[0] << 16) | (buffer[1] << 8) | (buffer[2]));
    }
    return argOut;
}

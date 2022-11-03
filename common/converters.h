//
// Created by pkyle on 10/31/22.
//

#ifndef BYTECONVERSIONS_CONVERTERS_H
#define BYTECONVERSIONS_CONVERTERS_H

enum numBits{twentyFour = 24,thirtyTwo = 32};

#define XYZ_CONVERT 10000;
#define RPY_CONVERT 10000;
#define SIGSIZE twentyFour


/// @brief  RightShift: Takes an int and converts it to a n bit signal of your choice between 24 and 32 converted value is a char[numbits/8].
/// @note   your input Int can't be larger than +-((2^n)/2)-1;
/// @note   Assumes Big Endianness
void rightShift(int argIn, char *argOut, numBits bits);


/// @brief  LeftShift: Takes a char[numbits/8] and outputs the signed int that char represents.
/// @note   argIn must be 3 or 4 bytes (twentyFour or thirtyTwo bits).
/// @note   Assumes Big Endianness
int leftShift(const char *argIn, numBits bits);


/// @brief floatToSignal(): Inputs pose data and packages it into a buffer.
/// @param pose - expects a float Array: {x, y, z, r, p, yaw}
/// @param bits - number of bits representing each value in pose.
/// @param buffer - buffer[25] sent to socket MUST BE 25 Long to fit all data!!!
/// @note ASSUMES Big Endianness
/// @details buffer[0]: client/Server State:    0 - Off         1 - On)
/// @details buffer[1]: Error Handling:         0 - No Errors   1-127 Available Err Codes;
/// @details buffer[2]: Data Mode:              0 - RGB Only    1 - RGBD (+Depth)
/// @details buffer[3]: CPU/GPU:                0 - use CPU     1 - use GPU
/// @details buffer[4]: Hand Detect:            0 - Off         1 On
/// @details buffer[5 - 22]: 24 bit pose data, (x, y, z, roll, pitch, yaw).
/// @details buffer[23 & 24]: 0. these are extra.
void floatToSignal(float pose[6],  char buffer[25]);

#endif //BYTECONVERSIONS_CONVERTERS_H

// File:   logger.h
// Brief:  Functions for library debug mode
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <sstream>
#include <iostream>


extern const char logInfoPrefix[];
extern const char logErrorPrefix[];


#define LOG(M, P)                          \
    do {                                   \
        if (logger_IsDebugModeEnabled())   \
        {                                  \
            std::stringstream stream;      \
            stream << P;                   \
            stream << M;                   \
            stream << std::endl;           \
            std::cout << stream.str();     \
            std::cout.flush();             \
        }                                  \
    } while (false)


#define LOG_INFO(M)     LOG(M, logInfoPrefix)
#define LOG_ERROR(M)    LOG(M, logErrorPrefix)


/* Enable or disable debug messages on stdout */
void logger_SetEnable(bool enable);

/* Check if debug mode is enabled */
bool logger_IsDebugModeEnabled();

#endif //_LOGGER_H_

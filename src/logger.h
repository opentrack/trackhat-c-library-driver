// File:   logger.h
// Brief:  Functions for library debug mode
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <cstddef>
#include <string>
#include <sstream>

#define logFunctionBadUse "Bad use of the function."
typedef void(*log_handler_t)(const char*, int, const char*, char, const char*, size_t);

#if defined _MSC_VER
#   define LOG_FUNCTION __FUNCSIG__
#else
#   define LOG_FUNCTION __PRETTY_FUNCTION__
#endif

#define LOG(L, M)                                                       \
    do {                                                                \
        if (logger_IsDebugModeEnabled())                                \
            logger_LogLine(__FILE__, __LINE__, LOG_FUNCTION,            \
                           (L), (std::stringstream() << M).str());      \
    } while (false)

#define LOG_INFO(...)  LOG('I', __VA_ARGS__)
#define LOG_ERROR(...) LOG('E', __VA_ARGS__)

/* Enable or disable debug messages on stdout */
void logger_SetEnable(bool enable);

/* Check if debug mode is enabled */
bool logger_IsDebugModeEnabled();

void logger_SetHandler(log_handler_t fn);
void logger_LogLine(const char*, int, const char*, char, const std::string&);

#endif //_LOGGER_H_

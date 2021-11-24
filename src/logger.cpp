// File:   logger.cpp
// Brief:  Functions for library debug mode
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "logger.h"

static log_handler_t log_handler;
static bool debugModeEnabled = false;

void logger_SetEnable(bool enable)
{
    debugModeEnabled = enable;
}

bool logger_IsDebugModeEnabled()
{
    return debugModeEnabled;
}

void logger_SetHandler(log_handler_t fn)
{
    log_handler = fn;
}

void logger_LogLine(const char* file, int line,
                    const char* function, char level,
                    const std::string& str)
{
    if (log_handler)
        log_handler(file, line, function, level, str.c_str(), str.size());
}

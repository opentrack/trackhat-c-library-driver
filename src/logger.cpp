// File:   logger.cpp
// Brief:  Functions for library debug mode
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "logger.h"


static bool debugModeEnabled = false;

const char logInfoPrefix[]  = "TrackHat INFO: ";
const char logErrorPrefix[] = "TrackHat ERROR: ";
const char logFunctionBadUse[] = "Bad use of the function.";

void logger_SetEnable(bool enable)
{
    debugModeEnabled = enable;
}

bool logger_IsDebugModeEnabled()
{
    return debugModeEnabled;
}

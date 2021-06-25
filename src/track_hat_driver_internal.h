// File:   track_hat_driver.h
// Brief:  TrackHat driver private header
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _TRACK_HAT_DRIVER_INTERNAL_H_
#define _TRACK_HAT_DRIVER_INTERNAL_H_


#include "track_hat_types_internal.h"


/* Dunction that runs on a separate thread for data received from the camera  */
DWORD WINAPI trackHat_receiverThreadFunction(LPVOID lpParameter);


/* Handle the new parsed message */
void trackHat_handleNewMessage(void* message, uint8_t messageId);


/* Handle the new message event */
TH_ErrorCode trackHat_waitForNewMessageEvent(HANDLE event);


#endif //_TRACK_HAT_DRIVER_INTERNAL_H_

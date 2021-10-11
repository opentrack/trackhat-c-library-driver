// File:   track_hat_driver.h
// Brief:  TrackHat driver private header
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _TRACK_HAT_DRIVER_INTERNAL_H_
#define _TRACK_HAT_DRIVER_INTERNAL_H_

#include "track_hat_types_internal.h"

#include "usb_serial.h"


/* Function that runs on a separate thread for data received from the camera */
DWORD WINAPI trackHat_ReceiverThreadFunction(LPVOID lpParameter);


/* Function that runs on a separate thread for callback system */
DWORD WINAPI trackHat_CallbackThreadFunction(LPVOID lpParameter);


/* Run callback function with provided parameters */
void trackHat_CallbackFunction(trackHat_PointsCallback_t callbackFunction, TH_ErrorCode errorCode,
                               const trackHat_Points_t* const points);


/* Handle the new message event */
TH_ErrorCode trackHat_WaitForNewMessageEvent(HANDLE event, const char* eventName = nullptr);


/* Update internal Status message */
TH_ErrorCode trackHat_UpdateInternalStatus(trackHat_Device_t* device);


/* Update internal DeviceInfo Message */
TH_ErrorCode trackHat_UpdateInternalDeviceInfo(trackHat_Device_t* device);


/* Start or stop sending coordinates from the TrackHat camera */
TH_ErrorCode trackHat_EnableSendingCoordinates(trackHat_Device_t* device, bool enable, TH_FrameType frameType);


#endif //_TRACK_HAT_DRIVER_INTERNAL_H_

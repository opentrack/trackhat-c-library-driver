// File:   track_hat_driver.h
// Brief:  TrackHat driver private header
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _TRACK_HAT_DRIVER_INTERNAL_H_
#define _TRACK_HAT_DRIVER_INTERNAL_H_

#include "track_hat_types_internal.h"

#include "usb_serial.h"


/* Dunction that runs on a separate thread for data received from the camera  */
DWORD WINAPI trackHat_receiverThreadFunction(LPVOID lpParameter);


/* Handle the new message event */
TH_ErrorCode trackHat_waitForNewMessageEvent(HANDLE event);


/* Update internal Status message */
TH_ErrorCode trackHat_updateInternalStatus(trackHat_Device_t* device);


/* Update internal DeviceInfo Message */
TH_ErrorCode trackHat_updateInternalDeviceInfo(trackHat_Device_t* device);


/* Start or stop sending coordinates from the TrackHat camera */
TH_ErrorCode trackHat_enableSendingCoordinates(trackHat_Device_t* device, bool enable);


#endif //_TRACK_HAT_DRIVER_INTERNAL_H_

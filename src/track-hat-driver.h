// File:   track-hat-driver.h
// Brief:  TrackHat driver library
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _TRACK_HAT_DRIVER_H_
#define _TRACK_HAT_DRIVER_H_

/* Macro EXPORT_CPP export symbol to DLL. */
#if defined(_WIN32)
  #define EXPORT_API __declspec(dllexport)
#else
  #error "Currently only Windows is supported."
#endif

/* Export symbol as in C language. */
#ifdef __cplusplus
  extern "C" {
#endif


#include "track-hat-types.h"


/**
 * Initialize structure 'track_hat_device_t'.
 */
EXPORT_API
void trackHat_Initialize(trackHat_Device_t* device);

/**
 * Deinitialize structure 'track_hat_device_t' and disconnect device (if connected).
 */
EXPORT_API
THErrorCode trackHat_Deinitialize(trackHat_Device_t* device);

/**
 * Detect TrackHat device connected to USB port.
 */
EXPORT_API
THErrorCode trackHat_DetectDevice(trackHat_Device_t* device);

/**
 * Connect with TrackHat device.
 *
 * Note: The device must first be detected using the 'trackHat_DetectDevice()'
 */
EXPORT_API
THErrorCode trackHat_Connect(trackHat_Device_t* device);

/**
 * Disconnect with TrackHat device.
 */
EXPORT_API
THErrorCode trackHat_Disconnect(trackHat_Device_t* device);

/**
 * Update information about TrackHat device.
 */
EXPORT_API
THErrorCode trackHat_UpdateInfo(trackHat_Device_t* device);

/**
 * Get uptime of the TrackHat device in seconds.
 */
EXPORT_API
THErrorCode trackHat_GetUptime(trackHat_Device_t* device, uint32_t* seconds);

/**
 * Get list of detected points by the TrackHat camera.
 *
 * Note: This function waits for the next set of points with 50 ms timeout.
 */
EXPORT_API
THErrorCode trackHat_GetDetectedPoints(trackHat_Device_t* device, trackHat_Points_t* points);

/**
 * Add callback that will be executed automatically when new data is received.
 */
EXPORT_API
THErrorCode trackHat_AddCallback(trackHat_Device_t* device, trackHat_PointsCallback_t newPoints_callback);

/**
 * Remove previously added callback.
 */
EXPORT_API
THErrorCode trackHat_RemoveCallback(trackHat_Device_t* device, trackHat_PointsCallback_t newPoints_callback);

/**
 * Remove all previously added callbacks.
 */
EXPORT_API
THErrorCode trackHat_RemoveAllCallbacks(trackHat_Device_t* device);


#ifdef __cplusplus
}	// extern "C"
#endif

#endif //_TRACK_HAT_DRIVER_H_

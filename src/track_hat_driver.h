// File:   track_hat_driver.h
// Brief:  TrackHat driver library API
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
  extern "C"
  {
#endif


#include "track_hat_types.h"


/**
 * Initialize structure 'track_hat_device_t'.
 * 
 * \param[in]  device   pointer to trackHat_Device_t.
 *
 * \return     TH_SUCCESS or error code.
 */
EXPORT_API
TH_ErrorCode trackHat_Initialize(trackHat_Device_t* device);

/**
 * Deinitialize structure 'track_hat_device_t' and disconnect device (if connected).
 * 
 * * Note: 'device' parameter should be first initialized with 'trackHat_Initialize()'.
 *
 * \param[in]  device   pointer to trackHat_Device_t.
 *
 */
EXPORT_API
void trackHat_Deinitialize(trackHat_Device_t* device);

/**
 * Detect TrackHat device connected to USB port.
 * 
 * Note: 'device' parameter should be first initialized with 'trackHat_Initialize()'.
 *
 * \param[in]  device   pointer to trackHat_Device_t.
 *
 * \return     TH_SUCCESS if detected or error code.
 */
EXPORT_API
TH_ErrorCode trackHat_DetectDevice(trackHat_Device_t* device);

/**
 * Connect with TrackHat device.
 *
 * Note: The device must first be detected using the 'trackHat_DetectDevice()'
 */
EXPORT_API
TH_ErrorCode trackHat_Connect(trackHat_Device_t* device);

/**
 * Disconnect with TrackHat device.
 */
EXPORT_API
TH_ErrorCode trackHat_Disconnect(trackHat_Device_t* device);

/**
 * Update information about TrackHat device.
 */
EXPORT_API
TH_ErrorCode trackHat_UpdateInfo(trackHat_Device_t* device);

/**
 * Get uptime of the TrackHat device in seconds.
 */
EXPORT_API
TH_ErrorCode trackHat_GetUptime(trackHat_Device_t* device, uint32_t* seconds);

/**
 * Get list of detected points by the TrackHat camera.
 *
 * Note: This function waits for the next set of points with 50 ms timeout.
 */
EXPORT_API
TH_ErrorCode trackHat_GetDetectedPoints(trackHat_Device_t* device, trackHat_Points_t* points);

/**
 * Add callback that will be executed automatically when new data is received.
 */
EXPORT_API
TH_ErrorCode trackHat_AddCallback(trackHat_Device_t* device, trackHat_PointsCallback_t newPoints_callback);

/**
 * Remove previously added callback.
 */
EXPORT_API
TH_ErrorCode trackHat_RemoveCallback(trackHat_Device_t* device, trackHat_PointsCallback_t newPoints_callback);

/**
 * Remove all previously added callbacks.
 */
EXPORT_API
TH_ErrorCode trackHat_RemoveAllCallbacks(trackHat_Device_t* device);


#ifdef __cplusplus
  } // extern "C"
#endif

#endif //_TRACK_HAT_DRIVER_H_

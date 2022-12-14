// File:   track_hat_driver.h
// Brief:  TrackHat driver library API
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _TRACK_HAT_DRIVER_H_
#define _TRACK_HAT_DRIVER_H_

/* Macro EXPORT_CPP export symbol to DLL. */
#if defined(_WIN32)
  #define EXPORT_API
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
 * Enables the debugging mode of the library, i.e. the console output will show additional
 * messages about the operation of the TrackHat library (disabled by default).
 */
EXPORT_API
void trackHat_EnableDebugMode(void);

/**
 * Disable the debugging mode of the library. 
 */
EXPORT_API
void trackHat_DisableDebugMode(void);

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
TH_ErrorCode trackHat_Connect(trackHat_Device_t* device, TH_FrameType frameType);

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
 * Get list of detected points by the TrackHat camera.
 *
 * Note: This function waits for the next set of points with 50 ms timeout.
 */
EXPORT_API
TH_ErrorCode trackHat_GetDetectedPointsExtended(trackHat_Device_t* device, trackHat_ExtendedPoints_t* points);


/**
 * Set callback that will be executed automatically when new set of points are received.
 */
EXPORT_API
TH_ErrorCode trackHat_SetCallback(trackHat_Device_t* device, trackHat_PointsCallback_t newPoints_callback);

/**
 * Set extended points callback that will be executed automatically when new set of points are received.
 */
EXPORT_API
TH_ErrorCode trackHat_SetExtendedPointsCallback(trackHat_Device_t* device, trackHat_ExtendedPointsCallback_t newExtendedPointsCallback);


/**
 * Remove previously added callback.
 */
EXPORT_API
TH_ErrorCode trackHat_RemoveCallback(trackHat_Device_t* device);

/**
 * Set a single register value
 */
EXPORT_API
TH_ErrorCode trackHat_SetRegisterValue(trackHat_Device_t* device, trackHat_SetRegister_t* newRegisterValue);

/**
 * Set all LEDs status
 */
EXPORT_API
TH_ErrorCode trackHat_SetLeds(trackHat_Device_t* device, trackHat_SetLeds_t* newLedState);

/**
 * Set a group of register values
 */
EXPORT_API
TH_ErrorCode trackHat_SetRegisterGroupValue(trackHat_Device_t* device, trackHat_SetRegisterGroup_t* newRegisterGroupValue);

/**
 * Enable bootloader for firmware upgrade
 */
EXPORT_API
TH_ErrorCode trackHat_EnableBootloader(trackHat_Device_t* device, TH_BootloaderMode bootloaderMode);

EXPORT_API
void trackHat_SetDebugHandler(TH_LogHandler_t fn);

void setRegisterGroupValue(uint8_t registerBank, uint8_t registerAdress, uint8_t registerValue, trackHat_SetRegisterGroup_t& setRegisterGroup);

#ifdef __cplusplus
  } // extern "C"
#endif

#endif //_TRACK_HAT_DRIVER_H_

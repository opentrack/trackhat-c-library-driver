// File:   track_hat_types.h
// Brief:  TrackHat driver types
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _TRACK_HAT_TYPES_H_
#define _TRACK_HAT_TYPES_H_

/* Export symbol as in C language. */
#ifdef __cplusplus
  extern "C"
  {
#endif

#include <stdint.h>


/* TrackHat library error codes. */
enum TH_ErrorCode
{
  TH_SUCCESS = 0,
  TH_ERROR_DEVICE_NOT_DETECTED = -1,
  TH_ERROR_DEVICE_DISCONECTED = -2,
  TH_ERROR_DEVICE_COMUNICATION_FAILD = -3,
  TH_ERROR_DEVICE_COMUNICATION_TIMEOUT = -4,
  TH_ERROR_CAMERA_INTERNAL_BROKEN = -5,
  TH_ERROR_CAMERA_INITIALIZING_FAILD = -6,
  TH_ERROR_CAMERA_SELFT_TEST_FAILD = -7,
  TH_ERROR_WRONG_PARAMETER = -8,
};

/* Maximum number of points detected by TrackHat camera. */
#define TRACK_HAT_POINTS_NUMBER 16

/* TrackHat device instance.  */
typedef struct
{
  void*    m_internal;
  uint32_t m_serialNumber;
  uint8_t  m_softwareVersionMajor;
  uint8_t  m_softwareVersionMinor;
  uint8_t  m_hardwareVersion;
  uint8_t  m_isIdleMode;
} trackHat_Device_t;

/* TrackHat single point. */
typedef struct
{
  uint16_t m_x;
  uint16_t m_y;
  uint8_t  m_brightness;
} trackHat_Point_t;

/* TrackHat set of points. */
typedef struct
{
  trackHat_Point_t m_point[TRACK_HAT_POINTS_NUMBER];
} trackHat_Points_t;

/**
 * Declaration type of callback to call after receiving new points from the TrackHat device.
 */
typedef void (*trackHat_PointsCallback_t)(TH_ErrorCode error, const trackHat_Points_t* const points);


#ifdef __cplusplus
  } // extern "C"
#endif

#endif //_TRACK_HAT_TYPES_H_

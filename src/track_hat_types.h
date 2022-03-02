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
    TH_ERROR_DEVICE_ALREADY_OPEN = -3,
    TH_ERROR_DEVICE_NOT_OPEN = -4,
    TH_ERROR_DEVICE_COMUNICATION_FAILD = -5,
    TH_ERROR_DEVICE_COMUNICATION_TIMEOUT = -6,
    TH_ERROR_CAMERA_INTERNAL_BROKEN = -7,
    TH_ERROR_CAMERA_INITIALIZING_FAILD = -8,
    TH_ERROR_CAMERA_SELFT_TEST_FAILD = -9,
    TH_ERROR_WRONG_PARAMETER = -10,
    TH_MEMORY_ALLOCATION_FIELD = -11,
    TH_FAILED_TO_SET_REGISTER = -12
};

enum TH_FrameType
{
    TH_FRAME_BASIC = 0,
    TH_FRAME_EXTENDED = 1,
};

enum TH_BootloaderMode
{
    TH_BOOTLOADER_OFF = 0,
    TH_BOOTLOADER_ON = 1,
};

/* Maximum number of points detected by TrackHat camera. */
#define TRACK_HAT_NUMBER_OF_POINTS 16

/* TrackHat device instance. */
typedef struct
{
    void*    m_pInternal; /* Private context data of the device */
    uint32_t m_serialNumber;
    uint8_t  m_softwareVersionMajor;
    uint8_t  m_softwareVersionMinor;
    uint8_t  m_hardwareVersion;
    uint8_t  m_isIdleMode;
    TH_FrameType m_frameType;
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
    trackHat_Point_t m_point[TRACK_HAT_NUMBER_OF_POINTS];
} trackHat_Points_t;


/* TrackHat single extended point. */
#pragma pack()
typedef struct trackHat_ExtendedPointRaw_t
{
    uint8_t m_areaLow;
    uint8_t m_areaHigh;
    uint8_t m_coordinateXLow;
    uint8_t m_coordinateXHigh;
    uint8_t m_coordinateYLow;
    uint8_t m_coordinateYHigh;
    uint8_t m_averageBrightness;
    uint8_t m_maximumBrightness;
    uint8_t m_range:4;
    uint8_t m_radius:4;
    uint8_t m_boundryLeft;
    uint8_t m_boundryRigth;
    uint8_t m_boundryUp;
    uint8_t m_boundryDown;
    uint8_t m_aspectRatio;
    uint8_t m_vx;
    uint8_t m_vy;
} trackHat_ExtendedPointRaw_t;


/* TrackHat single extended point. */
typedef struct trackHat_ExtendedPoint_t
{
    uint16_t m_area;
    uint16_t m_coordinateX;
    uint16_t m_coordinateY;
    uint8_t m_averageBrightness;
    uint8_t m_maximumBrightness;
    uint8_t m_range:4;
    uint8_t m_radius:4;
    uint8_t m_boundryLeft;
    uint8_t m_boundryRigth;
    uint8_t m_boundryUp;
    uint8_t m_boundryDown;
    uint8_t m_aspectRatio;
    uint8_t m_vx;
    uint8_t m_vy;
} trackHat_ExtendedPoint_t;

/* TrackHat set of points. */
#pragma pack()
typedef struct
{
    trackHat_ExtendedPoint_t m_point[TRACK_HAT_NUMBER_OF_POINTS];
} trackHat_ExtendedPoints_t;

/**
 * Declaration type of callback to call after receiving new points from the TrackHat device.
 */
typedef void (*trackHat_PointsCallback_t)(TH_ErrorCode error, const trackHat_Points_t* const points);
typedef void (*trackHat_ExtendedPointsCallback_t)(TH_ErrorCode error, const trackHat_ExtendedPoints_t* const points);

typedef struct trackHat_SetRegister_t
{
    uint8_t m_registerBank;
    uint8_t m_registerAddress;
    uint8_t m_registerValue;
} trackHat_SetRegister_t;

typedef void(*TH_LogHandler_t)(const char* file, int line, const char* function, char level, const char* msg, size_t len);

#ifdef __cplusplus
  } // extern "C"
#endif

#endif //_TRACK_HAT_TYPES_H_

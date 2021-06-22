// File:   track_hat_messages.h
// Brief:  TrackHat frame messages
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _TRACK_HAT_MESSAGES_H_
#define _TRACK_HAT_MESSAGES_H_

#include "track_hat_types.h"

#include <windows.h>

enum MessageID : uint8_t
{
    ID_ACK             = 0x00,
    ID_GET_STATUS      = 0x01,
    ID_STATUS          = 0x02,
    ID_GET_DEVICE_INFO = 0x03,
    ID_DEVICE_INFO     = 0x04,
    ID_SET_MODE        = 0x05,
    ID_COORDINATE      = 0x0b,
    ID_NACK            = 0xff
};

enum CameraStatus : uint8_t
{
    CAM_NOT_PRESENT = 0x00,
    CAM_PRESENT = 0x01,
};

enum CameraMode : uint8_t
{
    CAM_IDLE = 0x00,
    CAM_COORDINATE = 0x01,
};


/* Thread safe protection for Messages */
struct MessageProtect
{
protected:
    MessageProtect() :
        m_newMessageEvent(CreateEvent(NULL, true, 0, NULL)),
        m_mutex(CreateMutex(NULL, false, NULL))
    { }

    ~MessageProtect()
    { 
        CloseHandle(m_newMessageEvent);
        CloseHandle(m_mutex);
    }

public:
    HANDLE m_newMessageEvent;
    HANDLE m_mutex;
};

/* Thread safe protection for Messages */
struct MessageBase
{
    uint8_t m_transactionID;
};


struct MessageStatus : public MessageBase, public MessageProtect
{
    static const size_t FrameSize = 10;

    CameraStatus m_camStatus = CAM_NOT_PRESENT;
    CameraMode   m_camMode = CAM_IDLE;
    uint32_t     m_uptimeInSec = 0;
};


struct MessageDeviceInfo : public MessageBase, public MessageProtect
{
    static const size_t FrameSize = 11;

    uint8_t m_hardwareVersion;
    uint8_t m_softwareVersionMajor;
    uint8_t m_softwareVersionMinor;
    uint32_t m_serialNumber;
};

struct MessageCoordinates : public MessageBase, public MessageProtect
{
    static const size_t FrameSize = 83;

    trackHat_Points_t m_points;
};

struct MessageAck : public MessageBase
{
    static const size_t FrameSize = 4;
};

struct MessageNAck : public MessageBase
{
    static const size_t FrameSize = 4;
};



#endif //_TRACK_HAT_MESSAGES_H_

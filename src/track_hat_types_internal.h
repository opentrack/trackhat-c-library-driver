// File:   track_hat_types_internal.h
// Brief:  TrackHat driver private types
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _TRACK_HAT_TYPES_INTERNAL_H_
#define _TRACK_HAT_TYPES_INTERNAL_H_

#include "track_hat_messages.h"
#include "usb_serial.h"

/* TrackHat camera USB IDs */
#define TRACK_HAT_USB_VENDOR_ID      0x0483
#define TRACK_HAT_USB_PRODUCT_ID     0x5740

/* Maximum time for new message events in ms */
#define MESSAGE_EVENT_TIMEOUT_MS  2000

/* Size of the buffer for messages to transmit */
#define MESSAGE_TX_BUFFER_SIZE  64

/* Size of the buffer for messages to receive */
#define MESSAGE_RX_BUFFER_SIZE  256


/* Structure for the last messages received from the TrackHat camera. */
typedef struct
{
    MessageStatus      m_status;
    MessageDeviceInfo  m_deviceInfo;
    MessageCoordinates m_coordinates;
    MessageNACK        m_nack;
    uint8_t            m_lastACKTransactionId;
} trackHat_Messages_t;


/* Structure for the data receiving thread. */
typedef struct
{
    HANDLE  m_threadHandler = NULL;
    DWORD   m_threadID;
    bool    m_isRunning = false;
} trackHat_Thread_t;


/* Structure for the callback receiving thread. */
typedef struct
{
    trackHat_Thread_t m_thread;
    trackHat_PointsCallback_t m_function = nullptr;
    HANDLE m_mutex = nullptr;
} trackHat_Callback_t;


/* TrackHat device internal instance. */
typedef struct
{
    usbSerial_t         m_serial;
    trackHat_Thread_t   m_receiver;
    trackHat_Callback_t m_callback;
    trackHat_Messages_t m_messages;
    bool m_isOpen = false;
    bool m_isUnplugged = false;  /* Was connected, is disconnected */
} trackHat_Internal_t;


#endif //_TRACK_HAT_TYPES_INTERNAL_H_


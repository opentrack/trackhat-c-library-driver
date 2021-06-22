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

/* Size of the buffer for messages */
#define MESSAGE_BUFFER_SIZE  256

/* Structure for the last messages received from the TrackHat camera. */
typedef struct
{
    MessageStatus      m_status;
    MessageDeviceInfo  m_deviceInfo;
    MessageCoordinates m_coordinateso;
    uint8_t            m_lastACKTransactionId;
    uint8_t            m_lastNACKTransactionId;
} trackHat_Messages_t;


/* Structure for the data receiving thread. */
typedef struct
{
    HANDLE  m_threadHandler = NULL;
    DWORD   m_threadID;
    bool    m_isRunning = false;
} trackHat_Receiver_t;


/* TrackHat device internal instance. */
typedef struct
{
    usbSerial_t         m_serial;
    trackHat_Receiver_t m_receiver;
    trackHat_Messages_t m_messages;
} trackHat_Internal_t;


#endif //_TRACK_HAT_TYPES_INTERNAL_H_

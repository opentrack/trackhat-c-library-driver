// File:   usb_serial.cpp
// Brief:  Functions to control serial port over USB
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _USB_SERIAL_H_
#define _USB_SERIAL_H_

#include "track_hat_types.h"

#include <stdint.h>
#include <windows.h>


/* Structure for serial port suppoer */
typedef struct
{
    uint16_t m_comNumber;           // Number of COM port
    char     m_comFileName[20];     // Contain Windows friendly name
    HANDLE   m_comHandler;          // Handle to the serial port
    COMMTIMEOUTS m_timeouts;        // Initializing timeouts structure
    bool     m_isPortOpened;        // Port is open or close
} usbSerial_t;


/**
 * Get COM port number witf specyfic vendor ID and product ID.
 *
 * Note: This function returns only one COM port but is able to detect all COM ports
 * with specyfic vendor ID and product ID (for future expansion possibilities).
 *
 * \param[in]  vendorId   USB vendor ID.
 * \param[in]  productId  USB product ID.
 *
 * \return     Number of COM or '0' if not found
 */
uint16_t usbGetComPort(uint16_t vendorId, uint16_t productId);

/**
 * Open serial port based on 'm_serialCom' and start reading thread.
 *
 * \param[in]  serial   Structure of 'usbSerial_t.
 *
 * \return     TH_SUCCESS or error code.
 */
TH_ErrorCode usbSerialOpen(usbSerial_t& serial);

/**
 * Close serial port based on 'm_serialCom' and stop reading thread.
 *
 * \param[in]  serial   Structure of 'usbSerial_t.
 *
 * \return     TH_SUCCESS or error code.
 */
TH_ErrorCode usbSerialClose(usbSerial_t& serial);


#endif //_USB_SERIAL_H_

// File:   usb_serial.cpp
// Brief:  Functions to control serial port over USB
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _USB_SERIAL_H_
#define _USB_SERIAL_H_


#include <stdint.h>


/* Structure for serial port suppoer */
typedef struct
{
    uint16_t m_comNumber;           // Number of COM port
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

// File:   usb_serial.h
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
    uint16_t m_comNumber = 0;       // Number of COM port
    char     m_comFileName[20];     // Contain Windows friendly name
    HANDLE   m_comHandler = 0;          // Handle to the serial port
    COMMTIMEOUTS m_timeouts;        // Initializing timeouts structure
    bool     m_isPortOpen = false;  // Port is open or close
} usbSerial_t;

namespace UsbSerial {

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
    uint16_t getComPort(uint16_t vendorId, uint16_t productId);

    /**
     * Open serial port based on 'm_serialCom' and start reading thread.
     *
     * \param[in]  serial   Structure of 'usbSerial_t.
     *
     * \return     TH_SUCCESS or error code.
     */
    TH_ErrorCode open(usbSerial_t& serial);

    /**
     * Close serial port based on 'm_serialCom' and stop reading thread.
     *
     * \param[in]  serial   Structure of 'usbSerial_t.
     *
     * \return     TH_SUCCESS or error code.
     */
    TH_ErrorCode close(usbSerial_t& serial);

    /**
     * Send data via serial port.
     *
     * Note: Serial port must be opened befor call this function.
     *
     * \param[in]  serial   Structure of 'usbSerial_t'.
     * \param[in]  buffer   Data to send.
     * \param[in]  size     Amount of data to transfer.
     *
     * \return     TH_SUCCESS or error code.
     */
    TH_ErrorCode write(usbSerial_t& serial, const uint8_t* const buffer, size_t size);

    /**
     * Receive data from serial port.
     *
     * Note: Serial port must be opened befor call this function.
     * Note: If there is no data the function returns after 50 ms with 'readSizeOutput' as 0.
     *
     * \param[in]  serial          Structure of 'usbSerial_t'.
     * \param[in]  buffer          Buffer for imput data.
     * \param[in]  maxSize         Maximum buffer size.
     * \param[out] readSizeOutput  Amount of receive data.
     *
     * \return     TH_SUCCESS or error code.
     */
    TH_ErrorCode read(usbSerial_t& serial, uint8_t* buffer, const size_t maxSize, size_t& readSizeOutput);


} // namespace UsbSerial

#endif //_USB_SERIAL_H_

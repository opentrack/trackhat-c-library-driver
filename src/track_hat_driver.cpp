// File:   track_hat_driver.cpp
// Brief:  TrackHat driver library
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "track_hat_driver.h"
#include "track_hat_driver_internal.h"

#include "usb_serial.h"

#include <cstdio>
#include <string>


TH_ErrorCode trackHat_Initialize(trackHat_Device_t* device)
{
    memset(device, 0, sizeof(trackHat_Device_t));

    device->m_pInternal = malloc(sizeof(trackHat_Internal_t));
    if (device->m_pInternal == nullptr)
        return TH_MEMORY_ALLOCATION_FIELD;

    memset(device->m_pInternal, 0, sizeof(trackHat_Internal_t));

    return TH_SUCCESS;
}

void trackHat_Deinitialize(trackHat_Device_t* device)
{
    if (device->m_pInternal!=nullptr)
        free(device->m_pInternal);
    memset(device, 0, sizeof(trackHat_Device_t));
}

TH_ErrorCode trackHat_DetectDevice(trackHat_Device_t* device)
{
    if ((device == nullptr) || (device->m_pInternal == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    usbSerial_t& serial = internal.m_serial;
    serial.m_comNumber = usbGetComPort(TRACK_HAT_USB_VENDOR_ID, TRACK_HAT_USB_PRODUCT_ID);

    if (serial.m_comNumber == 0)
        return TH_ERROR_DEVICE_NOT_DETECTED;

    return TH_SUCCESS;
}


TH_ErrorCode trackHat_Connect(trackHat_Device_t* device)
{
    if ((device == nullptr) || (device->m_pInternal == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    usbSerial_t& serial = internal.m_serial;

    if (serial.m_comNumber == 0)
        return TH_ERROR_DEVICE_NOT_DETECTED;

    return usbSerialOpen(serial);
}


TH_ErrorCode trackHat_Disconnect(trackHat_Device_t* device)
{
    if ((device == nullptr) || (device->m_pInternal == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    usbSerial_t& serial = internal.m_serial;

    return usbSerialClose(serial);
}



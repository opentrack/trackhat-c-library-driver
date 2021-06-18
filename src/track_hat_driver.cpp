// File:   track_hat_driver.cpp
// Brief:  TrackHat driver library
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "track_hat_driver.h"
#include "track_hat_driver_internal.h"

#include "logger.h"
#include "usb_serial.h"

#include <cstdio>
#include <string>
#include <vector>


void trackHat_EnableDebugMode()
{
    logger_SetEnable(true);
}

void trackHat_DisableDebugMode()
{
    logger_SetEnable(false);
}

TH_ErrorCode trackHat_Initialize(trackHat_Device_t* device)
{
    LOG_INFO("Structure initialization.");

    memset(device, 0, sizeof(trackHat_Device_t));

    device->m_pInternal = malloc(sizeof(trackHat_Internal_t));
    if (device->m_pInternal == nullptr)
    {
        LOG_ERROR("Lack of memory.");
        return TH_MEMORY_ALLOCATION_FIELD;
    }

    memset(device->m_pInternal, 0, sizeof(trackHat_Internal_t));

    return TH_SUCCESS;
}

void trackHat_Deinitialize(trackHat_Device_t* device)
{
    LOG_INFO("Structure deinitialization.");

    if (device->m_pInternal!=nullptr)
        free(device->m_pInternal);
    memset(device, 0, sizeof(trackHat_Device_t));
}

TH_ErrorCode trackHat_DetectDevice(trackHat_Device_t* device)
{
    LOG_INFO("Detecting TrackHat camera.");

    if ((device == nullptr) || (device->m_pInternal == nullptr))
    {
        LOG_ERROR("Bad use of the function.");
        return TH_ERROR_WRONG_PARAMETER;
    }

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    usbSerial_t& serial = internal.m_serial;
    serial.m_comNumber = usbGetComPort(TRACK_HAT_USB_VENDOR_ID, TRACK_HAT_USB_PRODUCT_ID);

    if (serial.m_comNumber == 0)
    {
        LOG_INFO("Camera NOT detected.");
        return TH_ERROR_DEVICE_NOT_DETECTED;
    }

    LOG_INFO("Camera detected on the COM" << serial.m_comNumber << " port.");
    return TH_SUCCESS;
}


TH_ErrorCode trackHat_Connect(trackHat_Device_t* device)
{
    LOG_INFO("Connecting.");

    if ((device == nullptr) || (device->m_pInternal == nullptr))
    {
        LOG_ERROR(logFunctionBadUse);
        return TH_ERROR_WRONG_PARAMETER;
    }

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    usbSerial_t& serial = internal.m_serial;

    if (serial.m_comNumber == 0)
    {
        LOG_ERROR(logFunctionBadUse);
        return TH_ERROR_DEVICE_NOT_DETECTED;
    }

    return usbSerialOpen(serial);
}


TH_ErrorCode trackHat_Disconnect(trackHat_Device_t* device)
{
    LOG_INFO("Disconnection.");

    if ((device == nullptr) || (device->m_pInternal == nullptr))
    {
        LOG_ERROR(logFunctionBadUse);
        return TH_ERROR_WRONG_PARAMETER;
    }

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    usbSerial_t& serial = internal.m_serial;

    return usbSerialClose(serial);
}


TH_ErrorCode trackHat_UpdateInfo(trackHat_Device_t* device)
{
    //TODO this function will be reimplemented

    if ((device==nullptr) || (device->m_pInternal == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    usbSerial_t& serial = internal.m_serial;

    char serialOutput[] = { 0x01, 0x02, 0x03, 0x04 };
    char serialInput[256];
    uint32_t readSize = 0;

    std::vector<char> inputBuffer;

    while (true)
    {
        TH_ErrorCode result = usbSerialWrite(serial, serialOutput, sizeof(serialOutput));
        if (result != TH_SUCCESS)
        {
            return result;
        }

        result = usbSerialRead(serial, serialInput, sizeof(serialInput), readSize);
        if (result != TH_SUCCESS)
        {
            return result;
        }

        if (inputBuffer.size() < 100)
        {
            if (readSize > 1)
            {
                inputBuffer.insert(inputBuffer.end(), serialInput, serialInput + readSize);
                printf("Buffer len %dn", inputBuffer.size());
            }
            else if (readSize == 1)
            {
                inputBuffer.push_back(serialInput[0]);
                printf("Buffer len %d\n", inputBuffer.size());
            }
        }

        Sleep(100);
    }

    return TH_SUCCESS;
}


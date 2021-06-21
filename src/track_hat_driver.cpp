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


//TODO to remove
HANDLE testNewMessageEvent;


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

    testNewMessageEvent = CreateEvent(0, true, 0, 0);

    return TH_SUCCESS;
}


void trackHat_Deinitialize(trackHat_Device_t* device)
{
    LOG_INFO("Structure deinitialization.");

    if (device->m_pInternal != nullptr)
    {
        CloseHandle(testNewMessageEvent);
        free(device->m_pInternal);
    }
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
    serial.m_comNumber = UsbSerial::getComPort(TRACK_HAT_USB_VENDOR_ID, TRACK_HAT_USB_PRODUCT_ID);

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

    trackHat_Internal_t* internal = reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    trackHat_Receiver_t& receiver = internal->m_receiver;
    usbSerial_t& serial = internal->m_serial;

    if (serial.m_comNumber == 0)
    {
        LOG_ERROR(logFunctionBadUse);
        return TH_ERROR_DEVICE_NOT_DETECTED;
    }

    TH_ErrorCode result = UsbSerial::open(serial);
    if (result != TH_SUCCESS)
    {
        return result;
    }

    // Start receiving thread
    receiver.m_runFlag = true;
    receiver.m_threadHandler = CreateThread(0, 0, trackHat_receiverThreadFunction, internal, 0, &receiver.m_threadID);
    if (receiver.m_threadHandler == nullptr)
    {
        LOG_ERROR("Cannot start rceiving. Error " << GetLastError() <<".");
        trackHat_Disconnect(device);
        return TH_ERROR_DEVICE_COMUNICATION_FAILD;
    }

    return TH_SUCCESS;
}


TH_ErrorCode trackHat_Disconnect(trackHat_Device_t* device)
{
    LOG_INFO("Disconnecting.");

    if ((device == nullptr) || (device->m_pInternal == nullptr))
    {
        LOG_ERROR(logFunctionBadUse);
        return TH_ERROR_WRONG_PARAMETER;
    }

    trackHat_Internal_t* internal = reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    trackHat_Receiver_t& receiver = internal->m_receiver;
    usbSerial_t& serial = internal->m_serial;

    // Stop receiving thread
    receiver.m_runFlag = false;
    if (receiver.m_threadHandler != nullptr)
    {
        WaitForSingleObject(receiver.m_threadHandler, 10000);
        CloseHandle(receiver.m_threadHandler);
        receiver.m_threadHandler = nullptr;
        receiver.m_threadID = 0;
    }

    return UsbSerial::close(serial);
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
        TH_ErrorCode result = UsbSerial::write(serial, serialOutput, sizeof(serialOutput));
        if (result != TH_SUCCESS)
        {
            return result;
        }

        result = UsbSerial::read(serial, serialInput, sizeof(serialInput), readSize);
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

DWORD WINAPI trackHat_receiverThreadFunction(LPVOID lpParameter)
{
    trackHat_Internal_t* internal = reinterpret_cast<trackHat_Internal_t*>(lpParameter);
    trackHat_Receiver_t& receiver = internal->m_receiver;
    usbSerial_t& serial = internal->m_serial;
    TH_ErrorCode result = TH_SUCCESS;

    std::vector<uint8_t> inputBuffer; // Input for data to parse
    uint8_t rawBuffer[256];           // Buffer per iteration for serial data and parsed message
    size_t readSize;
    uint8_t newMessageId = 0;
    bool newMessage = false;

    inputBuffer.reserve(256);

    LOG_INFO("Receiving started.");

    while (receiver.m_runFlag)
    {
        result = UsbSerial::read(serial, rawBuffer, sizeof(rawBuffer), readSize);

        if (!receiver.m_runFlag)
            break;

        if ((result==TH_SUCCESS) && (readSize>0))
        {
            inputBuffer.insert(inputBuffer.end(), rawBuffer, rawBuffer + readSize);
        }
    }

    LOG_INFO("Receiving finished.");
    return 0;
}

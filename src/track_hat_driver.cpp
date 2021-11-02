// File:   track_hat_driver.cpp
// Brief:  TrackHat driver library
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "track_hat_driver.h"
#include "track_hat_driver_internal.h"

#include "logger.h"
#include "track_hat_parser.h"
#include "usb_serial.h"

#include <cstdio>
#include <string>
#include <time.h>
#include <vector>
#include <chrono>

const size_t CAMERA_ERROR_CHECK_INTERVAL = 2;

void trackHat_EnableDebugMode(void)
{
    logger_SetEnable(true);
}


void trackHat_DisableDebugMode(void)
{
    logger_SetEnable(false);
}


TH_ErrorCode trackHat_Initialize(trackHat_Device_t* device)
{
    LOG_INFO("Structure initialization.");

    memset(device, 0, sizeof(trackHat_Device_t));

    device->m_pInternal = new trackHat_Internal_t();
    if (device->m_pInternal == nullptr)
    {
        LOG_ERROR("Lack of memory.");
        return TH_MEMORY_ALLOCATION_FIELD;
    }

    return TH_SUCCESS;
}


void trackHat_Deinitialize(trackHat_Device_t* device)
{
    LOG_INFO("Structure deinitialization.");

    if (device->m_pInternal != nullptr)
    {
        trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
        if (internal.m_receiver.m_threadHandler != nullptr)
        {
            trackHat_Disconnect(device);
        }
        delete device->m_pInternal;
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

TH_ErrorCode trackHat_waitForResponse(trackHat_Internal_t* internal, uint8_t transactionID)
{
    trackHat_Messages_t* messages = &internal->m_messages;
    std::chrono::high_resolution_clock clk;
    auto start = clk.now();
    using namespace std::chrono_literals;
    constexpr auto timeout = 100ms;
    do {
        if (messages->m_nack.m_transactionID == transactionID)
            return TH_FAILED_TO_SET_REGISTER;
        else if (messages->m_lastACKTransactionId >= transactionID)
            return TH_SUCCESS;
        Sleep(1);
    } while (clk.now() - start <= timeout);

    return TH_ERROR_DEVICE_COMUNICATION_TIMEOUT;
}


TH_ErrorCode trackHat_Connect(trackHat_Device_t* device, TH_FrameType frameType)
{
    LOG_INFO("Connecting.");

    if ((device == nullptr) || (device->m_pInternal == nullptr))
    {
        LOG_ERROR(logFunctionBadUse);
        return TH_ERROR_WRONG_PARAMETER;
    }

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    trackHat_Thread_t& receiverThread = internal.m_receiver;
    trackHat_Thread_t& callbackThread = internal.m_callback.m_thread;
    usbSerial_t& serial = internal.m_serial;

    if (internal.m_isOpen == true)
    {
        return TH_ERROR_DEVICE_ALREADY_OPEN;
    }

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
    internal.m_isUnplugged = false;

    // Start receiving thread
    receiverThread.m_isRunning = true;
    receiverThread.m_threadHandler = CreateThread(0, 0, trackHat_ReceiverThreadFunction, &internal, 0, &receiverThread.m_threadID);
    if (receiverThread.m_threadHandler == nullptr)
    {
        LOG_ERROR("Cannot start rceiving. Error " << GetLastError() <<".");
        trackHat_Disconnect(device);
        return TH_ERROR_WRONG_PARAMETER;
    }

    // Start callback thread
    callbackThread.m_isRunning = true;
    callbackThread.m_threadHandler = CreateThread(0, 0, trackHat_CallbackThreadFunction, &internal, 0, &callbackThread.m_threadID);
    if (callbackThread.m_threadHandler == nullptr)
    {
        LOG_ERROR("Cannot start callback system. Error " << GetLastError() << ".");
        trackHat_Disconnect(device);
        return TH_ERROR_WRONG_PARAMETER;
    }
    internal.m_callback.m_mutex = CreateMutex(NULL, false, NULL);

    Sleep(50);  // Give 50 ms time to flush the buffers.

    // Update device info
#if 0
    result = trackHat_UpdateInfo(device);
    if (result != TH_SUCCESS)
    {
        trackHat_Disconnect(device);
        return result;
    }
#endif

    // Disable Idle mode
    result = trackHat_EnableSendingCoordinates(device, true, frameType);
    if (result != TH_SUCCESS)
    {
        trackHat_Disconnect(device);
        return result;
    }

    internal.m_isOpen = true;
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
    trackHat_Thread_t& callbackThread = internal->m_callback.m_thread;
    trackHat_Thread_t& receiverThread = internal->m_receiver;
    usbSerial_t& serial = internal->m_serial;

    if (serial.m_isPortOpen)
    {
        // Enable Idle mode
        trackHat_EnableSendingCoordinates(device, false, TH_FRAME_BASIC);
    }

    // Stop receiving thread
    receiverThread.m_isRunning = false;
    if (receiverThread.m_threadHandler != nullptr)
    {
        WaitForSingleObject(receiverThread.m_threadHandler, MESSAGE_EVENT_TIMEOUT_MS);
        CloseHandle(receiverThread.m_threadHandler);
        receiverThread.m_threadHandler = nullptr;
        receiverThread.m_threadID = 0;
    }

    // Stop callback thread
    callbackThread.m_isRunning = false;
    if (callbackThread.m_threadHandler != nullptr)
    {
        WaitForSingleObject(callbackThread.m_threadHandler, MESSAGE_EVENT_TIMEOUT_MS);
        CloseHandle(callbackThread.m_threadHandler);
        callbackThread.m_threadHandler = nullptr;
        callbackThread.m_threadID = 0;
    }
    if (internal->m_callback.m_mutex != nullptr)
    {
        CloseHandle(internal->m_callback.m_mutex);
        internal->m_callback.m_mutex = nullptr;
    }

    internal->m_isOpen = false;
    return UsbSerial::close(serial);
}


TH_ErrorCode trackHat_UpdateInfo(trackHat_Device_t* device)
{
    if ((device==nullptr) || (device->m_pInternal == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Internal_t* internal = reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    if (internal->m_isUnplugged)
    {
        return TH_ERROR_DEVICE_DISCONECTED;
    }

    LOG_INFO("Update device info.");

    // Update DeviceInfo
    TH_ErrorCode result = trackHat_UpdateInternalDeviceInfo(device);
    if (result == TH_SUCCESS)
    {
        // Update Status
        result = trackHat_UpdateInternalStatus(device);
    }

    return result;
}


TH_ErrorCode trackHat_UpdateInternalStatus(trackHat_Device_t* device)
{
    if ((device == nullptr) || (device->m_pInternal == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    MessageStatus& messageStatus = internal.m_messages.m_status;
    usbSerial_t& serial = internal.m_serial;

    if (internal.m_isUnplugged)
    {
        return TH_ERROR_DEVICE_DISCONECTED;
    }

    //LOG_INFO("Update internal status.");

    uint8_t txMessage[MESSAGE_TX_BUFFER_SIZE];
    size_t  txMessageSize = Parser::createMessageGetStatus(txMessage);

    // Update Status

    ResetEvent(messageStatus.m_newMessageEvent);
    TH_ErrorCode result = UsbSerial::write(serial, txMessage, txMessageSize);
    if (result != TH_SUCCESS)
    {
        return result;
    }

    result = trackHat_WaitForNewMessageEvent(messageStatus.m_newMessageEvent, "Status message");
    if (result != TH_SUCCESS)
    {
        return result;
    }

    WaitForSingleObject(messageStatus.m_mutex, INFINITE);
    CameraStatus cameraStatus = messageStatus.m_camStatus;
    device->m_isIdleMode = messageStatus.m_camMode == CameraMode::CAM_IDLE;
    ReleaseMutex(messageStatus.m_mutex);

    switch (cameraStatus)
    {
        case CameraStatus::CAM_READY_TO_USE:
            result = TH_SUCCESS;
            break;

        case CameraStatus::CAM_NOT_PRESENT:
            LOG_ERROR("The camera in the TrackHad device does not work.");
            result = TH_ERROR_CAMERA_INTERNAL_BROKEN;
            break;

        case CameraStatus::CAM_NOT_INITIALIZED:
            LOG_ERROR("The camera in the TrackHat device does not want to start.");
            result = TH_ERROR_CAMERA_INITIALIZING_FAILD;
            break;

        case CameraStatus::CAM_SELF_TEST_FAILD:
            LOG_ERROR("The camera in the the TrackHad device does not work properly.");
            result = TH_ERROR_CAMERA_SELFT_TEST_FAILD;
            break;

        default:
            LOG_ERROR("Internal library fault.");
            result = TH_ERROR_WRONG_PARAMETER;
            break;
    }

    return result;
}


TH_ErrorCode trackHat_UpdateInternalDeviceInfo(trackHat_Device_t* device)
{
    if ((device == nullptr) || (device->m_pInternal == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    MessageDeviceInfo& messageDeviceInfo = internal.m_messages.m_deviceInfo;
    usbSerial_t& serial = internal.m_serial;

    if (internal.m_isUnplugged)
    {
        return TH_ERROR_DEVICE_DISCONECTED;
    }

    //LOG_INFO("Update internal device info.");

    uint8_t txMessage[MESSAGE_TX_BUFFER_SIZE];
    size_t txMessageSize = Parser::createMessageGetDeviceInfo(txMessage);

    ResetEvent(messageDeviceInfo.m_newMessageEvent);
    TH_ErrorCode result = UsbSerial::write(serial, txMessage, txMessageSize);
    if (result != TH_SUCCESS)
    {
        return result;
    }

    result = trackHat_WaitForNewMessageEvent(messageDeviceInfo.m_newMessageEvent, "Device Info message");
    if (result != TH_SUCCESS)
    {
        return result;
    }

    WaitForSingleObject(messageDeviceInfo.m_mutex, INFINITE);
    device->m_hardwareVersion = messageDeviceInfo.m_hardwareVersion;
    device->m_softwareVersionMajor = messageDeviceInfo.m_softwareVersionMajor;
    device->m_softwareVersionMinor = messageDeviceInfo.m_softwareVersionMinor;
    device->m_serialNumber = messageDeviceInfo.m_serialNumber;
    ReleaseMutex(messageDeviceInfo.m_mutex);

    return TH_SUCCESS;
}


TH_ErrorCode trackHat_EnableSendingCoordinates(trackHat_Device_t* device, bool enable, TH_FrameType frameType)
{
    if ((device == nullptr) || (device->m_pInternal == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    usbSerial_t& serial = internal.m_serial;
    internal.m_frameType = frameType;

    if (internal.m_isUnplugged)
    {
        return TH_ERROR_DEVICE_DISCONECTED;
    }

    LOG_INFO((enable ? "Enable" : "Disable") << " sending of the coordinates.");

    uint8_t txMessage[MESSAGE_TX_BUFFER_SIZE];
    size_t  txMessageSize = Parser::createMessageSetMode(txMessage, enable, frameType);

    TH_ErrorCode result = UsbSerial::write(serial, txMessage, txMessageSize);
    if (result != TH_SUCCESS)
    {
        return result;
    }

    result = trackHat_UpdateInternalStatus(device);
    if (result != TH_SUCCESS)
    {
        return result;
    }

    if ((enable && (device->m_isIdleMode == 1)) ||
        (!enable && (device->m_isIdleMode == 0)))
    {
        LOG_ERROR("Setting the operation mode failed.");
        return TH_ERROR_DEVICE_COMUNICATION_FAILD;
    }

    return TH_SUCCESS;
}


TH_ErrorCode trackHat_GetUptime(trackHat_Device_t* device, uint32_t* seconds)
{
    if ((device == nullptr) || (device->m_pInternal == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);

    if (internal.m_isUnplugged)
    {
        return TH_ERROR_DEVICE_DISCONECTED;
    }

    LOG_INFO("Get uptime.");

    // Update Status
    TH_ErrorCode result = trackHat_UpdateInternalStatus(device);
    if (result != TH_SUCCESS)
    {
        return result;
    }

    *seconds = internal.m_messages.m_status.m_uptimeInSec;

    return TH_SUCCESS;
}


DWORD WINAPI trackHat_ReceiverThreadFunction(LPVOID lpParameter)
{
    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(lpParameter);
    trackHat_Thread_t& receiver = internal.m_receiver;
    trackHat_Messages_t& messages = internal.m_messages;
    usbSerial_t& serial = internal.m_serial;
    TH_ErrorCode result = TH_SUCCESS;

    // Buffer for serial per iteration MessageCoordinates::FrameSize size makes the USB
    // return the data immediately after receiving the coordinate frame and not wait until
    // the end of the timeout in case the buffer is large 
    uint8_t serialBuffer[MessageExtendedCoordinates::FrameSize];
    size_t readSize = 0;

    std::vector<uint8_t> dataBuffer;   // data to parse
    dataBuffer.reserve(MESSAGE_RX_BUFFER_SIZE);

    LOG_INFO("Receiving started.");

    while (receiver.m_isRunning)
    {
        result = UsbSerial::read(serial, serialBuffer, sizeof(serialBuffer), readSize);

        if (!receiver.m_isRunning)
            break;

        if ((result==TH_SUCCESS) && (readSize>0))
        {
            dataBuffer.insert(dataBuffer.end(), serialBuffer, serialBuffer + readSize);

            Parser::parseInputData(dataBuffer, messages);
        }
        else if (result==TH_ERROR_DEVICE_COMUNICATION_FAILD)
        {
            LOG_ERROR("Device unplugged.");
            internal.m_isUnplugged = true;
            receiver.m_isRunning = false;
        }
    }

    LOG_INFO("Receiving finished.");
    return 0;
}


DWORD WINAPI trackHat_CallbackThreadFunction(LPVOID lpParameter)
{
    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(lpParameter);
    trackHat_Callback_t& callback = internal.m_callback;
    MessageCoordinates& coordinates = internal.m_messages.m_coordinates;
    MessageExtendedCoordinates& extendedCoordinates = internal.m_messages.m_extendedCoordinates;

    time_t lastErrorTimeSec = 0;
    trackHat_Points_t points;
    trackHat_ExtendedPoints_t extendedPoints;
    DWORD result;

    LOG_INFO("Callback system started.");

    while (callback.m_thread.m_isRunning)
    {
        if (internal.m_frameType == TH_FRAME_BASIC)
        {
            // Check 'm_thread.m_isRunning' every 100 ms

            result = WaitForSingleObject(coordinates.m_newCallbackEvent, 100);

            if (callback.m_thread.m_isRunning == false)
                break;

            WaitForSingleObject(callback.m_mutex, INFINITE);

            if (callback.m_simplePointsCallbackFunction != nullptr)
            {
                if (result == WAIT_OBJECT_0)
                {
                    WaitForSingleObject(coordinates.m_mutex, INFINITE);
                    memcpy(&points, &coordinates.m_points, sizeof(trackHat_Points_t));
                    ReleaseMutex(coordinates.m_mutex);

                    trackHat_CallbackFunction(callback.m_simplePointsCallbackFunction, TH_SUCCESS, &points);
                }
                else
                {
                    time_t currentTimeSec;
                    time(&currentTimeSec);

                    // Run callback function with error at intervals of 2 seconds
                    if (currentTimeSec - lastErrorTimeSec > CAMERA_ERROR_CHECK_INTERVAL)
                    {
                        TH_ErrorCode error = TH_ERROR_WRONG_PARAMETER;
                        if (result == WAIT_TIMEOUT)
                        {
                            if (internal.m_isUnplugged)
                                error = TH_ERROR_DEVICE_DISCONECTED;
                            else if (internal.m_isOpen)
                                error = TH_ERROR_DEVICE_COMUNICATION_TIMEOUT;
                            else
                                error = TH_ERROR_DEVICE_NOT_OPEN;
                        }

                        trackHat_CallbackFunction(callback.m_simplePointsCallbackFunction, error, nullptr);
                        lastErrorTimeSec = currentTimeSec;
                    }
                }
            }
        }
        else if (internal.m_frameType == TH_FRAME_EXTENDED)
        {
            // Check 'm_thread.m_isRunning' every 100 ms

            result = WaitForSingleObject(extendedCoordinates.m_newCallbackEvent, 100);

            if (callback.m_thread.m_isRunning == false)
                break;

            WaitForSingleObject(callback.m_mutex, INFINITE);

            if (callback.m_extendedPointsCallbackFunction != nullptr)
            {
                if (result == WAIT_OBJECT_0)
                {
                    WaitForSingleObject(extendedCoordinates.m_mutex, INFINITE);
                    memcpy(&extendedPoints, &extendedCoordinates.m_points, sizeof(trackHat_ExtendedPoints_t));
                    ReleaseMutex(extendedCoordinates.m_mutex);

                    trackHat_CallbackFunction(callback.m_extendedPointsCallbackFunction, TH_SUCCESS, &extendedPoints);
                }
                else
                {
                    time_t currentTimeSec;
                    time(&currentTimeSec);

                    // Run callback function with error at intervals of 2 seconds
                    if (currentTimeSec - lastErrorTimeSec > 2)
                    {
                        TH_ErrorCode error = TH_ERROR_WRONG_PARAMETER;
                        if (result == WAIT_TIMEOUT)
                        {
                            if (internal.m_isUnplugged)
                                error = TH_ERROR_DEVICE_DISCONECTED;
                            else if (internal.m_isOpen)
                                error = TH_ERROR_DEVICE_COMUNICATION_TIMEOUT;
                            else
                                error = TH_ERROR_DEVICE_NOT_OPEN;
                        }

                        trackHat_CallbackFunction(callback.m_extendedPointsCallbackFunction, error, nullptr);
                        lastErrorTimeSec = currentTimeSec;
                    }
                }
            }
        }
        ReleaseMutex(callback.m_mutex);

    }

    LOG_INFO("Callback system finished.");
    return 0;
}


void trackHat_CallbackFunction(trackHat_PointsCallback_t callbackFunction, TH_ErrorCode errorCode, const trackHat_Points_t* const points)
{
    try
    {
        callbackFunction(errorCode, points);
    }
    catch (const std::exception&)
    {
        LOG_ERROR("An exception has occurred in the callback function.");
    }
}

/* Run callback function with provided parameters */
void trackHat_CallbackFunction(trackHat_ExtendedPointsCallback_t callbackFunction,
                               TH_ErrorCode errorCode,
                               const trackHat_ExtendedPoints_t * const points)
{
    try
    {
        callbackFunction(errorCode, points);
    }
    catch (const std::exception&)
    {
        LOG_ERROR("An exception has occurred in the callback function.");
    }
}


TH_ErrorCode trackHat_WaitForNewMessageEvent(HANDLE event, const char* eventName)
{
    if (eventName == nullptr)
        eventName = "";

    DWORD result = WaitForSingleObject(event, MESSAGE_EVENT_TIMEOUT_MS);
    switch (result)
    {
        case WAIT_OBJECT_0:
            //LOG_INFO("Receiving event " << eventName << " OK.");
            return TH_SUCCESS;

        case WAIT_TIMEOUT:
            LOG_ERROR("Receiving event " << eventName << " tiemout.");
            return TH_ERROR_DEVICE_COMUNICATION_TIMEOUT;

        default:
            LOG_ERROR("Receiving event " << eventName <<" filed. Error " << GetLastError() << ".");
            return TH_ERROR_DEVICE_COMUNICATION_FAILD;
    }
}


TH_ErrorCode trackHat_GetDetectedPoints(trackHat_Device_t* device, trackHat_Points_t* points)
{
    if ((device == nullptr) || (device->m_pInternal == nullptr) || (points == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    MessageCoordinates& coordinates = internal.m_messages.m_coordinates;
    TH_ErrorCode result = TH_ERROR_WRONG_PARAMETER;

    if (internal.m_isUnplugged)
    {
        return TH_ERROR_DEVICE_DISCONECTED;
    }

    result = trackHat_WaitForNewMessageEvent(coordinates.m_newMessageEvent /*, "Coordinates message" */);
    ResetEvent(coordinates.m_newMessageEvent);

    if (result != TH_SUCCESS)
        return result;

    WaitForSingleObject(coordinates.m_mutex, INFINITE);
    memcpy(points, &coordinates.m_points, sizeof(trackHat_Points_t));
    ReleaseMutex(coordinates.m_mutex);

    return TH_SUCCESS;
}

TH_ErrorCode trackHat_GetDetectedExtendedPoints(trackHat_Device_t* device, trackHat_ExtendedPoints_t* points)
{
    if ((device == nullptr) || (device->m_pInternal == nullptr) || (points == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    MessageExtendedCoordinates& extendedCoordinates = internal.m_messages.m_extendedCoordinates;
    TH_ErrorCode result = TH_ERROR_WRONG_PARAMETER;

    if (internal.m_isUnplugged)
    {
        return TH_ERROR_DEVICE_DISCONECTED;
    }

    result = trackHat_WaitForNewMessageEvent(extendedCoordinates.m_newMessageEvent /*, "Coordinates message" */);
    ResetEvent(extendedCoordinates.m_newMessageEvent);

    if (result != TH_SUCCESS)
        return result;

    WaitForSingleObject(extendedCoordinates.m_mutex, INFINITE);
    memcpy(points, &extendedCoordinates.m_points, sizeof(trackHat_ExtendedPoints_t));
    ReleaseMutex(extendedCoordinates.m_mutex);

    return TH_SUCCESS;
}


TH_ErrorCode trackHat_SetCallback(trackHat_Device_t* device, trackHat_PointsCallback_t newPoints_callback)
{
    if ((device == nullptr) || (device->m_pInternal == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Callback_t& callback = reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal)->m_callback;


    if (newPoints_callback != nullptr)
    {
        LOG_INFO("Set callback.");
    }
    else if (callback.m_simplePointsCallbackFunction != nullptr)
    {
        // (newPoints_callback == nullptr) && (callback.m_function != nullptr)
        LOG_INFO("Remove callback.");
    }

    WaitForSingleObject(callback.m_mutex, INFINITE);
    callback.m_simplePointsCallbackFunction = newPoints_callback;
    ReleaseMutex(callback.m_mutex);

    return TH_SUCCESS;
}


TH_ErrorCode trackHat_RemoveCallback(trackHat_Device_t* device)
{
    return trackHat_SetCallback(device, nullptr);
}

TH_ErrorCode trackHat_SetExtendedPointsCallback(trackHat_Device_t *device, trackHat_ExtendedPointsCallback_t newExtendedPointsCallback)
{
    if ((device == nullptr) || (device->m_pInternal == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Callback_t& callback = reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal)->m_callback;


    if (newExtendedPointsCallback != nullptr)
    {
        LOG_INFO("Set callback.");
    }
    else if (callback.m_simplePointsCallbackFunction != nullptr)
    {
        // (newPoints_callback == nullptr) && (callback.m_function != nullptr)
        LOG_INFO("Remove callback.");
    }

    WaitForSingleObject(callback.m_mutex, INFINITE);
    callback.m_extendedPointsCallbackFunction = newExtendedPointsCallback;
    ReleaseMutex(callback.m_mutex);

    return TH_SUCCESS;
}

TH_ErrorCode trackHat_GetDetectedPointsExtended(trackHat_Device_t *device, trackHat_ExtendedPoints_t *points)
{
    if ((device == nullptr) || (device->m_pInternal == nullptr) || (points == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Internal_t& internal = *reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    MessageExtendedCoordinates& extendedCoordinates = internal.m_messages.m_extendedCoordinates;
    TH_ErrorCode result = TH_ERROR_WRONG_PARAMETER;

    if (internal.m_isUnplugged)
    {
        return TH_ERROR_DEVICE_DISCONECTED;
    }

    result = trackHat_WaitForNewMessageEvent(extendedCoordinates.m_newMessageEvent /*, "Coordinates message" */);
    ResetEvent(extendedCoordinates.m_newMessageEvent);

    if (result != TH_SUCCESS)
        return result;

    WaitForSingleObject(extendedCoordinates.m_mutex, INFINITE);
    memcpy(points, &extendedCoordinates.m_points, sizeof(trackHat_ExtendedPoints_t));
    ReleaseMutex(extendedCoordinates.m_mutex);

    return TH_SUCCESS;
}

TH_ErrorCode trackHat_SetRegisterValue(trackHat_Device_t* device, trackHat_SetRegister_t* newRegisterValue)
{
    if ((device==nullptr) || (device->m_pInternal == nullptr))
        return TH_ERROR_WRONG_PARAMETER;

    trackHat_Internal_t* internal = reinterpret_cast<trackHat_Internal_t*>(device->m_pInternal);
    usbSerial_t& serial = internal->m_serial;

    if (internal->m_isUnplugged)
    {
        return TH_ERROR_DEVICE_DISCONECTED;
    }

    uint8_t transactionID = 0;
    uint8_t txMessage[MESSAGE_TX_BUFFER_SIZE] = {};
    size_t  txMessageSize = Parser::createMessageSetRegister(txMessage, MESSAGE_TX_BUFFER_SIZE, newRegisterValue, &transactionID);
    TH_ErrorCode result = UsbSerial::write(serial, txMessage, txMessageSize);
    if (result != TH_SUCCESS)
    {
        return result;
    }

    result = trackHat_waitForResponse(internal, transactionID);

    return result;
}

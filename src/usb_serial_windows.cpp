// File:   usb_serial_windows.cpp
// Brief:  Functions to control serial port over USB on Windows
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

// Link Setupapi.lib library
#pragma comment (lib, "Setupapi.lib")

#include "usb_serial.h"

#include "track_hat_types.h"
#include "track_hat_types_internal.h"

#include <cstdio>
#include <cstring>
#include <windows.h>
#include <Setupapi.h>


uint16_t usbGetComPort(uint16_t vendorId, uint16_t productId)
{
    HDEVINFO deviceInfoSet;
    DWORD    deviceIndex = 0;
    SP_DEVINFO_DATA deviceInfoData;
    PCSTR devEnum = "USB";
    DEVPROPTYPE ulPropertyType;
    DWORD dwSize = 0;
    DWORD error = 0;
    uint16_t detectedComPort = 0;
    char expectedDeviceIds[80];
    char szBuffer[1024] = { 0 };

    // Create device hardware id
    sprintf(expectedDeviceIds, "VID_%04x&PID_%04x", vendorId, productId);

    // SetupDiGetClassDevs returns a handle to a device information set
    deviceInfoSet = SetupDiGetClassDevs(NULL,
                                        devEnum,
                                        NULL,
                                        DIGCF_ALLCLASSES | DIGCF_PRESENT);

    if (deviceInfoSet==INVALID_HANDLE_VALUE)
        return 0;

    //Fills a block of memory with zeros
    ZeroMemory(&deviceInfoData, sizeof(SP_DEVINFO_DATA));
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    //Receive information about an enumerated device
    while (SetupDiEnumDeviceInfo(deviceInfoSet,
                                 deviceIndex,
                                 &deviceInfoData))
    {
        deviceIndex++;

        //Retrieves a specified Plug and Play device property
        if (SetupDiGetDeviceRegistryProperty(deviceInfoSet,
                                             &deviceInfoData,
                                             SPDRP_HARDWAREID,
                                             &ulPropertyType,
                                             (BYTE*)szBuffer,
                                             sizeof(szBuffer),
                                             &dwSize))
        {
            HKEY hDeviceRegistryKey;

            //Get the key
            hDeviceRegistryKey = SetupDiOpenDevRegKey(deviceInfoSet,
                                                      &deviceInfoData,
                                                      DICS_FLAG_GLOBAL,
                                                      0,
                                                      DIREG_DEV, KEY_READ);
            if (hDeviceRegistryKey == INVALID_HANDLE_VALUE)
            {
                //Not able to open registry
                error = GetLastError();
                break;
            }
            else
            {
                const char* detectedIds = strstr(szBuffer, expectedDeviceIds);
                if (detectedIds != NULL)
                {
                    // Read in the name of the port
                    char serialPortName[32];
                    DWORD dwSize = sizeof(serialPortName);
                    DWORD dwType = 0;
                    if ((RegQueryValueEx(hDeviceRegistryKey, "PortName", NULL, &dwType, (LPBYTE)serialPortName, &dwSize) == ERROR_SUCCESS)
                                         && (dwType == REG_SZ))
                    {
                        // Check if it really is a com port
                        if (strncmp(serialPortName, "COM", 3) == 0)
                        {
                            int32_t comPortNo = atoi(serialPortName + 3);
                            if (comPortNo != 0)
                            {
                                detectedComPort = comPortNo;
                            }
                        }
                    }
                }

                // Close the key now that we are finished with it
                RegCloseKey(hDeviceRegistryKey);
            }
        }
    }
    if (deviceInfoSet)
    {
        SetupDiDestroyDeviceInfoList(deviceInfoSet);
    }

    return detectedComPort;
}


TH_ErrorCode usbSerialOpen(usbSerial_t& serial)
{
    if (serial.m_isPortOpened)
        return TH_ERROR_DEVICE_ALREADY_OPEN;

    BOOL status = FALSE;

    if (serial.m_comNumber == 0)
        return TH_ERROR_DEVICE_NOT_DETECTED;

    // "\\\\.\\COM1" is Windows format
    sprintf(serial.m_comFileName, "\\\\.\\COM%d", serial.m_comNumber);

    //Open the serial COM port
    serial.m_comHandler = CreateFile(serial.m_comFileName, // COM friendly name
                        GENERIC_READ | GENERIC_WRITE,      // Read/Write Access
                        0,                                 // No Sharing, ports cant be shared
                        NULL,                              // No Security
                        OPEN_EXISTING,                     // Open existing port only
                        0,                                 // Non Overlapped I/O
                        NULL);                             // Null for Comm Devices
    if (serial.m_comHandler == INVALID_HANDLE_VALUE)
    {
        return TH_ERROR_DEVICE_COMUNICATION_FAILD;
    }
    
    //Setting Timeouts
    //TODO check timouts
    serial.m_timeouts.ReadIntervalTimeout = 50;
    serial.m_timeouts.ReadTotalTimeoutConstant = 50;
    serial.m_timeouts.ReadTotalTimeoutMultiplier = 1;
    serial.m_timeouts.WriteTotalTimeoutConstant = 50;
    serial.m_timeouts.WriteTotalTimeoutMultiplier = 1;

    if (SetCommTimeouts(serial.m_comHandler, &serial.m_timeouts) == FALSE)
    {
        return TH_ERROR_DEVICE_COMUNICATION_FAILD;
    }

    serial.m_isPortOpened = true;
    return TH_SUCCESS;
}

TH_ErrorCode usbSerialClose(usbSerial_t& serial)
{
    if (serial.m_isPortOpened)
        CloseHandle(serial.m_comHandler);

    serial.m_isPortOpened = false;
    return TH_SUCCESS;
}

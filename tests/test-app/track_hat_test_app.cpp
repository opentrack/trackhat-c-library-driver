// File:   track_hat_test_app.cpp
// Brief:  Sample test application for TrackHat driver
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "track_hat_driver.h"

#include <iostream>
#include <Windows.h>

/* Operate the TrackHat camera after initializ and connect */
void operateTrackHat(trackHat_Device_t* device);

/* Operate the TrackHat camera after initializ and connect */
void printTrackHatInfo(trackHat_Device_t* device);

int main()
{
    trackHat_Device_t device;
    TH_ErrorCode result;

    // Enable debug mode
    trackHat_EnableDebugMode();

    // Intitialize structure
    result = trackHat_Initialize(&device);
    if (result == TH_SUCCESS)
    {
        // Detect device on USB port
        result = trackHat_DetectDevice(&device);
        if (result == TH_SUCCESS)
        {
            // Connect to device
            result = trackHat_Connect(&device);
            if (result == TH_SUCCESS)
            {
                // Camera is ready to use
                operateTrackHat(&device);

                // Disconnect from device
                result = trackHat_Disconnect(&device);
                if (result != TH_SUCCESS)
                {
                    printf("Device not detected. Error %d\n", result);
                }
            }
            else
            {
                printf("Cannot connect to the device. Error %d\n", result);
            }
        }
        else
        {
            printf("Device not detected. Error %d\n", result);
        }

        // Deintitialize structure
        trackHat_Deinitialize(&device);
    }
    else
    {
        printf("Initializing filed. Error %d\n", result);
    }

    system("pause");
    return 0;
}

void printTrackHatInfo(trackHat_Device_t* device)
{
    if (device)
    {
        uint32_t uptimeSec = 0;
        TH_ErrorCode result = trackHat_GetUptime(device, &uptimeSec);

        uptimeSec = 60 * 60 + 61;
        uint32_t seconds = uptimeSec % 60;
        uint32_t minutes = uptimeSec / 60;
        uint32_t hours =   minutes / 60;
        minutes %= 60;

        printf("TrackHat camera info:\n");
        printf("    Hardware ver. : r%d\n", device->m_hardwareVersion);
        printf("    Software ver. : %d.%d\n", device->m_softwareVersionMajor,
                                              device->m_softwareVersionMinor);
        printf("    Serial Number : %d\n", device->m_serialNumber);
        printf("    Mode          : %s\n", (device->m_isIdleMode == true) ? 
                                           "idle" : "sending coordinates" );
        if (result == TH_SUCCESS)
            printf("    Uptime        : %d:%02d:%02d\n", hours, minutes, seconds);
        else
            printf("    Uptime        : error %d\n" , result);
        printf("\n");
    }
}

void operateTrackHat(trackHat_Device_t* device)
{
    printTrackHatInfo(device);

    // Update information about device
    TH_ErrorCode result = trackHat_UpdateInfo(device);
    if (result != TH_SUCCESS)
    {
        printf("Can not update device info. Error %d\n", result);
    }

    printf("After updating the TrackHat device information again.\n");
    printTrackHatInfo(device);

}
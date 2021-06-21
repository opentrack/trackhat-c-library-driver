// File:   track_hat_test_app.cpp
// Brief:  Sample test application for TrackHat driver
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "track_hat_driver.h"

#include <iostream>
#include <Windows.h>

/* Operate initialized and connected TrackHat camera */
void operateTrackHatReadyForOperation(trackHat_Device_t* device);

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
                // Camera is read to use
                operateTrackHatReadyForOperation(&device);

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


void operateTrackHatReadyForOperation(trackHat_Device_t* device)
{
    // Update information about device
    TH_ErrorCode result = trackHat_UpdateInfo(device);
    if (result != TH_SUCCESS)
    {
        printf("Can not update device info. Error %d\n", result);
    }
}
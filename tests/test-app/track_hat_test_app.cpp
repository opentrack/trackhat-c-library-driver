// File:   track_hat_test_app.cpp
// Brief:  Sample test application for TrackHat driver
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "track_hat_driver.h"

#include <iostream>
#include <Windows.h>

int main()
{
    trackHat_Device_t device;
    TH_ErrorCode result;

    // Intitialize structure
    result = trackHat_Initialize(&device);
    if (TH_SUCCESS != result)
    {
        printf("Initializing filed. Error %d\n", result);
        goto exit;
    }

    // Detect device on USB port
    result = trackHat_DetectDevice(&device);
    if (TH_SUCCESS != result)
    {
        printf("Device not detected. Error %d\n", result);
        goto deinitialize_exit;
    }

    // Connect to device
    result = trackHat_Connect(&device);
    if (TH_SUCCESS != result)
    {
        printf("Device not detected. Error %d\n", result);
        goto deinitialize_exit;
    }

    // Update information about device
    result = trackHat_UpdateInfo(&device);
    if (TH_SUCCESS != result)
    {
        printf("Device not detected. Error %d\n", result);
    }

    // Disconnect from device
    result = trackHat_Disconnect(&device);
    if (TH_SUCCESS != result)
    {
        printf("Device not detected. Error %d\n", result);
    }


deinitialize_exit:
    trackHat_Deinitialize(&device);

exit:
    system("pause");
    return 0;
}

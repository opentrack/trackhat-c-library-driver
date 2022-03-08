// File:   track_hat_test_app.cpp
// Brief:  Sample test application for TrackHat driver
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "track_hat_driver.h"

#include <iostream>
#include <signal.h>
#include <time.h>
#include <Windows.h>


/* Use callback to get the coordinates */
#define USE_CALLBACK_FUNCTION 1

/* Use 'trackHat_GetDetectedPoints()' to get the coordinates */
#define USE_GET_FUNCTION 1

#define USE_EXTENDED_COORDINATES 1

/* Run app flag */
bool runApplication = true;

/* Raprot error if detected */
bool errorDetected = false;

/* Operate the TrackHat camera after initializ and connect */
void useCoordinates(trackHat_Device_t* device);

/* Print recived points */
void printCoordinates(const trackHat_Points_t* const points);

/* Print extended points */
void printCoordinates(const trackHat_ExtendedPoints_t* const points);

/* Print information aboit TrackHat device */
void printTrackHatInfo(trackHat_Device_t* device);

/* New TrackHat points callback */
void newPointCallback(TH_ErrorCode error, const trackHat_Points_t* const points);

void newPointCallbackExtended(TH_ErrorCode error, const trackHat_ExtendedPoints_t* const points);

/* handler for terminate signal */
void signalHandler(int signal)
{
    if (signal == SIGINT)
    {
        runApplication = false;
        printf("Stop the application\n");
    }
}

int main()
{
    trackHat_Device_t device;
    TH_ErrorCode result;

    signal(SIGINT, signalHandler);

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
#if USE_EXTENDED_COORDINATES
            result = trackHat_Connect(&device, TH_FRAME_EXTENDED);
#else
            result = trackHat_Connect(&device, TH_FRAME_BASIC);
#endif
            if (result == TH_SUCCESS)
            {
                // Camera is ready for use
                printTrackHatInfo(&device);

                printf("TrackHat camera is readu for use.\n");
                system("pause");

                /*
                 * Example how to set leds
                 * TH_LedState::TH_BLINK - red LED
                 * TH_LedState::TH_OFF - green LED
                 * TH_LedState::TH_SOLID - blue LED
                 * */

                trackHat_SetLeds_t setLeds = {TH_LedState::TH_BLINK, TH_LedState::TH_OFF, TH_LedState::TH_SOLID};
                trackHat_SetLeds(&device, &setLeds);



                /*
                 * Example how to set register
                 * 0X00 - register bank
                 * 0x19 - register address
                 * Register controls how many points are detected
                 * */
                trackHat_SetRegister_t registerValue = {0x00, 0x19, 0x03};
                trackHat_SetRegisterValue(&device, &registerValue);
                registerValue = {0x00, 0x19, 0x02};
                trackHat_SetRegisterValue(&device, &registerValue);
                registerValue = {0x00, 0x19, 0x01};
                trackHat_SetRegisterValue(&device, &registerValue);
                useCoordinates(&device);

                // Disconnect from device
                result = trackHat_Disconnect(&device);


                if (result != TH_SUCCESS)
                {
                    printf("Device not detected. Error %d\n", result);
                    errorDetected = true;
                }
            }
            else
            {
                printf("Cannot connect to the device. Error %d\n", result);
                errorDetected = true;
            }
        }
        else
        {
            printf("Device not detected. Error %d\n", result);
            errorDetected = true;
        }

        // Deintitialize structure
        trackHat_Deinitialize(&device);
    }
    else
    {
        printf("Initializing filed. Error %d\n", result);
        errorDetected = true;
    }

    if (errorDetected)
    {
        printf("SOME ERRORS HAVE BEED DETECTED. Check the logs.\n");
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

void useCoordinates(trackHat_Device_t* device)
{
#if USE_CALLBACK_FUNCTION

#if USE_EXTENDED_COORDINATES
    printf("Start work with callback.\n");

    trackHat_SetExtendedPointsCallback(device, newPointCallbackExtended);

    while (runApplication)
    {
        Sleep(100);
    };

    trackHat_RemoveCallback(device);

    printf("Stop work with callback.\n");

#else
    printf("Start work with callback.\n");

    trackHat_SetCallback(device, newPointCallback);

    while (runApplication)
    {
        Sleep(100);
    };

    trackHat_RemoveCallback(device);

    printf("Stop work with callback.\n");
#endif


#endif //USE_CALLBACK_FUNCTION

#if USE_GET_FUNCTION


    time_t currentTimeSec;
    time_t lastTimeSec = 0;
    time_t timeoutSec = 1;

    time(&currentTimeSec);

    while (1)//runApplication)
    {
        Sleep(static_cast<time_t>(timeoutSec * 1000));

#if USE_EXTENDED_COORDINATES
        trackHat_ExtendedPoints_t extendedPoints;

        TH_ErrorCode result = trackHat_GetDetectedPointsExtended(device, &extendedPoints);

        if (result == TH_SUCCESS)
        {
            time(&currentTimeSec);
            if (lastTimeSec != currentTimeSec)
            {
                lastTimeSec = currentTimeSec;
                printCoordinates(&extendedPoints);
            }
        }
        else
        {
            printf("Get coordinates error: %d\n", result);
            errorDetected = true;
            Sleep(static_cast<time_t>(timeoutSec * 1000));
        }
#else
        trackHat_Points_t points;

        TH_ErrorCode result = trackHat_GetDetectedPoints(device, &points);

        if (result == TH_SUCCESS)
        {
            time(&currentTimeSec);
            if (lastTimeSec != currentTimeSec)
            {
                lastTimeSec = currentTimeSec;
                printCoordinates(&points);
            }
        }
        else
        {
            printf("Get coordinates error: %d\n", result);
            errorDetected = true;
            Sleep(timeoutSec * 1000);
        }
#endif

    }

#endif //USE_GET_FUNCTION

}


void newPointCallback(TH_ErrorCode error, const trackHat_Points_t* const points)
{
    static const time_t timeoutSec = 1;
    static time_t lastTimeSec = 0;
    time_t currentTimeSec;

    if (error == TH_SUCCESS)
    {
        time(&currentTimeSec);
        if (currentTimeSec - lastTimeSec > timeoutSec)
        {
            lastTimeSec = currentTimeSec;
            printCoordinates(points);
        }
    }
    else
    {
        printf("Get coordinates error: %d\n", error);
        errorDetected = true;
    }
}

void newPointCallbackExtended(TH_ErrorCode error, const trackHat_ExtendedPoints_t* const points)
{
    static const time_t timeoutSec = 1;
    static time_t lastTimeSec = 0;
    time_t currentTimeSec;

    if (error == TH_SUCCESS)
    {
        time(&currentTimeSec);
        if (currentTimeSec - lastTimeSec > timeoutSec)
        {
            lastTimeSec = currentTimeSec;
            printCoordinates(points);
        }
    }
    else
    {
        printf("Get coordinates error: %d\n", error);
        errorDetected = true;
    }
}


void printCoordinates(const trackHat_Points_t* const points)
{
    if (runApplication)
    {
        system("cls");
        printf("TrackHat points:\n");
        for (int i = 0; i < TRACK_HAT_NUMBER_OF_POINTS; i++)
        {
            if (points->m_point[i].m_brightness > 0)
            {
                printf("%d: X: %d    Y: %d\n", i,
                       points->m_point[i].m_x, points->m_point[i].m_y);
            }
        }
        fflush(stdout);
    }
}

void printCoordinates(const trackHat_ExtendedPoints_t* const points)
{
    if (runApplication)
    {
        system("cls");
        printf("TrackHat points:\n");
        for (int i = 0; i < TRACK_HAT_NUMBER_OF_POINTS; i++)
        {
            if (points->m_point[i].m_averageBrightness > 0)
            {
                printf("%d: X: %d    Y: %d Brightness: %d Area: %d\n", i,
                       points->m_point[i].m_coordinateX, points->m_point[i].m_coordinateY,
                       points->m_point[i].m_averageBrightness, points->m_point[i].m_area);
            }
        }
        fflush(stdout);
    }
}

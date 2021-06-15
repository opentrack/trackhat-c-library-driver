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

  result = trackHat_Initialize(&device);
  if (TH_SUCCESS != result)
  {
    printf("Initializing filed. Error %d\n", result);
    goto exit;
  }

  trackHat_Deinitialize(&device);

exit:
  system("pause");
  return 0;
}

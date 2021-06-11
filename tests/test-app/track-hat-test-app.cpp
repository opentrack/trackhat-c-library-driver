// File:   track-hat-test-app.cpp
// Brief:  Sample test application for TrackHat driver
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "track-hat-driver.h"

#include <iostream>
#include <Windows.h>

int main()
{
  trackHat_Device_t device;
  trackHat_Initialize(&device);
  system("pause");
  return 0;
}

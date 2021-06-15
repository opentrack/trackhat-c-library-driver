// File:   track_hat_driver.cpp
// Brief:  TrackHat driver library
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "track_hat_driver.h"
#include "track_hat_driver_internal.h"

#include <cstdio>
#include <string>


TH_ErrorCode trackHat_Initialize(trackHat_Device_t* device)
{
  memset(device, 0, sizeof(trackHat_Device_t));

  device->m_internal = malloc(sizeof(trackHat_InternalDevice_t));
  if (device->m_internal == nullptr)
    return TH_MEMORY_ALLOCATION_FIELD;

  memset(device->m_internal, 0, sizeof(trackHat_InternalDevice_t));

  return TH_SUCCESS;
}

void trackHat_Deinitialize(trackHat_Device_t* device)
{
  if (device->m_internal)
    free(device->m_internal);
  memset(device, 0, sizeof(trackHat_Device_t));
}

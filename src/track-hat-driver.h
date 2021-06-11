// File:   track-hat-driver.h
// Brief:  TrackHat driver library
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _TRACK_HAT_DRIVER_H_
#define _TRACK_HAT_DRIVER_H_

/* Macro EXPORT_CPP export symbol to DLL. */
#if defined(_WIN32)
  #define EXPORT_CPP __declspec(dllexport)
#else
  #error "Currently only Windows is supported."
#endif

/* Macro EXPORT_C export symbol as in C language */
#ifdef __cplusplus
  #define EXPORT_C extern "C" EXPORT_CPP
#else
  #define EXPORT_C  EXPORT_CPP
#endif 


/**
 * TrackHat library initializing.
 */
EXPORT_C int track_hat_initialize(void);

#endif //_TRACK_HAT_DRIVER_H_
// File:   track_hat_types_internal.h
// Brief:  TrackHat driver private types
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _TRACK_HAT_TYPES_INTERNAL_H_
#define _TRACK_HAT_TYPES_INTERNAL_H_

#define TRACK_HAT_USB_VENDOR_ID      0x0483
#define TRACK_HAT_USB_PRODUCT_ID     0x5740

/* TrackHat device internal instance. */
typedef struct
{
    uint16_t m_serialCom;
} trackHat_Internal_t;


#endif //_TRACK_HAT_TYPES_INTERNAL_H_

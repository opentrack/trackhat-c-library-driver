// File:   track-hat-driver.cpp
// Brief:  TrackHat driver library
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "track-hat-driver.h"
#include <iostream>


EXPORT_C int track_hat_initialize(void)
{
    std::cout << "Track Hat library test initializing\n";
    return 0;
}

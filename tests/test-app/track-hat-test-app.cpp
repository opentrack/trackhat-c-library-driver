// File:   track-hat-test-app.cpp
// Brief:  Sample test application for TrackHat driver
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "track-hat-driver.h"
#include <Windows.h>

int main()
{
	int ret = track_hat_initialize();
	system("pause");
	return ret;
}

/*
 * BoardMenager.h
 *
 *  Created on: 19.10.2017
 *      Author: Karol
 */

#ifndef BOARDMENAGER_BOARDMENAGER_H_
#define BOARDMENAGER_BOARDMENAGER_H_

#include "USBDriver.h"
#include "GPIODriver.h"
#include "RCCDriver.h"
#include "FMCDriver.h"
#include "LTDCDriver.h"

#include "RTLSDR.h"

#define ADS_B_FREQUENCY 1090000000U
#define ADS_B_SAMPLING  2000000U
class BoardMenager
{
public:
	BoardMenager();

	RTLSDR& GetRTLSDR() { return rtlSdrHandle;}
	USBDriver& GetUSB() { return usbDriverHandle;}
private:
	RCCDriver rccDriver;
	GPIODriver gpioDriver;
	FMCDriver fmcDriver;
	LTDCDriver ltdcDriver;
	USBDriver usbDriverHandle;


	RTLSDR rtlSdrHandle;

};

#endif /* BOARDMENAGER_BOARDMENAGER_H_ */

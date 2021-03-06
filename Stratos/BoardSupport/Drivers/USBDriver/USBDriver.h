/*
 * USBDriver.h
 *
 *  Created on: 18.10.2017
 *      Author: Karol
 */

#ifndef DRIVERS_USBDRIVER_USBDRIVER_H_
#define DRIVERS_USBDRIVER_USBDRIVER_H_

#include <cstdint>

#include "usb_host.h"
#include "usbh_def.h"

#include "GPIODriver.h"

#define USB_PIPE_NUMBER 0x81

#define CTRL_IN		(USB_REQ_TYPE_VENDOR | USB_D2H)
#define CTRL_OUT	(USB_REQ_TYPE_VENDOR | USB_H2D)

class USBDriver
{
public:
	USBDriver(USBH_HandleTypeDef* usbHostHandle,GPIODriver& gpioDriver);
	void InitHost();
	void USBHostProcess();
	bool NewDeviceWasAttached();
	bool DeviceWasDisconnected();

	USBH_StatusTypeDef ControlTransfer(uint8_t request_type,
									   uint8_t bRequest,
									   uint16_t wValue,
									   uint16_t wIndex,
									   unsigned char* data,
									   uint16_t wLength);
	void TurnOnPSO();
	void TurnOfPSO();
private:

	void AllocTransferPipe();

	bool usbDeviceReady;
	uint8_t inPipe;

	GPIODriver& gpioDriver;
	USBH_HandleTypeDef* usbHostHandle;

};

#endif /* DRIVERS_USBDRIVER_USBDRIVER_H_ */

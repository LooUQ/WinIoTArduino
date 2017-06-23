/* WinIoTArduino: Arduino emulation for Win10 IoT
 * Copyright 2017 LooUQ Incorporated
 * Licensed AGPL
*/

// Project specific WinIoTArduino header file: specifies HW and project required abstractions (class hierarchy, method signatures, etc.)

#pragma once

#define WinIotDevice RaspberryPI

#include "RHWinIotArduinoSPI.h"
//#include "RHWinIotArduinoI2c.h"

// create additional HW_* headers for other devl boards with difference GPIO/SPI/I2C configurations

#if (WinIotDevice == RaspberryPI)
	#include "HW_RaspberryPI.h"
#endif

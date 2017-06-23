/////////////////////////////////////////////////////////////////////
/// \class RHHardwareSPI RHHardwareSPI.h <RHHardwareSPI.h>
/// \brief Encapsulate a hardware SPI bus interface
///
/// This concrete subclass of the RadioHead GenericSPIClass that supports the Raspberry PI running Windows 10 IoT Core
/*  Copyright 2017 LooUQ Incorporated.
 *  Licensed under AGPLv3
 */

#pragma once

#include <RHGenericSPI.h>
#include "WinIotArduinoSpi.h"


class RHWinIotArduinoSPI : public RHGenericSPI
{
public:
	RHWinIotArduinoSPI(Frequency frequency = Frequency1MHz, BitOrder bitOrder = BitOrderMSBFirst, DataMode dataMode = DataMode0);
	RHWinIotArduinoSPI(Frequency frequency, BitOrder bitOrder, DataMode dataMode, int csGpio);

	void begin();
	void end();

	uint8_t transfer(uint8_t data);

private:
	WinIotArduinoSPI winIotSpi;
	int frequencyAsInt();
};

extern RHWinIotArduinoSPI RHWinIotSpi;


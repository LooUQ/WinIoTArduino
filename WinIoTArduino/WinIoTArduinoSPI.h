#pragma once

#include "pch.h"
#include "WinIoTArduino.h"

#define DEFAULT_SPI_CTRLINDX 0
#define DEFAULT_SPI_CLOCKFREQ 1000000
#define DEFAULT_SPI_MODE SpiMode::Mode0

using namespace Platform;
using namespace Windows::Devices::Spi;
using namespace Windows::Devices::Gpio;


typedef enum
{
	MSBFIRST = 0,   /*!< MSB First */
	LSBFIRST = 1	/*!< LSB First */
} SpiBitOrder;



class WinIotArduinoSPI
{
public:
	WinIotArduinoSPI(int spiIndex = DEFAULT_SPI_CTRLINDX, SpiMode spiMode = DEFAULT_SPI_MODE, SpiBitOrder bitOrder = MSBFIRST, int clockFrequency = DEFAULT_SPI_CLOCKFREQ);
	~WinIotArduinoSPI();

	void begin();
	void end();
	uint8_t transfer(uint8_t data);

private:
	int _ctrlIndex;
	SpiMode _spiMode;
	SpiBitOrder _bitOrder;			// bit order on WinIoT implementation of SPI is MSB, could implement bit flipper in future
	int _clockFrequency;

	SpiDevice^ _spiDevice;
	GpioPin^ _spiCsGpio;
};



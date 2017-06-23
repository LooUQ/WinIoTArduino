#include "pch.h"
#include "RHWinIotArduino.h"
#include "RHWinIotArduinoSPI.h"

//using namespace IotqiDevices::Network::RadioHead;

#define SPI_INDEX 1
#define SPI_CS 26

RHWinIotArduinoSPI::RHWinIotArduinoSPI(Frequency frequency, BitOrder bitOrder, DataMode dataMode) :
	RHWinIotArduinoSPI(frequency, bitOrder, dataMode, SPI_CS) { }


RHWinIotArduinoSPI::RHWinIotArduinoSPI(Frequency frequency, BitOrder bitOrder, DataMode dataMode, int csGpio)
{
	_frequency = frequency;
	_bitOrder = bitOrder;			// ignored by Win Iot (always sends MSB first)
	_dataMode = dataMode;

	int freqAsInt = frequencyAsInt();
	Windows::Devices::Spi::SpiMode winSpiMode = (Windows::Devices::Spi::SpiMode)_dataMode;

	RHWinIotArduinoSPI::winIotSpi = WinIotArduinoSPI(SPI_INDEX, winSpiMode, csGpio, freqAsInt);
}


void RHWinIotArduinoSPI::begin()
{
}


void RHWinIotArduinoSPI::end()
{
}


uint8_t RHWinIotArduinoSPI::transfer(uint8_t data)
{
	return RHWinIotArduinoSPI::winIotSpi.transfer(data);
}


int RHWinIotArduinoSPI::frequencyAsInt()
{
	int intFreq = 1000000;
	switch (_frequency)
	{
	case RHGenericSPI::Frequency::Frequency2MHz:
		intFreq = 2000000;
		break;
	case RHGenericSPI::Frequency::Frequency4MHz:
		intFreq = 4000000;
		break;
	case RHGenericSPI::Frequency::Frequency8MHz:
		intFreq = 8000000;
		break;
	case RHGenericSPI::Frequency::Frequency16MHz:
		intFreq = 16000000;
		break;
	default:
		intFreq = 1000000;
		break;
	}
	return intFreq;
}


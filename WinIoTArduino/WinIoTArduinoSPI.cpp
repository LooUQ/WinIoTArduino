#include "pch.h"
#include "WinIotArduino.h"
#include "WinIotArduinoSpi.h"


using namespace Platform;
using namespace Windows::Devices::Spi;
using namespace Windows::Devices::Gpio;
using namespace Windows::Devices::Enumeration;
using namespace Windows::System::Threading;
using namespace concurrency;


WinIotArduinoSPI::WinIotArduinoSPI(int ctrlIndex, SpiMode spiMode, SpiBitOrder bitOrder, int clockFrequency)
{
	_ctrlIndex = ctrlIndex;
	_spiMode = spiMode;
	_bitOrder = bitOrder;					// bit order on WinIoT implementation of SPI is MSB, could implement bit flipper in future
	_clockFrequency = clockFrequency;
}

WinIotArduinoSPI::~WinIotArduinoSPI()
{
}


void WinIotArduinoSPI::begin()
{
	// configure Windows SPI device
	String^ spiDeviceSelector = SpiDevice::GetDeviceSelector();

	auto spiInit = create_task(DeviceInformation::FindAllAsync(spiDeviceSelector))
		.then([this](DeviceInformationCollection^ devices)
	{
		auto spiSettings = ref new SpiConnectionSettings(0);
		spiSettings->ClockFrequency = _clockFrequency;
		spiSettings->Mode = _spiMode;

		auto spiDevice = create_task(SpiDevice::FromIdAsync(devices->GetAt(_ctrlIndex)->Id, spiSettings))
			.then([this](SpiDevice^ selectedSpiDevice)
		{
			_spiDevice = selectedSpiDevice;
		});
	});
	while (_spiDevice == nullptr);			// no exit until SPI device pointer set, SPI must finish initialization
}

void WinIotArduinoSPI::end()
{
}


uint8_t WinIotArduinoSPI::transfer(uint8_t data)
{
	auto writeData = ref new Array<uint8_t>(1);
	auto readData = ref new Array<uint8_t>(1);

	writeData[0] = data;
	_spiDevice->TransferFullDuplex(writeData, readData);
	return readData[0];
}



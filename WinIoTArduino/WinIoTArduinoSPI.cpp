#include "pch.h"
#include "WinIotArduino.h"
#include "WinIotArduinoSpi.h"

using namespace Platform;
using namespace Windows::Devices::Spi;
using namespace Windows::Devices::Gpio;
using namespace Windows::Devices::Enumeration;
using namespace Windows::System::Threading;
using namespace concurrency;


WinIotArduinoSPI::WinIotArduinoSPI(int spiIndex, Windows::Devices::Spi::SpiMode spiMode, int csPin, int clockFrequency)
{
	// TODO: remove #define RADIO_SPI_CS_PIN once converted to class fields 

	// configure Windows GPIO (CS) pin
	spiDeviceCS = gpioEmulator.OpenGpio(csPin);

	// configure Windows SPI device
	String^ spiDeviceSelector = SpiDevice::GetDeviceSelector();

	auto spiInit = create_task(DeviceInformation::FindAllAsync(spiDeviceSelector))
		.then([&](DeviceInformationCollection^ devices)
	{
		auto spiSettings = ref new SpiConnectionSettings(0);
		spiSettings->ClockFrequency = clockFrequency;
		spiSettings->Mode = spiMode;

		return create_task(SpiDevice::FromIdAsync(devices->GetAt(spiIndex)->Id, spiSettings));
	}).then([&](SpiDevice^ selectedSpiDevice)
	{
		spiDevice = selectedSpiDevice;
	});
	spiInit.wait();					// wait for config task to finish before returning
}


WinIotArduinoSPI::~WinIotArduinoSPI()
{
}


void WinIotArduinoSPI::begin()
{
}


void WinIotArduinoSPI::end()
{
}


uint8_t WinIotArduinoSPI::transfer(uint8_t data)
{
	auto writeData = ref new Array<uint8_t>(1);
	auto readData = ref new Array<uint8_t>(1);

	writeData[0] = data;
	spiDevice->TransferFullDuplex(writeData, readData);
	return readData[0];
}


#pragma once


using namespace Windows::Devices::Spi;

#ifdef DEBUG

#endif // DEBUG


/*! \brief WINIOTSPIBitOrder SPI Bit order
Specifies the SPI data bit ordering for WinIot_spi_setBitOrder()
*/
typedef enum
{
	LSBFIRST = 0,  /*!< LSB First */
	MSBFIRST = 1   /*!< MSB First */
}WinIot_SPIBitOrder;

/*! \brief SPI Data mode
Specify the SPI data mode to be passed to WinIot_spi_setDataMode()
*/
typedef enum
{
	MODE0 = 0,  /*!< CPOL = 0, CPHA = 0 */
	MODE1 = 1,  /*!< CPOL = 0, CPHA = 1 */
	MODE2 = 2,  /*!< CPOL = 1, CPHA = 0 */
	MODE3 = 3   /*!< CPOL = 1, CPHA = 1 */
} WinIot_SPIMode;



class WinIotArduinoSPI
{
public:
	WinIotArduinoSPI(int spiIndex = 1, SpiMode spiMode = SpiMode::Mode0, int csPin = 26, int clockFrequency = 100000);
	~WinIotArduinoSPI();

	void begin();
	void end();

	uint8_t transfer(uint8_t data);

private:
	Windows::Devices::Spi::SpiDevice^ spiDevice;
	Windows::Devices::Gpio::GpioPin^ spiDeviceCS;
};



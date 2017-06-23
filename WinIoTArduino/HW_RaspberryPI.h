// Hardware specific enums
// Raspberry PI 2 or 3

#pragma once

typedef enum
{
	SpiController0 = 0,
	SpiController1 = 1
} WinIotSpiControllers;


typedef enum
{
	/* RPi B+ J8 header, also RPi 2\3 40 pin GPIO header */
	RPI_BPLUS_GPIO_J8_03 = 2,  /*!< B+, Pin J8-03 */
	RPI_BPLUS_GPIO_J8_05 = 3,  /*!< B+, Pin J8-05 */
	RPI_BPLUS_GPIO_J8_07 = 4,  /*!< B+, Pin J8-07 */
	RPI_BPLUS_GPIO_J8_08 = 14,  /*!< B+, Pin J8-08, defaults to alt function 0 UART0_TXD */
	RPI_BPLUS_GPIO_J8_10 = 15,  /*!< B+, Pin J8-10, defaults to alt function 0 UART0_RXD */
	RPI_BPLUS_GPIO_J8_11 = 17,  /*!< B+, Pin J8-11 */
	RPI_BPLUS_GPIO_J8_12 = 18,  /*!< B+, Pin J8-12, can be PWM channel 0 in ALT FUN 5 */
	RPI_BPLUS_GPIO_J8_13 = 27,  /*!< B+, Pin J8-13 */
	RPI_BPLUS_GPIO_J8_15 = 22,  /*!< B+, Pin J8-15 */
	RPI_BPLUS_GPIO_J8_16 = 23,  /*!< B+, Pin J8-16 */
	RPI_BPLUS_GPIO_J8_18 = 24,  /*!< B+, Pin J8-18 */
	RPI_BPLUS_GPIO_J8_19 = 10,  /*!< B+, Pin J8-19, MOSI when SPI0 in use */
	RPI_BPLUS_GPIO_J8_21 = 9,  /*!< B+, Pin J8-21, MISO when SPI0 in use */
	RPI_BPLUS_GPIO_J8_22 = 25,  /*!< B+, Pin J8-22 */
	RPI_BPLUS_GPIO_J8_23 = 11,  /*!< B+, Pin J8-23, CLK when SPI0 in use */
	RPI_BPLUS_GPIO_J8_24 = 8,  /*!< B+, Pin J8-24, CE0 when SPI0 in use */
	RPI_BPLUS_GPIO_J8_26 = 7,  /*!< B+, Pin J8-26, CE1 when SPI0 in use */
	RPI_BPLUS_GPIO_J8_29 = 5,  /*!< B+, Pin J8-29,  */
	RPI_BPLUS_GPIO_J8_31 = 6,  /*!< B+, Pin J8-31,  */
	RPI_BPLUS_GPIO_J8_32 = 12,  /*!< B+, Pin J8-32,  */
	RPI_BPLUS_GPIO_J8_33 = 13,  /*!< B+, Pin J8-33,  */
	RPI_BPLUS_GPIO_J8_35 = 19,  /*!< B+, Pin J8-35, can be PWM channel 1 in ALT FUN 5 */
	RPI_BPLUS_GPIO_J8_36 = 16,  /*!< B+, Pin J8-36,  */
	RPI_BPLUS_GPIO_J8_37 = 26,  /*!< B+, Pin J8-37,  */
	RPI_BPLUS_GPIO_J8_38 = 20,  /*!< B+, Pin J8-38,  */
	RPI_BPLUS_GPIO_J8_40 = 21   /*!< B+, Pin J8-40,  */
} WinIotGPIOPins;


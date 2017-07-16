// Hardware specific enums
// DragonBoard 410C

#pragma once

typedef enum
{
	SPI_0 = 0
} WinIotSpiControllers;


typedef enum
{
	I2C_0 = 0,
	I2C_1 = 1
} WinIoTI2cControllers;


typedef enum
{
	UART_0 = 0,
	UART_1 = 1,
} WinIoTUartControllers;


typedef enum
{
	GPIO_2 = 2,
	GPIO_3 = 3,
	GPIO_4 = 4,
	GPIO_5 = 5,
	GPIO_6 = 6,
	GPIO_7 = 7,
	GPIO_8 = 8,
	GPIO_9 = 9,
	GPIO_10 = 10,
	GPIO_11 = 11,
	GPIO_12 = 12,
	GPIO_13 = 13,
	// GPIO_14 not supported under WinIoT
	// GPIO_15 not supported under WinIoT
	GPIO_16 = 16,
	GPIO_17 = 17,
	GPIO_18 = 18,
	GPIO_19 = 19,
	GPIO_20 = 20,
	GPIO_21 = 21,
	GPIO_22 = 22,
	GPIO_23 = 23,
	GPIO_24 = 24,
	GPIO_25 = 25,
	GPIO_26 = 26,
	GPIO_27 = 27,
	// SPI CS labels supported
	SPI_0_0 = 8,
	SPI_0_1 = 7,
	SPI_1_0 = 16
} WinIotGPIOPins;


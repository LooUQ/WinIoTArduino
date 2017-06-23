// IotqiRasPiRH: iotQi UWP implementation for Radio Head
// Creates abstractions from Arduino conventions used by Radio Head to Windows 10 IoT Core structures implemented on Raspberry PI 
// Copyright 2017 LooUQ Incorporated 
// License: AGPL-V3


#pragma once

#define RPI_GPIO_PIN_COUNT 26

#define RADIO_SPI_CS_PIN 26
#define RADIO_SPI_INDEX 1

// create simple definitions for Arduino header definitions
#define OUTPUT GPIO_MODE_OUTPUT
#define INPUT GPIO_MODE_INPUT
#define HIGH HIGH
#define LOW LOW
#define CHANGE 1
#define FALLING 2
#define RISING 3

using namespace Windows::Devices::Gpio;


typedef enum
{
	INPUT = 0x00,
	OUTPUT = 0x01
} WinIot_GPIOMode;

typedef enum
{
	LOW = 0x00,
	HIGH = 0x01
} WinIot_GPIOValue;


ref class MillisEmulator sealed
{
public:
	MillisEmulator();
	uint32_t GetMillis();

private:
	uint32_t millisValue;
	Windows::System::Threading::ThreadPoolTimer^ millisTimer;
};

extern MillisEmulator millisEmulator;


class SerialEmulator
{
public:
#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

	// TODO: move these from being inlined
	static void begin(int baud);
	static size_t println(const char* s = "");
	static size_t print(const char* s);
	static size_t print(unsigned int n, int base = DEC);
	static size_t print(char ch);
	static size_t println(char ch);
	static size_t print(unsigned char ch, int base = DEC);
	static size_t println(unsigned char ch, int base = DEC);
	static void printf(const char * fmt ...);
	static size_t write(const char* s, const size_t bytes);

	static void debugObject(Platform::Object^ parameter);
};

extern SerialEmulator Serial;



// gpio servicing class
// ====================================================================================================================
class GpioEmulator
{
public:
	GpioPin^ OpenGpio(uint8_t pin);

	GpioPin^ gpioPins[RPI_GPIO_PIN_COUNT + 1];

private:
	bool gpioInUse[RPI_GPIO_PIN_COUNT + 1];
};

extern GpioEmulator gpioEmulator;


// classless functionality to simulate on Raspberry PI (UWP::Device)
// ====================================================================================================================

void pinMode(unsigned char pin, unsigned char mode);
void digitalWrite(unsigned char pin, unsigned char value);
uint8_t digitalRead(unsigned char pin);

uint32_t millis();
void delay(uint32_t delayMillis);
int32_t random(int32_t min = LONG_MIN, int32_t max = LONG_MAX);

//void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode)
void attachInterrupt(uint8_t interrupt, void(*ISR)(void), int mode);
void detachInterrupt(uint8_t interrupt);


// mutex lock class, implemements ATOMIC_BLOCK_ macro and used for protecting RH _mode\chip mode changes
// ====================================================================================================================

class MutexLock
{
public:
	MutexLock();
	~MutexLock();

	void acquire();
	bool tryAcquire(size_t timeOutMillis);
	void release();

private:
	HANDLE m_hMutex;
};

extern MutexLock radioModeMutex;
extern MutexLock atomicBlockMutex;

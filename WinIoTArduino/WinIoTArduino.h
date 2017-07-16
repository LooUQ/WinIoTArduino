// IotqiRasPiRH: iotQi UWP implementation for Radio Head
// Creates abstractions from Arduino conventions used by Radio Head to Windows 10 IoT Core structures implemented on Raspberry PI 
// Copyright 2017 LooUQ Incorporated 
// License: AGPL-V3


#pragma once
#include "HW_WinIoTArduino.h"

#ifndef WINIOT_DEVICE
#define WINIOT_DEVICE WINIOT_DEVICE_RASPBERRYPI
#endif // !WinIotDevice

// create additional HW_* headers for other devl boards with difference GPIO/SPI/I2C configurations
#if (WINIOT_DEVICE == WINIOT_DEVICE_RASPBERRYPI3)
#include "HW_RaspberryPI3.h"
#elif (WINIOT_DEVICE == WINIOT_DEVICE_DRAGONBOARD401C)
#include "HW_DragonBoard410C.h"
#endif


// create simple definitions matching Arduino header definitions
#define OUTPUT GPIO_MODE_OUTPUT
#define INPUT GPIO_MODE_INPUT
#define HIGH HIGH
#define LOW LOW
#define RISING (int)WinIot_IrqEventMode::Rising
#define FALLING (int)WinIot_IrqEventMode::Falling
#define CHANGE (int)WinIot_IrqEventMode::Change


using namespace Windows::Devices::Gpio;
using namespace Windows::Foundation;

typedef enum
{
	Rising = 1,
	Falling = 2,
	Change = 3
} WinIot_IrqEventMode;

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


// classless functionality to simulate on Raspberry PI (UWP::Device)
// ====================================================================================================================

void pinMode(unsigned char pin, unsigned char mode);
void digitalWrite(unsigned char pin, unsigned char value);
uint8_t digitalRead(unsigned char pin);

uint32_t millis();
void delay(uint32_t delayMillis);
int32_t random(int32_t min = LONG_MIN, int32_t max = LONG_MAX);

//void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode)
void attachInterrupt(uint8_t irqNum, void(*ISR)(void), int eventMode);
void detachInterrupt(uint8_t irqNum);
void disableInterrupts();
void enableInterrupts();

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
	GpioEmulator();
	GpioPin^ OpenGpio(uint8_t pin);

private:
	GpioController^ _gpioController;
	//std::vector<std::pair<uint8_t, GpioPin^>> _gpioPins;
	std::map<uint8_t, GpioPin^> _gpioPins;

	//GpioEmulatorPin _gpioPins[WINIOT_GPIO_PINS];
};

extern GpioEmulator gpioEmulator;


// mutex lock class, implemements ATOMIC_BLOCK_ macro and used for protecting RH _mode\chip mode changes
// ====================================================================================================================

class MutexLock
{
public:
	MutexLock();
	MutexLock(char symbol);
	~MutexLock();

	void acquire(char* dbgToken = "");
	bool tryAcquire(size_t timeOutMillis, char* dbgToken = "");
	void release();

private:
	HANDLE m_hMutex;
	char _symbol;
};


// semaphore lock class, implemements ATOMIC_BLOCK_ type macros, used for protecting critical sections
// ====================================================================================================================

class SemaphoreLock
{
public:
	SemaphoreLock();
	SemaphoreLock(int maxSignals);
	SemaphoreLock(int maxSignals, char dbgSymbol, LPCSTR name);
	~SemaphoreLock();

	void acquire(const char* dbgToken = "-");
	bool tryAcquire(size_t timeOutMillis, const char* dbgToken = "-");
	void release();

private:
	HANDLE _hSemaphore;
	char _dbgSymbol;
};




// IRQ enable\disable implemented with semaphore lock class
// ====================================================================================================================

typedef void(*ISR)(void);
typedef std::tuple<WinIot_IrqEventMode, ISR, EventRegistrationToken> IrqItem;
typedef std::map<uint8_t, IrqItem> IrqRegistrations;

typedef enum
{
	EventMode,
	Isr,
	EventRegToken
} WinIot_IrqItem;


ref class InterruptsEmulator sealed
{
internal:
	InterruptsEmulator();

	void AttachInterrupt(uint8_t interrupt, ISR isr, WinIot_IrqEventMode irqEvent);
	void DetachInterrupt(uint8_t interrupt);
	void InterruptPinChangedEventHandler(GpioPin ^sender, GpioPinValueChangedEventArgs ^args);

	void EnableInterrupts();
	void DisableInterrupts();

	bool WaitIrqEnabled();
	bool WaitIrqEnabled(size_t timeout);

private:
	IrqRegistrations _irqRegistrations;
};

extern InterruptsEmulator interruptsEmulator;


//namespace WinIot
//{
//	void InterruptPinChanged(Windows::Devices::Gpio::GpioPin ^sender, Windows::Devices::Gpio::GpioPinValueChangedEventArgs ^args);
//
//	void EnableInterrupts();
//	void DisableInterrupts();
//
//	bool WaitIrqEnabled();
//	bool WaitIrqEnabled(size_t timeout);
//}



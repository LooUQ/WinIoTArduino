#include "pch.h"
#include "WinIotArduino.h"
#include "Windows.h"

// uncomment lines below to show debug print() statements
//#define DEBUG_SHOW_IRQPIN
//#define DEBUG_SHOW_MUTEX


using namespace Windows::Foundation;
using namespace Windows::Devices::Gpio;
using namespace Windows::System::Threading;
using namespace Windows::System::Diagnostics;

#include <ctime>

void pinMode(unsigned char pin, unsigned char mode)
{
	auto gpioPin = gpioEmulator.OpenGpio(pin);

	switch (mode)
	{
	case WinIot_GPIOMode::GPIO_MODE_OUTPUT:
		gpioPin->SetDriveMode(GpioPinDriveMode::Output);
		break;
	case WinIot_GPIOMode::GPIO_MODE_INPUT:
		gpioPin->SetDriveMode(GpioPinDriveMode::Input);
		break;
	default:
		break;
	}
}


void digitalWrite(unsigned char pin, unsigned char value)
{
	auto gpioPin = gpioEmulator.OpenGpio(pin);

	if (gpioPin->IsDriveModeSupported(GpioPinDriveMode::Output))
	{
		switch (value)
		{
		case WinIot_GPIOValue::HIGH:
			gpioPin->Write(GpioPinValue::High);
			break;
		case WinIot_GPIOValue::LOW:
			gpioPin->Write(GpioPinValue::Low);
			break;
		default:
			break;
		}
	}
}

uint8_t digitalRead(unsigned char pin) 
{
	auto gpioPin = gpioEmulator.OpenGpio(pin);
	GpioPinValue pinValue = gpioPin->Read();
	return (uint8_t)pinValue;
}


uint32_t millis()
{
	return clock();
}



void delay(uint32_t delayMillis)										// simulate blocking delay like Arduino: RadioHead is Arduino based
{
	//WaitForSingleObjectEx(::GetCurrentThread(), delayMillis, FALSE);
	//std::this_thread::sleep_for(std::chrono::milliseconds(millisToWait));
	Sleep(delayMillis);
}


int32_t random(int32_t min, int32_t max)
{
	long diff = max - min;
	long retVal = diff * rand() + min;
	return retVal;
}

// =====================================================================================================================


typedef void(*ISR)(void);

ISR isr[RPI_GPIO_PIN_COUNT + 1];

void InterruptPinChanged(Windows::Devices::Gpio::GpioPin ^sender, Windows::Devices::Gpio::GpioPinValueChangedEventArgs ^args);


void attachInterrupt(uint8_t interrupt, void(*ISR)(void), int mode)
{
	isr[interrupt] = ISR;

	GpioPin^ intPin = gpioEmulator.OpenGpio(interrupt);

	if (intPin->IsDriveModeSupported(GpioPinDriveMode::InputPullDown))
	{
		intPin->SetDriveMode(GpioPinDriveMode::InputPullDown);
		Serial.println("Interrupt pin set to InputPullDown");
	}
	else
	{
		intPin->SetDriveMode(GpioPinDriveMode::Input);
		Serial.println("Interrupt pin set to Input");
	}

	intPin->Write(GpioPinValue::Low);

	intPin->ValueChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Devices::Gpio::GpioPin ^, Windows::Devices::Gpio::GpioPinValueChangedEventArgs ^>(&InterruptPinChanged);
}

void detachInterrupt(uint8_t interrupt)
{
	isr[interrupt] = nullptr;
}


void InterruptPinChanged(Windows::Devices::Gpio::GpioPin ^sender, Windows::Devices::Gpio::GpioPinValueChangedEventArgs ^args)
{
	uint8_t intPin = sender->PinNumber;

	#ifdef DEBUG_SHOW_IRQPIN
		char symb[2] = { '.', '^' };
		Serial.printf(" %c", symb[(uint8_t)args->Edge]);
	#endif // DEBUG_SHOWIRQPIN

	if (args->Edge == GpioPinEdge::RisingEdge && isr[sender->PinNumber] != nullptr)
	{
		isr[intPin]();
	}
}

// =====================================================================================================================


SerialEmulator Serial;

void SerialEmulator::begin(int baud)
{
	// only printing to debug output window
}

void SerialEmulator::debugObject(Platform::Object^ parameter)
{
	auto parameterAsString = parameter->ToString();
	auto formattedText = std::wstring(parameterAsString->Data()).append(L"\r\n");
	OutputDebugString(formattedText.c_str());
}

size_t SerialEmulator::print(const char * s)
{
	auto formattedText = std::string(s);
	OutputDebugStringA(formattedText.c_str());
	return strlen(s);
}

size_t SerialEmulator::println(const char * s)
{
	auto formattedText = std::string(s).append("\r\n");
	OutputDebugStringA(formattedText.c_str());
	return strlen(s) + 2;
}

size_t SerialEmulator::print(unsigned int n, int base)
{
	int sz;
	char printBuffer[10];
	if (base == DEC)
		sz = std::snprintf(printBuffer, sizeof(printBuffer), "%d", n);
	else if (base == HEX)
		sz = std::snprintf(printBuffer, sizeof(printBuffer), "%02x", n);
	else if (base == OCT)
		sz = std::snprintf(printBuffer, sizeof(printBuffer), "%o", n);

	auto formattedText = std::string(printBuffer);
	OutputDebugStringA(formattedText.c_str());
	return sz;
}

size_t SerialEmulator::print(char ch)
{
	char printBuffer[2];
	int sz;
	sz = std::snprintf(printBuffer, sizeof(printBuffer), "%c", ch);
	auto formattedText = std::string(printBuffer);
	OutputDebugStringA(formattedText.c_str());
	return 1;
}

size_t SerialEmulator::println(char ch)
{
	print(ch);
	OutputDebugStringA(std::string("\r\n").c_str());
	return 3;
}

size_t SerialEmulator::print(unsigned char ch, int base)
{
	int sz;
	char printBuffer[10];
	if (base == DEC)
		sz = std::snprintf(printBuffer, sizeof(printBuffer), "%d", ch);
	else if (base == HEX)
		sz = std::snprintf(printBuffer, sizeof(printBuffer), "%02x", ch);
	else if (base == OCT)
		sz = std::snprintf(printBuffer, sizeof(printBuffer), "%o", ch);

	auto formattedText = std::string(printBuffer);
	OutputDebugStringA(formattedText.c_str());
	return sz;
}

size_t SerialEmulator::println(unsigned char ch, int base)
{
	int sz;
	sz = print(ch, base);
	OutputDebugStringA(std::string("\r\n").c_str());
	return sz + 2;
}

void SerialEmulator::printf(const char * fmt ...)
{
	va_list args;
	va_start(args, fmt);

	int sz;
	char printBuffer[256];
	sz = std::vsnprintf(printBuffer, sizeof(printBuffer), fmt, args);
	//vprintf(fmt, args);

	auto formattedText = std::string(printBuffer);
	OutputDebugStringA(formattedText.c_str());
	va_end(args);
}

size_t SerialEmulator::write(const char * s, const size_t bytes)
{
	size_t writeSz = min(bytes, strlen(s));
	auto formattedText = std::string(s).substr(0, writeSz);
	OutputDebugStringA(formattedText.c_str());
	return strlen(s);

	return bytes;
}


// =====================================================================================================================

GpioEmulator gpioEmulator;

Windows::Devices::Gpio::GpioPin^ GpioEmulator::OpenGpio(uint8_t pin)
{
	if (gpioInUse[pin])
	{
		return gpioPins[pin];
	}
	else
	{
		GpioController^ gpioController = GpioController::GetDefault();
		gpioPins[pin] = gpioController->OpenPin(pin);
		gpioInUse[pin] = true;
		return gpioPins[pin];
	}
}


// =====================================================================================================================

	MutexLock::MutexLock()
	{
		m_hMutex = (HANDLE) ::CreateMutex(0, 0, 0);
		if (NULL == m_hMutex)
		{
			Serial.println("***ERROR: Cannot create mutex");
		}
	}

	MutexLock::~MutexLock()
	{
		::CloseHandle(m_hMutex);
	}


	void MutexLock::acquire()
	{
		if (::WaitForSingleObject(m_hMutex, INFINITE) != WAIT_OBJECT_0)
		{
			Serial.println("***ERROR: Cannot acquire mutex");
		}
		#ifdef DEBUG_SHOW_MUTEX
			Serial.print(" mA[ ");
		#endif // DEBUG_SHOW_MUTEX
	}

	bool MutexLock::tryAcquire(size_t timeOutMillis)
	{
		bool retval = false;
		switch (::WaitForSingleObject(m_hMutex, timeOutMillis))
		{
		case WAIT_OBJECT_0:
			retval = true;
			#ifdef DEBUG_SHOW_MUTEX
				Serial.print(" X[ ");
			#endif // DEBUG_SHOW_MUTEX
			break;
		case WAIT_TIMEOUT:
			Serial.println("***ERROR: Timeout-Cannot acquire mutex");
			break;
		default:
			Serial.println("***ERROR: Cannot acquire mutex");
			break;
		}

		return retval;
	}

	void MutexLock::release()
	{
		if (::ReleaseMutex(m_hMutex) == 0)
		{
			Serial.println("***ERROR: Cannot release mutex");
		}
		#ifdef DEBUG_SHOW_MUTEX
			Serial.print(" ]X ");
		#endif // DEBUG_SHOW_MUTEX
	}

	MutexLock radioModeMutex;
	MutexLock atomicBlockMutex;

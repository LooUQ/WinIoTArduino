#include "pch.h"
#include "WinIotArduino.h"
#include "Windows.h"
#include <ctime>

// uncomment lines below to show debug print() statements
#define DEBUG_IRQPIN
//#define DEBUG_MUTEX
//#define DEBUG_SEMAPHORE

using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::Devices::Gpio;
using namespace Windows::System::Threading;
using namespace Windows::System::Diagnostics;


void pinMode(unsigned char gpio, unsigned char mode)
{
	auto gpioPin = gpioEmulator.OpenGpio(gpio);

	if (gpioPin != nullptr)
	{
		try
		{
			switch (mode)
			{
			case WinIot_GPIOMode::GPIO_MODE_OUTPUT:
				if (gpioPin->IsDriveModeSupported(GpioPinDriveMode::Output))
					gpioPin->SetDriveMode(GpioPinDriveMode::Output);
				break;
			case WinIot_GPIOMode::GPIO_MODE_INPUT:
				if (gpioPin->IsDriveModeSupported(GpioPinDriveMode::Input))
					gpioPin->SetDriveMode(GpioPinDriveMode::Input);
				break;
			default:
				break;
			}
		}
		catch (const std::out_of_range&)
		{
			Serial.println("***Error: Memory ref error in pinMode()");
		}
	}
}


void digitalWrite(unsigned char gpio, unsigned char value)
{
	auto gpioPin = gpioEmulator.OpenGpio(gpio);

	if (gpioPin != nullptr && gpioPin->GetDriveMode() == GpioPinDriveMode::Output)
	{
		try
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
		catch (const std::out_of_range&)
		{
			Serial.println("***Error: Memory ref error in digitalWrite()");
		}
	}
}

uint8_t digitalRead(unsigned char gpio)
{
	auto gpioPin = gpioEmulator.OpenGpio(gpio);

	if (gpioPin != nullptr)
	{
		GpioPinValue pinValue = gpioPin->Read();
		return (uint8_t)pinValue;
	}
	return 0;
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

void attachInterrupt(uint8_t irqNum, void(*isr)(void), int eventMode)
{
	WinIot_IrqEventMode mode = static_cast<WinIot_IrqEventMode>(eventMode);
	interruptsEmulator.AttachInterrupt(irqNum, isr, mode);
}

void detachInterrupt(uint8_t irqNum)
{
	interruptsEmulator.DetachInterrupt(irqNum);
}

void disableInterrupts()
{
	interruptsEmulator.DisableInterrupts();
}

void enableInterrupts()
{
	interruptsEmulator.EnableInterrupts();
}

// =====================================================================================================================


GpioEmulator gpioEmulator;

GpioEmulator::GpioEmulator()
{
	_gpioController = GpioController::GetDefault();
}

Windows::Devices::Gpio::GpioPin^ GpioEmulator::OpenGpio(uint8_t gpioNum)
{
	auto findIt = _gpioPins.find(gpioNum);

	if (findIt == _gpioPins.end())		// open GPIO
	{
		GpioOpenStatus openStatus;
		GpioPin^ gpioPin;
		if (_gpioController->TryOpenPin(gpioNum, GpioSharingMode::Exclusive, &gpioPin, &openStatus))
		{
			_gpioPins[gpioNum] = gpioPin;
			return gpioPin;
		}
		else
		{
			Serial.printf("***Error Open GPIO failed, GPIO#: %d invalid or unavailable\r\n", gpioNum);
			_gpioPins[gpioNum] = nullptr;
			return nullptr;
		}
	}
	else								// return existing opened GPIO
	{
		return findIt->second;
	}
}

// =====================================================================================================================

//#define WINIOT_GPIO_PINS 27
//
//ISR isr[WINIOT_GPIO_PINS + 1];
//int irqMode[WINIOT_GPIO_PINS + 1];
//EventRegistrationToken irqHndlr[WINIOT_GPIO_PINS + 1];

InterruptsEmulator interruptsEmulator;


InterruptsEmulator::InterruptsEmulator()
{
}

void InterruptsEmulator::AttachInterrupt(uint8_t irqNum, ISR isr, WinIot_IrqEventMode irqEvent)
{
	auto findIt = _irqRegistrations.find(irqNum);

	if (findIt == _irqRegistrations.end())
	{
		GpioPin^ irqGpio = gpioEmulator.OpenGpio(irqNum);
		if (irqGpio != nullptr)
		{
			irqGpio->SetDriveMode(GpioPinDriveMode::Input);
			EventRegistrationToken eventRegToken = irqGpio->ValueChanged += ref new TypedEventHandler<GpioPin ^, GpioPinValueChangedEventArgs ^>(this, &InterruptsEmulator::InterruptPinChangedEventHandler);

			//                        std::tuple<WinIot_IrqEventMode, ISR, EventRegistrationToken>
			IrqItem newIrqItem = std::make_tuple(irqEvent, isr, eventRegToken);
			_irqRegistrations[irqNum] = newIrqItem;
		}
	}
	else
		Serial.printf("***Error - Interrupt already attached at: %d\r\n", irqNum);
}


void InterruptsEmulator::DetachInterrupt(uint8_t irqNum)
{
	GpioPin^ irqGpio = gpioEmulator.OpenGpio(irqNum);
	irqGpio->ValueChanged -= std::get<WinIot_IrqItem::EventRegToken>(_irqRegistrations[irqNum]);
	irqGpio = nullptr;
}


void InterruptsEmulator::InterruptPinChangedEventHandler(Windows::Devices::Gpio::GpioPin ^sender, Windows::Devices::Gpio::GpioPinValueChangedEventArgs ^args)
{
#ifdef DEBUG_IRQPIN
	char symb[2] = { '_', '^' };
	Serial.printf(" %c", symb[(uint8_t)args->Edge]);
#endif // DEBUG_SHOWIRQPIN

	try
	{
		auto irqItem = _irqRegistrations.at(sender->PinNumber);
		WinIot_IrqEventMode irqMode = std::get<WinIot_IrqItem::EventMode>(_irqRegistrations[sender->PinNumber]);

		if ((irqMode == (int)WinIot_IrqEventMode::Change) ||
			(irqMode == (int)WinIot_IrqEventMode::Rising && (uint8_t)args->Edge == (int)GpioPinEdge::RisingEdge) ||
			(irqMode == (int)WinIot_IrqEventMode::Falling && (uint8_t)args->Edge == (int)GpioPinEdge::FallingEdge))
		{
			InterruptsEmulator::WaitIrqEnabled();
			ISR isr = std::get<WinIot_IrqItem::Isr>(_irqRegistrations[sender->PinNumber]);
			isr();
		}
	}
	catch (const std::out_of_range&)
	{
		Serial.printf("***Error - No interrupt attached at: %d", sender->PinNumber);
	}
}

bool _interruptsEnabled = true;

void InterruptsEmulator::EnableInterrupts()
{
	_interruptsEnabled = true;
}

void InterruptsEmulator::DisableInterrupts()
{
	_interruptsEnabled = false;
}

bool InterruptsEmulator::WaitIrqEnabled()
{
	int yieldWaitMillis = 5;

	while (!_interruptsEnabled)
		WaitForSingleObjectEx(::GetCurrentThread(), yieldWaitMillis, FALSE);
	return true;
}

bool InterruptsEmulator::WaitIrqEnabled(size_t timeout)
{
	int yieldWaitMillis = 5;

	unsigned long starttime = millis();
	while ((millis() - starttime) < timeout)
	{
		if (!_interruptsEnabled)
			return true;
		WaitForSingleObjectEx(::GetCurrentThread(), yieldWaitMillis, FALSE);
	}
	return false;

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


MutexLock::MutexLock()
{
	_symbol = 'M';
	m_hMutex = (HANDLE) ::CreateMutex(NULL, FALSE, NULL);
	if (NULL == m_hMutex)
	{
		Serial.println("***ERROR: Cannot create mutex");
	}
}

MutexLock::MutexLock(char symbol)
{
	_symbol = symbol;
	m_hMutex = (HANDLE) ::CreateMutex(NULL, FALSE, NULL);
	if (NULL == m_hMutex)
	{
		Serial.println("***ERROR: Cannot create mutex");
	}
}

MutexLock::~MutexLock()
{
	::CloseHandle(m_hMutex);
}


void MutexLock::acquire(char* dbgToken)
{
	if (::WaitForSingleObject(m_hMutex, INFINITE) != WAIT_OBJECT_0)
	{
		Serial.println("***ERROR: Cannot acquire mutex");
	}
#ifdef DEBUG_MUTEX
	Serial.printf(" %c{%s ", _symbol, dbgToken);
#endif // DEBUG_MUTEX
}

bool MutexLock::tryAcquire(size_t timeOutMillis, char* dbgToken)
{
	bool retval = false;
	switch (::WaitForSingleObject(m_hMutex, timeOutMillis))
	{
	case WAIT_OBJECT_0:
		retval = true;
#ifdef DEBUG_MUTEX
		Serial.printf(" %c{%s ", _symbol, dbgToken);
#endif // DEBUG_MUTEX
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
#ifdef DEBUG_MUTEX
	Serial.printf("}%c ", _symbol);
#endif // DEBUG_MUTEX
}

// =====================================================================================================================


SemaphoreLock::SemaphoreLock()
{
	_dbgSymbol = ' ';
	int maxSignals = 1;
	const char* name = "semaphoreLock";

	_hSemaphore = (HANDLE)::CreateSemaphoreA(
		NULL,
		maxSignals,
		maxSignals,
		name
	);
	if (_hSemaphore == NULL)
	{
		Serial.println("***ERROR: Cannot create semaphore object");
	}
}

SemaphoreLock::SemaphoreLock(int maxSignals)
{
	_dbgSymbol = ' ';
	const char* name = "semaphoreLock";

	_hSemaphore = (HANDLE)::CreateSemaphoreA(
		NULL,
		maxSignals,
		maxSignals,
		name
	);
	if (_hSemaphore == NULL)
	{
		Serial.println("***ERROR: Cannot create semaphore object");
	}
}

SemaphoreLock::SemaphoreLock(int maxSignals, char dbgSymbol, LPCSTR name)
{
	_dbgSymbol = dbgSymbol;
	_hSemaphore = (HANDLE)::CreateSemaphoreA(
		NULL,
		maxSignals,
		maxSignals,
		name
	);
	if (_hSemaphore == NULL)
	{
		Serial.println("***ERROR: Cannot create semaphore object");
	}
}

SemaphoreLock::~SemaphoreLock()
{
	::CloseHandle(_hSemaphore);
}


void SemaphoreLock::acquire(const char* dbgToken)
{
	if (::WaitForSingleObject(_hSemaphore, INFINITE) != WAIT_OBJECT_0)
	{
		Serial.println("***ERROR: Cannot acquire semaphore");
	}
#ifdef DEBUG_SEMAPHORE
	Serial.printf(" %c[%s", _dbgSymbol, dbgToken);
#endif // DEBUG_SEMAPHORE
}

bool SemaphoreLock::tryAcquire(size_t timeOutMillis, const char* dbgToken)
{
	bool retval = false;
	switch (::WaitForSingleObject(_hSemaphore, timeOutMillis))
	{
	case WAIT_OBJECT_0:
		retval = true;
#ifdef DEBUG_SEMAPHORE
		Serial.printf(" %c[%s", _dbgSymbol, dbgToken);
#endif // DEBUG_SEMAPHORE
		break;
	case WAIT_TIMEOUT:
		Serial.println("***ERROR: Cannot acquire semaphore/timeout");
		break;
	default:
		Serial.println("***ERROR: Cannot acquire semaphore");
		break;
	}
	return retval;
}

void SemaphoreLock::release()
{
	if (!ReleaseSemaphore(_hSemaphore, 1, NULL))
	{
		Serial.println("***ERROR: Cannot release semaphore");
	}
#ifdef DEBUG_SEMAPHORE
	Serial.printf("]%c ", _dbgSymbol);
#endif // DEBUG_SEMAPHORE
}


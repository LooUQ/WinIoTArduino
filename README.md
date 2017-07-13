# WinIoTArduino
Arduino emulation for Win10 IoT Core (Raspberry PI, Dragon, etc.)

WinIoTArduino provides an Arduino emulation to simplify porting libraries written for the Arduino to Windows 10 IoT Core.  We originally developed it here at LooUQ to facilitate our port of the AirSpayce Radio Head library to run on the Raspberry PI and DragonBoard.  After that we saw the benefit of using it for other Arduino to WinIoT library ports.  So here it is, still a work in progress, but a start.
LooUQ is open-source licensing it under AGPL-v3.  Enjoy, hope it helps accelerate your project.

## What’s Here
**WinIoTArduino** folder contains the emulations, there are 2 currently.  
* The WinIoTArduino (.h, .cpp) contain the basics: millis(), delay(), GPIO and a mutex class to make certain I/O code areas atomic (protected from other threads).  
* The WinIoTArduinoSPI (.h, .cpp) contain the SPI functions.  Note: this library implements the CS for SPI outside of the standard Win IoT approach, enabling use of any GPIO for SPI device CS; we do not use the CS0 and CS1 within the SPI controller.
* Future: WinIoTArduinoI2c 
* Each development board will have a HW_* header file to create enums specific to the capabilities of the board.  Raspberry PI is there now for PI2 and PI3.  Win IoT supports Dragon, MinnowMax, hopefully more soon.

The **ProjectSpecificFiles** folder contains the project header file (RHWinIotArduino.h in the repo), which specifies hardware, and optionally includes SPI additional functions such as I2C, PWM, UART, etc.  If your project needs specific behaviors such as base class inheritance or method signatures, you can implement them here as wrapper classes\methods around the WinIoTArduino classes and methods.

In the included example, Radio Head required that the SPI class be derived from one of their base classes (RHGenericSPI).  Additionally they expected a different signature in the constructor; the RHWinIoTArduinoSPI (.h, .cpp) file abstracts the desired consumer behavior from the standard WinIoTArduinoSPI class.

You will get more mileage by creating your own project specific abstractions for your project over modifying the WinIoTArduino library itself.

### Notes
* Pull request are appreciated
* There is no I2C or PWM libraries at this time

Thanks for looking!
Greg

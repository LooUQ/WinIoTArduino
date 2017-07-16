# WinIoTArduino
Arduino emulation for Win10 IoT Core (Raspberry PI, Dragon, etc.)

Review the README.md in the folder above for information regarding the WinIoTArduino project.  

This README is specific to the project specific files included as an example.

The **ProjectSpecificFiles** folder contains the project header file (RHWinIotArduino.h in the repo), which specifies hardware, and optionally includes SPI additional functions such as I2C, PWM, UART, etc.  If your project needs specific behaviors such as base class inheritance or method signatures, you can implement them here as wrapper classes\methods around the WinIoTArduino classes and methods.

In the included example, Radio Head required that the SPI class be derived from one of their base classes (RHGenericSPI).  Additionally they expected a different signature in the constructor; the RHWinIoTArduinoSPI (.h, .cpp) file abstracts the desired consumer behavior from the standard WinIoTArduinoSPI class.

You will get more mileage by creating your own project specific abstractions for your project over modifying the WinIoTArduino library itself.

### Notes
* Pull request are appreciated
* There is no I2C or PWM libraries at this time

Thanks for looking!
Greg

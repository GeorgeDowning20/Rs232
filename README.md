# Rs232 for Mac, Windows, Linux

This project updates Teunis van Beelen's RS232 Library for mac compatiblity

- **Teunis van Beelen's RS232 Library**
- **Blink Serial Program by Arthur Jones and Louise Brown**


## Acknowledgments

This project draws from the work of two major resources:
- **[Teunis van Beelen's RS232 Library](https://www.teuniz.net/RS-232/)** - Provides a stable and versatile C/C++ library for RS232 serial communication.
- **Arthur Jones and Louise Brown's Blink Serial Program** - A simple and effective program for controlling LED blink operations via serial inputs.

## Requirements

- **C/C++ compiler** (e.g., GCC)
- **Serial Port Library (RS232)** - The RS232 library can be downloaded and installed from Teunis van Beelen's website.
- **Optional hardware** - A compatible microcontroller or serial device to test the LED blink feature if desired.

## Setup

1. **Clone this repository:**
   ```bash
   git clone git@github.com:GeorgeDowning20/Rs232.git

2. **Compile**
   ```bash
   gcc -o BlinkSerial BlinkSerial.c rs232.c

3. **Run**
    ```bash
    ./BlinkSerial
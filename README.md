# Microkernel for LPC2105 and nRF52840

This project is a lightweight microkernel designed for the **LPC2105** and **nRF52840** microcontrollers. It features an event queue system and includes drivers for several peripherals such as LEDs and GPIO buttons. A demo program of the classic **Simon Says** game is also provided to showcase the kernel's capabilities.

## Features

- Compatible with **LPC2105** and **nRF52840**
- Event-driven architecture with an internal **event queue**
- Peripheral drivers included:
  - **LEDs**
  - **Buttons / GPIO**
- **Simon Says** demo game included
- Developed with **uVision 5**
- Auto-generated documentation using **Doxygen**

## Getting Started

### Requirements

- **Keil uVision 5** IDE
- LPC2105 or nRF52840 development board or simulator
- Doxygen (optional, for documentation)

### Build and Flash

1. Open the project in **uVision 5**.
2. Select the appropriate target (LPC2105 or nRF52840).
3. Build the project.
4. Flash it to the microcontroller.

## Documentation

The project documentation is generated using **Doxygen** and is available in:

'doc/html/index.html'

##Licence

CC BY-SA 4.0

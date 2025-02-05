# HMI infortainment Sub System

Overview
The HMI infotainment subsystem is designed to provide real-time vehicle telemetry and controls through an intuitive graphical interface. The system is based on the iWave RZ/G2L SMARC Development Kit and features a 10.1-inch display running a Linux-based operating system. The HMI serves as the primary interface between the user and the vehicle’s motor control ECU Unit, which is connected via Ethernet.

## Components

1. Rz/g2l Applications processors
2. Linux OS
3. 10Inch touch  LCD
4. LVGL graphics software

## Boot Process

1. Splash Screen – On startup, the system displays a splash screen.
2. Dashboard Interface – After booting, the user is presented with the main dashboard.

## Dashboard Components

The dashboard provides essential real-time vehicle data and controls, including:

- Speedometer – Displays current vehicle speed.
- Battery Gauge – Indicates the battery charge level.
- Accelerator & Brake Controls – Virtual controls for acceleration and braking.
- Regenerative Braking Bar – Shows the level of energy being recovered through braking.
- Motor Temperature Bar – Displays the current temperature of the motor.
- Fault Detection Indicator – Alerts the user to system faults or errors.
- Start/Stop Button – Used to power the vehicle system on or off.
- Menu Option – Opens an interactive settings window.

## Menu System

Clicking on the Menu Option opens a pop-up window that:

- Allows for lower-level interaction with the board.
- Displays company credits and additional system details.

## Steps to compile

We can compile the lvgl block by :

1. Install dependencency

- cmake
- sdl2

2. clone the repository

```sh
git clone --recursive https://github.com/DevHeadsCommunity/motor-control-linux-HMI.git
cd motor-control-linux-HMI
chmod +x build.sh
```

3. Compile

`./build.sh`

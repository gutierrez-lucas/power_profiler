# POWER PROFILER

## Intro

>> I needed a power profiler for my battery based projects but they are too expensive. So I designed one with a ATMEGA328p (using AVR-C) and a INA219 current/voltage sensor. I made my own driver for the sensor in order to better understand its architecture, and a small python program for configuration (resolution, integration time, attenuation, etc).


## BOM
1) Atmeg328p
2) INA219


## Wiring

Connect using the following schematic: 

![Schematic](/Docs/Schematic.png)


## Usage

1) Compile the project and flash, or directly flash the .hex, into the Atmega328p. 
2) Excecute the (very-very awful looking) python script with:

```
python3 /python_plot/plot.py
```

Use it, is self explanatory. 


## Examples

## Simple LED
Connect a green led with a 170ohm resistor to Vcc (5V) and get the current:

Schematic:

![Example1](/Docs/example-led.png)

Result:

![Example1-result](/Docs/example-led-results.png)

## Variable tension / current

Using a variable laboratory power source, change current from 2V to 6V with the same led-resistor scheme from last example

![Example2-result](/Docs/example-variable-psu-results.png)

## ESP32 WiFi during connection

The current consumed by an ESP32 SoC will be measured, using a WROOM32-dev development board. 
This will be powered by the adjustable supply, by 5V to the regulator input of the board. Although the regulator delivers the 3V3 required by the ESP32, the current that circulates between the source and the assembly will be measured and, therefore, a voltage of 5V will be seen.

Schematic:

![Example3](/Docs/example-esp32.png)

Results:

![Example3-result](/Docs/example-esp32-results.png)

Reference:

1) Start the device
2) Start WiFi module
3) Wait 5 seconds
4) Connect to a known WiFi with a good signal
5) Send a data packet 
6) Wait 1 second
7) Go into deep sleep for 20 seconds
8) Restart.

## ESP32 During connection with reduced current

The laboratory power source currnt limit was configured to 70mA, not enough for a succesful WiFi connection. We see a sudden drop in the voltage as the current gets to that limit. 
We cannot either see the characteristic power consumption of the succesful connection shown in the previous example, showing that the device is not being able to connect.

![Example4-result](/Docs/example-esp32-no-current.png)


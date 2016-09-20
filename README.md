# Meditation Trainer
Breathing/relaxation trainer for Circuit Playground
based on Pulse Sensor Amped 1.4 by Joel Murphy and Yury Gitman   http://www.pulsesensor.com

 Measures pulse rate and changes and inter-beat interval. Uses a Circuit Playground board's built-in LEDs to display a breath pacer, pulse, and relaxation rating

**Hardware requirements:**
 - Adafruit Circuit Playground https://www.adafruit.com/products/3000
 - Pulse Sensor Amped https://www.adafruit.com/products/1093

**Wiring:**
 - Pulse Sensor Black wire to Circuit Playground GND
 - Pulse Sensor Red wire to Circuit Playground VBATT
 - Pulse Sensor Purple wire to Circuit Playground #6

**Breath Pacer:** 
 Eight of the neopixel LEDs on the Circuit Playground are used to display a breath pacer which is helpful for ideal timing of inhale/exhale cycles.  Breath in as the LEDs pulse toward one side, and breath out as they move back to the other side.

**Pulse/Relaxation:** 
 The remaining two LEDs flash with each pulse detected and their color indicates a basic HRV "coherence" rating which roughly translates to the user's level of relaxation:
 - red = low relaxation
 - blue = moderate relaxation
 - green = high relaxation
 
Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!

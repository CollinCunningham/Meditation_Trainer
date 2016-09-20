
/*  Meditation Trainer by Collin Cunningham for Adafruit Industries
 based on Pulse Sensor Amped 1.4 by Joel Murphy and Yury Gitman   http://www.pulsesensor.com

 Uses a Circuit Playground board's built-in LEDs to diplay a breath pacer, pulse, and relaxation rating

 Hardware requirements:
 - Adafruit Circuit Playground https://www.adafruit.com/products/3000
 - Pulse Sensor Amped https://www.adafruit.com/products/1093

 Wiring:
 - Pulse Sensor Black wire to Circuit Playground GND
 - Pulse Sensor Red wire to Circuit Playground VBATT
 - Pulse Sensor Purple wire to Circuit Playground #6

 Breath Pacer: 
 Eight of the neopixel LEDs on the Circuit Playground are used to display a breath pacer which is helpful for ideal timing of inhale/exhale cycles.  Breath in as the LEDs pulse toward one side, and breath out as they move back to the other side.

 Pulse/Relaxation: 
 The remaining two LEDs flash with each pulse detected and their color indicates a basic HRV "coherence" rating which roughly translates to the user's level of relaxation:
 - red = low relaxation
 - blue = moderate relaxation
 - green = high relaxation
 
*/

#include <Adafruit_CircuitPlayground.h>

#define FIRST_BREATH_LED 0
#define LAST_BREATH_LED 9

//  Variables
int pulsePin = 7;                 // Pulse Sensor purple wire connected to analog pin 7

int fadeRate = 0;                 // used to fade LED on with PWM on fadePin
int numPixels = 10;
int breathLED = FIRST_BREATH_LED;
int prevBreathLED = FIRST_BREATH_LED;
bool breathToggle = false;
unsigned long lastBreath = 0;
boolean alive = false;     // we've detected a hearbeat in the last 1.5sec
unsigned long lastAlive;

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.
volatile int tCoh = 10;             // coherence values total
volatile int rCoh = 50;             // coherence rating

// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = false;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse

unsigned long ledTimes[] = {520, 520, 520, 520, 520, 520, 520, 520, 745, 745};  //mostly linear led times for breath pacer

void setup() {
  Serial.begin(115200);             // we agree to talk fast!
  CircuitPlayground.begin();
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS
  // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE,
  // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN4
  //   analogReference(EXTERNAL);

  lastBreath = millis();
}


//  Where the Magic Happens
void loop() {

  serialOutput() ;

  if (QS == true) {    // A Heartbeat Was Found
    //    updateBreathLED();
    alive = true;
    lastAlive = millis();
    // BPM and IBI have been Determined
    // Quantified Self "QS" true when arduino finds a heartbeat
    fadeRate = 255;         // Makes the LED Fade Effect Happen
    // Set 'fadeRate' Variable to 255 to fade LED with pulse
    serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.
    QS = false;                      // reset the Quantified Self flag for next time
  }
  
  unsigned long now = millis();
  if (now - lastAlive > 5000) {
    alive = false;
    //reset coherence values
    tCoh = 10;
    rCoh = 10;
  }

  ledFadeToBeat();                      // Makes the LED Fade Effect Happen
  
  delay(20);                             //  take a break
  
}


void updateBreathLED() {

  //  if (!alive) {
  //    for (int i = 1; i <= numPixels - 2; i++) {
  //      CircuitPlayground.strip.setPixelColor(i, CircuitPlayground.strip.Color(0, 0, 0));
  //    }
  //    breathLED = 0;
  //    return;
  //  }

  if (breathLED < FIRST_BREATH_LED) {
    breathLED = FIRST_BREATH_LED;
    breathToggle = false;
  }
  else if (breathLED > LAST_BREATH_LED) {
    breathLED = LAST_BREATH_LED;
    breathToggle = true;
  }

  //check if we're due to update the breath indicator
  unsigned long timeNow = millis();
  unsigned long timeSince = timeNow - lastBreath;

  unsigned long breathTime = ledTimes[breathLED];
  if (breathToggle) { breathTime = ledTimes[9 - breathLED]; }

  //  if (timeSince >= 545) {
  if (timeSince >= breathTime) {
    prevBreathLED = breathLED;  //save ref to last LED for chase/fade
    if (breathToggle) {
      breathLED -= 1;
    }
    else {
      breathLED += 1;
    }
    lastBreath = timeNow;
  }

  for (int i = 1; i <= numPixels - 1; i++) {
    if ((i == FIRST_BREATH_LED ) || (i == LAST_BREATH_LED)) { //first and last LEDs are used by pulse
      continue;
    }
    if (i == breathLED) { //leading breath LED
      CircuitPlayground.strip.setPixelColor(i, CircuitPlayground.strip.Color(200, 200, 200));
    }
    else if (i == prevBreathLED) {  //chaser/fade LED
      CircuitPlayground.strip.setPixelColor(i, CircuitPlayground.strip.Color(16, 16, 16));
    }
    else CircuitPlayground.strip.setPixelColor(i, CircuitPlayground.strip.Color(0, 0, 0));
  }

}


void ledFadeToBeat() {
  fadeRate -= 15;                         //  set LED fade value
  fadeRate = constrain(fadeRate, 0, 255); //  keep LED fade value from going into negative numbers!

  //find color mods
  int r, g, b;
  if (tCoh >= 6) {
    r = 1; g = 0; b = 0;
  }
  else if (tCoh >= 3) {
    r = 0; g = 0; b = 1;
  }
  else {
    r = 0; g = 1; b = 0;
  }

  //fade first and last pixels to represent pulse
  CircuitPlayground.strip.setPixelColor(0, CircuitPlayground.strip.Color(fadeRate * r, fadeRate * g, fadeRate * b));
  
  CircuitPlayground.strip.setPixelColor(9, CircuitPlayground.strip.Color(fadeRate * r, fadeRate * g, fadeRate * b));

  //move breath LED
  updateBreathLED();

  CircuitPlayground.strip.show();

}





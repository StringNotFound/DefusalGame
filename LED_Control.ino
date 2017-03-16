#include "PinDefinitions.h"

byte LED_banks[NUM_LED_BANKS];
int LED_blink[NUM_LEDS];
unsigned long LED_blink_next[NUM_LEDS];

/*
 * Call before any other LED operations
 */
void initializeLEDs() {
  pinMode(LED_LATCH_PIN, OUTPUT);
  pinMode(LED_CLOCK_PIN, OUTPUT);
  pinMode(LED_DATA_PIN, OUTPUT);
  // Since LED_banks is global, contents are initialized to 0
  // so we don't have to change anything
  updateLEDs(); // Also sets the latch pin
}

/*
 * turns the given LED on if value
 * is 1 and off if value is 0
 * 
 * behavior is undefined if value is not 0 or 1
 */
void setLED(int led, int value) {
  int bank = led / 8;
  byte bankVal = LED_banks[bank];
  led -= bank * 8;
  bankVal &= ~(1 << led); // 0 out the led's digit
  bankVal |= (value << led); // set the led's digit to value
  LED_banks[bank] = bankVal;
}

/*
 * toggles the given LED
 */
void toggleLED(int led) {
  int bank = led / 8;
  byte bankVal = LED_banks[bank];
  led -= bank * 8;
  bankVal ^= (1 << led); // toggle the led's digit
  LED_banks[bank] = bankVal;
}

/*
 * sets the given led to blink at freq frequency
 * (set freq to 0 for the led to not blink at all)
 * frequency is in seconds
 */
void setBlinkLED(int led, int freq) {
  LED_blink[led] = (1000 * freq);
  LED_blink_next[led] = (1000 * freq) + millis();
}

/*
 * sets the given led to blink at freq frequency with
 * an initial state of value
 */
void setBlinkLED(int led, int freq, int value) {
  setLED(led, value);
  setBlinkLED(led, freq);
}

/*
 * returns 1 if the given LED is on,
 * else 0
 */
int getLED(int led) {
  int bank = led / 8;
  led -= bank * 8;
  return ((LED_banks[bank] & (1 << led)) >> led);
}

void blinkLEDs() {
  // should we update the bit-shift registers?
  int stateChanged = 0;
  for (int led = 0; led < NUM_LEDS; led++) {
    // if the LED is supposed to blink and a cycle of time has passed
    if (LED_blink[led] != 0 && millis() > LED_blink_next[led]) {
      stateChanged = 1;
      toggleLED(led);
      LED_blink_next[led] += LED_blink[led]; // increase the cycle time
    }
  }

  // don't update the bit-shift registers if we don't have to
  if (stateChanged) {
    updateLEDs();
  }
}

/*
 * updates the LEDs to display the current state of
 * the LED_banks array and blinks LEDs
 */
void updateLEDs() {
  // update the LED display by writing to the shift registers
  digitalWrite(LED_LATCH_PIN, LOW);
  for (int i = NUM_LED_BANKS - 1; i >= 0; i--) {
    shiftOut(LED_DATA_PIN, LED_CLOCK_PIN, MSBFIRST, LED_banks[i]);
  }
  digitalWrite(LED_LATCH_PIN, HIGH);
}

void resetLEDs() {
  for (int i = 0; i < NUM_LED_BANKS; i++) {
    LED_banks[i] = 0;
  }
}

/*
 * convenience function: call if changing a single LED and want
 *                       immediate change
 */
void setAndUpdateLED(int led, int value) {
  setLED(led, value);
  updateLEDs();
}

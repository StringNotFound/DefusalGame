#include "PinDefinitions.h"

int nixies[NUM_NIXIES];

/*
 * Call before any other Nixie operations
 */
void initializeNixies() {
  pinMode(NIXIE_LATCH_PIN, OUTPUT);
  pinMode(NIXIE_CLOCK_PIN, OUTPUT);
  pinMode(NIXIE_DATA_PIN, OUTPUT);
  // We want the nixies to be initially blank. Thus we set their
  // displays to be 11 (See the SN74141 datasheet)
  for (int i = 0; i < NUM_NIXIES; i++) {
    nixies[i] = 11;
  }
  updateNixies(); // Also sets the latch pin
}

/*
 * Sets nixie's corresponding Nixie tube to display
 * value. Note that value must be in [0,9]
 * 
 * behavior is undefined if value > 16 or < 0, but if
 * value is in [10,16], the nixie tube will turn off
 */
void setNixie(int nixie, int value) {
  nixies[nixie] = value;
}

/*
 * returns the integer that the given nixie is currently set to display
 * (note that this may not be the integer that the nixie is actually displaying
 *  if the nixie's corresponding array value has been changed without updating)
 */
int getNixie(int nixie) {
  return nixies[nixie];
}

/*
 * updates the Nixies to display the current state of
 * the nixies array
 */
void updateNixies() {
  int numbytes = NUM_NIXIES / 2;
  byte nixie_bytes[numbytes];
  for (int i = 0; i < numbytes; i++) {
    // 15 = 0b00001111
    // merges the first and second nixie's values to display
    // on the first shift register, etc
    nixie_bytes[i] = nixies[i] | ((nixies[i + 1] & 15) << 4);
  }

  // Now we actually write out the values...
  digitalWrite(NIXIE_LATCH_PIN, LOW);
  for (int i = numbytes - 1; i >= 0; i--) {
    shiftOut(NIXIE_DATA_PIN, NIXIE_CLOCK_PIN, MSBFIRST, nixie_bytes[i]);
  }
  digitalWrite(NIXIE_LATCH_PIN, HIGH);
}

/*
 * convenience function: call if changing a single Nixie and want
 *                       immediate change
 */
void setAndUpdateNixie(int nixie, int value) {
  setNixie(nixie, value);
  updateNixies();
}

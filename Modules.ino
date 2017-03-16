#include "Modules.h"
#include "Game_Control.h"
#include "LED_Control.h"
#include "Meters.h"
#include "Nixie_Control.h"
#include "PinDefinitions.h"

void (*allModules[NUM_MODULES])() = {testModule};

/*
 * (ID, state) or just (state)
 */
extern int (*switch_CHANGED)(int, int);
extern int (*switchX_CHANGED[NUM_SWITCHES])(int);
extern int (*button_CHANGED)(int, int);
extern int (*buttonX_CHANGED[NUM_BUTTONS])(int);
extern int (*knob_CHANGED)(int, int);
extern int (*knobX_CHANGED[NUM_KNOBS])(int);
extern int (*bolts_CHANGED)(int);
extern int (*Special)();

//--------------------------------TEST MODULE----------------------------------------//
int printButtonState(int button, int state) {
  Serial.print("Button #");
  Serial.print(button);
  if (state) {
    Serial.println(" was pressed");
  } else {
    Serial.println(" was released");
  }
  return 0;
}

int printSwitchState(int s, int state) {
  Serial.print("Switch #");
  Serial.print(s);
  if (state) {
    Serial.println(" was turned on");
  } else {
    Serial.println(" was turned off");
  }
  return 0;
}

int printKnobState(int knob, int state) {
  Serial.print("Knob #");
  Serial.print(knob);
  Serial.print(" has a new value of ");
  Serial.println(state);
  return 0;
}

int printBoltsChanged(int loc) {
  Serial.println("The bolt connections changed. No more info at this time");
}


void testModule() {
  button_CHANGED = printButtonState;
  switch_CHANGED = printSwitchState;
  knob_CHANGED = printKnobState;
  bolts_CHANGED = printBoltsChanged;

  //turn on all LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    setLED(i, 1);
  }
  updateLEDs();
}


#include "LED_Control.h"
#include "Nixie_Control.h"
#include "Input_Control.h"
#include "Game_Control.h"

#define NUM_STAGES 4
#define NUM_STRIKES 3
#define TIMER 300

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  initializeLEDs();
  initializeNixies();
  initializeInput();
}

void loop() {
  // put your main code here, to run repeatedly:
  initializeGame();
  waitForGameStart();
  // we should probably have some way for the user to select
  // the number of stages, but this will suffice for now
  // IDEA: we could use the knobs to select the difficulty
  runGame(NUM_STAGES, NUM_STRIKES, TIMER);
}

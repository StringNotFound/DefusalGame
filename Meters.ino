#include "Meters.h"
#include "PinDefinitions.h"

double maxStrikes;
double numStrikes;

unsigned long startTime;
unsigned long amtTime;

double numMods;
double numCompletedMods;

/*
 * Call before any other meter operations
 */
void initializeMeters(int stages, int strikes, int sec) {
  maxStrikes = (double) strikes;
  numStrikes = 0;
  
  numMods = (double) stages;
  numCompletedMods = 0;

  startTime = millis();
  amtTime = 1000 * sec;

  // no strikes initially
  analogWrite(STRIKE_METER, 0);

  // time meter is maxed initially
  analogWrite(TIME_METER, TIME_METER_MAX_VOLTAGE / 5);
  
  analogWrite(PROGRESS_METER, OUTPUT);
}

void updateStrikeMeter() {
  numStrikes++;
  analogWrite(STRIKE_METER, (int)((numStrikes / maxStrikes) * STRIKE_CONST));
}

void updateTimeMeter() {
  double remaining = (1 - ((millis() - startTime) / amtTime));
  analogWrite(TIME_METER, (int)(remaining * TIME_CONST));
}

void updateProgressMeter() {
  numCompletedMods++;
  analogWrite(STRIKE_METER, (int)((numCompletedMods / numMods) * PROGRESS_CONST));
}


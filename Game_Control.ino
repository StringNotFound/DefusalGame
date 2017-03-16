#include "PinDefinitions.h"
#include "LED_Control.h"
#include "Nixie_Control.h"
#include "Input_Control.h"
#include "Game_Control.h"
#include "Modules.h"
#include "Meters.h"

// check if this works
// this array of module functions is defined in Modules.ino
extern void (*allModules[NUM_MODULES])();

// how many strikes are left?
int strikesLeft;
// has the bomb exploded?
int exploded;
// at what time should the bomb explode?
unsigned long explodeTime;

/* Array of pointers to
 * (int) -> void functions
 * 
 * these functions are called when their corresponding
 * event occurs. The INPUTX_CHANGED functions are called
 * with the new state of their relevant INPUT. The INPUT_CHANGED
 * functions are called with the specific INPUT that changed
 * (the id) along with the new state of that INPUT
 * 
 * the functions return 1 if their running module should end
 * (the module, not the game!)
 */
int (*switch_CHANGED)(int, int);
int (*switchX_CHANGED[NUM_SWITCHES])(int);
int (*button_CHANGED)(int, int);
int (*buttonX_CHANGED[NUM_BUTTONS])(int);
int (*knob_CHANGED)(int, int);
int (*knobX_CHANGED[NUM_KNOBS])(int);
// this function is called when any of the bolt connections changed
// with the parameter of the hashed bolt connection index
int (*bolts_CHANGED)(int);
// this function is called every refresh
int (*Special)();

// both do nothing- used for default handlers
int null2(int ID, int curState) {
  return 0;
}
int null1(int curState) {
  return 0;
}
int null0() {
  return 0;
}

// calls strike()- used for default handlers
int strikeFunction(int i) {
  return strike();
}


// only calls strike on button pressed (not released)-
// used for default handlers
int strikePressedFunction(int curState) {
  if (curState == 1) {
    return strike();
  }
  return 0;
}

/*
 * sets up the game
 */
void initializeGame() {
  // don't need to reset the event handlers, as 
  // we're doing that in runGame

  // The bomb has not exploded at this point
  exploded = 0;

  // reset the getNextModule static variable thing
  getNextModule(NULL, 0);
  
  // deals with inputs that trancend the game
  pinMode(GAME_BUTTON, INPUT);
  pinMode(GAME_SWITCH, INPUT);
  pinMode(NIXIE_POWER_SWITCH, OUTPUT);
  
  // Turn off nixies (perhaps redundant)
  digitalWrite(NIXIE_POWER_SWITCH, LOW);
}

/*
 * waits until both the button and the switch are pressed and then
 * turns on the nixies and allows other functions to execute
 * 
 * call after initializeGame
 */
void waitForGameStart() {
  // button and switch must be turned on to start game
  while(!(digitalRead(GAME_BUTTON) && digitalRead(GAME_SWITCH))) {
    // turn on nixies
    digitalWrite(NIXIE_POWER_SWITCH, HIGH);
  }
  // terminate and let runGame run
  return;
}

/*
 * sets all the event handlers to default
 */
void setHandlersToDefault() {
  Special = null0;
  bolts_CHANGED = null1;
  switch_CHANGED = null2;
  for (int i = 0; i < NUM_SWITCHES; i++) {
    switchX_CHANGED[i] = strikeFunction;
  }
  button_CHANGED = null2;
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttonX_CHANGED[i] = strikePressedFunction;
  }
  knob_CHANGED = null2;
  for (int i = 0; i < NUM_KNOBS; i++) {
    knobX_CHANGED[i] = strikeFunction;
  }
}

/*
 * This function returns a random module from a given array of
 * modules. It is guarnteed to return each module once over n
 * consecutive calls where n is the length of the array. It returns
 * the modules in a random order.
 * 
 * I'm sorry about this function. Here's the typesig:
 * 
 * modArray- a pointer to an array of (void-> int) module functions
 * len- the length of modArray
 * 
 * Returns- a pointer to a single (void-> int) module function
 * 
 * note that before using this function one should call it with
 * modArray as NULL.
 * 
 */
void (*getNextModule(void (**modArray)(), int len))() {
  static int i = 0;
  if (modArray == NULL) {
    i = 0;
    return NULL;
  }
  if (i >= len) {
    i = 0;
  }

  // select an element in [i, len)
  int r = random(i, len);
  void (*tmp)() = modArray[r];

  // switch this element with the ith element
  modArray[r] = modArray[i];
  modArray[i] = tmp;
  i++;

  // return the selected element after ensuring that
  // we won't pick it again by incrementing i
  return tmp;
}

/*
 * checks if the bomb's timer has gone off using the
 * global variable timeLeft
 */
void checkTime() {
  if (millis() > explodeTime) {
    exploded = 1;
  }
}

/*
 * Called as frequently as possible. Scans every input mechanism
 * and when it finds one that has changed since the last time this
 * method was called, it calls the corresponding event handler.
 * 
 * Call after running a module function
 * 
 * Return- 0 if the calling module should continue; 1 if it should terminate
 */
int registerChange() {
  int curState;
  int modOver = 0;
  for (int i = 0; i < NUM_KNOBS && !modOver; i++) {
    // recall that knobChanged returns -1 if the knob hasn't changed, otherwise
    // it returns the knob's new value
    curState = knobChanged(i);
    if (curState >= 0) {
      // if the knob changed, run the corresponding event handler
      modOver = knobX_CHANGED[i](curState);
    }
  }
  for (int i = 0; i < NUM_SWITCHES && !modOver; i++) {
    curState = switchChanged(i);
    if (curState >= 0) {
      // if the switch changed, run the corresponding event handler
      modOver = switchX_CHANGED[i](curState);
    }
  }
  for (int i = 0; i < NUM_BUTTONS && !modOver; i++) {
    curState = buttonChanged(i);
    if (curState >= 0) {
      // if the button changed, run the corresponding event handler
      modOver = buttonX_CHANGED[i](curState);
    }
  }
  return modOver;
}

/*
 * called when the user gets a strike
 * 
 * returns 0
 */
int strike() {
  strikesLeft--;
  if (strikesLeft == 0) {
    // the game should end
    exploded = 1;
  }
  updateTimeMeter();
  // by default, don't terminate the module
  return 0;
}

/*
 * main method for running the game. Call after 
 * initializeGame()
 * 
 * numStages - the number of modules that the bomb should have
 * numStrikes- the number of strikes before the bomb explodes
 * bombTimerSec- the amount of time (in seconds) before the bomb explodes
 */
void runGame(int numStages, int numStrikes, int bombTimerSec) {
  strikesLeft = numStrikes;
  // set the time at which the bomb should explode
  explodeTime = millis() + (1000 * bombTimerSec);
  initializeMeters(numStages, numStrikes, bombTimerSec);

  while (!exploded && numStages > 0) {
    // keep track of when the module has finished
    int modOver = 0;

    // reset all the handlers before running the module
    setHandlersToDefault();
    resetLEDs();
    
    // get the next module and execute its initialization
    getNextModule(allModules, NUM_MODULES)();

    // run the module
    while(!modOver && !exploded) {
      updateTimeMeter();
      // update the LED blinking
      blinkLEDs();
      // registerChange will return 1 if the module should stop
      modOver = registerChange();
      if (!modOver && !exploded) {
        // run the Special routine- nullFunction by default
        modOver = Special();
      }
    }

    // we successfully completed the module (or the bomb exploded)
    numStages--;
    updateProgressMeter();
  }

  // if exploded is true, the bomb exploded and the player lost
  if (exploded) {
    loseAction();
  } else {
    winAction();
  }
  endGame();
}

/*
 * called if the player lost
 */
void loseAction() {
}

/*
 * called if the player won
 */
void winAction() {
}

/*
 * called after the game has ended
 */
void endGame() {
  // turn off nixies
  digitalWrite(NIXIE_POWER_SWITCH, LOW);
}


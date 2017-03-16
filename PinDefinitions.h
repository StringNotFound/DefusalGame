//----------------Global Pin definitions-------------------//
#define LED_LATCH_PIN 8
#define LED_DATA_PIN 11
#define LED_CLOCK_PIN 12

#define NIXIE_LATCH_PIN 8
#define NIXIE_DATA_PIN 11
#define NIXIE_CLOCK_PIN 12

#define GAME_BUTTON 0
#define GAME_SWITCH 0
#define NIXIE_POWER_SWITCH 0

#define STRIKE_METER 0
#define TIME_METER 0
#define PROGRESS_METER 0

// This is unfortunate, but to add another knob, button, bolt,
// or switch, you must increment NUM_XXX by 1, define the 
// PIN to which it's connected, and add it to the XXX_ARRAY
// defined at the top of Input_Control
#define KNOB0 0
#define KNOB1 0
#define KNOB2 0
#define NUM_KNOBS 3

#define BOLT0 0
#define BOLT1 0
#define BOLT2 0
#define BOLT3 0
#define BOLT4 0
#define BOLT5 0
#define BOLT6 0
#define BOLT7 0
#define NUM_BOLTS 8

#define BTN0 0
#define BTN1 0
#define BTN2 0
#define BTN3 0
#define BTN4 0
#define BTN5 0
#define BTN6 0
#define NUM_BUTTONS 7

#define SWITCH0 0
#define SWITCH1 0
#define SWITCH2 0
#define SWITCH3 0
#define NUM_SWITCHES 4

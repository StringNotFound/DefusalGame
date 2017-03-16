#define NUM_LEDS 12
#define NUM_LED_BANKS NUM_LEDS / 8 + 1

void initializeLEDs();
void setLED(int led, int value);
void toggleLED(int led);
void setBlinkLED(int led, int freq);
void setBlinkLED(int led, int freq, int value);
void blinkLEDs();
int getLED(int led);
void updateLEDs();
void setAndUpdateLED(int led, int value);
void resetLEDs();

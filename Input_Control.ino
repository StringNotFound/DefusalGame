#include "PinDefinitions.h"
#include "Input_Control.h"

int knobs[NUM_KNOBS] = {KNOB0, KNOB1, KNOB2};
int switches[NUM_SWITCHES] = {SWITCH0, SWITCH1, SWITCH2, SWITCH3};
int buttons[NUM_BUTTONS] = {BTN0, BTN1, BTN2, BTN3, BTN4, BTN5, BTN6};
int bolts[NUM_BOLTS] = {BOLT0, BOLT1, BOLT2, BOLT3, BOLT4, BOLT5, BOLT6, BOLT7};

int knob_values[NUM_KNOBS];
int knob_values_old[NUM_KNOBS];
int switch_values[NUM_SWITCHES];
int button_values[NUM_BUTTONS];

byte *bolt_connections;

void initializeInput() {
  for (int i = 0; i < NUM_KNOBS; i++) {
    pinMode(knobs[i], INPUT);
  }
  for (int i = 0; i < NUM_SWITCHES; i++) {
    pinMode(switches[i], INPUT);
  }
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttons[i], INPUT);
    // engage the pullup resistor!
    digitalWrite(buttons[i], HIGH);
  }
  for (int i = 0; i < NUM_BOLTS; i++) {
    pinMode(bolts[i], INPUT);
    digitalWrite(bolts[i], LOW);
  }

  // One should wait before reading input
  // from analog pins after having changed
  // the input mode
  delay(50); // delay 50ms

  // initialize the value arrays to the current state
  for (int i = 0; i < NUM_KNOBS; i++) {
    int value = analogRead(knobs[i]);
    knob_values[i] = value;
    knob_values_old[i] = value;
  }
  for (int i = 0; i < NUM_SWITCHES; i++) {
    switch_values[i] = analogRead(switches[i]);
  }
  for (int i = 0; i < NUM_BUTTONS; i++) {
    button_values[i] = analogRead(buttons[i]);
  }
  bolt_connections = getEmptyConnectionMatrix();
  // fill bolt_connections with the current bolt configuration
  boltsChanged();
}

int getKnobValue(int k) {
  return knob_values[k];
}

int getOldKnobValue(int k) {
  return knob_values_old[k];
}

int getSwitchValue(int s) {
  return switch_values[s];
}

int getButtonValue(int b) {
  return button_values[b];
}

/*
 * returns -1 if the knob's value did not change
 * (within the tolerance), else returns the current value
 * of the knob
 */
int knobChanged(int k) {
  int newValue = analogRead(knobs[k]);
  if (abs(newValue - knob_values[k]) > KNOB_CHANGE_TOLERANCE) {
    knob_values_old[k] = knob_values[k];
    knob_values[k] = newValue;
    return newValue;
  }
  return -1;
}

/*
 * returns -1 if the switch's value did not change
 * else returns the current value of the switch
 */
int switchChanged(int s) {
  int newValue = digitalRead(switches[s]);
  if (newValue != switch_values[s]) {
    switch_values[s] = newValue;
    return newValue;
  }
  return -1;
}

/*
 * returns -1 if the button's value did not change
 * else returns the current value of the button
 */
int buttonChanged(int b) {
  // the button has a pullup resistor, so it will be LOW if pressed
  int newValue = !digitalRead(buttons[b]);
  if (newValue != button_values[b]) {
    button_values[b] = newValue;
    return newValue;
  }
  return -1;
}

/*
 * returns a pointer to an empty connection matrix
 */
byte *getEmptyConnectionMatrix() {
  /*
   * So, this is weird. Connection matricies are by nature square matrices
   * where entry [i,j] represents whether pin i is connected to pin j. As a
   * result, the diagonal entries are always 1 (a pin is always connected to itself)
   * and the non-diagonal entries are merely reflections across the diagonal (if [i,j]
   * is connected, [j,i] is also connected). Therefore, we can represent the relevant
   * data with an array storing the elements above the diagonal. With a clever (if I
   * do say so myself) formula, we can then retreive and modify relevant matrix
   * entries by converting the matrix coordinates to array indexes. See setConnected
   * for this formula. This saves both space and CPU relative to a, say, 2D 
   * array representation
   */
  byte *mat = (byte *) malloc(sizeof(byte) * CONNECTION_ARRAY_SIZE);
  for (int i = 0; i < CONNECTION_ARRAY_SIZE; i++) {
    mat[i] = 0;
  }
  return mat;
}

/*
 * cleans up the given connection matrix mat
 */
void freeConnectionMatrix(byte *mat) {
  free(mat);
}

/*
 * converts the given coordinates into a number: think of
 * this as a hash function if used outside of this file
 * 
 * if used within this file, the number represents the index
 * of the coordinates in a connection matrix's array backend
 */
inline int coord2Index(int x, int y) {
  return x + (y * NUM_BOLTS) - (((y + 1) * (y + 2)) / 2);
}

/*
 * indicates a connection between x and y in the given matrix mat
 */
inline void setConnected(byte *mat, int x, int y, byte c) {
  if (x == y) {
    // we don't mess with the diagonal: it's always 1
    return;
  }
  
  if (x < y) {
    // the matrix is reflected, so we just have to modify the
    // entries above the diagonal
    mat[coord2Index(y, x)] = c;
  } else {
    mat[coord2Index(x, y)] = c;
  }
}

/*
 * returns 1 if the matrix mat indicates a connection
 * between pins x and y, 0 otherwise
 */
inline byte isConnected(byte *mat, int x, int y) {
  if (x == y) {
    // a bolt is always connected to itself
    return 1;
  }

  // the matrix is upper triangular
  if (x < y) {
    return mat[coord2Index(y, x)];
  } else {
    return mat[coord2Index(x, y)];
  }
}

/*
 * returns whether or not the pins given by x
 * and y are connected in real life
 */
byte isConnected(int x, int y) {
  return isConnected(bolt_connections, x, y);
}

/*
 * returns 1 if the connection matrices are the
 * same, 0 otherwise
 */
int matricesEqual(byte *mat1, byte *mat2) {
  for (int i = 0; i < CONNECTION_ARRAY_SIZE; i++) {
    if (mat1[i] != mat2[i]) {
      return 0;
    }
  }
  return 1;
}

/*
 * updates the global array blot_connections, returning the array index 
 * of the changed bolt connection if the array changed, else -1
 */
int boltsChanged() {
  int changed = -1;
  
  for (int out = 0; out < NUM_BOLTS; out++) {
    // turn on out's pin
    pinMode(bolts[out], OUTPUT);
    digitalWrite(bolts[out], HIGH);
    
    // see which pins are connected to out
    for (int in = out + 1; in < NUM_BOLTS; in++) {
      // Since only out is HIGH, in will be HIGH iff
      // it's connected to out.
      byte conn = digitalRead(bolts[in]);
      /* we're modifying the array directly because
       * it's faster this way (rather than calling isConnected
       * and setConnected)
       */
      int index = coord2Index(in, out);
      if (bolt_connections[index] != conn) {
        changed = index;
        bolt_connections[index] = conn;
      }
    }

    // turn off out's pin
    digitalWrite(bolts[out], LOW);
    pinMode(bolts[out], INPUT);
  }

  return changed;
}




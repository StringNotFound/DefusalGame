#define KNOB_CHANGE_TOLERANCE 0
#define CONNECTION_ARRAY_SIZE (((NUM_BOLTS - 2) * (NUM_BOLTS - 1)) / 2)

void initializeInput();

int getKnobValue(int k);
int getOldKnobValue(int k);
int getSwitchValue(int s);
int getButtonValue(int b);

int knobChanged(int k);
int switchChanged(int s);
int buttonChanged(int b);

byte *getEmptyConnectionMatrix();
void freeConnectionMatrix(byte *mat);
inline int coord2Index(int x, int y);
inline void setConnected(byte *mat, int x, int y, byte c);
inline byte isConnected(byte *mat, int x, int y);
byte isConnected(int x, int y);
int matricesEqual(byte *mat1, byte *mat2);
int boltsChanged();

int nullFunction(int ID, int curState);
int strikeFunction(int i, int j);
int strikePressedFunction(int ID, int curState);

void initializeGame();
void waitForGameStart();
void setHandlersToDefault();
void (*getNextModule(void (**modArray)(), int len))();
int registerChange();
int strike();
void runGame(int numStages, int numStrikes, int bombTimerSec);
void loseAction();
void winAction();
void endGame();

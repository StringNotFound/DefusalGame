// Must be evenly divisible by 2
#define NUM_NIXIES 6

void initializeNixies();
void setNixie(int nixie, int value);
int getNixie(int nixie);
void updateNixies();
void setAndUpdateNixie(int nixie, int value);

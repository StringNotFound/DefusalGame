//-----------------------CHANGE THESE AS NECESSARY----------------------------//
#define STRIKE_METER_MAX_VOLTAGE (double) 5
#define TIME_METER_MAX_VOLTAGE (double) 5
#define PROGRESS_METER_MAX_VOLTAGE (double) 5

//-----------------------DON'T CHANGE THESE---------------------------------//
#define STRIKE_CONST (int)(255 * (STRIKE_METER_MAX_VOLTAGE / 5))
#define TIME_CONST (int)(255 * (TIME_METER_MAX_VOLTAGE / 5))
#define PROGRESS_CONST (int)(255 * (PROGRESS_METER_MAX_VOLTAGE / 5))

void initializeMeters(int stages, int strikes, int sec);
void updateStrikeMeter();
void updateTimeMeter();
void updateProgressMeter();

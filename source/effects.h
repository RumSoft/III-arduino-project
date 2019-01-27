
//freqshift
#define maxFreqShift 500
int freqShiftArray[maxFreqShift];
unsigned int selectedFreqShift = 0;
unsigned int freqShiftCounter = 0;

int processFreqShift(int value, int _shift)
{
	if(_shift < maxFreqShift / 100)
		return value; 

	freqShiftArray[freqShiftCounter] = value;
	freqShiftCounter = (freqShiftCounter + 1) % _shift;
	return (freqShiftArray[freqShiftCounter] + value)>>1; 
}


//delay
#define maxDelay 15000
int delayArray[maxDelay];
unsigned int selectedDelay = 0;
unsigned int delayCounter = 0;

int processDelay(int value, int _delay)
{
	if(_delay < maxDelay/100)
		return value;

	delayArray[delayCounter] = (value + delayArray[delayCounter]) >> 1;
	delayCounter = (delayCounter + 1) % _delay;
	return delayArray[delayCounter];
}


//chorus
#define maxChorusSpeed 30
#define maxChorusDepth 80
unsigned int selectedChorusSpeed = 0;
unsigned int selectedChorusDepth = 0;

#define MAX_DELAY 80
#define chorusOffset 2
int DELAY[MAX_DELAY + chorusOffset];
int Delay2 = 0;
float delay_sr = 0;
int delay_int = 0;
float frac = 0;
int j = 0;

int processChorus(int value)
{
	POT0 = map(POT0 >> 2, 0, 1024, 1, 60); //empirical adjusts
	Delay2 = POT0 / 2;

	for (int u = 0; u <= POT0; u++)
		DELAY[POT0 + 1 - u] = DELAY[POT0 - u];

	DELAY[0] = value;

	POT1 = map(POT1 >> 2, 0, 1024, 1, 16); // empirical adjusts

	delay_sr = Delay2 - Delay2 * sinf_8k[(j * POT1) / 20];
	delay_int = int(delay_sr);
	frac = delay_sr - delay_int;

	frac = betweenF(frac, 0.01, 0.99);

	j++;
	if (j * POT1 / 20 >= sine_samples)
		j = 0;

	return (DELAY[delay_int + chorusOffset] * frac + DELAY[delay_int] * (1 - frac));
}
//freqshift
#define maxFreqShift 500
int freqShiftArray[maxFreqShift];
unsigned int selectedFreqShift = 0;
unsigned int freqShiftCounter = 0;

int processFreqShift(int value, int _shift)
{
    if (_shift < maxFreqShift / 100)
        return value;

    freqShiftArray[freqShiftCounter] = value;
    freqShiftCounter = (freqShiftCounter + 1) % _shift;
    return (freqShiftArray[freqShiftCounter] + value) >> 1;
}

//delay
#define maxDelay 15000
int delayArray[maxDelay];
unsigned int selectedDelay = 0;
unsigned int delayCounter = 0;

int processDelay(int value, int _delay)
{
    if (_delay < maxDelay / 100)
        return value;

    delayArray[delayCounter] = (value + delayArray[delayCounter]) >> 1;
    delayCounter = (delayCounter + 1) % _delay;
    return delayArray[delayCounter];
}

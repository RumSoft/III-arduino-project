
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

//chorus

#define MAX_DELAY 500
#define MIN_DELAY 200

uint16_t sDelayBuffer0[MAX_DELAY+500];
uint16_t sDelayBuffer1[MAX_DELAY+500];
unsigned int DelayCounter = 0;
unsigned int Delay_Depth = 300;
unsigned int count_up=1;
int p;

int processChorus(int value, int POT0, int POT1)
{
   //Store current readings  
  sDelayBuffer0[DelayCounter] = value;
 
  //Adjust Delay Depth based in pot0 position.
  POT0=map(POT0>>2,0,1024,1,25); //25 empirically chosen
 
  DelayCounter++;
  if(DelayCounter >= Delay_Depth) 
  {
    DelayCounter = 0; 
    if(count_up)
    {
       digitalWrite(LED_BUILTIN, HIGH);
       for(p=0;p<POT0+1;p++) 
       sDelayBuffer0[Delay_Depth+p]=sDelayBuffer0[Delay_Depth-1]; 
       Delay_Depth=Delay_Depth+POT0;
       if (Delay_Depth>=MAX_DELAY)count_up=0;
    }
    else
    {
       digitalWrite(LED_BUILTIN, LOW); 
       Delay_Depth=Delay_Depth-POT0;
       if (Delay_Depth<=MIN_DELAY)count_up=1;
    }
  }
 
  return sDelayBuffer0[DelayCounter] >> 5;
}
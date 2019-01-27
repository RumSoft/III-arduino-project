#include "header.h"
#include "variables.h"

int in_ADC0, in_ADC1;
int out_DAC0, out_DAC1;
int POT0, POT1, POT2, POT3;
int value;

//freqshift
#define maxFreqShift 500
int freqShiftArray[maxFreqShift];
unsigned int selectedFreqShift = 0;
unsigned int freqShiftCounter = 0;

//delay
#define maxDelay 15000
int delayArray[maxDelay];
unsigned int selectedDelay = 0;
unsigned int delayCounter = 0;


// #define maxChorusSpeed 30
// #define maxChorusDepth 80
// unsigned int selectedChorusSpeed = 0;
// unsigned int selectedChorusDepth = 0;

// #define MAX_DELAY 80
// #define chorusOffset 2
// int DELAY[MAX_DELAY + chorusOffset];
// int Delay2 = 0;
// float delay_sr = 0;
// int delay_int = 0;
// float frac = 0;
// int j = 0;

void setup()
{
	my_setup();
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(4800);
}

void updateValues()
{
  TC_GetStatus(TC1, 1); //DAC

  value = in_ADC0 - in_ADC1;

  selectedDelay = map(POT3 >> 5, 0, 128, 0, maxDelay);
  selectedFreqShift = map(POT2 >> 5, 0, 128, 0, maxFreqShift);
  // selectedChorusSpeed = map(POT1 >> 5, 0, 128, 0, maxChorusSpeed);
  // selectedChorusDepth = map(POT0 >> 5, 0, 128, 0, maxChorusDepth);
}

void loop()
{
	while ((ADC->ADC_ISR & 0x3CC0) != 0x3CC0)
		; // read & wait for ADC

	//read ADC
	in_ADC0 = ADC->ADC_CDR[7];
	in_ADC1 = ADC->ADC_CDR[6];

	//pots ADC8,9,10,11
	POT0 = ADC->ADC_CDR[10];
	POT1 = ADC->ADC_CDR[11];
	POT2 = ADC->ADC_CDR[12];
	POT3 = ADC->ADC_CDR[13];
}



int processFreqShift(int value, int _shift)
{
	if(_shift < maxFreqShift / 100)
		return value; 

	freqShiftArray[freqShiftCounter] = value;
	freqShiftCounter = (freqShiftCounter + 1) % _shift;
	return (freqShiftArray[freqShiftCounter] + value)>>1; 
}

int processDelay(int value, int _delay)
{
	if(_delay < maxDelay/100)
		return value;

	delayArray[delayCounter] = (value + delayArray[delayCounter]) >> 1;
	delayCounter = (delayCounter + 1) % _delay;
	return delayArray[delayCounter];
}

// int processFlanger(int value, int depth, int speed)
// {
// 	Delay2 = depth/2;

// 	for (int u = 0; u <= depth; u++)
// 		DELAY[depth + 1 - u] = DELAY[depth - u];

// 	DELAY[0] = value;

// 	POT1 = map(POT1 >> 2, 0, 1024, 1, 30); // empirical adjusts

// 	delay_sr = Delay2 - Delay2 * sinf_8k[(j * POT1) / 20];
// 	frac = betweenF(delay_sr - int(delay_sr), 0.01, 0.99);

// 	j++;
// 	if (j * POT1 / 20 >= sine_samples)
// 		j = 0;

// 	return (DELAY[int(delay_sr) + chorusOffset] * frac + DELAY[int(delay_sr)] * (1 - frac));
// }

void TC4_Handler()
{

	updateValues();

	//value = processFlanger(value, selectedChorusDepth, selectedChorusSpeed);
	value = processFreqShift(value, selectedFreqShift);
	value = processDelay(value, selectedDelay);

	writeOutput(value);
}

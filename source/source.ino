#include "header.h"
#include "variables.h"
#include "effects.h"

int in_ADC0, in_ADC1;
int out_DAC0, out_DAC1;
int POT0, POT1, POT2, POT3;
int value;




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
  selectedFreqShift = map(POT2 >> 3, 0, 512, 0, maxFreqShift);
  selectedChorusSpeed = map(POT1 >> 5, 0, 128, 0, maxChorusSpeed);
  selectedChorusDepth = map(POT0 >> 5, 0, 128, 0, maxChorusDepth);
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




int processFlanger(int value)
{
	POT0 = map(POT0 >> 2, 0, 1024, 1, 45); //empirical adjusts
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

void TC4_Handler()
{

	updateValues();

	//value = processFlanger(value);
	value = processFreqShift(value, selectedFreqShift);
	value = processDelay(value, selectedDelay);

	writeOutput(value);
}

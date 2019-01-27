#include "header.h"
#include "variables.h"
#include "effects.h"

int in_ADC0, in_ADC1;
int out_DAC0, out_DAC1;
int POT0, POT1, POT2, POT3;
int value;

void setup()
{
	my_setup();
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(4800);
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

void updateValues()
{
  TC_GetStatus(TC1, 1); //DAC

  value = in_ADC0 - in_ADC1;

  selectedDelay = map(POT3 >> 5, 0, 128, 0, maxDelay);
  selectedFreqShift = map(POT2 >> 3, 0, 512, 0, maxFreqShift);
  selectedChorusSpeed = map(POT1 >> 5, 0, 128, 0, maxChorusSpeed);
  selectedChorusDepth = map(POT0 >> 5, 0, 128, 0, maxChorusDepth);
}

void TC4_Handler()
{

	updateValues();

	value = processFreqShift(value, selectedFreqShift);
	value = processDelay(value, selectedDelay);

	writeOutput(value);
}

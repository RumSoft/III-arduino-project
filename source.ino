int in_ADC0, in_ADC1;
int out_DAC0, out_DAC1;

int POT0, POT1, POT2, POT3;
int LED = 13;
int FOOTSWITCH = 7;
int TOGGLE = 2;

int value;

#define maxFreqShift 500
int freqShiftArray[maxFreqShift];
unsigned int selectedFreqShift = 0;
unsigned int freqShiftCounter = 0;

#define maxDelay 20000
int delayArray[maxDelay];
unsigned int selectedDelay = 0;
unsigned int delayCounter = 0;

//c == 0? if value > 0 then return value
//c == 1? if value < 0 then return -value
inline int separateValue(int val, int c)
{
	return min(4090, c == 0
						 ? val > 0 ? val : 0
						 : val < 0 ? -val : 0);
}

void setup()
{
	/* turn on the timer clock in the power management controller */
	pmc_set_writeprotect(false);
	pmc_enable_periph_clk(ID_TC4);

	/* we want wavesel 01 with RC */
	TC_Configure(/* clock */ TC1, /* channel */ 1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK2);
	TC_SetRC(TC1, 1, 238); // sets <> 44.1 Khz interrupt rate
	//TC_SetRC(TC1, 1, 109); // sets <>   96 Khz interrupt rate

	TC_Start(TC1, 1);

	// enable timer interrupts on the timer
	TC1->TC_CHANNEL[1].TC_IER = TC_IER_CPCS;
	TC1->TC_CHANNEL[1].TC_IDR = ~TC_IER_CPCS;

	/* Enable the interrupt in the nested vector interrupt controller */
	/* TC4_IRQn where 4 is the timer number * timer channels (3) + the channel number
	(=(1*3)+1) for timer1 channel1 */
	NVIC_EnableIRQ(TC4_IRQn);

	//ADC Configuration
	ADC->ADC_MR |= 0x80;	// DAC in free running mode.
	ADC->ADC_CR = 2;		// Starts ADC conversion.
	ADC->ADC_CHER = 0x1CC0; // Enable ADC channels 0 and 1.

	//enable DAC
	analogWrite(DAC0, 0);
	analogWrite(DAC1, 0);
}

void loop()
{
	while ((ADC->ADC_ISR & 0x1CC0) != 0x1CC0)
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

int processFreqShift(int value)
{
	selectedFreqShift = map(POT1 >> 5, 0, 128, 0, maxFreqShift);
	if(selectedFreqShift <= 0)
		return value;
	freqShiftArray[freqShiftCounter] =  value;
	freqShiftCounter = (freqShiftCounter + 1) % selectedFreqShift;
	return freqShiftArray[freqShiftCounter];
}

int processDelay(int value)
{
	selectedDelay = map(POT0 >> 5, 0, 128, 1000, maxDelay);
	delayArray[delayCounter] = (value + delayArray[delayCounter]) >> 1;
	delayCounter = (delayCounter + 1) % selectedDelay;
	return delayArray[delayCounter];
}

void TC4_Handler()
{
	// We need to get the status to clear it and allow the interrupt to fire again
	TC_GetStatus(TC1, 1);

	value = in_ADC0 - in_ADC1;



	// value = processChorus(value);
	value = processFreqShift(value);
	//value = processDelay(value);

	//Write the DACs
	dacc_set_channel_selection(DACC_INTERFACE, 0);						 //select DAC channel 0
	dacc_write_conversion_data(DACC_INTERFACE, separateValue(value, 1)); //write on DAC
	dacc_set_channel_selection(DACC_INTERFACE, 1);						 //select DAC channel 1
	dacc_write_conversion_data(DACC_INTERFACE, separateValue(value, 0)); //write on DAC
}
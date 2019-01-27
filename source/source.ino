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


#define MAX_DELAY 80
#define no_samples 8000 
int   DELAY[MAX_DELAY+2];+
int   Delay2 = 0;
float delay_sr = 0;
int   delay_int = 0;
float frac = 0;
float Seno_table[no_samples];
int n,j=0;
 
 
void cria_seno() 
{
  for( n=0; n<no_samples; n++) 
    Seno_table[n] = (0.99 * cos(((2.0*PI)/no_samples)*n));
}



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
	 cria_seno();
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

	pinMode(LED_BUILTIN, OUTPUT);
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

int processFreqShift(int value, int shift)
{
	if ((POT1 >> 5) <= 1)
		return value;

	freqShiftArray[freqShiftCounter] = value;
	freqShiftCounter = (freqShiftCounter + 1) % shift;
	return freqShiftArray[freqShiftCounter];
}

int processDelay(int value, int delay)
{
	if ((POT3 >> 5) <= 1)
		return value;

	delayArray[delayCounter] = (value + delayArray[delayCounter]) >> 1;
	delayCounter = (delayCounter + 1) % delay;
	return delayArray[delayCounter];
}

inline int between(int val, int a, int b)
{
	if (val < a)
		return a;
	if (val > b)
		return b;
	return val;
}

inline float betweenF(float val, float a, float b)
{
	if (val < a)
		return a;
	if (val > b)
		return b;
	return val;
}

int processFlanger(int value)
{
	POT0 = map(POT0 >> 2, 0, 1024, 1, 45); //empirical adjusts
	Delay2 = POT0 / 2;

	for (int u = 0; u <= POT0; u++)
		DELAY[POT0 + 1 - u] = DELAY[POT0 - u];

	DELAY[0] = value;

	POT1 = map(POT1 >> 2, 0, 1024, 1, 16); // empirical adjusts

	delay_sr = Delay2 - Delay2 * Seno_table[(j * POT1) / 20];
	delay_int = int(delay_sr);
	frac = delay_sr - delay_int;

	frac = betweenF(frac, 0.01, 0.99);

	j++;
	if (j * POT1 / 20 >= no_samples)
		j = 0;

	return (DELAY[delay_int + 2] * frac + DELAY[delay_int] * (1 - frac));
}

void updatePots()
{
	selectedDelay = map(POT2 >> 5, 0, 128, 0, maxDelay);
	selectedFreqShift = map(POT0 >> 5, 0, 128, 0, maxFreqShift);
}

void writeOutput(int value)
{
	dacc_set_channel_selection(DACC_INTERFACE, 0);
	dacc_write_conversion_data(DACC_INTERFACE, separateValue(value, 1));
	dacc_set_channel_selection(DACC_INTERFACE, 1);
	dacc_write_conversion_data(DACC_INTERFACE, separateValue(value, 0));
}

void TC4_Handler()
{
	// We need to get the status to clear it and allow the interrupt to fire again
	TC_GetStatus(TC1, 1);
	value = in_ADC0 - in_ADC1;

	updatePots();

	value = processFlanger(value);
	//value = processFreqShift(value, selectedFreqShift);
	//value = processDelay(value, selectedDelay);

	writeOutput(value);
}
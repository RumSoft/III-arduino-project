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

//c == 0? if value > 0 then return value
//c == 1? if value < 0 then return -value
inline int separateValue(int val, int c)
{
	return min(4090, c == 0
						 ? val > 0 ? val : 0
						 : val < 0 ? -val : 0);
}

void writeOutput(int value)
{
	dacc_set_channel_selection(DACC_INTERFACE, 0);
	dacc_write_conversion_data(DACC_INTERFACE, separateValue(value, 1));
	dacc_set_channel_selection(DACC_INTERFACE, 1);
	dacc_write_conversion_data(DACC_INTERFACE, separateValue(value, 0));
}

void my_setup() {
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
	ADC->ADC_CHER = 0x3CC0; // Enable ADC channels 0 and 1.

	//enable DAC
	analogWrite(DAC0, 0);
	analogWrite(DAC1, 0);

}
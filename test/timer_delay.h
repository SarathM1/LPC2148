int tmrF = 1;

void timer0_isr()__irq
{
	if(T0IR == 0X01)
	{
		tmrF = 0;
		T0IR = (1<<0);
		VICVectAddr = 0;
	}
}

void timer_init()
{
	T0TCR = (1<<1); 		// Disable timer0, reset when enabled
	T0MR0  = 30000; // T0MR0 = (Req.Time(in sec) * PCLK)
						// Here PCLK = 30* 10^6
	T0MCR = (1<<0)|(1<<1);  // inter on match, reset MR0 on match
	
	VICIntEnable = (1<<4); // Enable timer 0 interrupt
	VICVectCntl0 = (1<<5)| 4 ; // Timer0 interrupt in slot 0
	VICVectAddr0 = 	(unsigned)timer0_isr;
}

void delay_ms(int x)
{
	for(;x>0;x--)
	{
		while(tmrF);
		tmrF = 1;
	}
}

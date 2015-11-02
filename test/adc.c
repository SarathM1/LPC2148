#include<lpc214x.h>
#include "timer_delay.h"
#include "uart0_rx_interrupt.h"
void adc_init()
{
	PINSEL1 |= (1<<24);		// P0.28 as AD0.1
	AD0CR = (1<<1)|(1<<21)|(1<<8)|(1<<9);
}

int main()
{
	int val;
//	int chn;
	adc_init();
	uart_init();
	timer_init();
	while(1)
	{
		AD0CR |= (1<<24);
		while(!(AD0GDR & (1<<31)));
		//chn = (AD0GDR & ((1<<24)|(1<<25)|(1<<26)))>>24;
		val = (AD0DR0 & 0xffc0) >> 6;
		uart_tx_int(val);
//		uart_tx_char(' ');
//	   	uart_tx_int(chn);
		uart_tx_char('\r');
		delay_ms(500);	
	}
}

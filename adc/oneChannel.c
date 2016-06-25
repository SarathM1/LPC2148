#include<lpc214x.h>
#include "timer_delay.h"
#include "uart0_rx_interrupt.h"
void adc_init()
{
	PINSEL1 |= (1<<24); 		// P0.28 as  AD0.1
	AD0CR = (1<<1)|(1<<21)|(1<<8)|(1<<9); // Select AD0.1; PDN = 1; 
											//CLKDIV = 3 (to generate ADC clk = 4.5MHz)
}

int main()
{
	int val;
	adc_init();
	uart_init();
	timer_init();
	while(1)
	{
		AD0CR |= (1<<24);			  // Start ADC
		while(!(AD0GDR & (1<<31))) ;  // Waiting for adc conversion to finish
		val = (AD0DR1 & 0xFFC0) >> 6;  // Result in bits 15:6
		uart_tx_int(val);
		uart_tx_char('\r');
		delay_ms(100);				  // To avoid PC from hanging
	}
}

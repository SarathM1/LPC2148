#include<lpc214x.h>
#include "timer_delay.h"
#include "uart0_rx_interrupt.h"
void adc_init()
{
	PINSEL1 |= (1<<24)|(1<<26)|(1<<28); 		// P0.28,P0.29,P0.30 as  AD0.1,AD0.2,AD0.3
}

void adc_read(char chn)
{
	AD0CR = (1<<chn)|(1<<21)|(1<<8)|(1<<9);		// Selct channel ;PDN = 1; CLKDIV = 3 (to make ADC clk = 4.5MHz); 
	AD0CR |= (1<<24);			  // Start ADC
	while(!(AD0STAT & (1<<chn)));
}

int main()
{
	int val;
	adc_init();
	uart_init();
	timer_init();
	while(1)
	{
		adc_read(1);
		val = (AD0DR1 & 0xFFC0) >> 6;  // Result in bits 15:6
		val = val * 5;
		uart_tx_int(val);				// Scaling Value
		uart_tx_char(' ');

		adc_read(2);
		val = (AD0DR2 & 0xFFC0) >> 6;  // Result in bits 15:6
		val = val * 5;					// Scaling Value
		uart_tx_int(val);
		uart_tx_char(' ');

		adc_read(3);
		val = (AD0DR3 & 0xFFC0) >> 6;  // Result in bits 15:6
		val = val * 5;				   // Scaling Value
		uart_tx_int(val);
		uart_tx_char('\r');
		delay_ms(500);				  // To avoid PC from hanging
	}
}

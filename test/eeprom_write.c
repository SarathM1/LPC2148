#include<lpc214x.h>
#include "uart0_rx_interrupt.h"

#define AA (1<<2)
#define STO (1<<4)
#define STA (1<<5)
#define I2EN (1<<6)
#define STAC (1<<5)
#define SIC (1<<3)

int res;
char flag = 0;
char done = 1;
char str[] = "wirewordsAcademy";

void i2c_isr()__irq
{
	char status;
	status = I2C0STAT;
	uart_tx_int(status);
	uart_tx_char('-');
	switch(status)
	{
		case 8:	// START condtn transmitted
			I2C0DAT = 0xA0;
			I2C0CONCLR = SIC|STAC;
			uart_tx_str("START transmitted\r\n");
			break;
		case 32: // SA + W transmitted, NOT ACK received
			I2C0CONSET |= STO;
			I2C0CONCLR = SIC;
			uart_tx_str("SLA+W transmitted, NOT ACK received\r\n");
			break;		
		case 24: // SA + W transmitted, ACK received
			I2C0DAT = 0X00;
			I2C0CONCLR = SIC;
			uart_tx_str("SLA+W transmitted, ACK received\r\n");	
			break;
		case 40: //Data byte in I2C0DAT transmitted, ACK received
			if(flag==0)
			{
				uart_tx_str("Word Address 0x00 transmitted, ACK received\r\n");
				I2C0DAT = 0X00;
				I2C0CONCLR = SIC;
				uart_tx_str("Word Address 0x00 transmitted, ACK received\r\n");	
			}
			else if(flag<=10)
			{
				I2C0DAT = str[flag-1];	  // rtcInit is one ahead
				uart_tx_str("Writing to eeprom, char = ");
				uart_tx_char(I2C0DAT);
				I2C0CONCLR = SIC;
				uart_tx_str("\r\n");					
			}
			else 
			{
				I2C0CONSET |= STO;
				I2C0CONCLR = SIC;
				uart_tx_str("Data byte transmitted, ACK received, Stopping\r\n");
				uart_tx_str("-------------------------------------------------\r\n");
				done = 0;
				flag = 0;
			}
			flag++;
			break;
		default:
			while(1)			// Debugging
			{
				uart_tx_str("while\r\n");
			}
	}

	VICVectAddr = 0;
}

void i2c_write(char data)
{
	I2C0DAT = data;
	I2C0CONCLR = SIC;
}

void i2c_init()
{
	PINSEL0 |= ((1<<4)|(1<<6));
	I2C0CONSET	=  I2EN;
	I2C0CONCLR = SIC;
	I2C0SCLL = 180;		// i2C rate = PCLK/(SCLL + SCLH)
	I2C0SCLH = 0;

	VICIntEnable |= (1<<9);	 // Enable i2c0 interrupt
	VICVectCntl0 = (1<<5)|9; //	Assign i2c0 interr slot 0; enable slot 0
	VICVectAddr0 = 	(unsigned)i2c_isr;
}



int main()
{
	i2c_init();
	uart_init();
	while(1)
	{
		I2C0CONCLR = 0XFF;			// Clearin I2C0CONSET
		I2C0CONSET	=  I2EN|AA;		// Enable I2C and set AA
		I2C0CONSET	|=	STA;		// i2c start
		while(done);
		done = 1;
	}
}

#include<lpc214x.h>
#include "uart0_rx_interrupt.h"

#define AA (1<<2)
#define STO (1<<4)
#define STA (1<<5)
#define I2EN (1<<6)
#define STAC (1<<5)
#define SIC (1<<3)
#define SA 0x68		  //Slave address

char flag= 'w';

void i2c_start (void)
{
	I2C0CONCLR	=	STO;
	I2C0CONSET	|=	STA;
}

void i2c_isr()__irq
{
	char status;
	status = I2C0STAT;
	uart_tx_int(status);
	uart_tx_char('-');
	switch(status)
	{
		case 0x08:	// START condtn transmitted
			if(flag == 'w')
			{
				I2C0DAT = 0xD0;
				I2C0CONCLR = SIC|STAC;
				uart_tx_str("START transmitted\r\n");
			}
			else
			{
				I2C0DAT = 0xD1;
				I2C0CONCLR = SIC|STAC;
				uart_tx_str("START transmitted\r\n");
			}
			break;
		case 0x20: // SA + W transmitted, NOT ACK received
			I2C0CONSET |= STO;
			I2C0CONCLR = SIC;
			uart_tx_str("SLA+W transmitted, NOT ACK received\r\n");
			break;		
		case 0x18: // SA + W transmitted, ACK received
			I2C0DAT = 0X00;
			I2C0CONCLR = SIC;
			uart_tx_str("SLA+W transmitted, ACK received\r\n");
			break;
		case 0x28: //Data byte in I2C0DAT transmitted, ACK received
			I2C0CONSET |= STO;
			I2C0CONCLR = SIC;
			uart_tx_str("data byte transmitted, ACK received\r\n");
			flag = 0;
			break;
		case 0x40: // SLA + R transmitted, ACK recieved
			uart_tx_str("SLA + R transmitted, ACK recieved\r\n");
			I2C0CONSET |= AA; 	  // To acknowledge Data received
			uart_tx_str("Data = ");
			uart_tx_int(I2C0DAT);
			uart_tx_str("\r\n");
			I2C0CONCLR = SIC;
			break;
		case 0x50: // Data byte has been received ACK returned
			I2C0CONSET	|= AA;
			uart_tx_str("Data byte has been received ACK returned\r\n");
			uart_tx_str("Data = ");
			uart_tx_int(I2C0DAT);
			uart_tx_str("\r\n");
			I2C0CONCLR = SIC;
			break;
		default:
			while(1)			// Debugging
			{
				uart_tx_str("while\r\n");
			}
	}

	VICVectAddr = 0;
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

void rtc_read()
{
	flag = 'w';
	i2c_start();
	while(flag == 'w');
	i2c_start();
}

int main()
{
	i2c_init();
	uart_init();
	uart_tx_str("Starting Program\r\n");
	i2c_start();
	while(flag);
	i2c_start();
	while(1);
}

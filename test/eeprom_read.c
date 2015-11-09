#include<lpc214x.h>
#include "uart0_rx_interrupt.h"

#define AA (1<<2)
#define STO (1<<4)
#define STA (1<<5)
#define I2EN (1<<6)
#define STAC (1<<5)
#define SIC (1<<3)

int cnt = 0;
int res;
int rtcInitFlag = 0;




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
			if(rtcInitFlag == 0)
			{
				uart_tx_str("Word Address 0x00 transmitted, ACK received\r\n");
				I2C0DAT = 0X00;
				I2C0CONCLR = SIC;
				rtcInitFlag = 1;
			}
			else
			{
				uart_tx_str("Word Address 0x00 transmitted, ACK received\r\n");
				I2C0CONSET	|=	STA;
				I2C0CONCLR = SIC;
			}
			break;
		case 16:
			I2C0DAT = 0xA1;
			I2C0CONCLR = SIC|STAC;
			uart_tx_str("REPEATED START transmitted\r\n");
			break;
		case 64: // SLA + R transmitted, ACK recieved
			uart_tx_str("SLA + R transmitted, ACK recieved\r\n");
			I2C0CONCLR = SIC;
			break;
		case 80: // Data byte has been received ACK returned
			if(cnt < 10)
			{
				cnt++;
				I2C0CONSET	|= AA;
				uart_tx_str("Data = ");
				res = I2C0DAT;
				uart_tx_char(res);
				uart_tx_str("\r\n");
				uart_tx_str("Data byte has been received ACK returned\r\n");
				I2C0CONCLR = SIC;
			}
			else
			{
				I2C0CONSET |= STO;
				I2C0CONCLR = SIC;
				uart_tx_str("Stopping\r\n");
			}
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
	I2C0CONSET	=  I2EN|AA;
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
	I2C0CONCLR = 0XFF;			// Clearin I2C0CONSET
	I2C0CONSET	=  I2EN|AA;		// Enable I2C and set AA
	I2C0CONSET	|=	STA;		// i2c start
	while(1);
}

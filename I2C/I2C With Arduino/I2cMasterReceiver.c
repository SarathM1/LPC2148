/******************************************************************************************************************
-> I2c Communication between LPC2148 and Arduino Uno.
-> LPC2148 in Master Reciver mode and ARduino in Slave Transmitter mode
-> Connection :-
 	LPC2148		Arduino
	GND()			GND
	SDA(P0.3)		SDA(A4)
	SCL(P0.2)		SCL(A5)
-> Pull-up SDA and SCL connection to 3.3 V
-> Assign 7-bit slave address 112 to Arduino (command: wire.begin(112))
-> Use 0xE1 for Slave Address + Read operation (7 bit address + Read bit)
******************************************************************************************************************/

#include<lpc214x.h>
#include "uart0_rx_interrupt.h"

#define AA (1<<2)
#define STO (1<<4)
#define STA (1<<5)
#define I2EN (1<<6)
#define STAC (1<<5)
#define SIC (1<<3)
#define AAC (1<<2)

int cnt = 0;
int res;
int flag = 0;
char done = 1;


void i2c_isr()__irq
{
	char status;
	status = I2C0STAT;
	uart_tx_int(status);
	uart_tx_char('-');
	switch(status)
	{
		case 8:	// 0x08: START condtn transmitted
			I2C0DAT = 0xE1;
			I2C0CONCLR = SIC|STAC;
			uart_tx_str("START transmitted\r\n");
			break;

		case 64: // 0x40: SLA + R transmitted, ACK recieved
			uart_tx_str("SLA + R transmitted, ACK recieved\r\n");
			I2C0CONCLR = SIC;
			break;
		case 80: // 0x50: Data byte has been received ACK returned
			if(cnt <= 4 )
			{
				cnt++;
				I2C0CONSET	|= AA;
				uart_tx_str("Data = ");
				uart_tx_char(I2C0DAT);
				uart_tx_str("\r\n");
				uart_tx_str("Data byte has been received ACK returned\r\n");
				I2C0CONCLR = SIC;
			}
			else
			{
				cnt++;
				I2C0CONCLR = AAC;
				uart_tx_str("Data = ");
				uart_tx_char(I2C0DAT);
				uart_tx_str("\r\n");
				uart_tx_str("Data byte has been received ACK returned\r\n");
				I2C0CONCLR = SIC;
			}
			break;
		case 88: // 0x58: Data byte has been received, NOT ACK returned
			I2C0CONSET |= STO;
			I2C0CONCLR = SIC|AAC;
			uart_tx_str("Data byte has been received, NOT ACK returned\r\n");
			uart_tx_str("Stopping . . \r\n");
			uart_tx_str("-------------------------------------------------\r\n");
			done = 0;
			cnt = 0	; 		// Resetting all flags
			break;
		default:
			uart_tx_str("Bug\r\n");	// Debugging
			while(1);
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
	uart_tx_str("Starting program!!\r\n");
	while(1)
	{
		I2C0CONCLR = 0XFF;			// Clearin I2C0CONSET
		I2C0CONSET	=  I2EN|AA;		// Enable I2C and set AA
		I2C0CONSET	|=	STA;		// i2c start
		while(done);
		done=1;
	}
}

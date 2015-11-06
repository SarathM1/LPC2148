/***********************************************************************************************
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||;
;|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	Important Note ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|;
;|~																							~|;
;|~		This part of the software coding is the sole property of the company!				~|;
;|~		Any unauthorized re-usage or transformation of this code is prohibited.				~|;
;|~																							~|;
;|~				© Pantech Solutions Pvt Ltd.  All rights Reserved							~|;
;|~																							~|;
;|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|;
;|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|;
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||;
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||;
;|	Title		: I2C_RTC_Interface.c												     	 |;
;|	Status		: Completed															 		 |;
;|	Features	: Program to RTC interface using I2C						 				 |;
;|	Description	: I2C Used    :  P0.11 and P0.14 (I2C1 of ARM7TDMI)							 |;
;|				                                                 		         			 |;
;|																							 |;
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||;
***********************************************************************************************/

/************************************   Directives   *******************************************/

#include <LPC214x.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "UART.h"

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/************************************    Macros     *******************************************/
#define MAX		8
#define	AA		2
#define	SI		3
#define	STO		4
#define	STA		5
#define	I2EN	6

#define ESC 0x1B

#define SW3		1<<24
#define SW4		1<<25
#define SW5		1<<26
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   Declarations  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void I2C_ISR(void)__irq;
void UART0_ISR (void)__irq;
void Wait (unsigned int);
void I2C_Init (void);

int I2C_Start (void);
int I2C_Write (unsigned char *Buff, unsigned int Count);

char Buff[MAX]			=	{0x59/*CH + 10Sec + Sec*/,0x58/*Minutes*/,0x64/*0+ 12/24 + AM/PM + 10Hr + Hr*/,
							 0x04/*Wednesday*/, 0x04 /*Date*/, 0x11 /*Month-June*/, 0x09 /*Year*/
							};

unsigned char Day[7][10]=	{"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
unsigned char Rec[MAX]	=	{"NO-DATA"};
unsigned char index		=	0;
unsigned char flag = 0, Ready=0;
int ii=0;
char Key;

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void Delay(void)
{
	unsigned int i,j;
	
	for(i=0;i<150;i++)
		for(j=0;j<900;j++);
}

void Wait (unsigned int Delay)
{
	while(Delay--);
}

void I2C_Init (void)
{
	I2C1SCLH	=	150;			//50%duty cycle .....   I2C Frequency ->100 KHz for PCLK = 30MHz 
	I2C1SCLL	=	150;
	I2C1CONSET	=	1 << I2EN;	//Enable I2C 0 

}

int I2C_Start (void)
{
	I2C1CONCLR	=	1 << STO;
	I2C1CONSET	=	1 << AA;

	I2C1CONSET	=	1 << STA;

	return 0;
}

int I2C_Write (unsigned char *Buff, unsigned int Count)
{
	while(Count--)
	{
		I2C1DAT		=	*Buff++;
	}
	return 0;
}

void RTC_Setup(char *Buff)
{

	unsigned char TimE;
	char i=0;

	for(i=0;i<2;i++)
	{
		while(!isdigit(Key));					//Wait till Key = 0 to 9
		if (i==0)
		{
			TimE	=	Key - '0';
			TimE 	<<=	4;
		}
		if (i==1)
		{
			TimE	|=	Key - '0';
		}
		//putchar(Key);
		Key 	=	0;

	}

	*Buff	=	TimE;							//Load Setup New Value
}

unsigned char BCD2HEX (unsigned char BCD)
{
	unsigned char HEX;
	HEX	=	(BCD >> 4)*10 | (BCD & 0x0F);
	return HEX;
}

//------------------------------
//	Serial Port Initialize
//------------------------------
void serial_init(void)
{
  PINSEL0 |= 0x00000005;           /* Enable RxD0 and TxD0                	*/
  U0LCR = 0x83;                   /* 8 bits, no Parity, 1 Stop bit          */
  U0DLL = 195;                     /* 9600 Baud Rate @ 12MHz VPB Clock       */
  U0LCR = 0x03;                   /* DLAB = 0        						*/
}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   Main Function  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


int main()
{
	unsigned char i, AP;
	VPBDIV	=	0x02;
	PINSEL0	=	0x00000005;			// P0.3 - SDA0 and P0.2	- SCL0 
	PINSEL0	|=	0x30C00000;
	
	IODIR1	=	0x00 << SW3;

	serial_init ();

	U0IER	=	3;

	VICIntSelect	= 0<<6;						//UART0 ('0' - irq   '1'-fiq)
	VICVectCntl6	= 0x020 | 6;				//VIC slot enabled
	VICVectAddr6	= (unsigned long)UART0_ISR;//pass address of UART0
	VICIntEnable	= 1 << 6;				//Enable  UART0 Interrupt

	VICIntSelect	= 0<<19;
	VICVectCntl0 	= 0x020 | 19 ;
	VICVectAddr0 	= (unsigned long)I2C_ISR;
	VICIntEnable 	= 1<<19;

	/*	Before the master transmitter mode
		can be entered, the I2CONSET register must be initialized
	*/
	printf("HI");

	I2C_Init();

	UART0_PutS ("\n\r********* ARM Tyro LPC2148 I2C RTC Demo **********\n\n\r");
	UART0_PutS ("\r-------------------------------------------\n");
	UART0_PutS("\n\rVersion Release v1.0 25/05/09\n");
	UART0_PutS("\rResearch & Development Divison\n");	
	UART0_PutS("\r(c) Pantech Solutions Pvt Ltd.,\nwww.pantechsolutions.net\n");
	UART0_PutS("\rChennai - India\n");
	UART0_PutS ("\n\rDS1307 Ext RTC\n\n\r");
		//UART0_PutS (">	 Turn SW3 ON and Check Hyperteminal to Set Time...\n\r");
	UART0_PutS ("\n\r___________________________________________________\n\n\r");	
	Wait (5000);			
	flag	=	'W';
	I2C_Start ();

	for (i=0;i<30;i++)	Wait(10000);

	I2C1CONCLR	=	1 << SI;
	printf ("\n\n\rTime");
	printf ("\n\r------\n\n\r");
	Delay();
	while (1)
	{

			Wait (5000);Wait (5000);
			for (i=0;i<250;i++)	Wait(10000);
		   	if (Ready == 'T')
			{
				AP = (Rec[3] & 0x40)?'P':'A';				
				ii = 1;
				Delay();
				printf ("%02x : %02x : %02x %cM | ",(Rec[3] & 0x1F), Rec[2], Rec[1], AP );
				Delay();Delay();
				printf ("%s, %02x / %02x / %02x \r", Day[Rec[4]-1], Rec[5], Rec[6], Rec[7]);
				Delay();Delay();
				Ready = 'F';
			}
		}
	

	

}


void I2C_ISR(void) __irq
{
	if (I2C1CONSET & 0x08)
	{
		

		switch (I2C1STAT)
		{
			case (0x08)	:/*... A START condition has been Transmitted...*/	

							if (flag == 'W')
							{
								I2C1CONCLR	=	1 << STO;
								I2C1CONCLR	=	1 << STA;	//Clear START Bit
								I2C1CONSET	=	1 << AA;
								I2C1DAT		=	0xD0;		//Slave Addr + W  1010 p2 p1 p0 w
								I2C1CONCLR	=	1 << SI;

							//	U0THR	=	'S';
								
							}
							else if (flag == 'R')
							{
							
								I2C1DAT		=	0xD0;		//Slave Addr + R  1010 p2 p1 p0 r	
								I2C1CONSET	=	1 << STA;
								I2C1CONCLR	=	1 << SI;
								
								
							//	U0THR	=	's';
							}
							index	=	0;
							break;
	
			case (0x10)	:	/*...A repeated START condition has been Transmitted...*/

							I2C1CONCLR	=	1 << STA;	
							if (flag == 'W')
							{
								I2C1DAT	=	0xD0;		//Slave Addr + W  1010 p2 p1 p0 w
							//	U0THR	=	'Q';
							}
							else if (flag == 'R')
							{
								//I2C1CONCLR	=	1 << STA;
								I2C1DAT		=	0xD1;		//Slave Addr + R  1010 p2 p1 p0 r	
								I2C1CONCLR	=	1 << STO;	
								I2C1CONSET	=	1 << AA;
								I2C1CONCLR	=	1 << SI;

								index = 0;
							//	U0THR	=	'q';
							}
							
							break;
	
			case (0x18)	:	/*....SLA+W has been transmitted; ACK has been received....*/

							if (flag == 'W')
							{
								I2C1DAT		=	0x00;
								index=0;
								I2C1CONCLR	=	1 << STA;		//Clear START Bit
								I2C1CONCLR	=	1 << STO;		//Clear all flags (STA, STO, SI)
							//	U0THR	=	'D';
							}
							else
							{
								I2C1DAT	=	0x00;
								index 	=	0;
								I2C1CONCLR	=	1 << STA;		//Clear START Bit
								I2C1CONCLR	=	1 << STO;		//Clear all flags (STA, STO, SI)
								I2C1CONSET	=	1 << AA;
							//	U0THR	=	'd';
							}
							I2C1CONCLR	=	1 << SI;
							break;
	
			case (0x20)	:	/*...SLA+W has been transmitted; NOT ACK has been received....*/

							if (flag == 'W')
							{
								I2C1DAT		&=	0x7F;
								index=0;
								I2C1CONCLR	=	1 << STA;		//Clear START Bit
								I2C1CONCLR	=	1 << STO;		//Clear all flags (STA, STO, SI)
							//	U0THR	=	'E';
							}
							else
							{
								I2C1DAT	=	0xD1;
								index 	=	0;
								I2C1CONCLR	=	1 << STA;		//Clear START Bit
								I2C1CONCLR	=	1 << STO;		//Clear all flags (STA, STO, SI)
								I2C1CONSET	=	1 << AA;
							//	U0THR	=	'e';
							}
							I2C1CONCLR	=	1 << SI;							
							break;
	
			case (0x28)	:	/*...Data byte in I2DAT has been transmitted; ACK has been received....*/

							if (index < MAX && flag == 'W')			// Place all the contents of Buff[] in I2C1DAT
							{
							
								I2C1DAT		=	Buff[index];
								I2C1CONCLR	=	0x20;		//Clear START Bit
								I2C1CONCLR	=	0x38;		//Clear all flags (STA, STO, SI)
								index++;
						//		U0THR	=	'F';
							//	U0THR	=	'W';
															
							}
						
							else if (flag == 'R')	//Generate a Repeated Start to jump to MST/REC Mode
							{
								I2C1CONCLR	=	1 << STO;
								I2C1CONSET	=	1 << STA;
								I2C1CONCLR	=	1 << SI;
							//	U0THR	=	'J';
									
							}
							
							else								//Stop Transmission to EEPROM.... Data Count Reset..
							{
								index = 0;
								flag = 'R';
								I2C1CONSET	=	1 << STO;		//Send STOP Bit
								I2C1CONCLR	=	1 << AA;
								//printf ("\n\r Data Successfully Written on Memory!\n");
								I2C1CONSET	=	1 << STA;
								I2C1CONCLR	=	1 << SI;
							//	U0THR	=	'P';
							}
							break;
	
			case (0x30)	:	/*... Data byte in I2DAT has been transmitted; NOT ACK has been received ...*/

						if (index < MAX && flag == 'W')			// Place all the contents of Buff[] in I2C1DAT
						{
							
							I2C1DAT		=	Buff[index];
							I2C1CONCLR	=	0x20;		//Clear START Bit
							I2C1CONCLR	=	0x38;		//Clear all flags (STA, STO, SI)
							index++;
						//	U0THR	=	'G';
						//	U0THR	=	'W';
														
						}
						
						else if (flag == 'R')	//Generate a Repeated Start to jump to MST/REC Mode
						{
							I2C1CONCLR	=	1 << STO;
							I2C1CONSET	=	1 << STA;
							I2C1CONCLR	=	1 << SI;
						//	U0THR	=	'j';
								
						}
						
						else								//Stop Transmission to EEPROM.... Data Count Reset..
						{
							index = 0;
							flag = 'R';
							I2C1CONSET	=	1 << STO;		//Send STOP Bit
							I2C1CONCLR	=	1 << AA;
							//printf ("\n\r Data Successfully Written on Memory!\n");
							I2C1CONSET	=	1 << STA;
							I2C1CONCLR	=	1 << SI;
						//	U0THR	=	'p';
						}
						break;
	
			case (0x38)	:	/*... Arbitration lost in SLA+R/W or Data bytes ....*/

							I2C1CONSET	=	0x20;
							break;

			case (0x40)	:	/*... SLA+R has been transmitted; ACK has been received.*/
							//index = 0;
							if (index < MAX)
							{
								Rec [index]	=	I2C1DAT;
								index++;
								I2C1CONSET	=	1 << AA;
								I2C1CONCLR	=	1 << STA;
								I2C1CONCLR	=	1 << STO;
							}
							else
							{
								index = 0;
								I2C1CONCLR	=	1 << AA;
								I2C1CONSET	=	1 << STO;
								I2C1CONCLR	=	1 << STA;
							}
						//	U0THR = 'y';
							I2C1CONCLR	=	1 << SI;
							break;

			case (0x48)	:	/*...SLA+R has been transmitted; NOT ACK has been received ...*/
						if (index < MAX)
							{
								Rec [index]	=	I2C1DAT;
								index++;
								I2C1CONSET	=	1 << AA;
								I2C1CONCLR	=	1 << STA;
								I2C1CONCLR	=	1 << STO;
							}
							else
							{
								index = 0;
								I2C1CONCLR	=	1 << AA;
								I2C1CONSET	=	1 << STO;
								I2C1CONCLR	=	1 << STA;
							}
							
							I2C1CONCLR	=	1 << SI;
						//	U0THR = 'z';
					
							break;

			case (0x50)	:	/*... Data byte has been received; ACK has been returned ....*/

							I2C1CONSET	=	1 << AA;	
							if (index < MAX)
							{
								Rec [index]	=	I2C1DAT;
								I2C1CONSET	=	1 << AA;
								I2C1CONCLR	=	1 << STA;
								I2C1CONCLR	=	1 << STO;
								index++;
							//	U0THR = 'x';
							
							}
							else
							{
								I2C1CONCLR	=	1 << STO;
								I2C1CONCLR	=	1 << AA;
								//I2C1CONSET	=	1 << STA;
								I2C1CONCLR	=	1 << SI;
								index = 0;
								Ready	=	'T';			//Make Ready TRUE after Reading is over!
							//	U0THR = 'X';

							}
							
							break;

			case (0x58)	:	/*... Data byte has been received; NOT ACK has been returned ....*/
			
							I2C1CONSET	=	1 << STO;
							I2C1CONSET	=	1 << STA;
							I2C1CONSET	=	1 << AA;
							I2C1CONCLR	=	1 << SI;
							flag = 'R';
							index = 0;
							Ready	=	'T';			//Make Ready TRUE after Reading is over!
							//printf ("\n\rReading Done!!\n\r");
							break;
							
		}
	}
	I2C1CONCLR	=	1 << SI;
	VICVectAddr	=	0x00;

}


void UART0_ISR (void)__irq
{
	
	char Msg;

	if(((Msg = U0IIR) & 0x01) == 0)					//Check Flag Status of Recieve Interrupt
	{
		switch	(Msg & 0x0E)						//Filter Msg
		{
			case 0x04:	while (!(U0LSR & 0x20));	//Recieve Data 
					   	Key = U0RBR;
						

			case 0x02:	break;						//THRE Interrupt

			default	 :	break;
		}
	}
	VICVectAddr = 6;

}

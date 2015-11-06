/**************************************************************************************************

	#######################################################################
	##																	 ##
	## 		Important : Not to be Disclosed to the Clients..			 ##
	## 		This is transferred into as an object File and...			 ##
	## 		is included only for the Company's Development Purpose...	 ##
	##												  					 ##
	##		© Pantech Solutions | All rights Reserved.					 ##
	##																	 ##
	#######################################################################

	UART0 and UART1 Configuration code
 	----------------------------------------
  	
	Description
	-----------
	Configures the UARTs of ARM LPC21xx

	Test configuration:
	-------------------
	Oscillator:      12.0 MHz (cclk = 60.0 MHz, Fcco = 240.0 MHz)

**************************************************************************************************/

#include <LPC213x.h>
#include "UART.h"


#define _PCLK 30000000  // Define PCLK for configuration baudrate



void DelayMS(void)
{
	unsigned int i,j;
	for(i=0;i<20;i++)
		for(j=0;j<300;j++);
}
void UART1_Init(unsigned int _baudrate)
{
    unsigned short u1dl;    
    u1dl = _PCLK/(16*_baudrate);    // Calculate for U1DL value
    PINSEL0 |= 0x00050000;	// Enable rx,tx
	U1LCR = 0x00000083;		// 8 bit data,1 stop bit,no parity bit
    U1DLL = u1dl & 0xFF;    // U1DL for low byte
    U1DLM = (u1dl>>8);      // U1DL for high byte
    U1LCR = 0x00000003;		// DLAB =0
}
//------------------------------------------------------------------------------------------------//
//---------------------------- Function for send character 1 time via UART1-----------------------//
//------------------------------------------------------------------------------------------------//
void UART1_PutC(char c)
{
	while(!(U1LSR & 0x20)); // Wait until UART1 ready to send character     
	U1THR = c; 
	DelayMS();				 // Send character
}
//------------------------------------------------------------------------------------------------//
//---------------------------- Function for send string via UART1---------------------------------//
//------------------------------------------------------------------------------------------------//
void UART1_PutS(char *p)
{
	while(*p) // Point to character
	{
		UART1_PutC(*p++);   // Send character then point to next character
	}
}
//------------------------------------------------------------------------------------------------//
//---------------------------- Function for Initial UART0 ----------------------------------------//
//------------------------------------------------------------------------------------------------//
void UART0_Init(unsigned int _baudrate)
{
	unsigned short u0dl;
    u0dl = _PCLK/(16*_baudrate); // Calculate for U0DL value
    PINSEL0 |= 0x00000005;	// Enable rx,tx
	U0LCR = 0x00000083;		// 8 bit data,1 stop bit,no parity bit
    U0DLL = u0dl & 0xFF;    // U0DL for low byte
    U0DLM = (u0dl>>8);      // U0DL for high byte
    U0LCR = 0x00000003;		// DLAB =0
}
//------------------------------------------------------------------------------------------------//
//---------------------------- Function for send character 1 time via UART0-----------------------//
//------------------------------------------------------------------------------------------------//
void UART0_PutC(char c)
{
	while(!(U0LSR & 0x20)); // Wait until UART0 ready to send character  
	U0THR = c; // Send character
	DelayMS();
}
//------------------------------------------------------------------------------------------------//
//---------------------------- Function for send string via UART1---------------------------------//
//------------------------------------------------------------------------------------------------//
void UART0_PutS(char *p)
{
	while(*p) // Point to character
	{
		UART0_PutC(*p++);  // Send character then point to next character
	}
}

#ifndef __UART_H

#define __UART_H

void UART1_Init(unsigned int);	/*UART1*///Call the Function with a Baud Rate
void UART1_PutC(char);			/*UART1*///Call this function to display a character
void UART1_PutS(char *);		/*UART1*///Call this function to display a string 

void UART0_Init(unsigned int);	/*UART0*///Call the Function with a Baud Rate
void UART0_PutC(char);			/*UART0*///Call this function to display a character
void UART0_PutS(char *);		/*UART0*///Call this function to display a string
void DelayMS (void);

#endif

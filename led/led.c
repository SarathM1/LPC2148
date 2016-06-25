#include<stdio.h>
#include"lpc21xx.h" // header file
#include<string.h>

void DELAY(void);

int main()
{

	IODIR0=0XFFFFFFFF;
	
	while(1)
	{
	
		IOPIN0=0XFFFFFFFF;
		DELAY();
		DELAY();
		
		IOPIN0=0X00000000;
		DELAY();
		DELAY();
	
	}
}


void DELAY(void)
{
	unsigned int x=1000000;
	while(x--);

}

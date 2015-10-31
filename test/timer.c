#include <lpc214x.h>
#include"timer_delay.h"
int main()
{
	timer_init();
	delay_ms(1);
	while(1);
}

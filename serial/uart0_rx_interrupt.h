void uart(void)__irq // ISR for UART0
{
	U0THR = U0RBR; 		// Echo what is recieved
	while(!(U0LSR & 0x20));	// Wait for tx to complete
	VICVectAddr = 0;
}

void uart_tx_char(char val)
{
	U0THR = val;
	while(!(U0LSR & 0x20));
}

void uart_tx_int(long int num)
{
	char str[10];
	int i,r,j;

	for(i=0;i<4;i++,num/=10)
	{
		r = num%10;
		str[i] = r + 48;
	}
	str[i]='\0';

	for(j=i-1;j>=0;j--)
		uart_tx_char(str[j]);

}

void uart_init()
{
	PINSEL0 |= 0x05; // Enable Txd0 on P0.0, Rxd0 on P0.1
	
	U0IER = 0x01; // Enable RX interrupt
	U0FCR = 0x01; //FIFO Enable
	
	U0LCR = 0x83;
	
	U0DLM = 0x00;
	U0DLL = 117;
	
	U0LCR = 0x03;

	VICIntEnable = 0x00000040; // Enable UART0 Interrupt
	VICIntSelect = 0x00000000; // Set UART0 interrupt as IRQ
	VICVectCntl0 = 0x26;	// Asign 6 for UART0 and set IRQslot_en
	VICVectAddr0 = (unsigned)uart;	 // Address for ISR
	uart_tx_char('k');
}

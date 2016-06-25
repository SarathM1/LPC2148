Configuring Keil :-
================

Before running the code refer the following link to setup Keil IDE: https://www.rhydolabz.com/wiki/?p=14658

Wihtout these settings the peripherals like UART and interrupts may not work properly

UART :-
=======

  To set the baudrate the easy way is to use debugger; set the DLAB bit and change *U0DLL*,  *DIVADDVAL* and *MULVAL* and correct 
  baudrate to approx 9600 by trial and error

/******************************************************************************/
/*  This file is part of the uVision/ARM development tools                    */
/*  Copyright KEIL ELEKTRONIK GmbH 2002-2004                                  */
/******************************************************************************/
/*                                                                            */
/*  SERIAL.C:  Low Level Serial Routines                                      */
/*                                                                            */
/******************************************************************************/

#include <LPC21xx.H>                     /* LPC21xx definitions               */

#define CR     0x0D


int putchar (int ch)  {                  /* Write character to Serial Port    */

  if (ch == '\n')  {
    while (!(U0LSR & 0x20));
    U0THR = CR;                          /* output CR */
  }
  while (!(U0LSR & 0x20));
  return (U0THR = ch);
}


int getchar (void)  {                    /* Read character from Serial Port   */

  while (!(U0LSR & 0x01));

  return (U0RBR);
}

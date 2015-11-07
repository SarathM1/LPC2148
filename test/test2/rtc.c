/*--------------------------------------------------------------------------------
                               8051 DS1307  library
Filename: DS1307.c
Controller: P89V51RD2/89c51(8051 family)
Oscillator: 11.0592 MHz
Author: XploreLabz
website: www.xplorelabz.com 
 
 ---------------------------------------------------------------------------------*/
 
#include "ds1307.h"
#include "i2c.h"
#include "delay.h"
 
 
 
/* Below values are fixed and should not be changed.
    Refer Ds1307 DataSheet for more info*/
 
#define DS1307_ID 0xD0     // DS1307 ID
 
#define SEC_ADDRESS   0x00 // Address to access Ds1307 SEC register
#define DATE_ADDRESS  0x04 // Address to access Ds1307 DATE register
#define CONTROL 0x07       // Address to access Ds1307 CONTROL register
 
 
 
 
 
 
/*---------------------------------------------------------------------------------
                         void DS1307_Init()
 ----------------------------------------------------------------------------------
 * I/P Arguments: none.
 * Return value : none
 
 * description :This function is used to initialize the Ds1307 RTC.
                Ds1307 ic is enabled by sending the DS1307 id on the I2C bus.
                After selecting DS1307, write 0x00 into Control register of Ds1307
 
------------------------------------------------------------------------------------*/
void DS1307_Init()
{
    I2C_Start();            // Start I2C communication
 
    DS1307_Write(DS1307_ID);   // Connect to DS1307 by sending its ID on I2c Bus
    DS1307_Write(CONTROL);     // Select the Ds1307 ControlRegister to configure Ds1307
 
    DS1307_Write(0x00);        // Write 0x00 to Control register to disable SQW-Out
 
    I2C_Stop();             // Stop I2C communication after initilizing DS1307
 
 }
 
 
 
 
 
 
 
 
 
 
 
 
/*---------------------------------------------------------------------------------
                         void DS1307_Write(unsigned char dat)
 ----------------------------------------------------------------------------------
 * I/P Arguments: char-> Data to be written into DS1307.
 * Return value : none
 
 * description :This function is used to initialize the Ds1307 RTC.
                Ds1307 ic is enabled by sending the DS1307 id on the I2C bus.
                After selecting DS1307, write 0x00 into Control register of Ds1307
 
------------------------------------------------------------------------------------*/
void DS1307_Write(unsigned char dat)
{
    I2C_Write(dat);   // Connect to DS1307 by sending its ID on I2c Bus
    I2C_Clock();
 }
 
 
 
 
 
 
 
 
/*---------------------------------------------------------------------------------
                         unsigned char DS1307_Read()
 ----------------------------------------------------------------------------------
 * I/P Arguments: char-> Data to be written into DS1307.
 * Return value : none
 
 * description :This function is used to initialize the Ds1307 RTC.
                Ds1307 ic is enabled by sending the DS1307 id on the I2C bus.
                After selecting DS1307, write 0x00 into Control register of Ds1307
 
------------------------------------------------------------------------------------*/
unsigned char DS1307_Read()
{
    unsigned char dat;
    dat = I2C_Read();   // Connect to DS1307 by sending its ID on I2c Bus
    return(dat);
 }
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
/*----------------------------------------------------------------------------------
        void DS1307_SetTime(unsigned char hh, unsigned char mm, unsigned char ss)
 -----------------------------------------------------------------------------------
 * I/P Arguments: char,char,char-->hh,mm,ss to initilize the time into DS1307.
 * Return value : none
 
 * description  :This function is used to set Time(hh,mm,ss) into the Ds1307 RTC.
                 Ds1307 ic is enabled by sending the DS1307 id on the I2C bus.
                 After selecting DS1307, select the RAM address 0x00 to point to sec
                 Initilze Sec, MIN, Hour one after the other.
                 Stop the I2c communication.
-----------------------------------------------------------------------------------*/
void DS1307_SetTime(unsigned char hh, unsigned char mm, unsigned char ss)
{
    I2C_Start();            // Start I2C communication
 
    DS1307_Write(DS1307_ID);    // connect to DS1307 by sending its ID on I2c Bus
    DS1307_Write(SEC_ADDRESS); // Select the SEC RAM address
 
    DS1307_Write(ss);           // Write sec on RAM address 00H
    DS1307_Write(mm);           // Write min on RAM address 01H
    DS1307_Write(hh);           // Write hour on RAM address 02H
 
    I2C_Stop();             // Stop I2C communication after Setting the Time
}
 
 
 
 
 
 
/*---------------------------------------------------------------------------------
          void DS1307_SetDate(unsigned char dd, unsigned char mm, unsigned char yy)
 ----------------------------------------------------------------------------------
 * I/P Arguments: char,char,char-->day,month,year to initilize the Date into DS1307.
 * Return value : none
 
 * description  :This function is used to set Date(dd,mm,yy) into the Ds1307 RTC.
                 Ds1307 ic is enabled by sending the DS1307 id on the I2C bus.
                 After selecting DS1307, select the RAM address 0x04 to point to day
                 Initilze Day,Month and Year one after the other.
                 Stop the I2c communication.
----------------------------------------------------------------------------------*/
void DS1307_SetDate(unsigned char dd, unsigned char mm, unsigned char yy)
{
    I2C_Start();            // Start I2C communication
 
    DS1307_Write(DS1307_ID);    // connect to DS1307 by sending its ID on I2c Bus
    DS1307_Write(DATE_ADDRESS);     // Request DAY RAM address at 04H
 
    DS1307_Write(dd);           // Write date on RAM address 04H
    DS1307_Write(mm);           // Write month on RAM address 05H
    DS1307_Write(yy);           // Write year on RAM address 06h
 
    I2C_Stop();             // Stop I2C communication after Setting the Date
}
 
 
 
 
 
 
 
/*----------------------------------------------------------------------------------
 void DS1307_GetTime(unsigned char *h_ptr,unsigned char *m_ptr,unsigned char *s_ptr)
 -----------------------------------------------------------------------------------
 * I/P Arguments: char *,char *,char *-->pointers to get the hh,mm,ss.
 * Return value : none
 
 * description  :This function is used to get the Time(hh,mm,ss) from Ds1307 RTC.
                 Ds1307 ic is enabled by sending the DS1307 id on the I2C bus.
                 After selecting DS1307, select the RAM address 0x00 to point to sec
                 Get Sec, MIN, Hour one after the other.
                 Stop the I2c communication.
-----------------------------------------------------------------------------------*/
void DS1307_GetTime(unsigned char *h_ptr,unsigned char *m_ptr,unsigned char *s_ptr)
{
     I2C_Start();           // Start I2C communication
 
    DS1307_Write(DS1307_ID);    // connect to DS1307 by sending its ID on I2c Bus
    DS1307_Write(SEC_ADDRESS);          // Request Sec RAM address at 00H
 
     I2C_Stop();            // Stop I2C communication after selecting Sec Register
 
    I2C_Start();                // Start I2C communication
    DS1307_Write(0xD1);         // connect to DS1307( under Read mode)
                                //by sending its ID on I2c Bus
 
  *s_ptr = DS1307_Read();  I2C_Ack();     // read second and return Positive ACK
  *m_ptr = DS1307_Read();  I2C_Ack();      // read minute and return Positive ACK
  *h_ptr = DS1307_Read();  I2C_NoAck();   // read hour and return Negative/No ACK
 
  I2C_Stop();               // Stop I2C communication after reading the Time
 }
 
 
 
 
 
 
 
 
 
 
 
/*----------------------------------------------------------------------------------
 void DS1307_GetDate(unsigned char *y_ptr,unsigned char *m_ptr,unsigned char *d_ptr)
 -----------------------------------------------------------------------------------
 * I/P Arguments: char *,char *,char *-->pointers to get the y,m,d.
 * Return value : none
 
 * description  :This function is used to get the Date(y,m,d) from Ds1307 RTC.
                 Ds1307 ic is enabled by sending the DS1307 id on the I2C bus.
                 After selecting DS1307, select the RAM address 0x00 to point to DAY
                 Get Day, Month, Year one after the other.
                 Stop the I2c communication.
-----------------------------------------------------------------------------------*/
void DS1307_GetDate(unsigned char *d_ptr,unsigned char *m_ptr,unsigned char *y_ptr)
{
    I2C_Start();            // Start I2C communication
 
    DS1307_Write(DS1307_ID);    // connect to DS1307 by sending its ID on I2c Bus
    DS1307_Write(DATE_ADDRESS);     // Request DAY RAM address at 04H
 
    I2C_Stop();             // Stop I2C communication after selecting DAY Register
 
 
   I2C_Start();             // Start I2C communication
   DS1307_Write(0xD1);          // connect to DS1307( under Read mode)
                            // by sending its ID on I2c Bus
 
  *d_ptr = DS1307_Read(); I2C_Ack();     // read Day and return Positive ACK
  *m_ptr = DS1307_Read(); I2C_Ack();     // read Month and return Positive ACK
  *y_ptr = DS1307_Read(); I2C_NoAck();   // read Year and return Negative/No ACK
 
  I2C_Stop();         // Stop I2C communication after reading the Time
 }''
/******************************************************************************************************************
-> I2c Communication between LPC2148 and Arduino Uno.
-> LPC2148 in Master Reciver mode and Arduino in Slave Transmitter mode
-> Connection :-
 	LPC2148		        Arduino
	GND()		-	GND
	SDA(P0.3)	-	SDA(A4)
	SCL(P0.2)	-	SCL(A5)
-> Pull-up SDA and SCL connection to 3.3 V
-> Assign 7-bit slave address 112 to Arduino (command: wire.begin(112))
-> Use 0xE1 for Slave Address + Read operation (7 bit address + Read bit)
******************************************************************************************************************/


#include <Wire.h>

void setup()
{
  Wire.begin(112);                // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
}

void loop()
{
  delay(100);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Wire.write("hello "); // respond with message of 6 bytes
  // as expected by master
}

/*
 * I2C.c
 *
 * Created: 4/23/2017 10:12:53 PM
 *  Author: TravisCucore
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#define F_CPU 8000000

#include <util/delay.h>


//sends a start condition
 void TWI_Start(void)
 {
	 TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //clears interrupt, enables TWI and sends start condtion
	 while ((TWCR & (1<<TWINT)) == 0) //waits for interrupt to indicate start was sent
	 {
		 //do nothing
	 }
 }
 
 //generates stop condition
 void TWI_Stop()
 {
	 //clears interrupt, issues stop condition and enables TWI
	 TWCR = ((1<<TWINT)|(1<<TWSTO)|(1<<TWEN));
 }
 
 //sends a message over I2C
 void TWI_Write(unsigned char address)
 {
	 TWDR = (address<<1); //load address address shifted to the left one to make room for read write bit
	 TWCR = (1<<TWINT)|(1<<TWEN); //clear TWI interrupt and ensure TWI is enabled
	 while ((TWCR & (1<<TWINT)) == 0) //check if transmission was completed
	 {
		 //do nothing
	 }
 }
 
  //sends reads a message over I2C
  void TWI_Read(unsigned char address)
  {
	  TWDR = (address<<1)+1;  //load address shifted one to the left and set LSb to reflec a read request
	  TWCR = (1<<TWINT)|(1<<TWEN); //clear TWI interrupt and ensure TWI is enabled
	  while ((TWCR & (1<<TWINT)) == 0) //wait for transmission to complete
	  {
		  //do nothing
	  }
  }
  
  //sends data
   void TWI_Data(unsigned char data)
   {
		TWDR = (data); // load data to be sent
		TWCR = (1<<TWINT)|(1<<TWEN); //clear TWI interrupt and ensure TWI is enabled
		while ((TWCR & (1<<TWINT)) == 0) //wait for flag to indicate transmission is complete
		{
			//do nothing
		}   
   }

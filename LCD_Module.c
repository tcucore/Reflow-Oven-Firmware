/*
 * LCD_Module.c
 * Created: 1/28/2017 9:42:48 PM
 * Author: Tcucore
 * Description: This file contains functions needed to run the LCD Module
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdio.h>

#define F_CPU 8000000

#include <util/delay.h>
#include "Protos.h"
#include "Abstractions.h"


volatile unsigned char packet;
volatile unsigned char currentData;
volatile char currentMenu;





void SendMessage(unsigned char address, unsigned char RS, char* data, unsigned char length)
{
	HomeCursor();
	unsigned char index = 0;
	TWI_Start();  //send start condition to I2C bus
	TWI_Write(address);  //Ping address and wait for ACK
	while(index < length)
	{
		TWI_Data(BuildPacket(RS, LCD_ENABLE, WRITE, *data, UPPER));  //stage upper nibble
		TWI_Data(BuildPacket(RS, LCD_DISABLE, WRITE, *data, UPPER)); //latch upper nibble
		TWI_Data(BuildPacket(RS, LCD_ENABLE, WRITE, *data, LOWER)); //stage lower nibble
		TWI_Data(BuildPacket(RS, LCD_DISABLE, WRITE, *data, LOWER)); //latch lower nibble
		data++;  //increment array to next character
		index++;  //increment index
	}
	TWI_Stop();  //send stop condition to I2C bus
}

unsigned char BuildPacket(unsigned char RS, unsigned char EN, unsigned char RW, unsigned char currentData, unsigned char nibble)
{
	switch(nibble)
	{
		case UPPER:
		packet = (((currentData & 0xF0)) | EN<<ENABLE | RW<<READ_WRITE | RS<<REGISTER_SELECT);
		break;
		
		case LOWER:
		packet = (((currentData & 0x0F)<<4) | EN<<ENABLE | RW<<READ_WRITE | RS<<REGISTER_SELECT);
		break;
	}
	return packet;
}


void HomeCursor()
{
	TWI_Start();
	TWI_Write(LCD_MODULE);
	TWI_Data(BuildPacket(COMMAND, LCD_ENABLE, WRITE, RETURN_HOME, UPPER));
	TWI_Data(BuildPacket(COMMAND, LCD_DISABLE, WRITE, RETURN_HOME, UPPER));
	TWI_Data(BuildPacket(COMMAND, LCD_ENABLE, WRITE, RETURN_HOME, LOWER));
	TWI_Data(BuildPacket(COMMAND, LCD_DISABLE, WRITE, RETURN_HOME, LOWER));
	TWI_Stop();
}
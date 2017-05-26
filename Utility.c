/*
 * Name: Utility.c
 * Created: 1/28/2017 8:23:52 PM
 *  Author: Tcucore
 * This file contains utility functions such as initializations
 * and helper functions.
 */ 
 
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#define F_CPU 8000000

#include <util/delay.h>
#include "Protos.h"
#include "Abstractions.h"
  
//this array holds the instructions required to setup the LCD Module
char enableFourBitMode[] = {0x33, 0x32};  //commands to set up 4bit mode
char initLCD[] = {0x0C,0x01,0x06}; //commands to set every thing else

//Startup routine
 void InitializeDevice(void)
 {
	_delay_ms(15);  //wait for devices to power on
	cli();  //disable global interrupts
	PortInit(); //Setup Port-Pins
	TwiInit(); //Initialize the Two Wire Interface for I2C
	InterruptInit(); //setup interrupts
	LcdInit(); 	//Initialize LCD
	sei(); //enable global interrupts
 }
 
//initializes the LCD 
void LcdInit()
{
	HomeCursor();
	SendMessage(LCD_MODULE, COMMAND, enableFourBitMode, sizeof(enableFourBitMode));
	SendMessage(LCD_MODULE, COMMAND, initLCD, sizeof(initLCD));
	DrawMenu(HOME);
	HomeCursor();
}


//initializes the Two Wire Interface (I2C)
void TwiInit()
{
	TWCR = 0x04;  //Enables Two wire interface and enable interrupts
	TWBR = 0x80;  //Division factor of TWo after prescale
	TWSR = 0x00; //prescaler
}


//Initializes interrupts
void InterruptInit()
{
	PCICR = 0x01;  //Enable Pin Change interrupt from PCIE(0:1)
	PORTA |= 0x07;  //set pull-up resistors
	PCMSK0 = 0x07;  //Allow Pin(39:40) to trigger an interrupt on its vector
}


//Initializes Port-Pins
void PortInit()
{
	DDRB |= 0x01;  //attached to LED for testing
}


 




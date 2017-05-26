/*
 * File: Reflow-Oven.c
 * Created: 1/28/2017 8:21:14 PM
 * Author : Tcucore
 * Description:  This program runs a re-flow oven
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#define F_CPU 8000000

#include <util/delay.h>
#include "Protos.h"
#include "Abstractions.h"

int main(void)
{
	InitializeDevice();  //Runs all Setup Routine
	while(1)
	{
	}
}



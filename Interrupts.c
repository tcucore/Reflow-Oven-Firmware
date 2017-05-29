#include <avr/io.h>

#define F_CPU 8000000  //this needs to be defined before the interrupt header is loaded

#include <util/delay.h>
#include <avr/interrupt.h>
#include "Protos.h"
#include "Abstractions.h"


//some containers for working with
volatile unsigned char currentState;
volatile unsigned char transitionState;
volatile unsigned char lastState;
volatile unsigned char isPressed;

extern char currentMenu;





//Handles A user pressing the accept button
ISR(PCINT0_vect)
{
	transitionState = (PINA & 0x06);  //mask PORTA to get the transition state
	CheckButton();	//Checks for the accept button being pressed
	isPressed = 0;	//Make not of return to resting state
	CheckDirection();  //Checks direction of rotation
	lastState = PINA & 0x06;  //save the last stable state
}



//determines the direction of rotation and calls the appropriate function.
void CheckDirection(void)
{
	switch(transitionState)  //switches on last known transition state
	{
		case 0x02:
			if (lastState == 0x06) //determines direction using last known resting state
			{
				InputAction('L');
			}
			else
			{
				InputAction('R');
			}
		break;

		case 0x04:  
			if(lastState == 0x00)  //direction using last known resting state
			{
				InputAction('L');
			}
			else
			{
				InputAction('R');
			}
		break;
	}
}


void CheckButton(void)
{
	while(!(PINA & 0x01))
	{
		isPressed = 1;  //make note the button is pressed
	}
		
	//this resolves a stacking interrupt issue
	if(isPressed == 1)
	{
		InputAction('A');
	}	
}

void InputAction(char action)
{
	switch(currentMenu)
	{
		case HOME:
		HomeMenu(action);
		break;
		
		case REFLOW:
		ReflowMenu(action);
		break;
		
		case EXPOSURE:
		ExposureMenu(action);
		break;
		
		case REFLOW_SETUP:
		ReflowSetupMenu(action);
		break;
		
		case REFLOW_STATUS:
		ReflowStatusMenu(action);
		break;
		
		case REFLOW_DWELL:
		ReflowDwellMenu(action);
		break;
		
		case REFLOW_CALIBRATE:
		ReflowCalibrateMenu(action);
		break;
		
		case CALIBRATE_PROBES:
		CalibrateProbesMenu(action);
		break;
		
		case CALIBRATE_REFLOW:
		CalibrateOvenMenu(action);
		break;
	}
	HomeCursor();
}
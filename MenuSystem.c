#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdio.h>

#define F_CPU 8000000

#include <util/delay.h>
#include "Protos.h"
#include "Abstractions.h"




//initialize buffers for holding Menus.
char homeScreen[80] =  {"----------------------------------------        MODE         [Reflow] Exposure  "};
char reflowScreen[80] ={"----------------------------------------       REFLOW       [Setup]  Cal   Home "};
char exposureScreen[80] = {"      EXPOSURE          Time: [5:00]    --------------------   Home     Start   "};
char reflowStatusScreen[80] = {"   Reflow  Status    Temp  Target  Next      Time: 1:20      000C   000C   000C "};
char reflowSetupScreen[80] = {"Soak      Reflow    Rmp 3 C/s Rmp 3 C/s Tmp 120 C Tmp 220 C Cool 3 C/s   [Dwell]"};
char reflowDwellScreen[80] = {"       DWELL           120s     120s       Soak    Reflow      Back    [Start]  "};
char calibrateScreen[80] = {"----------------------------------------      CALIBRATE        Oven    [Probes] "};
char calibrateOvenScreen[80] = {"OVEN"};
char calibrateProbeScreen[80] = {"Put Probe In IceBath V-junction: 000uV        Probe 1         [Back]    Next    "};
	
//working variables
char edit = '0'; //used to keep track of if a value is being changed
char intBuffer[3]; //used to store new values before being inserted into the menu array
int soakTmp = 120; //holds the user set temperature for soak
int reflowTmp = 220; //holds the user set temperature for reflow
int soakRmp = 3; //hold the user set ramp speed in c/s for moving from on to soak
int reflowRmp = 3; //holds the user set ramp speed in c/s for moving from soak to reflow
int downRmp = 3; //holds the user set ramp speed in c/s for cool-down
int soakDwell = 120; //holds the user set time to stay in soak
int reflowDwell = 120; //holds the user set time to stay in reflow
int currentTemp = 0; //holds the current average temperature reading across all probes
int nextTemp = 0; //holds the next temperature
int targetTemp = 0; //holds the current target temperature
int exposureMinutes = 5; //holds number of minutes to expose a board
int exposureSeconds = 0; //holds the number of seconds to expose a board
int probeOneTemp = 0;  // holds current temperature for probe 1
int probeTwoTemp = 0;  //holds current temperature for probe 2
int probeThreeTemp = 0;  //holds current temperature for probe 3
int probeFourTemp = 0;  //holds current temperature for probe 4
int probeFiveTemp = 0;  //holds current temperature for probe 5
int probeOneCal = 0;  //holds calibration point for 0C for probe 1
int probeTwoCal = 0;  //holds calibration point for 0C for probe 2
int probeThreeCal = 0;  //holds calibration point for 0C for probe 3
int probeFourCal = 0;  //holds calibration point for 0C for probe 4
int probeFiveCal = 0;  //holds calibration point for 0C for probe 5

extern char currentMenu;  //references the current menu variable that keeps track of which menu the user is in



/***********************************************************************************
Functiuon Name: DrawMenu()
Description:  This function draws the menu passed to it.  It also updates currentMenu
to represent the menu it was asked to draw.
Accepts: char
returns: nothing
***********************************************************************************/
void DrawMenu(char menu)
{
	switch(menu)
	{
		case HOME: //this is the screen the device loads first
		currentMenu = HOME;   //set current menu to home
		SendMessage(LCD_MODULE, DATA, homeScreen, sizeof(homeScreen));  //Draw current menu  
		break;
		
		case REFLOW: //this is the reflow menu
		currentMenu = REFLOW; //set current menu to reflow
		SendMessage(LCD_MODULE, DATA, reflowScreen, sizeof(reflowScreen)); //Draw reflow menu
		break;
		
		case EXPOSURE: //this is the exposure menu
		currentMenu = EXPOSURE; //Set current menu to exposure
		SendMessage(LCD_MODULE, DATA, exposureScreen, sizeof(exposureScreen)); //draw exposure menu
		break;
		
		case REFLOW_STATUS:  //this is the reflow status screen
		currentMenu = REFLOW_STATUS; //set current menu to reflow status
		SendMessage(LCD_MODULE, DATA, reflowStatusScreen, sizeof(reflowStatusScreen)); //draw reflow status menu
		break;
		
		case REFLOW_SETUP:  //this is the reflow setup screen.  This is a series of screens
		currentMenu = REFLOW_SETUP; //set current menu the reflow setup
		SendMessage(LCD_MODULE, DATA, reflowSetupScreen, sizeof(reflowSetupScreen)); //draw reflow setup menu
		break;
		
		case REFLOW_DWELL: //this is the reflow dwell menu
		currentMenu = REFLOW_DWELL; //set current menu to reflow dwell
		SendMessage(LCD_MODULE, DATA, reflowDwellScreen, sizeof(reflowDwellScreen)); //draw reflow dwell menu
		break;
		
		case REFLOW_CALIBRATE:
		currentMenu = REFLOW_CALIBRATE;
		SendMessage(LCD_MODULE, DATA, calibrateScreen, sizeof(calibrateScreen)); //draw calibration options screen
		break;
		
		case CALIBRATE_REFLOW:
		currentMenu = CALIBRATE_REFLOW;
		SendMessage(LCD_MODULE, DATA, calibrateOvenScreen, sizeof(calibrateOvenScreen));
		break;
		
		case CALIBRATE_PROBES:
		currentMenu = CALIBRATE_PROBES;
		SendMessage(LCD_MODULE, DATA, calibrateProbeScreen, sizeof(calibrateProbeScreen));
		break;
	}
}

/***********************************************************************************
Functiuon Name: HomeMenu()
Description:  This function determine what action should be taken when on HomeMenu
Accepts: char
returns: nothing
***********************************************************************************/
void HomeMenu(char action)
{
	switch(action) //determine what input action was received
	{
		case 'A':  //if rotary encoder button was pressed
		if(homeScreen[78] == ']') //if exposure is selected
		{
			DrawMenu(EXPOSURE); //move to exposure menu
		}
		else
		{
			if(homeScreen[68] == ']') //if reflow is selected
			{
				DrawMenu(REFLOW); //move to reflow screen
			}
		}
		break;
		
		case 'L': //if rotary encoder is rotated lect (CCW)
		if(homeScreen[68] == ']') //if reflow is selected change selection to exposure
		{
			homeScreen[68] = ' ';
			homeScreen[61] = ' ';
			homeScreen[78] = ']';
			homeScreen[69] = '[';
		}
		else
		{
			if(homeScreen[78] == ']') //if exposure is selected change selection to reflow
			{
				homeScreen[78] = ' ';
				homeScreen[69] = ' ';
				homeScreen[68] = ']';
				homeScreen[61] = '[';
			}
		}
		DrawMenu(HOME);  //redraw home menu
		break;
		
		case 'R': //if rotary encoder is rotated right (CW)
		if(homeScreen[68] == ']') //if reflow is selected change selection to exposure
		{
			homeScreen[68] = ' ';
			homeScreen[61] = ' ';
			homeScreen[78] = ']';
			homeScreen[69] = '[';
		}
		else
		{
			if(homeScreen[78] == ']') //if exposure is selected change selection to reflow
			{
				homeScreen[78] = ' ';
				homeScreen[69] = ' ';
				homeScreen[68] = ']';
				homeScreen[61] = '[';
			}
		}
		DrawMenu(HOME); //reflow home menu
		break;
	}
}


/***********************************************************************************
Functiuon Name: ReflowMenu()
Description:  This function determine what action should be taken when on ReflowMenu
Accepts: char
returns: nothing
***********************************************************************************/
void ReflowMenu(char action) 
{
	switch(action) //determine what input action was recieved
	{
		case 'A': //if rotary encoder button was pressed
		if(reflowScreen[66] == ']') //if setup is selected
		{
			DrawMenu(REFLOW_SETUP); // move to reflow setup menu
		}
		else
		{
			if(reflowScreen[72] == ']') //if cal is selected
			{
				DrawMenu(REFLOW_CALIBRATE); //move to reflow status menu
			}
			else
			{
				if(reflowScreen[79] == ']') //if home is selected
				{
					DrawMenu(HOME); //move to home menu
				}
			}
		}
		break;
			
		case 'L': //if rotart encoder is rotated lect (CCW)
		if(reflowScreen[66] == ']') //if setup is selected change selection to home
		{
			reflowScreen[66] = ' ';
			reflowScreen[60] = ' ';
			reflowScreen[79] = ']';
			reflowScreen[74] = '[';
		}
		else
		{
			if(reflowScreen[79] == ']') //if home is selected change selection to status
			{
				reflowScreen[79] = ' ';
				reflowScreen[74] = ' ';
				reflowScreen[72] = ']';
				reflowScreen[68] = '[';
			}
			else
			{
				if(reflowScreen[72] == ']') //if home is selected change selection to setup
				{
					reflowScreen[72] = ' ';
					reflowScreen[68] = ' ';
					reflowScreen[66] = ']';
					reflowScreen[60] = '[';
				}
			}
		}
		DrawMenu(REFLOW); //redraw reflow menu
		break;
			
		case 'R': //if rotary encoder is turned right (CW)
		if(reflowScreen[66] == ']') //if setup is selected change selection to status
		{
			reflowScreen[66] = ' ';
			reflowScreen[60] = ' ';
			reflowScreen[72] = ']';
			reflowScreen[68] = '[';
		}
		else
		{
			if(reflowScreen[72] == ']') //if status is selected change selection to home
			{
				reflowScreen[72] = ' ';
				reflowScreen[68] = ' ';
				reflowScreen[79] = ']';
				reflowScreen[74] = '[';
			}
			else
			{
				if(reflowScreen[79] == ']') //if home is selected change selection to setup
				{
					reflowScreen[79] = ' ';
					reflowScreen[74] = ' ';
					reflowScreen[66] = ']';
					reflowScreen[60] = '[';
				}
			}
		}
		DrawMenu(REFLOW); //redraw reflow menu
		break;
	}
}


/***********************************************************************************
Functiuon Name: ExposureMenu()
Description:  This function determine what action should be taken when on ExposureMenu
Accepts: char
returns: nothing
***********************************************************************************/
void ExposureMenu(char action)
{
	switch(edit) //check edit status
	{
		case '0': //if not editing
		switch(action) //check input action
		{
			case 'A': //if rotary encoder button is pressed
			if(exposureScreen[67] == ']')  //if home is selected
			{
				DrawMenu(HOME); //move to home menu
			}
			else
			{
				if(exposureScreen[77] == ']') //if start is selected
				{
					//todo write function to start exposure cycle.
				}
				else 
				{
					if(exposureScreen[35] == ']') //if time is selected
					{
						edit = '1'; //enter edit mode
					}
				}
			}
			break;
			
			case 'L': //if rotary encoder is rotated left (CCW)
			if(exposureScreen[77] == ']') //if start is selected change selection to home
			{
				exposureScreen[77] = ' ';
				exposureScreen[71] = ' ';
				exposureScreen[67] = ']';
				exposureScreen[62] = '[';
			}
			else
			{
				if(exposureScreen[67] == ']') //if home is selected change selection to time
				{
					exposureScreen[67] = ' ';
					exposureScreen[62] = ' ';
					exposureScreen[35] = ']';
					exposureScreen[30] = '[';
				}
				else
				{
					if(exposureScreen[35] == ']') //if time is selected change selection to start
					{
						exposureScreen[35] = ' ';
						exposureScreen[30] = ' ';
						exposureScreen[77] = ']';
						exposureScreen[71] = '[';
					}
				}
			}
			DrawMenu(EXPOSURE); //redraw exposure menu
			break;
			
			case 'R': //if rotary encoder is rotated right (CW)
			if(exposureScreen[67] == ']') //if home is selected change selection to start
			{
				exposureScreen[67] = ' ';
				exposureScreen[62] = ' ';
				exposureScreen[77] = ']';
				exposureScreen[71] = '[';
			}
			else
			{
				if(exposureScreen[35] == ']') //if time is selected change selection to home
				{
					exposureScreen[35] = ' ';
					exposureScreen[30] = ' ';
					exposureScreen[67] = ']';
					exposureScreen[62] = '[';
				}
				else
				{
					if(exposureScreen[77] == ']') //if start is selected change selection to time
					{
						exposureScreen[77] = ' ';
						exposureScreen[71] = ' ';
						exposureScreen[35] = ']';
						exposureScreen[30] = '[';
					}
				}
			}
			DrawMenu(EXPOSURE);  //redraw exposure menu
			break;
		}
		break;
		
		case '1': //if in edit mode
		switch(action)
		{
			case 'A': // if rotary encoder button is pressed
			edit = '0'; //exit edit mode
			break;
			
			case 'L': //if rotary encoder is rotated left (CCW)
			if(exposureScreen[35] == ']') //if time is selected
			{
				if(exposureSeconds == 0) //if seconds = 0
				{
					exposureMinutes--; //decrement minutes
					exposureSeconds = 59; //set seconds to 59
				}
				else
				{
					exposureSeconds--; //decrement seconds
				}
			}
			sprintf(intBuffer, "%d:%.2d", exposureMinutes, exposureSeconds);  //build a short string to hold current time
			//replace last time with current time
			exposureScreen[31] = intBuffer[0];
			exposureScreen[32] = intBuffer[1];
			exposureScreen[33] = intBuffer[2];
			exposureScreen[34] = intBuffer[3];
			DrawMenu(EXPOSURE); //redraw exposure menu
			break;
			
			case 'R': //if rotary encoder is rotated right (CW)
			if(exposureScreen[35] == ']') //if time is selected
			{
				if(exposureSeconds == 59) // if seconds = 59
				{
					exposureMinutes++; //increment minutes
					exposureSeconds = 0; //reset seconds to 0
				}
				else
				{
					exposureSeconds++; //increment seconds
				}
			}
			sprintf(intBuffer, "%d:%.2d", exposureMinutes, exposureSeconds); //build short string to hold current time
			//replace lst itme with current time
			exposureScreen[31] = intBuffer[0];
			exposureScreen[32] = intBuffer[1];
			exposureScreen[33] = intBuffer[2];
			exposureScreen[34] = intBuffer[3];
			DrawMenu(EXPOSURE); //redraw exposure menu
			break;
		}
		break;
	}
}


/***********************************************************************************
Functiuon Name: ReflowSetupMenu()
Description:  This function determine what action should be taken when on ReflowSetupMenu
Accepts: char
returns: nothing
***********************************************************************************/
void ReflowSetupMenu(char action)
{
	switch(edit) //check if in edit mode
	{
		case '0': //if not in exit mode
		switch(action)
		{
			case 'A': //if rotary encoder button is pressed
			if(reflowSetupScreen[79] == ']') //if dwell is selected
			{
				DrawMenu(REFLOW_DWELL); //move to dwell menu
				
			}
			else
			{
				edit = '1'; //enter edit mode
			}
			break;
			
			case 'L':  //if rotary encoder is rotated lect (CCW)
			if (reflowSetupScreen[79] == ']') //if dwell is selected change selection to ramp down
			{
				reflowSetupScreen[79] = ' ';
				reflowSetupScreen[73] = ' ';
				reflowSetupScreen[66] = ']';
				reflowSetupScreen[64] = '[';
			}
			else
			{
				if (reflowSetupScreen[66] == ']') // if ramp down is selected change selection to reflow ramp
				{
					reflowSetupScreen[66] = ' ';
					reflowSetupScreen[64] = ' ';
					reflowSetupScreen[35] = ']';
					reflowSetupScreen[33] = '[';
				}
				else
				{
					if (reflowSetupScreen[35] == ']') //if reflow ramp is selected change selection to soak ramp
					{
						reflowSetupScreen[35] = ' ';
						reflowSetupScreen[33] = ' ';
						reflowSetupScreen[25] = ']';
						reflowSetupScreen[23] = '[';
					}
					else
					{
						if (reflowSetupScreen[25] == ']') // if soak ramp is selected change selection to reflow temp
						{
							reflowSetupScreen[25] = ' ';
							reflowSetupScreen[23] = ' ';
							reflowSetupScreen[57] = ']';
							reflowSetupScreen[53] = '[';
						}
						else
						{
							if (reflowSetupScreen[57] == ']') // if reflow temp is selected change selection to soak temp
							{
								reflowSetupScreen[57] = ' ';
								reflowSetupScreen[53] = ' ';
								reflowSetupScreen[47] = ']';
								reflowSetupScreen[43] = '[';
							}
							else
							{
								if(reflowSetupScreen[47] == ']') // if reflow temp is selected change selection to dwell
								{
									reflowSetupScreen[47] = ' ';
									reflowSetupScreen[43] = ' ';
									reflowSetupScreen[79] = ']';
									reflowSetupScreen[73] = '[';
								}
							}
						}
					}
				}
			}
			DrawMenu(REFLOW_SETUP); //redraw reflow setup menu
			break;
			
			case 'R': // if rotary encoder is rotated right (CW)
			if (reflowSetupScreen[66] == ']') //if ramp down is selected change selection to dwell
			{
				reflowSetupScreen[66] = ' ';
				reflowSetupScreen[64] = ' ';
				reflowSetupScreen[79] = ']';
				reflowSetupScreen[73] = '[';
			}
			else
			{
				if (reflowSetupScreen[35] == ']') // if relow ramp is selected change selection to ramp down
				{
					reflowSetupScreen[35] = ' ';
					reflowSetupScreen[33] = ' ';
					reflowSetupScreen[66] = ']';
					reflowSetupScreen[64] = '[';
				}
				else
				{
					if (reflowSetupScreen[25] == ']') //if soak ramp is selected change seletion to reflow ramp
					{
						reflowSetupScreen[25] = ' ';
						reflowSetupScreen[23] = ' ';
						reflowSetupScreen[35] = ']';
						reflowSetupScreen[33] = '[';
					}
					else
					{
						if (reflowSetupScreen[57] == ']') // if reflow temp is selected change selection to soak ramp
						{
							reflowSetupScreen[57] = ' ';
							reflowSetupScreen[53] = ' ';
							reflowSetupScreen[25] = ']';
							reflowSetupScreen[23] = '[';
						}
						else
						{
							if (reflowSetupScreen[47] == ']') //if soak temp is selected change selection to reflow temp
							{
								reflowSetupScreen[47] = ' ';
								reflowSetupScreen[43] = ' ';
								reflowSetupScreen[57] = ']';
								reflowSetupScreen[53] = '[';
							}
							else
							{
								if(reflowSetupScreen[79] == ']') //if dwell is selected change selection to soak temp
								{
									reflowSetupScreen[79] = ' ';
									reflowSetupScreen[73] = ' ';
									reflowSetupScreen[47] = ']';
									reflowSetupScreen[43] = '[';
								}
							}
						}
					}
				}
			}
			DrawMenu(REFLOW_SETUP); // redraw reflow setup menu
			break;
		}
		break;
		
		case '1': //if in edit mode
		switch(action)
		{
			case 'A': // if rotary encoder button is pressed
			edit = '0'; //exit edit mode
			break;
			
			case 'L': // if rotary encoder is rotated left (CCW)
			if(reflowSetupScreen[66] == ']') // if ramp down is selecte
			{
				if(downRmp == 0) //if ramp down = 0
				{
					downRmp = 9; //reset ramp down to 9
					sprintf(intBuffer, "%d", downRmp); //stringify
					reflowSetupScreen[65] = intBuffer[0]; //change old ramp to new ramp
					DrawMenu(REFLOW_SETUP);
				}
				else
				{
					downRmp--; //decrement ramp down
					sprintf(intBuffer, "%d", downRmp); //stringify
					reflowSetupScreen[65] = intBuffer[0]; //replace old value with new value
					DrawMenu(REFLOW_SETUP);
				}
			}
			else
			{
				if(reflowSetupScreen[35] == ']') // if reflow ramp is selected
				{
					if(reflowRmp == 0) //if reflow ramp = 0
					{
						reflowRmp = 9; // reset reflow ramp to 9
						sprintf(intBuffer, "%d", reflowRmp); //stringify
						reflowSetupScreen[34] = intBuffer[0]; //replace old value with new one
						DrawMenu(REFLOW_SETUP);
					}
					else
					{
						reflowRmp--; //decrement reflow ramp
						sprintf(intBuffer, "%d", reflowRmp); //stringify
						reflowSetupScreen[34] = intBuffer[0]; //replace old value with new one
						DrawMenu(REFLOW_SETUP);
					}
				}
				else
				{
					if(reflowSetupScreen[25] == ']') //if soak ramp is selected
					{
						if(soakRmp == 0) //if soak ramp = 0
						{
							soakRmp = 9; //reset soak ramp to 9
							sprintf(intBuffer, "%d", soakRmp); //stringify
							reflowSetupScreen[24] = intBuffer[0]; //replace old value with new one
							DrawMenu(REFLOW_SETUP);
						}
						else
						{
							soakRmp--; //decrement soakramp
							sprintf(intBuffer, "%d", soakRmp);//stringify
							reflowSetupScreen[24] = intBuffer[0];//replace old value with new one
							DrawMenu(REFLOW_SETUP);
						}
					}
					else
					{
						if(reflowSetupScreen[57] == ']') //if reflow temp is selected
						{
							if(reflowTmp == 200) //if reflow temp = 200
							{
								reflowTmp = 300; //reset reflow temp to 300
								sprintf(intBuffer, "%d", reflowTmp); //stringify
								//replace old value with new one
								reflowSetupScreen[54] = intBuffer[0];
								reflowSetupScreen[55] = intBuffer[1];
								reflowSetupScreen[56] = intBuffer[2];
								DrawMenu(REFLOW_SETUP);
							}
							else
							{
								reflowTmp--; //decrement reflow temp
								sprintf(intBuffer, "%d", reflowTmp);//stringify
								//replace old value with new one
								reflowSetupScreen[54] = intBuffer[0];
								reflowSetupScreen[55] = intBuffer[1];
								reflowSetupScreen[56] = intBuffer[2];
								DrawMenu(REFLOW_SETUP);
							}
						}
						else
						{
							if (reflowSetupScreen[47] == ']') //if soak temp is selected
							{
								if(soakTmp == 100) //if soak temp = 100
								{
									soakTmp = 200; //reset soak temp to 200
									sprintf(intBuffer, "%d", soakTmp); //stringify
									//replace old value with new one
									reflowSetupScreen[44] = intBuffer[0];
									reflowSetupScreen[45] = intBuffer[1];
									reflowSetupScreen[46] = intBuffer[2];
									DrawMenu(REFLOW_SETUP);
								}
								else
								{
									soakTmp--; //decrement soak temp
									sprintf(intBuffer, "%d", soakTmp);//stringify
									//replace old value with new one
									reflowSetupScreen[44] = intBuffer[0];
									reflowSetupScreen[45] = intBuffer[1];
									reflowSetupScreen[46] = intBuffer[2];
									DrawMenu(REFLOW_SETUP);
								}
							}
						}
					}
				}
			}
			break;
			
			case 'R': //if rotary encoder is rotated right (CW)
			if(reflowSetupScreen[66] == ']')//if down ramp is selected
			{
				if(downRmp == 9) //if down ramp = 9
				{
					downRmp = 0; // reset down ramp to 0
					sprintf(intBuffer, "%d", downRmp); //stringify
					reflowSetupScreen[65] = intBuffer[0]; //replace old value with new one
					DrawMenu(REFLOW_SETUP); //redraw reflow setup menu
				}
				else
				{
					downRmp++; //decrement down ramp
					sprintf(intBuffer, "%d", downRmp);//stringify
					reflowSetupScreen[65] = intBuffer[0];//replace old value with new one
					DrawMenu(REFLOW_SETUP);//redraw reflow setup menu
				}
			}
			else
			{
				if(reflowSetupScreen[35] == ']') //if reflow ramp is selected
				{
					if(reflowRmp == 9) //if reflow ramp = 9
					{
						reflowRmp = 0;//reset reflow ramp to 0
						sprintf(intBuffer, "%d", reflowRmp);//stringify
						reflowSetupScreen[34] = intBuffer[0];//replace old value with new one
						DrawMenu(REFLOW_SETUP);//redraw reflow setup menu
					}
					else
					{
						reflowRmp++; //increment reflow ramp
						sprintf(intBuffer, "%d", reflowRmp); //stringify
						reflowSetupScreen[34] = intBuffer[0];//replace old value with new one
						DrawMenu(REFLOW_SETUP); //redraw reflow setup menu
					}
				}
				else
				{
					if(reflowSetupScreen[25] == ']') // if soak ramp is selected
					{
						if(soakRmp == 9) //if soak ramp = 9
						{
							soakRmp = 0; //reset soak ramp to 0
							sprintf(intBuffer, "%d", soakRmp);//stringify
							reflowSetupScreen[24] = intBuffer[0];//replace old value with new one
							DrawMenu(REFLOW_SETUP);//redraw reflow setup menu
						}
						else
						{
							soakRmp++;//increment soak ramp
							sprintf(intBuffer, "%d", soakRmp);//stringify
							reflowSetupScreen[24] = intBuffer[0];//replace old value with new one
							DrawMenu(REFLOW_SETUP);//redraw reflow setup menu
						}
					}
					else
					{
						if(reflowSetupScreen[57] == ']')//if reflow temp is selected
						{
							if(reflowTmp == 300) //if reflow temp = 300
							{
								reflowTmp = 200; //reset reflow temp to 200
								sprintf(intBuffer, "%d", reflowTmp);//stringify
								//replace old value with new one
								reflowSetupScreen[54] = intBuffer[0];
								reflowSetupScreen[55] = intBuffer[1];
								reflowSetupScreen[56] = intBuffer[2];
								DrawMenu(REFLOW_SETUP);//redraw reflow setup menu
							}
							else
							{
								reflowTmp++;//increment reflow temp
								sprintf(intBuffer, "%d", reflowTmp);//stringify
								//replace old value with new one
								reflowSetupScreen[54] = intBuffer[0];
								reflowSetupScreen[55] = intBuffer[1];
								reflowSetupScreen[56] = intBuffer[2];
								DrawMenu(REFLOW_SETUP);//redraw reflow setup menu
							}
						}
						else
						{
							if (reflowSetupScreen[47] == ']')//if soak temp is selected
							{
								if(soakTmp == 200)//if soak temp = 200
								{
									soakTmp = 100;// reset soak temp to 100
									sprintf(intBuffer, "%d", soakTmp);//stringify
									//replace old value with new one
									reflowSetupScreen[44] = intBuffer[0];
									reflowSetupScreen[45] = intBuffer[1];
									reflowSetupScreen[46] = intBuffer[2];
									DrawMenu(REFLOW_SETUP);//redraw reflow setup menu
								}
								else
								{
									soakTmp++;//increment soak temp
									sprintf(intBuffer, "%d", soakTmp);//stringify
									//replace old value with new one
									reflowSetupScreen[44] = intBuffer[0];
									reflowSetupScreen[45] = intBuffer[1];
									reflowSetupScreen[46] = intBuffer[2];
									DrawMenu(REFLOW_SETUP);//redraw reflow setup menu
								}
							}
						}
					}
				}
			}
			break;
		}
		break;
	}
}


/***********************************************************************************
Functiuon Name: ReflowDwellMenu()
Description:  This function determine what action should be taken when on ReflowDwellMenu
Accepts: char
returns: nothing
***********************************************************************************/
void ReflowDwellMenu(char action)
{
	switch(edit) //check for edit mode
	{
		case '0': //if not in edit mode
		switch(action) //check input action
		{
			case 'A': //if rotary encoder button is pressed
			if(reflowDwellScreen[77] == ']') //if start is selected
			{
				DrawMenu(REFLOW_STATUS);
				//todo write function to start reflow process.
			}
			else
			{
				if(reflowDwellScreen[67] == ']') //if back is selected
				{
					DrawMenu(REFLOW_SETUP); //move to reflow setup menu
				}
				else
				{
					edit = '1'; //enter edit mode
				}
			}
			break;
			
			case 'L': //if rotary encoder is rotated left (CCW)
			if(reflowDwellScreen[77] == ']') //if start is selected change selection to back
			{
				reflowDwellScreen[77] = ' ';
				reflowDwellScreen[71] = ' ';
				reflowDwellScreen[67] = ']';
				reflowDwellScreen[62] = '[';
			}
			else
			{
				if(reflowDwellScreen[67] == ']') //if back is selected change selection to reflow dwell time
				{
					reflowDwellScreen[67] = ' ';
					reflowDwellScreen[62] = ' ';
					reflowDwellScreen[36] = ']';
					reflowDwellScreen[31] = '[';
				}
				else
				{
					if(reflowDwellScreen[36] == ']') //if reflow dwell time is selected change selection to soak dwell time
					{
						reflowDwellScreen[36] = ' ';
						reflowDwellScreen[31] = ' ';
						reflowDwellScreen[27] = ']';
						reflowDwellScreen[22] = '[';
					}
					else
					{
						if(reflowDwellScreen[27] == ']') //if soak dwell time is selected change selection to start
						{
							reflowDwellScreen[27] = ' ';
							reflowDwellScreen[22] = ' ';
							reflowDwellScreen[77] = ']';
							reflowDwellScreen[71] = '[';
						}
					}
				}
			}
			DrawMenu(REFLOW_DWELL);//redraw reflow dwell menu
			break;
			
			case 'R': //if rotary encoder is rotated right (CW)
			if(reflowDwellScreen[67] == ']')//if back is selected change selection to start
			{
				reflowDwellScreen[67] = ' ';
				reflowDwellScreen[62] = ' ';
				reflowDwellScreen[77] = ']';
				reflowDwellScreen[71] = '[';
			}
			else
			{
				if(reflowDwellScreen[36] == ']')//if reflow dwell time is selected change selection to back
				{
					reflowDwellScreen[36] = ' ';
					reflowDwellScreen[31] = ' ';
					reflowDwellScreen[67] = ']';
					reflowDwellScreen[62] = '[';
				}
				else
				{
					if(reflowDwellScreen[27] == ']')//if soak dwell time is selected change selection to reflow dwell time
					{
						reflowDwellScreen[27] = ' ';
						reflowDwellScreen[22] = ' ';
						reflowDwellScreen[36] = ']';
						reflowDwellScreen[31] = '[';
					}
					else
					{
						if(reflowDwellScreen[77] == ']') //if start is selected change selection to soak dwell time
						{
							reflowDwellScreen[77] = ' ';
							reflowDwellScreen[71] = ' ';
							reflowDwellScreen[27] = ']';
							reflowDwellScreen[22] = '[';
						}
					}
				}
			}
			DrawMenu(REFLOW_DWELL);//redrawe reflow dwell menu
			break;
		}
		break;
		
		case '1': //if in edit mode
		switch(action)
		{
			case 'A': //if rotary encoder button is pressed
			edit = '0'; //exit edit mode
			break;
			
			case 'L'://if rotary encoder is rotated left (CCW)
			if(reflowDwellScreen[36] == ']')//if reflow dwell time is selected
			{
				if(reflowDwell == 0)//if reflow dwell = 0
				{
					reflowDwell = 200;//reset reflow dwell to 200
				}
				else
				{
					reflowDwell--;//decrement reflow dwell
					sprintf(intBuffer, "%d", reflowDwell);//stringify
					//replace old value with new one
					reflowDwellScreen[32] = intBuffer[0];
					reflowDwellScreen[33] = intBuffer[1];
					reflowDwellScreen[34] = intBuffer[2];
				}
			}
			else
			{
				if(reflowDwellScreen[27] == ']')//if soak dwell is selected
				{
					if(soakDwell == 0) //if soak dwell = 0
					{
						soakDwell = 200;//reset soak dwell to 200
					}
					else
					{
						soakDwell--; //decrement soak dwell
						sprintf(intBuffer, "%d", soakDwell);//stringify
						//replace old value with new one
						reflowDwellScreen[23] = intBuffer[0];
						reflowDwellScreen[24] = intBuffer[1];
						reflowDwellScreen[25] = intBuffer[2];
					}
				}
			}
			DrawMenu(REFLOW_DWELL);//redraw reflow dwell menu
			break;
			
			case 'R'://if rotary encoder is rotated right (CW)
			if(reflowDwellScreen[36] == ']')//if reflow dwell is selected
			{
				if(reflowDwell == 200)//if reflow dwell = 200
				{
					reflowDwell = 0;//reset reflow dwell to 0
				}
				else
				{
					reflowDwell++;//increment reflow dwell
					sprintf(intBuffer, "%d", reflowDwell);//stringify
					//replace old value with new one
					reflowDwellScreen[32] = intBuffer[0];
					reflowDwellScreen[33] = intBuffer[1];
					reflowDwellScreen[34] = intBuffer[2];
				}
				
			}
			else
			{
				if(reflowDwellScreen[27] == ']')//if soak dwell is selected
				{
					if(soakDwell == 200)//if soakDwell = 200
					{
						soakDwell = 0;//reset soakDwell to 0
					}
					else
					{
						soakDwell++;//increment soak dwell
						sprintf(intBuffer, "%d", soakDwell);//stringify
						//replace old value with new one
						reflowDwellScreen[23] = intBuffer[0];
						reflowDwellScreen[24] = intBuffer[1];
						reflowDwellScreen[25] = intBuffer[2];	
					}
				}
			}
			break;
		}
		DrawMenu(REFLOW_DWELL);//redraw reflow dwell screen
		break;
	}
}


/***********************************************************************************
Functiuon Name: ReflowStatusMenu()
Description:  This function determine what action should be taken when on ReflowStatusMenu
Accepts: char
returns: nothing
***********************************************************************************/
void ReflowStatusMenu(char action)
{
	switch(action) //determine input action
	{
		case'A'://if rotary encoder button is pressed.
		DrawMenu(HOME); //move to home menu
		break;
		
		case 'L':
		//this will probably stay empty
		break;
		
		case 'R':
		//this will probably stay empty
		break;
	}
}


/***********************************************************************************
Functiuon Name: ReflowCalibrateMenu()
Description:  This function presents two calibration options to the user
Accepts: char
returns: nothing
***********************************************************************************/
void ReflowCalibrateMenu(char action)
{
	switch(action)
	{
		case 'A':
		if(calibrateScreen[78] == ']')
		{
			DrawMenu(CALIBRATE_PROBES);
		}
		else
		{
			if(calibrateScreen[67] == ']')
			{
				DrawMenu(CALIBRATE_REFLOW);
			}
		}
		break;
		
		case 'L':
		if(calibrateScreen[78] == ']')
		{
			calibrateScreen[78] = ' ';
			calibrateScreen[71] = ' ';
			calibrateScreen[67] = ']';
			calibrateScreen[62] = '[';
		}
		else
		{
			if(calibrateScreen[67] == ']')
			{
				calibrateScreen[67] = ' ';
				calibrateScreen[62] = ' ';
				calibrateScreen[78] = ']';
				calibrateScreen[71] = '[';
			}
		}
		DrawMenu(REFLOW_CALIBRATE);
		break;
		
		case 'R':
		if(calibrateScreen[78] == ']')
		{
			calibrateScreen[78] = ' ';
			calibrateScreen[71] = ' ';
			calibrateScreen[67] = ']';
			calibrateScreen[62] = '[';
		}
		else
		{
			if(calibrateScreen[67] == ']')
			{
				calibrateScreen[67] = ' ';
				calibrateScreen[62] = ' ';
				calibrateScreen[78] = ']';
				calibrateScreen[71] = '[';
			}
		}
		DrawMenu(REFLOW_CALIBRATE);
		break;
	}
}


/***********************************************************************************
Functiuon Name: CalibrateProbesMenu()
Description:  This function runs the probe calibration screen
Accepts: char
returns: nothing
***********************************************************************************/
void CalibrateProbesMenu(char action)
{
	switch(action)
	{
		case 'A':
		if(calibrateProbeScreen[76] == ']')
		{
			switch(calibrateProbeScreen[52])
			{
				case '1':
				calibrateProbeScreen[52] = '2';
				//todo assign adc value to probe 1 0C for algorithm use later.
				DrawMenu(CALIBRATE_PROBES);
				break;

				case '2':
				calibrateProbeScreen[52] = '3';
				//todo assign adc value to probe 2 0C for algorithm use later.
				DrawMenu(CALIBRATE_PROBES);
				break;

				case '3':
				calibrateProbeScreen[52] = '4';
				//todo assign adc value to probe 3 0C for algorithm use later.
				DrawMenu(CALIBRATE_PROBES);
				break;

				case '4':
				calibrateProbeScreen[52] = '5';
				//todo assign adc value to probe 4 0C for algorithm use later.
				DrawMenu(CALIBRATE_PROBES);
				break;

				case '5':
				//todo assign adc value to probe 5 0C for algorithm use later
				DrawMenu(REFLOW_SETUP);
				break;
			}
		}
		else
		{
			if(calibrateProbeScreen[67] == ']')
			{
				switch(calibrateProbeScreen[52])
				{
					case '5':
					calibrateProbeScreen[52] = '4';
					DrawMenu(CALIBRATE_PROBES);
					break;

					case '4':
					calibrateProbeScreen[52] = '3';
					DrawMenu(CALIBRATE_PROBES);
					break;

					case '3':
					calibrateProbeScreen[52] = '2';
					DrawMenu(CALIBRATE_PROBES);
					break;

					case '2':
					calibrateProbeScreen[52] = '1';
					DrawMenu(CALIBRATE_PROBES);
					break;

					case '1':
					DrawMenu(REFLOW_CALIBRATE);
					break;
				}
			}
		}
		break;
		
		case 'L':
		if(calibrateProbeScreen[76] == ']')
		{
			calibrateProbeScreen[76] = ' ';
			calibrateProbeScreen[71] = ' ';
			calibrateProbeScreen[67] = ']';
			calibrateProbeScreen[62] = '[';
		}
		else
		{
			if(calibrateProbeScreen[67] == ']')
			{
				calibrateProbeScreen[67] = ' ';
				calibrateProbeScreen[62] = ' ';
				calibrateProbeScreen[76] = ']';
				calibrateProbeScreen[71] = '[';
			}
		}
		DrawMenu(CALIBRATE_PROBES);
		break;
		
		case 'R':
		if(calibrateProbeScreen[76] == ']')
		{
			calibrateProbeScreen[76] = ' ';
			calibrateProbeScreen[71] = ' ';
			calibrateProbeScreen[67] = ']';
			calibrateProbeScreen[62] = '[';
		}
		else
		{
			if(calibrateProbeScreen[67] == ']')
			{
				calibrateProbeScreen[67] = ' ';
				calibrateProbeScreen[62] = ' ';
				calibrateProbeScreen[76] = ']';
				calibrateProbeScreen[71] = '[';
			}
		}
		DrawMenu(CALIBRATE_PROBES);
		break;
	}
}

/***********************************************************************************
Functiuon Name: CalibrateOvenMenu()
Description:  This function runs the oven learning page
Accepts: char
returns: nothing
***********************************************************************************/
void CalibrateOvenMenu(char action)
{
	switch(action)
	{
		case 'A':
		break;
		
		case 'L':
		break;
		
		case 'R':
		break;
	}
}
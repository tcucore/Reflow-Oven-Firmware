/*
 * File: Abstractions.c
 * Created: 1/28/2017 8:27:43 PM
 * Author: Tcucore  
 * Description:  This file contains abstractions of common
 * tasks for the purpose of making this program more human readable.
 */ 

#include <avr/io.h>

//Menu Codes
#define HOME '0'
#define EXPOSURE '1'
#define REFLOW '2'
#define EXPOSURE_STATUS '3'
#define EXPOSURE_SETUP '4'
#define REFLOW_STATUS '5'
#define REFLOW_SETUP '6'
#define REFLOW_DWELL '7'


 //LDC Module CONTROL BITS
 #define COMMAND 0
 #define DATA 1
 #define READ 1
 #define WRITE 0
 #define LCD_ENABLE 1
 #define LCD_DISABLE 0
 #define REGISTER_SELECT 0 
 #define READ_WRITE 1
 #define ENABLE 2
 #define UPPER 1
 #define LOWER 0

 
 //LCD Module Commands
 #define CLEAR_DISPLAY 0x01
 #define RETURN_HOME 0x02
 #define LINE_1 0x46


//Address of LCD Module 
 #define LCD_MODULE 0x3F


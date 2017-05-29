/*
 * Protos.h
 *
 * Created: 1/28/2017 8:23:29 PM
 *  Author: Tcucore
 */ 



//functions interrupts
void CheckBusyFlag(void);
void InitializeDevice(void);
void Direction(void);
void Counts(void);
void CheckDirection(void);
void Accept(void);
void CW(void);
void CCW(void);
void CheckButton(void);

//Menu System
void HomeMenu(char);
void ReflowMenu(char);
void ReflowSetupMenu(char);
void ReflowDwellMenu(char);
void ReflowStatusMenu(char);
void ExposureMenu(char);
void ReflowCalibrateMenu(char);
void CalibrateProbesMenu(char);
void CalibrateOvenMenu(char);


//TWI Control Functions
void TWI_Start(void);
void TWI_Stop(void);
void TWI_Write(unsigned char);
void TWI_Read(unsigned char);
void TWI_Data(unsigned char);

//LCD Control Functions
void SendMessage(unsigned char, unsigned char, char*, unsigned char);
void ReadMessage(unsigned char, unsigned char, unsigned char);
unsigned char BuildPacket(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
void DrawMenu(char);
void HomeCursor(void);
void InputAction(char);

//Initialization Functions
void InterruptInit(void);
void PortInit(void);
void TwiInit(void);
void LcdInit(void);
#include <c8051F060.h>

#define LED_DISPLAY_PORT 	P2
#define DATABUS_PORT		P7

void LatchDataBusValue (unsigned char);
void WatchDog_Init (void);
void Timer3_Init(void);
void PORT_Init (void);

sfr16 RCAP3		= 0xCA;               // Timer3 reload value register
sfr16 TMR3     	= 0xCC;               // Timer3 counter register

sbit D_FLIP_FLOP_CLK = P5^6;

bit	flaga = 0;

unsigned char DisplayPortValue = 0xFE;

unsigned char displayData[5] = {0x7F, 0x3F, 0x39, 0x6D, 0x06}; 

static unsigned int DisplayPosition = 5; 

int main (void)
{
	WatchDog_Init ();
	Timer3_Init();
	PORT_Init ();

	EA = 1; 				//wlaczenie obslugi przerwan

	LatchDataBusValue (0x00);

	LED_DISPLAY_PORT = DisplayPortValue;

	SFRPAGE = CONFIG_PAGE;

	while(1);

	//return 0;
}

//------------------------------------------------------------------------------------
// PORT_Init
//------------------------------------------------------------------------------------
//
// Configure the Crossbar and GPIO ports
//

void LatchDataBusValue (unsigned char DataBusValue)
{
	unsigned char old_SFRPAGE;
	char	i = 0;

    old_SFRPAGE = SFRPAGE;
	SFRPAGE = CONFIG_PAGE;
	
	DATABUS_PORT = DataBusValue;

	D_FLIP_FLOP_CLK = 0;
    for (i = 0; i < 4; i++); 	
	D_FLIP_FLOP_CLK = 1;

	SFRPAGE = old_SFRPAGE;		
	return;
}


void WatchDog_Init (void)
{
 	WDTCN = 0xde;	// disable watchdog timer
 	WDTCN = 0xad;	
	return;
}

void PORT_Init (void)
{
    SFRPAGE   = CONFIG_PAGE;
    XBR2      = 0x40;			// Enable crossbar and weak pull-ups
	return;
}

void Timer3_Init(void)
{
	SFRPAGE   = TMR3_PAGE;
	RCAP3     = 0;     			// Init reload values
   	TMR3      = 0xffff;         // Set to reload immediately
	TMR3CN    = 0x04;
   	TMR3CF    = 0x00;
   	EIE2     |= 0x01;           // Enable Timer3 interrupts
	SFRPAGE   = CONFIG_PAGE;
	return;
}

void TIMER3_ISR (void) interrupt 14
{
	unsigned char old_SFRPAGE;

	TF3 = 0; 					 // Clear interrupt flag
    old_SFRPAGE = SFRPAGE;
	SFRPAGE   = CONFIG_PAGE;

	if (DisplayPosition == 5) 
	{
		DisplayPortValue = 0xFE;
		DisplayPosition = 0;
	}
	else
	{ 
		DisplayPortValue = (DisplayPortValue << 1) | (DisplayPortValue >> 7); 
	}	 
	
	LED_DISPLAY_PORT = DisplayPortValue; 
	LatchDataBusValue(displayData[DisplayPosition]); 
	SFRPAGE = old_SFRPAGE; 
	DisplayPosition++;
	return;
}

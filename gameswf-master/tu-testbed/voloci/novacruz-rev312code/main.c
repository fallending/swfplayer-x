/*
Project : X50Cpu
Version : 1.01
Date    : 1/9/2002
Author  : Jeff Greason                    
Company : True Logic
Comments: 


Chip type           : AT90S8535
Clock frequency     : 4.000000 MHz
Memory model        : Small
Internal SRAM size  : 512
External SRAM size  : 0
Data Stack size     : 128
*********************************************/

#include <90s8535.h>  

// Standard Input/Output functions
#include <stdio.h>      
#include <string.h>
#include <math.h>                       
                  
                  
//#define __48VOLTS	
#define __REV_3_95A


//#define _DEBUG_FREQ_OUTPUT

#define _SOFTWARE_VERSION "\015\012V100 R3.12\015\012" 
#define __CURRENT_SCALE_FACTOR	1.0

#ifdef __48VOLTS 
	#define	__NEW_NIMH_VOLTAGE	5225
	#define __NEW_SLA_VOLTAGE	5092
	#define __MIN_STARTUP_VOLTAGE	4400
	#define __LOW_BATTERY_VOLTAGE	3750
	#define __MAX_WATT_SECONDS 	1866000
	#define __USED_WATT_SECONDS	1492000
	#define __DEFAULT_WATT_SECONDS	380000
	#define __ADC0_DIV_FACTOR	0.186

#else
	#define __NEW_NIMH_VOLTAGE     3920
	#define __NEW_SLA_VOLTAGE      3820
	#define __MIN_STARTUP_VOLTAGE	3300
	#define __LOW_BATTERY_VOLTAGE	2800
	#define __MAX_WATT_SECONDS 	1400000
	#define __USED_WATT_SECONDS	1120000
	#define __DEFAULT_WATT_SECONDS	280000
	#define __ADC0_DIV_FACTOR	0.2046
#endif


#define TIMER_FREQ     		29440 
#define TIMER_HALF_SECOND     14720  
#define TIEMR_LED_SCAN		3000
#define TIMER_LONG_BLINK       22080 
#define AUTO_SHUTOFF_TIME	120
                                          


#define COM_SEQ_FREQ	1275   // 8 com outputs / second...
#define _CALC_SECOND	15
#define _MAX_LOW_VOLTAGE_TIME	80
                          
#pragma warn-     


//eeprom int LastBattVoltage = 0;
eeprom long PowerWattSeconds[4] = { __DEFAULT_WATT_SECONDS, __DEFAULT_WATT_SECONDS, __DEFAULT_WATT_SECONDS, __DEFAULT_WATT_SECONDS };

#pragma warn+
        
//char szVerStr[19] = { _SOFTWARE_VERSION };
unsigned char SOC_LEDS[6] = { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f };
int DischargePoints[5][5] = {
{ 3890, 3740, 3650, 3530, 3410 },
{ 3640, 3550, 3490, 3340, 3220 },
{ 3440, 3410, 3320, 3170, 2960 },
{ 3280, 3250, 3160, 3010, 2800 },
{ 3140, 3110, 2990, 2780, 2630 }
};

char BatteryPowerLow;


//char eeprom *eepptr;
//char *eepptr;  
//char *tptr;

// Gear ratios
// Motor 11
// Wheel 45
// Tire  68.5" around
// Interupt freq 31.25 Khz
// Halls / Rotation 4:1
           
#define	__PWM_RES_TOTAL 5 

#define _LED_DATA_MSK	0x04;
#define _LED_CLK_MSK	0x08;
//#define _LED_SEL_MSK	0x10;

         
            
#define TCOUNT_0	 78 
#define TCOUNT_1	65535 - 28 //  10Khz interupt rate... __PWM_RES_TOTAL

#define _COM_BUFFER_SIZE	15              
unsigned char ComStr[ _COM_BUFFER_SIZE + 5 ];
unsigned char ComRecBuff[ _COM_BUFFER_SIZE + 1 ];   
//unsigned char ComRecStr[ _COM_BUFFER_SIZE + 1 ];

unsigned char RecBuffPtr; 
unsigned char RecByteRdy;
 

//unsigned char PhaseOutREV[8] = { 0x07, 0x26, 0x0d, 0x25, 0x13, 0x16, 0x0b, 0x07 };
unsigned char PhaseOutFWD[8] = { 0x07, 0x0b, 0x16, 0x13, 0x25, 0x0d, 0x26, 0x07 };
//unsigned char PhaseSeq[8] = { 0, 3, 6, 2, 5, 1, 4, 0 };
unsigned char PhasePtr;
unsigned char PhaseErr;

unsigned char OvrTemp;
unsigned char CruiseMode;
                                                                                    
long MaxWattSeconds; // = 1400000; //1425000; 

unsigned char ActiveOut[8];

void WriteDataToConsoleLEDS( unsigned char cdata );

int AdcPWMVal;           
int AdcTempVal;
unsigned char HeatSinkTemp;    
 
unsigned char BatteryCurrent;
unsigned int BatteryVoltage; 
unsigned int AvgBatteryVoltage;
unsigned int LowBatteryVoltage;

//unsigned char PhaseCurrent;
char AutoUpdate;
int iMaxEconoSpeed;   
int iPhaseZero;
int iPhaseCurrent;
int iPhaseZeroPoints[ 6 ];


unsigned char cPWMVal;
unsigned char OldcPWMVal;

unsigned char hSensors;  
unsigned char prevhSensors;  



unsigned char LEDbyte; 
unsigned char OldLEDbyte;
unsigned char calcLEDbyte;
unsigned char LEDStatus;

unsigned char iTimerSOC;  // state of charge timer...

long CurrentWattSeconds;    
long SavedWattSeconds;
           
     
     
//unsigned char phase_drive;
long itrDelay;
char isecTmr;
int isecTimer;
char isecTmrWatts;
                    
unsigned char DriveEnable; 
                         
unsigned int iMotorTimer; // Timer for Speed measurement Timer Routine
unsigned int iMotorSpeed; // Raw counts for Timer Routine
int iMotorRPM; //
//int iBikeMPH;                 
unsigned char CurrentLimitByte;
  
char ComSequence;
float vtmp, ftmp;

int itmp;
unsigned char tmpChar = 0;            
unsigned char tmpPWMVal;


//------------------------------------------------------
//  Function Declarations
//------------------------------------------------------

unsigned int read_adc(unsigned char adc_input);   
void ComSend( char *UART_Str );    
unsigned char CalcStateOfChargeForConsole( void );

                          
unsigned char ConvertTempSensor( int ADCData );
void CalcCruiseControl( void );
void OutputComData( void );


void ProcessCommand( char *pRecStr );
void SendCRLFPrompt( void );
              
void SaveWattSecondsToEPPROM( void );
void ReadWattSecondsFromEEPROM( void );

int ReadBatteryVoltage( void );

void ShutdownSystem( void );


void ShutdownSystem( void )
{       
	char tmpChar;
	cPWMVal = 0;  // turn throttle off and wait to power down...
	
	tmpChar = 0;   
	while( 1 )
	{
	  	WriteDataToConsoleLEDS( 0x01 );	
	  	
		itrDelay = 0;
		while( itrDelay < TIMER_LONG_BLINK ) 
		{
		}
		WriteDataToConsoleLEDS( 0x00 );	
	  	
	  	itrDelay = 0;
		while( itrDelay < TIMER_LONG_BLINK ) 
		{
		}
		tmpChar++;
		            
		if( tmpChar > 5 )
		{
			cPWMVal = 0;
			PORTD = 0x20;	
		}
	}	

}

int ReadBatteryVoltage( void )
{
	AdcTempVal = read_adc( 0 );
	ftmp = AdcTempVal;
      	vtmp = ftmp / __ADC0_DIV_FACTOR;
      	return( (int)vtmp );
}


void SaveWattSecondsToEPPROM( void )
{	
	// Read Battery Voltage      
	AdcTempVal = read_adc( 0 );
	ftmp = AdcTempVal;
      	vtmp = ftmp / __ADC0_DIV_FACTOR;
	
	if( vtmp < 2700 )
		return;
	                       
	if( CurrentWattSeconds < 50 )
		CurrentWattSeconds = 50;
		
 	PowerWattSeconds[0] = CurrentWattSeconds;
 	PowerWattSeconds[1] = CurrentWattSeconds;
 	PowerWattSeconds[2] = CurrentWattSeconds;
 	PowerWattSeconds[3] = CurrentWattSeconds; 		
}     
void ReadWattSecondsFromEEPROM( void )
{
 	// read the values and compare them to eachother...
	
	if( PowerWattSeconds[2] == PowerWattSeconds[0] )
	{
		SavedWattSeconds = PowerWattSeconds[0];
        }
        else if( PowerWattSeconds[3] == PowerWattSeconds[1] )
        {
		SavedWattSeconds = PowerWattSeconds[1];
        }                                            
        else
        {
        	SavedWattSeconds = __DEFAULT_WATT_SECONDS;
        }
        
        if( SavedWattSeconds > MaxWattSeconds )
        {
         	// bad reading...
         	SavedWattSeconds = __DEFAULT_WATT_SECONDS;
        }                        
        if( SavedWattSeconds < 0 )
        {
        	SavedWattSeconds = __DEFAULT_WATT_SECONDS;
	}
                                       
}

// ComSend waits til the string is sent before returning 
void ComSend( char *UART_Str )
{
	int ptr, tilt;
  	tilt = 0;
	ptr = 0;
	while( UART_Str[ptr] != 0 )
	{  	
     		UDR = UART_Str[ptr];
     		ptr++; 
     		while(( USR & 0x40 ) == 0 )
     		{               
     			itrDelay = 0;
  			while( itrDelay < 10 ) 
			{
			}
	
	 		//delay_it( 100 );
     		}   
      		//delay_it( 200 );     	
		itrDelay = 0;
		while( itrDelay < 20 ) 
		{
		}
    }
    while(( USR & 0x40 ) == 0 )
    {             
    		itrDelay = 0;
		while( itrDelay < 20 ) 
		{
		}
		//delay_it( 100 );    	     	
    }
}

interrupt [UART_RXC] void UART_IntReceived(void) 
{
    unsigned char newchar;
    newchar = UDR; 
 
        if (USR & 0x18) newchar = 0xff;              
        ComRecBuff[RecBuffPtr] = newchar; 
		RecBuffPtr++; 
       
        if (newchar == 0x0A)
        {          
        	ComRecBuff[RecBuffPtr] = 0;   

           	RecByteRdy = RecBuffPtr;
        	RecBuffPtr = 0;
        }
}


// Timer 1 overflow interrupt service routine
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{                             
	unsigned char pcbyte;
		
	TCNT1=TCOUNT_1;

#ifdef _DEBUG_FREQ_OUTPUT
	PORTD = 0x04;
#endif

	// save old value
	prevhSensors = hSensors;
	
	// scan for new  HALL sensors
	hSensors = PINB & 0x07;
//	hSensors = hSensors & 0x07;
    	
        pcbyte = ActiveOut[ hSensors ];
        
	if( prevhSensors != hSensors )
	{
        	pcbyte = pcbyte | 0x07;
	}
        PORTC = pcbyte;
	PORTC = pcbyte + 0x40;
	PORTC = 0x00;

	PORTC = cPWMVal;    
	PORTC = cPWMVal + 0x80;
	PORTC = 0x00;
  	
	// do freq measurement here
	
	if( iMotorTimer < TIMER_FREQ )
	{ 
		iMotorTimer++;               		
	}
	else
	{
	   	iMotorSpeed = iMotorTimer;
	}
	if((( prevhSensors & 0x01 ) == 0 )&&(( hSensors & 0x01 ) == 1 )) 
	{
	   // hall A just went high again...
		iMotorSpeed = iMotorTimer;
		iMotorTimer = 0;   
	}
	itrDelay++;

#ifdef _DEBUG_FREQ_OUTPUT
	PORTD = 0x00;
#endif

}



#define ADC_VREF_TYPE 0x00
// Read the ADC conversion result
unsigned int read_adc(unsigned char adc_input)
{      
	int ret = 0; //, av[4];
	char indx;
	
	for( indx = 0; indx <= 7; indx++ )
	{	
		ADMUX=adc_input|ADC_VREF_TYPE;
		ADCSR|=0x40;
		while ((ADCSR&0x10)==0);
		ADCSR|=0x10;
		ret+= ADCW;
	}
	ret = ret >> 3;     
	return ret;	
}


void main( void )
{                             
 
	PhasePtr = 1;
	PhaseErr = 0;    
	iTimerSOC = 0;
    
	// Declare your local variables here

	// Port A
	PORTA=0x00;
	DDRA=0x00;

	// Port B
	PORTB=0x00;
	DDRB=0x00;

	// Port C
	PORTC=0x00;
	DDRC=0xFF;

	// Port D
	PORTD = 0x00;
	DDRD=0xfc;

	// Timer/Counter 0 initialization
	//TCCR0=0x04;  //    CLK / 64
	//TCNT0=0x00;

	// Timer/Counter 1 initialization
	TCCR1A=0x00;
	TCCR1B=0x02; // CLK / 8
	TCNT1H=0xff;
	TCNT1L=0x00;

	// External Interrupt(s) initialization
	GIMSK=0x00;
	MCUCR=0x00;

	// Timer(s)/Counter(s) Interrupt(s) initialization
	TIMSK=0x04;

	// UART initialization
	UCR=0x98;   //0x18;
	UBRR=0x19;  //0x0c for 4 MHZ 0x19 for 8MHZ

	// Analog Comparator initialization
	ACSR=0x80;

	// ADC initialization
	ADMUX=ADC_VREF_TYPE;
	ADCSR=0x81;

	// Prep Commutation array for forward motion
	for( itmp = 0; itmp <= 7; itmp++ )
	{
		ActiveOut[ itmp ] = PhaseOutFWD[ itmp ];
	}
                    

	//TCNT0 = TCOUNT_0;
	TCNT1 = TCOUNT_1;

       
    	PORTC = 0;    
	PORTC = PhaseOutFWD[ 0 ];
	PORTC = PhaseOutFWD[ 0 ];

	PORTC = 0x40 | PhaseOutFWD[ 0 ];
	PORTC = 0x40 | PhaseOutFWD[ 0 ];
	PORTC = 0x00;                  
     	
     	// Global enable interrupts

	#asm("sei")

	LEDbyte = 0;
	OldLEDbyte = 0;
	      
	
	ComSequence = 0;
	DriveEnable = 1;
	
	CurrentLimitByte = 0;
	OvrTemp = 0;             
	
	isecTmr = 0;
	isecTimer = 0;  
	isecTmrWatts = 0;
		
	AutoUpdate = 1;
	iMaxEconoSpeed = 1025; 
     	BatteryPowerLow = 0;
	
	//PORTD = 0x00;

	MaxWattSeconds = __MAX_WATT_SECONDS; //1400000;
	
	sprintf( ComStr, _SOFTWARE_VERSION );
	ComSend( ComStr );
	
	// Turn off all lights...
	for( itmp = 0; itmp <= 5; itmp++ )
	{
		WriteDataToConsoleLEDS( SOC_LEDS[ itmp ] );
		itrDelay = 0;
		while( itrDelay < TIEMR_LED_SCAN ) 
		{
		} 
	 	
		//Read Phase Current
		AdcTempVal = read_adc( 1 );  
		iPhaseZero = AdcTempVal;
		AdcTempVal = read_adc( 1 );  
	         
		iPhaseZero = iPhaseZero + AdcTempVal;
		iPhaseZero = iPhaseZero / 2; 
		iPhaseZeroPoints[ itmp ] = iPhaseZero;
	}
	
       
	// Turn on all lights...
	WriteDataToConsoleLEDS( 0x00 );

	//////////////////////////////////////////////////////////////
	// Startup Flash lights... and Check the Battery Voltage...

	// Read Battery Voltage      
	BatteryVoltage = ReadBatteryVoltage();	      	

	itrDelay = 0;
	while( itrDelay < TIMER_HALF_SECOND ) 
	{
		
	}       
	// Read Battery Voltage      
	BatteryVoltage = BatteryVoltage + ReadBatteryVoltage();
	BatteryVoltage = BatteryVoltage / 2;
	                                
	
	//check low voltage on startup...
	// if voltage is too low, disable the bike...
	if( BatteryVoltage < __MIN_STARTUP_VOLTAGE )
	{
		// if battery is weak, shut it off ...                  
		sprintf( ComStr, "Low Voltage\r\n" );
		ComSend( ComStr );
		
     		ShutdownSystem();
     	}                           

//	sprintf( ComStr, "BV,%d,", BatteryVoltage );
  //	ComSend( ComStr );
 	
 	
	// do New cruise control init here...
	CruiseMode = 0;  // deefault is performance mode...
	if(( PINB & 0x08 ) != 0 ) // j5 is shorted economy mode
	{
	 	CruiseMode = 1;
	}
	
	ReadWattSecondsFromEEPROM();          
	CurrentWattSeconds = SavedWattSeconds;	
	// Reset watt / second counting... 
	// calculate the state of charge on the battery...
	if(( PINB & 0x10 ) == 0 ) // j6 is shorted = SLA mode
	{       
        	if(( BatteryVoltage > __NEW_SLA_VOLTAGE )&&( SavedWattSeconds < __USED_WATT_SECONDS ))  // consider this battery new...
		{      
			//Reset the Watt / Second counter here...
	 		CurrentWattSeconds = MaxWattSeconds;
		}
	}
	else
	{
        	if(( BatteryVoltage > __NEW_NIMH_VOLTAGE )&&( SavedWattSeconds < __USED_WATT_SECONDS ))  // consider this battery new...
		{
		 	//Reset the Watt / Second counter here...
		 	CurrentWattSeconds = MaxWattSeconds;		 	
		}	
	}	
	
	LowBatteryVoltage = __LOW_BATTERY_VOLTAGE;		


	// SHOW THE CURRENT STATE OF CHARGE ON THE DISPLAY
	LEDStatus = CalcStateOfChargeForConsole();
	

	calcLEDbyte = SOC_LEDS[ LEDStatus ]; 
	LEDbyte = calcLEDbyte;
	OldLEDbyte = LEDbyte;
		
	WriteDataToConsoleLEDS( LEDbyte );
	
	// start of main loop
	while( 1 )
      	{         
                // timer routine for "real time" clock, calculated second...
		isecTmr++;
		if( isecTmr > _CALC_SECOND )
 		{
			isecTimer++;
			isecTmrWatts++;
				
   			isecTmr = 0;
     		}                   		
		
      		CurrentLimitByte = 0;  // clear out the current limit control
      		if(( PINB & 0x08 ) != 0 ) // j5 is shorted economy mode
		{                
		 	CurrentLimitByte = 0x80;
		 	PhaseErr = 0;
		}

      		// Read The Throttle Demand 
	      	AdcPWMVal = read_adc( 7 );
      		if( AdcPWMVal > 700 )
	      	{
      			AdcPWMVal = 0;
	      	}                           
	      	if( AdcPWMVal > 511 )
	      	{
	      	 	AdcPWMVal = 511;
	      	}
      		ftmp = AdcPWMVal;
	      	vtmp = (float)(512 - ftmp);
      		vtmp = 1 / vtmp;
	      	ftmp = sqrt( vtmp );
      		ftmp = (ftmp * 360) - 16;
		if( ftmp < 0 )
		{
			ftmp = 0;
		}      	
      	
		itmp = (int)ftmp;     	
      	
	      	//itmp = AdcPWMVal>>2;
      		if( itmp > 63  ) //;;__PWM_RES_TOTAL )
      			itmp = 63; //__PWM_RES_TOTAL;
	        
	        //	oldPWMVal = tmpPWMVal;	
      		tmpPWMVal = ( unsigned char )itmp & 0x3f;
      	 
	 	// do the Speed calculations here.
		vtmp = iMotorSpeed;
		ftmp = TIMER_FREQ / vtmp;
		ftmp = ftmp * 15; // 4 halls / rev 60 revs / sec = RPM
		iMotorRPM = (int)ftmp;  

		if( iMotorRPM < 30 )
		{
			iMotorRPM = 0;
		}  
	
	
		//if(( tmpPWMVal > 6 )&&( iMotorRPM < 75 ))  // 
		if( iMotorRPM < 75 )
		{                     
			CurrentLimitByte = 0xc0; //CurrentLimitByte | 0x40;  // set at lowest limit...
		}                         
      	
	      	// Read Battery Voltage      
 	      	AvgBatteryVoltage = AvgBatteryVoltage + ReadBatteryVoltage();
	      	
	      	if(( ComSequence == 0 )||( ComSequence == 4 ))
	      	{
	      	 	BatteryVoltage = ( AvgBatteryVoltage / 4 ) + 25;	      	
	      	 	AvgBatteryVoltage = 0;
		}
		
		// if batter is too low start timer to shut it off...
	      	if( BatteryVoltage < LowBatteryVoltage )
      		{
      	 		DriveEnable++;
	      	}
      		else
	      	{      
      			if( DriveEnable < _MAX_LOW_VOLTAGE_TIME )
			{
	    	 		DriveEnable = 0;
		 	}
      		}	
      	
	      	//Read Phase Current
      		AdcTempVal = read_adc( 1 );     		
	      	iPhaseCurrent = AdcTempVal - iPhaseZeroPoints[ LEDStatus ];
		if( iPhaseCurrent < 0 )
		{
			iPhaseCurrent = 0;
		}
		
	      	ftmp = iPhaseCurrent;
	      	//PhaseCurrent = iPhaseCurrent;
	      	
                if( ftmp > 36 )
                {
                 	ftmp = ftmp * 0.9333;
                }
                BatteryCurrent = ftmp;
	      	if( BatteryCurrent > 52 )
	      	{
	      		BatteryCurrent = 52;
	      	}	      	
	      	if( BatteryCurrent <= 1 )
	      	{
	      		BatteryCurrent = 0;
	      	}
     

		// Caclulate heat sink temperature 
		AdcTempVal = read_adc( 2 ); 
		HeatSinkTemp = ConvertTempSensor( AdcTempVal );
    
    		if(( HeatSinkTemp > 62 )||( OvrTemp == 1 ))
	    	{        
                        OvrTemp = 1;
			CurrentLimitByte = 0xc0;
			if( tmpPWMVal > 27 )
			{
				tmpPWMVal = 27;
//				CurrentLimitByte = 0xc0;  // set at lowest limit...
			}		
    			// too hot... shut it off completely
    			if( HeatSinkTemp > 75 )
	    		{
    				tmpPWMVal = 0;
    			} 
    		}
    		if( HeatSinkTemp < 60 )
    		{
 	 	 	OvrTemp = 0;
    		}

                
    		
    		// do Throttle Reprofileing here
		if(( iMotorRPM > 300 )&&( iMotorRPM < 650 )) //900 ))
		{
		 	ftmp = iMotorRPM - 300;
		 	ftmp = ftmp / 15; // down to 37
		 	tmpChar = 63 - (char)ftmp;
 			if( tmpChar < tmpPWMVal )
 			{
 			 	tmpPWMVal = tmpChar;
 			}	 	
		}        
		if(( iMotorRPM >= 650 )&&( iMotorRPM < 1700 )) //if(( iMotorRPM >= 900 )&&( iMotorRPM < 1400 ))
		{
		 	ftmp = iMotorRPM - 650;  // 900
		 	ftmp = ftmp / 44; //34;
		 	tmpChar = 39 + (char)ftmp;
 			if( tmpChar < tmpPWMVal )
 			{
 			 	tmpPWMVal = tmpChar;
 			}	 	
		}

	      	// Calculate Battery Temperature
     	        /*
  		AdcTempVal = read_adc( 6 ); 
		BatteryTemp = ConvertTempSensor( AdcTempVal ); 	
      	        */
      	                     
      		if( AutoUpdate == 1 )
			OutputComData();	  
	  
		// perform delay for RS-232 output...
		itrDelay = 0;
		while( itrDelay < COM_SEQ_FREQ ) // )
		{
		}
		
		ComSequence++;
		if( ComSequence > 7 )
		{
			ComSequence = 0;
		}	                                                

		CalcCruiseControl();

	 	// Check low voltage here... 
 		if( DriveEnable >= _MAX_LOW_VOLTAGE_TIME )
	 	{
 		   	tmpPWMVal = 0; 
 		   	DriveEnable = _MAX_LOW_VOLTAGE_TIME;
			
			// shut the drive off
			ShutdownSystem();
	 	}      


		// set the throttle to the new value...		
		cPWMVal = tmpPWMVal;
		
		
		// count up the watts used every second and subtract from the total 
		// to calculate the state of charge...
		iTimerSOC++;		
		if( iTimerSOC > 15 )
		{         
    			// Calculate the Watts / 0.5 seconds / power consumption...
    			ftmp = BatteryVoltage / 100;
    			CurrentWattSeconds = CurrentWattSeconds - ((long)( BatteryCurrent ) * ftmp);
                	if( CurrentWattSeconds < 0 )
	                {
        	        	CurrentWattSeconds = 0;
                	} 
    		
		
		 	iTimerSOC = 0;
		 	
			LEDStatus = CalcStateOfChargeForConsole();

			calcLEDbyte = SOC_LEDS[ LEDStatus ];
			
		}
	       	OldLEDbyte = LEDbyte;	
                LEDbyte = calcLEDbyte;

		// make the lowest voltage blink here...
	     	if( calcLEDbyte == 0 )
	     	{       
		     	if( ComSequence < 2 )
	     		{       
	     			LEDbyte = 0x01;
	   		}
	     		else
	     		{
	     			LEDbyte = 0;
	     		}
       		}
       		else if( OvrTemp == 1 )
	     	{       
		     	if(( ComSequence & 0x03 ) == 1 )
	     		{       
	     			LEDbyte = calcLEDbyte;
	     			
				if( calcLEDbyte == 0 )
				{               
	     				LEDbyte = 0x01;
	   			}
	     		}
	     		else
	     		{
	     			LEDbyte = 0;
	     		}
       		}
       		       		                
		if( isecTmrWatts > 10 )
		{
		 	isecTmrWatts = 0;
		 	OldLEDbyte = 0; // update the LEDS every 10 seconds
		 	SaveWattSecondsToEPPROM();		 	
		}
       		       		
		// do led routines here...
	      	if( LEDbyte != OldLEDbyte )
	     	{
     		        // voltage is really low.... blink the red light...
#ifndef _DEBUG_FREQ_OUTPUT
       			WriteDataToConsoleLEDS( LEDbyte );
#endif

	     	}   	

		// Apply Current limit data here...                          
	     	tmpChar = PORTD;
	     	tmpChar = tmpChar & 0x3f;
	     	tmpChar = tmpChar | CurrentLimitByte;
		PORTD = tmpChar;
		
		
		// auto shut down timer and control
		if(( cPWMVal > 0 )&&( iMotorRPM > 0 ))
		{
			isecTimer = 0;
		}
		
		if( isecTimer > AUTO_SHUTOFF_TIME )
		{
  			ShutdownSystem();	
		}
			
        	// check com port and service any command there...
   		itmp = RecByteRdy;
		RecByteRdy = 0;
		if( itmp > 0 )
		{      
			ProcessCommand( ComRecBuff );
			SendCRLFPrompt();		
		}
      };
}                               

void SendCRLFPrompt( void )
{
 	ComStr[0] = 0x0d;
	ComStr[1] = 0x0a;
	ComStr[2] = '>';
	ComStr[3] = 0;
				
	ComSend( ComStr );
}  

void OutputComData( void )
{
		//Prep Com Data 
		ComStr[ 0 ] = 0;
		if( ComSequence == 0 )
		{
	      		if(( PINB & 0x08 ) != 0 ) // j5 is shorted economy mode
			{                
		        	sprintf( ComStr, "Ec-TH,%d,", cPWMVal );
		 	}
		 	else
		 	{
		        	sprintf( ComStr, "Pf-TH,%d,", cPWMVal );
		 	}
		}
		if( ComSequence == 1 )
		{
		       	sprintf( ComStr, " RPM,%d,", iMotorRPM );
		}
		if( ComSequence == 2 )
		{
			sprintf( ComStr, "BV,%d,", BatteryVoltage );
		}
		if( ComSequence == 3 )
		{
			sprintf( ComStr, " BC,%d,", BatteryCurrent );
		}
		if( ComSequence == 4 )
		{
			sprintf( ComStr, "CT,%d,", HeatSinkTemp );
		}
		if( ComSequence == 5 )
		{
			sprintf( ComStr, " PC,%d,", iPhaseCurrent );
		}
		if( ComSequence == 6 )
		{                         
		        tmpChar = hSensors;
			sprintf( ComStr, " HS,%d", tmpChar );          
			//sprintf( ComStr, " SW:%d,", SavedWattSeconds / 1000 );
		}  
		if( ComSequence == 7 )
		{
			//sprintf( ComStr, " CW:%d\r\n", CurrentWattSeconds / 1000 );
			sprintf( ComStr, "\r\n" );
			// isecTimer
		}  

		// Send Com Data
 		ComSend( ComStr );	
	        
}

void CalcCruiseControl( void )
{
	// do cruise control for economy mode here...
	if(( PINB & 0x08 ) != 0 ) // j5 is shorted economy mode
	{                                                  
		if( CruiseMode < 1 )
		{ 
			CruiseMode = 1;
		}
		else if( CruiseMode == 2 )
		{
			// Set new Mode here...    
			if( iMotorRPM > 600 )
			{
				iMaxEconoSpeed = iMotorRPM;
			}
			CruiseMode = 3;		
		}		       
		
		// limit speed if in Economy mode...
		if( iMotorRPM > iMaxEconoSpeed + 50)
		{       
			// reduce the speed of the motor...
		 	if( OldcPWMVal > 12 )	               
		 	{
		 		OldcPWMVal = OldcPWMVal - 1;
		  	}
		 		 		 	
		}
		if( iMotorRPM < iMaxEconoSpeed - 50 )
		{                                   
			// increase the speed of the motor...
			if( OldcPWMVal < tmpPWMVal )
			{       
				itmp = tmpPWMVal - OldcPWMVal;
				if( itmp > 50 )
				{
					OldcPWMVal = tmpPWMVal / 2;
				}
				else
				{       if( itmp > 9 )
					{
					 	 OldcPWMVal = OldcPWMVal + 2;
					}
					else
				 	 	OldcPWMVal = OldcPWMVal + 1;
				}
				if( OldcPWMVal > tmpPWMVal )
				{					
					OldcPWMVal = tmpPWMVal;
				}
			}
		}
		else
		{
			// dont do anything
		}   
		if( tmpPWMVal < OldcPWMVal )
		{
			OldcPWMVal = tmpPWMVal;
		}			
		tmpPWMVal = OldcPWMVal;
	}
	else
	{
		// permormance mode
		if(( CruiseMode == 1 )||( CruiseMode == 3 ))
		{ 
			CruiseMode = 2; 
		}
	
	}
}
               


unsigned char CalcStateOfChargeForConsole( void )
{                               
        long longtmp;
	int ptr = 0; // point to first voltage chart on DischargePoints[][]                    
	ftmp = MaxWattSeconds;
	
 	if( BatteryCurrent > 8 ){ ptr = 1; }
 	if( BatteryCurrent > 16 ){ ptr = 2; }
 	if( BatteryCurrent > 25 ){ ptr = 3; }
 	if( BatteryCurrent > 35 ){ ptr = 4; }
 	
 	// if battery shows as really good then dont do anything...                                  
 	//if( BatteryVoltage > DischargePoints[ ptr ][ 0 ] ) { return( 5 ); } 
 	if( BatteryVoltage > DischargePoints[ ptr ][ 0 ] )
 	{
 		if( BatteryPowerLow > 0 ){ BatteryPowerLow = 0; }	
 	} 
 	
 	if( BatteryVoltage < DischargePoints[ ptr ][ 3 ] )
 	{       
 		vtmp = ftmp * 0.31;
		longtmp = (long)vtmp;
 	 	if( CurrentWattSeconds > longtmp )
 	 	{
 	 		BatteryPowerLow++;
 	 		if( BatteryPowerLow > 30 )
 	 		{
 	 			CurrentWattSeconds = CurrentWattSeconds - 100000;
 	 			BatteryPowerLow = 0;
 	 		} 	
 	 	}
 	} 
 	if( BatteryVoltage < DischargePoints[ ptr ][ 4 ] )
	{
		vtmp = ftmp * 0.18;
		longtmp = (long)vtmp;
 	 	if( CurrentWattSeconds > longtmp )
 	 	{
 	 		BatteryPowerLow++;
 	 		if( BatteryPowerLow > 20 )
 	 		{
 	 			CurrentWattSeconds = CurrentWattSeconds - 100000;
 	 			BatteryPowerLow = 0;
 	 		} 	
 	 	}
  	}
	
	vtmp = ftmp * 0.81;
	longtmp = (long)vtmp;
	if( CurrentWattSeconds > longtmp ){ return(5); }

	vtmp = ftmp * 0.61;
	longtmp = (long)vtmp;
	if( CurrentWattSeconds > longtmp ){ return(4); }

	vtmp = ftmp * 0.41;
	longtmp = (long)vtmp;
	if( CurrentWattSeconds > longtmp ){ return(3); }
	
	vtmp = ftmp * 0.21;
	longtmp = (long)vtmp;
	if( CurrentWattSeconds > longtmp ){ return(2); }

	vtmp = ftmp * 0.051;
	longtmp = (long)vtmp;
	if( CurrentWattSeconds > longtmp ){ return(1); }

	return( 0 );	
}

unsigned char ConvertTempSensor( int ADCData )
{      
 	unsigned char CelsTemp;
	
	if( ADCData < 10 ) { ADCData = 10; } 
	vtmp = ADCData;
	vtmp = vtmp / 204.6;
	//Vin = ADCVal / 204.6
                                  
	ftmp = 10000 * vtmp;
	ftmp = ftmp / ( 5 - vtmp );
	// ftmp is now the resistor value...	
	vtmp = 10000 / ftmp;
	ftmp = log( vtmp ) * 0.0002439;
	vtmp = 0.003354 - ftmp;
	ftmp = 1 / vtmp;
	ftmp = ftmp - 273.15;	
	CelsTemp = (char)ftmp;
	return( CelsTemp );
}



void WriteDataToConsoleLEDS( unsigned char cdata )
{
	unsigned char pdata, indx;
	cdata = cdata & 0x1f;
	cdata = cdata << 3;
	
	for( indx = 0; indx <= 4; indx++ )
	{       
		PORTD = PORTD & 0xe0;            
		// test the MSB for a non zero 
		if(( cdata & 0x80 ) != 0 )
		{
			pdata = 0x04; //_LED_DATA_MSK; // | _LED_CLK_MSK | _LED_SEL_MSK;                
	 	}
	 	else
	 	{
			pdata = 0x00; //_LED_CLK_MSK | _LED_SEL_MSK;                	 	
	 	}                                                           
	 	PORTD = PORTD | pdata; // + _LED_CLK_MSK + _LED_SEL_MSK;
	 	                             
	 	// now clock the data...
	 	
	 	PORTD = PORTD | _LED_CLK_MSK;
	 	
	 	cdata = cdata<<1;	
	}	
}

void ProcessCommand( char *pRecStr )
{
}
/*
void ProcessCommand( char *pRecStr )
{
    char ptr, uch, val; 
    //unsigned char *tptr; 
    long longtmp = 0;
    int multx;
  
	// filter out the Set and Get commands...
    longtmp = 0;
  
 	uch = pRecStr[3] - 0x30;
	// use 4 for low  voltage setting...
	if( uch < 0 ){ uch = 0; }
	if( uch > 3 ){ uch = 3; }

 
	if( pRecStr[0] == 'S' )
	{       
		multx = 1;
		for( ptr = (char)strlen( pRecStr ); ptr >= 3; ptr-- )
		{
			if(( pRecStr[ptr] >= 0x30 )&&( pRecStr[ptr] <= 0x39 ))
			{
				val = pRecStr[ptr] - 0x30;
			 	longtmp = longtmp + ((int)val * (int)multx );
			 	multx = multx * 10;			 					 				 		
			}
		}		
    	
		if( pRecStr[1] == '0' )  // parameter 0 is the max economy speed
	 	{    
                	AutoUpdate = (char)longtmp;
	 	} 

		if( pRecStr[1] == '1' )  // parameter 0 is the max economy speed
	 	{    
                	iMaxEconoSpeed = (int)longtmp;
	 	} 
    	
		if( pRecStr[1] == '2' )
	 	{    
                	MaxWattSeconds = longtmp * 1000;  //MaxWattSeconds
	 	} 
    	
		if( pRecStr[1] == '3' )
	 	{    
                	CurrentWattSeconds = longtmp * 1000;
	 	} 
    	
		if( pRecStr[1] == '4' )
	 	{    
                	//SavedWattSeconds = (int)longtmp ;
	 	} 
		return;	   
	}                              
	if( pRecStr[0] == 'G' )
	{
	
		if( pRecStr[1] == '0' )  // parameter 0 is the max economy speed
	 	{    
                	ComStr[ 0 ] = 0;
	               	sprintf( ComStr, "G0:%d\r\n", AutoUpdate );
	               	ComSend( ComStr );
	 	} 
    	
		if( pRecStr[1] == '1' )
	 	{    
                	ComStr[ 0 ] = 0;
	               	sprintf( ComStr, "G1:%d\r\n", iMaxEconoSpeed );
	               	ComSend( ComStr );
	 	} 
    	
		if( pRecStr[1] == '2' )
	 	{       
	 		longtmp = MaxWattSeconds / 1000;
	 	
                	ComStr[ 0 ] = 0;
	               	sprintf( ComStr, "G2:%d\r\n", (int)longtmp );
	               	ComSend( ComStr );
	  	}	          
    	
		if( pRecStr[1] == '3' )
	 	{
	 		longtmp = CurrentWattSeconds / 1000;
	 	
                	ComStr[ 0 ] = 0;
	               	sprintf( ComStr, "G3:%d\r\n", (int)longtmp );
	               	ComSend( ComStr );
		}
		if( pRecStr[1] == '4' )
	 	{            
	       		longtmp = SavedWattSeconds / 1000;	
                 	ComStr[ 0 ] = 0;
	               	sprintf( ComStr, "G4:%d\r\n", longtmp );
	               	ComSend( ComStr );
	 	} 
 		if( pRecStr[1] == '5' )
	 	{    
                 	ComStr[ 0 ] = 0;
	               	sprintf( ComStr, "G5:%d\r\n", iPhaseZeroPoints[ LEDStatus ] );
	               	ComSend( ComStr );
	 	} 	 	
	 	
	 	
	}          
	
	ComStr[0] = 0x0d;
	ComStr[1] = 0;
 	ComSend( ComStr );			
  	                    
}
*/
/*
void SendDataToCom( char eepindx, long data, char prec )
{
	char stimer;
	char *chptr;
	//stimer = tcounter;
	         
	eepptr = &szEppString[ eepindx ][0];

 	chptr = ComStr;
  	while(*eepptr != 0)
   	{             
   		*chptr = *eepptr;
   		chptr++;
   		eepptr++;
   	}
   *chptr = *eepptr;
   ComSend( ComStr );	  
	
	// provide a delay here...				 		
 //	while( stimer == tcounter )
 //	{
 //	}
 //	stimer = tcounter;
	delay_it( 10000 );
	
	FormatString( ComStr, prec, data );
	ComSend( ComStr ); 	
//	while( stimer == tcounter )
//	{
//	}
delay_it( 1000 );
} 		 		

char FormatString( char *szOutput, char Precision, long data )
{
 	char indx = 0;
 	char cptr = 9;
 	char szNewStr[12];
	long ltmp = 0;
 	
 	// value is negative...
 	if( data < 0 )
 	{
 	 	szOutput[indx] = '-';
 	 	indx++;
 	 	data = 0 - data;
 	}  	
	while(( data != 0 )||(( 9 - cptr ) < Precision + 2 ))
 	{              
		ltmp = data / 10;
		szNewStr[cptr] = (char)( data - (ltmp * 10)) + 0x30;
		cptr--;
		if( (9 - cptr ) == Precision )
		{
			szNewStr[cptr] = '.';
			cptr--;
		}
		data = ltmp;
 	}
	cptr++;
 	while( cptr <= 9 )
 	{ 
		szOutput[ indx ] = szNewStr[cptr];
		cptr++;
		indx++;
	}  
	szOutput[ indx ] = 0;
 	return( indx );	
}
*/


// Timer 0 overflow interrupt service routine 
/*
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{                      
	// 	Place your code here 
	//  TCOUNT_0;
	//	OCR1
}
*/
/*
 	if( BatteryCurrent > 8 ){ ptr = 1; }
 	if( BatteryCurrent > 16 ){ ptr = 2; }
 	if( BatteryCurrent > 25 ){ ptr = 3; }
 	if( BatteryCurrent > 35 ){ ptr = 4; }
 	                                  
 	if( BatteryVoltage > DischargePoints[ ptr ][ 0 ] ) { return( 5 ); } //0x1f ); }
 	if( BatteryVoltage > DischargePoints[ ptr ][ 1 ] ) { return( 4 ); } //0x0f ); }
 	if( BatteryVoltage > DischargePoints[ ptr ][ 2 ] ) { return( 3 ); } //0x07 ); }
 	if( BatteryVoltage > DischargePoints[ ptr ][ 3 ] ) { return( 2 ); } //0x03 ); }
 	if( BatteryVoltage > DischargePoints[ ptr ][ 0 ] ) { return( 1 ); } //0x01 ); } 	

*/
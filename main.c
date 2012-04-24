/*
 * Author: Ken Chida
 * Date: 20JUN2008
 *
 * Description: This is a simple controller for an altitude immersion pressure chamber.
 *
 * Note: "centiPSI == 0.01 pounds per square inch"  Don't ask! :)
 */

#include <stdint.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer.h"
#include "my_io.h"

/* Params */
#define NUM_CYCLES			2
#define MAX_HI_PRESS		1280  // centiPSI
#define MIN_HI_PRESS		1200  // centiPSI
#define MAX_LO_PRESS		50	  // centiPSI
#define PUMP_TIME			300   // seconds
#define BLEED_TIME			180   // seconds
#define DWELL_TIME			120   // seconds (for both vacuum and ambient cycles)

/* Global Variables */
uint16_t second = 0;
uint16_t pressure = 0;   // centiPSI

/* Function Prototypes */
int error(int);

int main(void)
{
	/* Init */
	int pump_timer = 0;
	int i = 0;
	uint16_t tzero = 0;

	DDRD = 0xff;		// initialize port B for output LEDs
	PORTD = 0xff;		// all LEDs initially OFF

	init_timer2();
	initIO();
	sei();
	
	for (int j=0; j < NUM_CYCLES; j++) {

		/* Retain for testing: Use to test delay function */
		//Delay(120);
		//while(1) PORTD=0;
		
		/* Retain for testing: use to cycle pressure read */
		//while(1)
		//read_pressure();
		
		read_pressure();
		close_valve();
		
		tzero = second;   // start time for each section of test
		                  // 'second' is a global RTC variable
		
		/*---- Initial pump cycle ----*/
		while ((second - tzero) < PUMP_TIME && pressure < MAX_HI_PRESS) {	

			read_pressure();
			start_pump();
			
			if (pressure > (MAX_HI_PRESS + 200))   // overpressure
				while (error(1));
		}		

		stop_pump();
	
		if (pressure < MAX_HI_PRESS)   // error if target pressure no reached
			while (error(2));

		/*---- Vacuum dwell starts here ----*/
		tzero = second;

		while ((second - tzero) < DWELL_TIME) {

			read_pressure();
			
			if (pressure > (MAX_HI_PRESS + 200))  //overpressure
				while (error(1));

			if (pressure < MIN_HI_PRESS) {

				pump_timer = 0;
				
				while (pressure < MAX_HI_PRESS && ((second - tzero) < DWELL_TIME)) {

					start_pump();
					read_pressure();
					
					for (i=0; i<1000; i++)    // 1 sec busy-wait 
					    delay_ms(1.0);
					
					pump_timer++;
					
					if (pump_timer >= 120)    // don't run pump 120 seconds
						while (error(3));

					if (pressure > (MAX_HI_PRESS + 200))  //overpressure
						while (error(1));
				}

				stop_pump();
			}
		}
		
		stop_pump();
		
		/*---- Bleed cycle start ----*/
		open_valve();
		tzero = second;
		
		while ((second - tzero) < BLEED_TIME && pressure > MAX_LO_PRESS)
			read_pressure();

		if (pressure > MAX_LO_PRESS)
			while (error(4));

		/*---- Ambient pressure dwell ----*/
		delay(DWELL_TIME);
	
		//lower = pressure & 0x00ff;
		//upper = (pressure & 0xff00)>>8;
		//PORTD = lower;
	
		//for (i=0; i<1000; i++) _delay_ms(1.0);
	
		//Delay(1);
		//PORTD = upper;
		//for (i=0; i<1000; i++) _delay_ms(1.0);
		//while(1);
	}
	
	while(1)             // completed successfully without errors! 
	    error(0);

	return 0;
}


int error(int error_type)
{
    /*---- Stop pump if error occurs ----*/
	switch(error_type)
	{
    	case 0:                // success! all LEDs on.
			PORTD = 0x00;
			stop_pump();
			close_valve();
			break;
		
		case 1:		           // overpressure - LED0 on
			PORTD = ~(0x01);
			stop_pump();
			close_valve();
			break;
		
		case 2:		           // cannot reach adequate pressure - LED1
			PORTD = ~(0x02);
			stop_pump();
			close_valve();
			break;
			
		case 3:			       // cannot sustain adequate pressure - LED2
			PORTD = ~(0x04);
			stop_pump();
			close_valve();
			break;

		case 4:			       // cannot bleed vacuum bell - LED3
			PORTD = ~(0x08);
			stop_pump();
			close_valve();
			break;
			
		default:
			break;
	
	}                       
       
	return 1;
}

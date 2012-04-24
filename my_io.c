/*
 * Author: Ken Chida
 * Date: 20JUN2008
 *
 * Description: This is a simple controller for an altitude immersion pressure chamber.
 */

#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "Timer.h"

void initIO(void)
{
	/*------------------ Set ADMUX values -------------------------- 
	 * Bit(s)   7: REFS1  = 0  Choose internal reference voltage of
	 *          6: REFS0  = 0  1.263 V.
	 *          5: ADLAR  = 0  Result is right-adjusted
	 *          4:  MUX4  = 0  Select ADC0 for conversion,
	 *          3:  MUX3  = 0  single-ended
	 *          2:  MUX2  = 0  
     *          1:  MUX1  = 0  
	 *          0:  MUX0  = 0  
	 */	 
	ADMUX = 0;

	/*------------------ Set ADCSRB values -------------------------- 
	 * Bit(s)   7: Unused = 0
	 *          6:  ACME  = 0
	 *        5-3: Unused = 0  
	 *          2:  ADTS2 = 0  Free running mode on A/D conversion.
     *          1:  ADTS1 = 0 
	 *          0:  ADTS0 = 0
	 */	 
	ADCSRB = 0;

	/*------------------ Set DIDR0 values -------------------------- 
	 * Bit(s) 7-0:  ADCxD = 0  Enable all input buffers.
	 */	 
	DIDR0 = 0;

    /*------------------ Set ADCSRA values -------------------------- 
	 * Bit(s)   7:  ADEN  = 1  Enable the ADC
	 *          6:  ADSC  = 1  Start a conversion 
	 *          5: ADATE  = 1  Enable auto triggerring
	 *          4:  ADIF  = 0  Interrupt flag
	 *          3:  ADIE  = 0  Don't enable interrupt
	 *          2: ADPS2  = 1  Divide clock by 128 to get  
     *          1: ADPS1  = 1  7.8 kHz.
	 *          0: ADPS0  = 1
	 */	 
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

	/*---- Set up pump (PC2) and valve (PC3) output ports ----*/
	DDRC |= ((1<<DDC5) | (1<<DDC4) | (1<<DDC3) | (1<<DDC2));
	
}


void start_pump(void)
{
	PORTC |= (1<<PORTC3 | 1<<PORTC2);
}

void stop_pump(void)
{
	PORTC &= ~(1<<PORTC3 | 1<<PORTC2);
}

void open_valve(void)
{
	PORTC |= (1<<PORTC5 | 1<<PORTC4);
}

void close_valve(void)
{
	PORTC &= ~(1<<PORTC5 | 1<<PORTC4);
}


uint16_t read_pressure(void)    // Pin1 = Vout (AVR Pin 23), Pin2 = GND, Pin3 = Vcc
{
    /*test variables*/
	//int i =0;
	//uint8_t lower;
	//uint8_t upper;

	uint16_t ADCount = 0;		            // count from ADC register (not voltage).
    extern uint16_t pressure;
    volatile uint16_t ADCHtemp, ADCLtemp;   // temp var

    ADCLtemp = ADCL;                        // read data from ADC registers.
	ADCHtemp = ADCH;
	
	/* Retain for testing */
	//PORTD = ~ADCLtemp;
	//for (i=0; i<1000; i++) _delay_ms(1.0);
	//PORTD = ~ADCHtemp; 
	//for (i=0; i<1000; i++) _delay_ms(1.0);
    
	ADCount = (ADCHtemp << 8) | ADCLtemp;
 	
    /*Retain for testing*/
	//lower = ADCount & 0x00FF;
	//upper = (ADCount & 0xFF00)>>8;
	//PORTD = ~lower;
	//for (i=0; i<1000; i++) _delay_ms(1.0);
	//PORTD = ~upper;
	//for (i=0; i<1000; i++) _delay_ms(1.0);

	/*****************************************************/
	/* Hook vacuum up to left port above pin 1.          */
	/* Min. ADCount is 0000101001(41) (0.2V) at 0psi.    */
	/* Max. ADCount is 1111000011(963) (4.7V) at 72.5psi */
	/* Span. ADCount is 1111000011(922) (4.5V)           */
	/*****************************************************/
    
    /* Error handling - negative pressures will mess up results */
	if (ADCount < 41 || ADCount > 1030)
	    pressure = 0;   
	else
	    pressure = ((ADCount - 41) * 7 + ((ADCount - 41) * 8)/10 + ((ADCount - 41)*6)/100);  //7.863

	/*Retain for testing - 1.6V should correspond to 2256 centiPSI (1000 11010000)*/
	//lower = pressure & 0x00FF;
	//upper = (pressure & 0xFF00)>>8;
	//PORTD = ~lower;
	//for (i=0; i<1000; i++) _delay_ms(1.0);
	//PORTD = ~upper;
	//for (i=0; i<1000; i++) _delay_ms(1.0);

    return pressure;
}

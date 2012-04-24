/*
 * Author: Ken Chida
 * Date: 20JUN2008
 *
 * Description: This is a simple controller for an altitude immersion pressure chamber.
 *
 * Note: "centiPSI == 0.01 pounds per square inch"  Don't ask! :)
 */

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/io.h>
#include "Timer.h"

extern volatile uint16_t second;

/*---- Function definitions ----*/
/*
 *  Set the timer's control registers to select the clock speed you
 *  require in order to implement your chosen time delay.
 */

void init_timer2(void)
{
    /* Set control register for:
     *  Bit 7 = 0       no forced output compare
     *  Bit 6 = 0       normal count mode
     *  Bit 5 = 0       normal port operation
     *  Bit 4 = 0
     *  Bit 3 = 0       normal count mode
     *  Bit 2 = 0       clock = normal speed, no prescale
     *  Bit 1 = 0
     *  Bit 0 = 1
     */

    unsigned int i;      // loop counter for crude time delay

    cli( );              // no interrupts while we're changing timer params!
    
    /* The 32.768kHz clock can take as long as 1 second to stabilize after a
     *  reset or a power-up.  So let's wait a bit before proceeding.
     */
    for (i=0; i<1000; i++) _delay_ms(1.0);

    TIMSK2 = 0;                               // disable OCIE2A and TOIE2

    ASSR = (1<<AS2);                          // select Timer2 mode to async
    
    TCNT2 = 0;                                // init count register
    
    TCCR2B = ((1 << CS22) | (1 << CS20));     //Divide 32.768khz by 128 prescale
    
    /* wait for status bits to be cleared */
    while ((ASSR & _BV(TCR2BUB)) | (ASSR & _BV(TCN2UB)));

    TIFR2 = (1 << OCF2A) | (1 << TOV2);       // clear interrupt-flags
    TIMSK2 = (1 << TOIE2);                    // enable Timer2 overflow interrupt

    //DDRB |= (1<<DDB0);
    //PORTB = 1<<PORTB0;

}

/*---- Simple busy wait ----*/
void delay(int target_time)  
{
    uint16_t time_sec = 0;
    int count_sec = 0;

    while (count_sec <= target_time)
    {
        time_sec = second;
        
        while (time_sec == second);
    
        count_sec++;
    }
}

/*---- Timer2 overflow interrupt handler ----*/
ISR(TIMER2_OVF_vect)
{
    TCNT2 = 0;                                  // re-initialize timer count
    second++;
    //increment_time();
    
    //int i =0;
    //if (PORTB & 0x01) PORTB &= ~(1<<PORTB0);  //if PB0 is high, make PB0 low = led on
    //else PORTB |= (1<<PORTB0);                //make pb0 high = led off
    //PORTD = PORTB;
    //for (i=0; i<1000; i++) _delay_ms(1.0);

    //debounce_switches();
    while (ASSR & _BV(TCN2UB));                 // wait for update
}

/* This handler for unused interrupts will cause the program to stall with 
 * the four middle LEDs ON.
 */
ISR(__vector_default)
{
    //PORTB = 0xC3;
    for ( ; ; );    // null statement - loop forever here
}

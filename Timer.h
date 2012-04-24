
#if !defined(TIMER_H)          // prevents including this file multiple times
#define TIMER_H

/*---- Constant Definitions ----*/

#define TIMER2_INIT_VAL     (uint8_t)255   // overflow at 7.9s = 255 x 1/(32.7kHz/1024)
                                           // typecast to remove warning.

/*---- Public function prototypes ----*/
void init_timer2(void);
//void increment_time(void);
void delay(int);

#endif



/*
 * Author: Ken Chida
 * Date: 20JUN2008
 *
 * Description: This is a simple controller for an altitude immersion pressure chamber.
 */

#if !defined(MY_IO_H)		/* Prevents including this file multiple times */
#define MY_IO_H
#include <stdint.h>

/*---- Public function prototypes ----*/
void initIO(void);
void start_pump(void);
void stop_pump(void);
void open_valve(void);
void close_valve(void);

uint16_t read_pressure(void);

#endif

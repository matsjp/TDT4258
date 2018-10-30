#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/*
 *Function: setupTimer
 *
 *Configures the registers 
 *
 *period: The period of the timer
 *
 *return: void
 */
void
setupTimer (uint16_t period)
{
  *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_TIMER1;
  *TIMER1_TOP = period;
  *TIMER1_IEN = 1;
}

/*
 *Function: startTimer
 *
 *Starts the timer
 *
 *return: void
 */
void
startTimer ()
{
  *TIMER1_CMD = 1;
}

/*
 *Function: stopTimer
 *
 *Stops the timer
 *
 *return: void
 */
void
stopTimer ()
{
  *TIMER1_CMD = 0x2;
}

#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/*
 *Function: setupDAC()
 *
 *Configures the registers we need to use the DAC
 *
 *return: void
 */
void
setupDAC ()
{
  *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_DAC0;
  *DAC0_CTRL = 0X50010;
  *DAC0_CH0CTRL = 1;
  *DAC0_CH1CTRL = 1;
}

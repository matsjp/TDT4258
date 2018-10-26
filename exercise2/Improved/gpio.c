#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/*
 *Function: setupGPIO
 *
 *Configures the registers we need for GPIO input
 *
 *return: void
 */
void
setupGPIO ()
{
  *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;	/* enable GPIO clock */
  *GPIO_PC_MODEL = 0x33333333;
  *GPIO_PC_DOUT = 0xff;

  //Setup GPIO Interrupt
  *GPIO_EXTIPSELL = 0x22222222;
  //*GPIO_EXTIRISE = 0xff;
  *GPIO_EXTIFALL = 0xff;
  *GPIO_IEN = 0xff;
  *GPIO_IFC = 0xffffffff;
}

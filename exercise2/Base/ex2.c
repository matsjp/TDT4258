#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "efm32gg.h"
#include "ex2.h"
#define   SAMPLE_PERIOD   317

int
main (void)
{
  /*
   * Call the peripheral setup functions 
   */
  setupGPIO ();
  setupDAC ();
  setupTimer (SAMPLE_PERIOD);

  startTimer ();

  //poll for button presses and play sound for specific button presses
  while (1)
    {
      int buttonPressed = *GPIO_PC_DIN;
      switch (buttonPressed)
	{
	case 0xfe:
	  playSound (&sound1);
	  break;
	case 0xfd:
	  playSound (&sound2);
	  break;
	case 0xfb:
	  playSound (&sound3);
	  break;
	case 0xf7:
	  playSound (&intro);
	  break;
	default:
	  break;
	}
    }
  return 0;
}

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "efm32gg.h"
#include "sound.h"
#include "ex2.h"

void GPIO_Handler ();
extern void playSound (uint16_t *);
extern volatile int soundIsPlaying;

/*
 * TIMER1 interrupt handler 
 */
void __attribute__ ((interrupt)) TIMER1_IRQHandler ()
{
  //If there's no more samples, setup such that a new sound can be played
  if (nextSound > soundLength)
    {
      soundPointer = NULL;
      nextSound = 1;
      enableEM4 ();
      soundIsPlaying = 0;
      stopTimer ();
    }
  //Play next sample
  else
    {
      *DAC0_CH0DATA = soundPointer[nextSound];
      *DAC0_CH1DATA = soundPointer[nextSound];
      nextSound++;
    }
  //Clear interrupt flag
  *TIMER1_IFC = 1;
}

/*
 * GPIO even pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler ()
{
  /*
   * TODO handle button pressed event, remember to clear pending
   * interrupt 
   */
  GPIO_Handler ();
  *GPIO_IFC = *GPIO_IF;
}

/*
 * GPIO odd pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler ()
{
  GPIO_Handler ();
  *GPIO_IFC = *GPIO_IF;
}

/*
 *Function GPIO_Handler()
 *
 *Check which button is pressed and starts playing the corresponding song
 *
 *returns: void
 */
void
GPIO_Handler ()
{
  int buttonPressed = *GPIO_PC_DIN;
  //Doesn't start playing if there's already sounds playing
  if (soundIsPlaying == 0)
    {
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
}

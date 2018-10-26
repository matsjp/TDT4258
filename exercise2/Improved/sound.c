#include <stdint.h>
#include "sound.h"
#include "ex2.h"

volatile int nextSound = 1;
volatile int soundIsPlaying = 0;
int volatile soundLength;
volatile uint16_t *soundPointer;

/*
 *Function playSound
 *
 *Starts playing the sound given as input
 *
 **sound: a pointer to the array of samples that makes the sound
 *
 *returns: void
 */
void
playSound (uint16_t * sound)
{
  soundIsPlaying = 1;
  soundPointer = sound;
  soundLength = soundPointer[0];
  enableEM1 ();
  startTimer ();
}

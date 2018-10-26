#include <stdint.h>
#include <stdlib.h>
#include "efm32gg.h"

/*
*Function: playSound
*--------------------
*Plays the sound that is given as input
*
**soundPointer: A pointer to the sound you want to play
*
*Returns: void
*/
void playSound(uint16_t *soundPointer){
	//The length of the sound is saved in the first element of the array containing the sound
	int soundLength = soundPointer[0];
	//timeTracker is used to play play sound on every tenth timer cycle. If this was not used the sound would be played too fast
	int timerTracker = 0;
	int nextSound = 1;
	while(1){
		//To poll the timer we check the interrupt flag and clear it later
		if(*TIMER1_IF == 1){
			if (timerTracker == 10){
				//Check if we have reached the end of the sound
				if (nextSound > soundLength) {
					break;
					//Else we play the next samle from the sound array
				} else {
					*DAC0_CH0DATA = soundPointer[nextSound];
					*DAC0_CH1DATA = soundPointer[nextSound];
					nextSound++;
				}
				timerTracker = 0;
			}
			else {
				timerTracker++;
			}
		}
	}
	//Clear interrupt flag
	*TIMER1_IFC = 1;
}

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "efm32gg.h"

/*
 * TODO calculate the appropriate sample period for the sound wave(s) you 
 * want to generate. The core clock (which the timer clock is derived
 * from) runs at 14 MHz by default. Also remember that the timer counter
 * registers are 16 bits. 
 */
/*
 * The period between sound samples, in clock cycles 
 */
#define   SAMPLE_PERIOD   317

/*
 * Declaration of peripheral setup functions 
 */
void setupTimer(uint32_t period);
void setupDAC();
void setupNVIC();
void setupGPIO();
void startTimer();
void stopTimer();
extern uint16_t sound1;
extern uint16_t sound2;
extern uint16_t sound3;
extern uint16_t intro;
int soundLength;
uint16_t *soundPointer;
int nextSound = 1;
int x = 0;
int soundIsPlaying = 0;
/*
 * Your code will start executing here 
 */
int main(void)
{
	/*
	 * Call the peripheral setup functions 
	 */
	setupGPIO();
	setupDAC();
	setupTimer(SAMPLE_PERIOD);

	
	while (1){
		
	}
	return 0;
}

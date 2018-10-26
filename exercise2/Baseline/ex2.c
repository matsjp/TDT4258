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
void toggleLED();
extern uint16_t sound1;
extern uint16_t sound2;
extern uint16_t sound3;
extern uint16_t intro;
int soundLength;
uint16_t *soundPointer;
int nextSound = 1;
int soundIsPlaying = 0;
extern void playSound();
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

	startTimer();
	while (1){
		int buttonPressed = *GPIO_PC_DIN;
		switch (buttonPressed) {
			case 0xfe:
				playSound(&sound1);
				break;
			case 0xfd:
				playSound(&sound2);
				break;
			case 0xfb:
				playSound(&sound3);
				break;
			case 0xf7:
				playSound(&intro);
				break;
			default:
				break;
		}
	}
	return 0;
}


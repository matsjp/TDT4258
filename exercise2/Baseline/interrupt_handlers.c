#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "efm32gg.h"
#include "sound.h"
void startTimer();
void GPIO_Handler();
void stopTimer();
void enableEM4();
void enableEM1();

extern uint16_t sound1;
extern uint16_t sound2;
extern uint16_t sound3;
extern uint16_t intro;
int soundLength;
uint16_t *soundPointer;


volatile int timerTracker = 0;
volatile int nextSound = 1;
volatile int s = 0;


volatile int x = 0;
volatile int soundIsPlaying = 0;
/*
 * TIMER1 interrupt handler 
 */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	/*
	 * TODO feed new samples to the DAC remember to clear the pending
	 * interrupt by writing 1 to TIMER1_IFC 
	 */
	/*if(soundPointer == NULL){
		*DAC0_CH0DATA = 0;
		*DAC0_CH1DATA = 0;
		enableEM4();
		soundIsPlaying = 0;
	}*/
	if (nextSound > soundLength) {
		soundPointer = NULL;
		nextSound = 1;
		enableEM4();
		soundIsPlaying = 0;
		stopTimer();
	} else {
		*DAC0_CH0DATA = soundPointer[nextSound];
		*DAC0_CH1DATA = soundPointer[nextSound];
		nextSound++;
	}
*TIMER1_IFC = 1;
}

/*
 * GPIO even pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
	/*
	 * TODO handle button pressed event, remember to clear pending
	 * interrupt 
	 */
	GPIO_Handler();
	*GPIO_IFC = *GPIO_IF;
}

/*
 * GPIO odd pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
	/*
	 * TODO handle button pressed event, remember to clear pending
	 * interrupt 
	 */
	GPIO_Handler();
	*GPIO_IFC = *GPIO_IF;
}

int y = 0;
void GPIO_Handler()
{
	int buttonPressed = *GPIO_PC_DIN;
	if (soundIsPlaying == 0) {
	x = 0;
		switch (buttonPressed) {
		case 0xfe:
			*GPIO_PA_DOUT = 0;
			soundIsPlaying = 1;
			soundPointer = &sound1;
			soundLength = soundPointer[0];
			enableEM1();
			startTimer();
			break;
		case 0xfd:
			soundIsPlaying = 1;
			soundPointer = &sound2;
			soundLength = soundPointer[0];
			enableEM1();
			startTimer();
			break;
		case 0xfb:
			soundIsPlaying = 1;
			soundPointer = &sound3;
			soundLength = soundPointer[0];
			enableEM1();
			startTimer();
			break;
		case 0xf7:
			soundIsPlaying = 1;
			soundPointer = &intro;
			soundLength = soundPointer[0];
			enableEM1();
			startTimer();
			break;
		default:
			*GPIO_PA_DOUT = 0b1111110111111101;
			break;
		}
	}
	else{
		*GPIO_PA_DOUT = 0b1111111011111110;
	}
}

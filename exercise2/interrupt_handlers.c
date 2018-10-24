#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "sound.h"
//TODO make function and variable names consistent
void startTimer();
void GPIO_Handler();
void stopTimer();
void enableEM4();
void enableEM1();

extern struct sound soundStruct1;
extern struct sound soundStruct2;
extern struct sound soundStruct3;
extern struct sound soundStruct4;
extern volatile struct sound *soundPointer;


volatile int timerTracker = 0;
volatile int nextSound = 0;
volatile int s = 0;

//TODO move sound stuff to its own file
/*int sound4[16] = {0xb0, 0xe0, 0xa0, 0x20, 0xb0, 0xe0, 0xa0, 0x20, 0xb0, 0xe0, 0xa, 0x2, 0xb, 0xe, 0xa, 0x2, 0xb, 0xe, 0xa, 0x2, 0xb, 0xe, 0xa, 0x2, 0xb, 0xe, 0xa, 0x2, 0xb, 0xe, 0xa, 0x2};*/
//int sound4[10] = { 160, 230, 300, 370, 440, 490, 420, 350, 280, 210 };


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
	//TODO S value is kept from old sound, fix it
	*GPIO_PA_DOUT = 0b1111101111111011;
	if (nextSound == 0) {
		s = soundPointer->soundList[0];
		nextSound++;
	}
	if (timerTracker == 100) {
		s = soundPointer->soundList[nextSound];
		if (nextSound >= soundPointer->length) {
			*GPIO_PA_DOUT = 0b1111011111110111;
			nextSound = 0;
			stopTimer();
			timerTracker = 0;
			soundIsPlaying = 0;
			enableEM4();
			s = 0;
			x = 30;
		} else {
			nextSound++;
		}
		timerTracker = 0;
	}
	if (x == 1){
		*GPIO_PA_DOUT = 0b1100111111001111;
		x = 0;
	}
	else if(x == 0) {
		*GPIO_PA_DOUT = 0b0011111100111111;
		x = 1;
	}
	timerTracker++;
	*DAC0_CH0DATA = s;
	*DAC0_CH1DATA = s;
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
		//TODO change binary literaly to hex literals
		switch (buttonPressed) {
		case 0xfe:
			soundIsPlaying = 1;
			soundPointer = &soundStruct1;
			enableEM1();
			startTimer();
			break;
		case 0xfd:
			soundIsPlaying = 1;
			soundPointer = &soundStruct2;
			enableEM1();
			startTimer();
			break;
		case 0xfb:
			soundIsPlaying = 1;
			soundPointer = &soundStruct3;
			enableEM1();
			startTimer();
			break;
		case 0xf7:
			soundIsPlaying = 1;
			soundPointer = &soundStruct4;
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

#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

int timerTracker = 0;
int nextSound = 0;
//TODO fix this
int s = 0xf;

//TODO move sound stuff to its own file

struct sound{
	int length;
	int soundList[50];
};

struct sound soundStruct1;
struct sound soundStruct2;
struct sound soundStruct3;
struct sound soundStruct4;
struct sound *soundPointer;
//int sound1[3] = {0xff, 0xbe, 0xaf};
//int sound2[4] = {0xfb, 0x3b, 0xf2, 0xae};

int sound1[3] = {0x2, 0x4, 0xa};
int sound2[4] = {0x6, 0x3, 0x2, 0x7};
int sound3[4] = {0x2, 0xf, 0xd, 0x8};
/*int sound4[16] = {0xb0, 0xe0, 0xa0, 0x20, 0xb0, 0xe0, 0xa0, 0x20, 0xb0, 0xe0, 0xa, 0x2, 0xb, 0xe, 0xa, 0x2, 0xb, 0xe, 0xa, 0x2, 0xb, 0xe, 0xa, 0x2, 0xb, 0xe, 0xa, 0x2, 0xb, 0xe, 0xa, 0x2};*/
int sound4[10] = {160, 230, 300, 370, 440, 490, 420, 350, 280, 210};

void initializeSounds(){
	addSound(&soundStruct1, sound1, sizeof(sound1)/sizeof(sound1[0]));
	addSound(&soundStruct2, sound2, sizeof(sound2)/sizeof(sound2[0]));
	addSound(&soundStruct3, sound3, sizeof(sound3)/sizeof(sound3[0]));
	addSound(&soundStruct4, sound4, sizeof(sound4)/sizeof(sound4[0]));
	//soundPointer = &soundStruct4;
}

void addSound(struct sound *soundStruct, int sound[], int length){
	soundStruct->length = length;
	for (int i = 0; i < 3; i++){
		soundStruct->soundList[i] = sound[i];
	}
}
int x = 0;
int soundIsPlaying = 0;
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
	if (nextSound == 0){
		s = soundPointer->soundList[0];
	}
	if (timerTracker == 100){
		s = soundPointer->soundList[nextSound];
		if (nextSound == soundPointer->length){
			if (x == 0){
				x = 1;
				*GPIO_PA_DOUT = 0;
			}
			else{
				x = 0;
				*GPIO_PA_DOUT = 0xffffffff;
			}
			nextSound = 0;
			stopTimer();
			soundIsPlaying = 0;
		}
		else{
			nextSound++;
		}
		timerTracker = 0;
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

void GPIO_Handler(){
	/*if (x == 0){
		x = 1;
		*GPIO_PA_DOUT = 0;
	}
	else{
		x = 0;
		*GPIO_PA_DOUT = 0xffffffff;
	}*/
	int buttonPressed = *GPIO_PC_DIN;
	if (soundIsPlaying == 0){
		switch(buttonPressed){
			case 0b11111110:
				soundIsPlaying = 1;
				soundPointer = &soundStruct1;
				startTimer();
				break;
			case 0b11111101:
				soundIsPlaying = 1;
				soundPointer = &soundStruct2;
				startTimer();
				break;
			case 0b11111011:
				soundIsPlaying = 1;
				soundPointer = &soundStruct3;
				startTimer();
				break;
			case 0b11110111:
				soundIsPlaying = 1;
				soundPointer = &soundStruct4;
				startTimer();
				break;
			default:
				break;
		}
	}
}

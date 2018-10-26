#include "efm32gg.h"

/*
*Function: enableEM4
*-------------------
*Enables energy mode 4
*
*
*/
void enableEM4(){
	*SCR = 0x6;
}

/*
*Function: enableEM1
*-------------------
*Enables energy mode 1
*
*
*/
void enableEM1(){
	*SCR = 0x2;
}

/*
*Function: disableRAMBlocks
*-------------------
*Disables sram blocks
*
*value: the ram blocks that you want to disable. See manual for values that *		disable which blocks
*/
void disableRAMBlocks(int value){
	*EMU_MEMCTRL = value;
}

/*
*Function: adjustCoreClock
*-------------------
*Adjusts the speed of the core clock
*
*value: how much you want to adjust the speed. See manual for valid values
*/
void adjustCoreClock(int value){
	*CMU_HFCORECLKDIV = value;
}

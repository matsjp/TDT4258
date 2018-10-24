#include "efm32gg.h"

void enableEM4(){
	*SCR = 0b110;
}

void enableEM1(){
	*SCR = 0b10;
}

void disableRAMBlocks(){
	*EMU_MEMCTRL = 0x4;
}

void adjustCoreClock(int value){
	*CMU_HFCORECLKDIV = value;
}

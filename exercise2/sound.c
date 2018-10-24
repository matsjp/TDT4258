#include "sound.h"

struct sound soundStruct1;
struct sound soundStruct2;
struct sound soundStruct3;
struct sound soundStruct4;
volatile struct sound *soundPointer;

int sound1[4] = { 0x22, 0x22, 0xf2, 0xd2 };
int sound2[5] = { 0x62, 0x32, 0x22, 0xf2, 0xd2 };
int sound3[6] = { 0x22, 0xf2, 0xd2, 0x22, 0xf2, 0xd2 };
int sound4[7] = { 0x22, 0xf2, 0xd2, 0x82, 0x22, 0xf2, 0xd2 };
void addSound(struct sound *soundStruct, int sound[], int length)
{
	soundStruct->length = length;
	for (int i = 0; i < 3; i++) {
		soundStruct->soundList[i] = sound[i];
	}
}

void initializeSounds()
{
	addSound(&soundStruct1, sound1, sizeof(sound1) / sizeof(sound1[0]));
	addSound(&soundStruct2, sound2, sizeof(sound2) / sizeof(sound2[0]));
	addSound(&soundStruct3, sound3, sizeof(sound3) / sizeof(sound3[0]));
	addSound(&soundStruct4, sound4, sizeof(sound4) / sizeof(sound4[0]));
}

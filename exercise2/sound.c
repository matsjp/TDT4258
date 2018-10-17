#include "sound.h"
int song1[3] = { 0xff, 0xb2, 0x11 };
int song2[4] = { 0xfb, 0x3b, 0xf2, 0xae };

void initializeSounds()
{
	addSongToStruct(sound1.soundList, sizeof(song1) / sizeof(song1[0]));
	addSongToStruct(sound2.soundList, sizeof(song2) / sizeof(song2[0]));
}

void addSongToStruct(struct sound sound, int song[], int length)
{
	sound.soundListLength = length;
	for (int i = 0; i < length; i++) {
		sound.soundList[i] = song[i];
	}
}

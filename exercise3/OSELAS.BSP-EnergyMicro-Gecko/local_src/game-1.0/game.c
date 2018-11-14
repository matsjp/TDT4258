#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

FILE* gamepad_driver;

void handler(void){
	printf("Handler\n");
}

int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");
	
	gamepad_driver = fopen("/dev/gamepad", "rb");
	signal(SIGIO, &handler);
	fcntl(fileno(gamepad_driver), F_SETOWN, getpid( ));
	long oflags = fcntl(fileno(gamepad_driver), F_GETFL);
	fcntl(fileno(gamepad_driver), F_SETFL, oflags | FASYNC);
	
	printf("Entering loop\n");
	while(1);
	
	exit(EXIT_SUCCESS);
}

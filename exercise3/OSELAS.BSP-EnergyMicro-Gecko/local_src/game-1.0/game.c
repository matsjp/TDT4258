#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

FILE* gamepad_driver;

char buffer[16] = {}; 

void handler(int signo){
	printf("Handler\n");
	int x = fgetc(gamepad_driver);
	printf("%x\n", x);
	if(x == 0xf7){
		printf("It works\n");
	}
	//down: f7
	//left: fe
	//right: fb
	//up: fd
}

int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");
	
	gamepad_driver = fopen("/dev/gamepad", "rd");
	if (!gamepad_driver) {
        printf("Unable to open driver device, maybe you didn't load the module?\n");
        return EXIT_FAILURE;
    }
    if (signal(SIGIO, &handler) == SIG_ERR) {
        printf("An error occurred while register a signal handler.\n");
        return EXIT_FAILURE;
    }
    if (fcntl(fileno(gamepad_driver), F_SETOWN, getpid()) == -1) {
        printf("Error setting pid as owner.\n");
        return EXIT_FAILURE;
    }
    long oflags = fcntl(fileno(gamepad_driver), F_GETFL);
    if (fcntl(fileno(gamepad_driver), F_SETFL, oflags | FASYNC) == -1) {
        printf("Error setting FASYNC flag.\n");
        return EXIT_FAILURE;
}
	
	printf("Entering loop\n");
	while(1);
	
	exit(EXIT_SUCCESS);
}

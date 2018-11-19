#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#define YELLOW 0xFFE0
#define BLACK 0x0000
#define WHITE 0xFFFF
#define CYAN 0x07FF

FILE* gamepad_driver;

char buffer[16] = {};

char gameboard[12][17] = {{"wwwwwwwwwwwwwwew\0"},
                          {"woooooooooooooow\0"},
                          {"wowwwwwwwwwwwwww\0"},
                          {"wowwwwwwwwwwwwww\0"},
                          {"wowwwwwwwwwwwwww\0"},
                          {"wowwwwwwwwwwwwww\0"},
                          {"wowwwwwwwwwwwwww\0"},
                          {"wowwwwwwwwwwwwww\0"},
                          {"wowwwwwwwwwwwwww\0"},
                          {"wowwwwwwwwwwwwww\0"},
                          {"wowwwwwwwwwwwwww\0"},
                          {"wpwwwwwwwwwwwwww\0"}
};
int playerx = 1;
int playery = 11;
int goalx = 14;
int goaly = 0;
int fbfd;
uint16_t* fbp;
int screensize_pixels;
int screensize_bytes;

struct fb_var_screeninfo vinfo;
struct fb_copyarea screen;

void draw_maze(){
	int i;
	int color;
	printf("MAze");
	for (i = 0; i < 12; i++){
		int j;
		for (j = 0; j < 16; j++){
			if (gameboard[i][j] == 'w'){
				color = BLACK;
			}
			else if (gameboard[i][j] == 'o'){
				color = WHITE;
			}
			else if (gameboard[i][j] == 'p'){
				color = YELLOW;
			}
			else if (gameboard[i][j] == 'e'){
				color = CYAN;
			}
			printf("%d\n", color);
			int k;
			for (k = 0; k < 20; k++){
				int t;
				for (t = 0; t < 20; t++){
					//TODO fix magic numbers
					int magic_number = i*20 + j*20 + k*320 + t;
					fbp[magic_number] = color;
				}
			}
		}
	}
}

int check_win(){
	return ((playerx == goalx) && (playery == goaly));
}

void print_board(){
	int i;
    for (i = 0; i < 12; i++){
            printf("%s\n", gameboard[i]);
    }
}

void move_left(){
    if (playerx != 0 && gameboard[playery][playerx - 1] != 'w'){
        gameboard[playery][playerx] = 'o';
        playerx -= 1;
        gameboard[playery][playerx] = 'p';
        print_board();
    }
}

void move_up(){
    printf("%c\n", gameboard[playery - 1][playerx]);
    if (playery != 0 && gameboard[playery - 1][playerx] != 'w'){
        gameboard[playery][playerx] = 'o';
        playery -= 1;
        gameboard[playery][playerx] = 'p';
        print_board();
    }
}

void move_right(){
    if (playerx != 15 && gameboard[playery][playerx + 1] != 'w'){
        gameboard[playery][playerx] = 'o';
        playerx += 1;
        gameboard[playery][playerx] = 'p';
        print_board();
    }
}

void move_down(){
    if (playery != 11 && gameboard[playery + 1][playerx] != 'w'){
        gameboard[playery][playerx] = 'o';
        playery += 1;
        gameboard[playery][playerx] = 'p';
        print_board();
    }
}



void handler(int signo){
	int input = fgetc(gamepad_driver);
	if(input == 0xf7){
		move_down();
	}
	if(input == 0xfe){
		move_left();
	}
	if(input == 0xfb){
		move_right();
	}
	if(input == 0xfd){
		move_up();
	}
	if(check_win()){
		printf("You win!");
	}
	//down: f7
	//left: fe
	//right: fb
	//up: fd
}

int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");
	
	gamepad_driver = fopen("/dev/gamepad", "r+");
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
	
	fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        printf("Error: unable to open framebuffer device.\n");
        return EXIT_FAILURE;
    }

    // Get screen size info
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        printf("Error: unable to get screen info.\n");
        return EXIT_FAILURE;
    }
    printf("Screeninfo: %d x %d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    screen.dx = 0;
    screen.dy = 0;
    screen.width = vinfo.xres;
    screen.height = vinfo.yres;

    screensize_pixels = vinfo.xres * vinfo.yres;
    screensize_bytes = screensize_pixels * vinfo.bits_per_pixel / 8;
    printf("%d\n", screensize_pixels);

    fbp = (uint16_t*)mmap(NULL, screensize_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == MAP_FAILED) {
        printf("Error: failed to memorymap framebuffer.\n");
        return EXIT_FAILURE;
}
	
	draw_maze();
	ioctl(fbfd, 0x4680, &screen);
	
	printf("Entering loop\n");
	while(1);
	
	exit(EXIT_SUCCESS);
}

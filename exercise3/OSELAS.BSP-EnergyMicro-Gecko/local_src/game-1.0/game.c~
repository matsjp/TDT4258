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
FILE* framebuffer_driver;

char gameboard[12][17] = {{"wwwwwwwwwwwwwwew\0"},
                          {"woooooooooooooow\0"},
                          {"wowwwwwwwwwwwwww\0"},
                          {"wowpowwooowoooww\0"},
                          {"wowwowwowowowoww\0"},
                          {"wowwoooowooowoww\0"},
                          {"wowoowwwwwwwwoww\0"},
                          {"wowowoowwoooooww\0"},
                          {"wowowowwwowwwwww\0"},
                          {"wowooowwwoooowww\0"},
                          {"wowwwwwwwwwwowww\0"},
                          {"woooooooooooowww\0"}
};
int playerx = 3;
int playery = 3;
int goalx = 14;
int goaly = 0;
int framebuffer_fd;
uint16_t* framebuffer_pointer;
int screensize_pixels;
int screensize_bytes;

struct fb_var_screeninfo vinfo;
struct fb_copyarea screen;

int get_row(int num){
	return (int) num/320;
}

int get_col(int num, int row){
	return (int)(((int)(num - row*320))/20);
}

void draw_maze(){
	int i;
	int color;
	for(i = 0; i<screensize_pixels; i++){
		int row = get_row(i);
		int col = get_col(i, row);
		row = (int)(row/20);
		char grid = gameboard[row][col];
		if (grid == 'w'){
			color = BLACK;
		}
		else if(grid == 'o'){
			color = WHITE;
		}
		else if(grid == 'p'){
			color = YELLOW;
		}
		else if(grid == 'e'){
			color = CYAN;
		}
		framebuffer_pointer[i] = color;
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

void redraw(){
	draw_maze();
	ioctl(framebuffer_fd, 0x4680, &screen);
}



void handler(int signo){
	int input = fgetc(gamepad_driver);
	if(input == 0xf7){
		move_down();
		redraw();
	}
	else if(input == 0xfe){
		move_left();
		redraw();
	}
	else if(input == 0xfb){
		move_right();
		redraw();
	}
	if(input == 0xfd){
		move_up();
		redraw();
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
	
	//fbfd = open("/dev/fb0", O_RDWR);
	framebuffer_driver = fopen("/dev/fb0", "r+");
	framebuffer_fd = fileno(framebuffer_driver);
    if (framebuffer_fd == -1) {
        printf("Error: unable to open framebuffer device.\n");
        return EXIT_FAILURE;
    }

    // Get screen size info
    if (ioctl(framebuffer_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
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

    framebuffer_pointer = (uint16_t*)mmap(NULL, screensize_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, framebuffer_fd, 0);
    if ((int)framebuffer_pointer == (int)MAP_FAILED) {
        printf("Error: failed to memorymap framebuffer.\n");
        return EXIT_FAILURE;
}
	
	draw_maze();
	ioctl(framebuffer_fd, 0x4680, &screen);
	
	printf("Entering loop\n");
	while(1);
	
	exit(EXIT_SUCCESS);
}

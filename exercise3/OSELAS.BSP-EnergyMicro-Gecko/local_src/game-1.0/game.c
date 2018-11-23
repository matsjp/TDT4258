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
#define INPUT_DOWN 0xf7
#define INPUT_LEFT 0xfe
#define INPUT_RIGHT 0xfb
#define INPUT_UP 0xfd


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

/*
 *Function: get_row
 *
 *this function determines which row on the gameboard a pixel belongs to
 *
 *int pixel: the pixel whos gameboard row you want
 *
 *returns: the row the pixel belongs to
 */
int get_row(int pixel){
	return (int) pixel/320;
}

/*
 *Function: get_col
 *
 *this function determines which column on the game board a pixel belongs to
 *
 *int pixel: the pixel whos gameboard column you want
 *
 *int row: the row which the pixel belongs to
 *
 *returns: the column the picel belongs to
 */
int get_col(int pixel, int row){
	return (int)(((int)(pixel - row*320))/20);
}

/*
 *Function: draw_maze
 *
 *this function determines which pixel corresponds to which grid on the game board, and gives that pixel the correct color
 *
 *returns: void
 */
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
	update_framebuffer();
	ioctl(framebuffer_fd, 0x4680, &screen);
}

/*
 *Function: check_win
 *
 *Checks if the player is in a winning state or not
 *
 *returns: 1 if the player is in a winning state, and 0 if not
 */
int check_win(){
	return ((playerx == goalx) && (playery == goaly));
}

/*
 *Function: update_framebuffer
 *
 *updates the framebuffer memory map so that it represents the current state of the game board
 *
 *return: void
 */

void update_framebuffer(){
	int i;
    for (i = 0; i < 12; i++){
            printf("%s\n", gameboard[i]);
    }
}


/*
 *Function: move_left
 *
 *Moves the player left on the gameboard if possible
 *
 *returns: void
 */
void move_left(){
    if (playerx != 0 && gameboard[playery][playerx - 1] != 'w'){
        gameboard[playery][playerx] = 'o';
        playerx -= 1;
        gameboard[playery][playerx] = 'p';
    }
}

/*
 *Function: move_up
 *
 *Moves the player up on the gameboard if possible
 *
 *returns: void
 */
void move_up(){
    printf("%c\n", gameboard[playery - 1][playerx]);
    if (playery != 0 && gameboard[playery - 1][playerx] != 'w'){
        gameboard[playery][playerx] = 'o';
        playery -= 1;
        gameboard[playery][playerx] = 'p';
    }
}

/*
 *Function: move_right
 *
 *Moves the player right on the gameboard if possible
 *
 *returns: void
 */
void move_right(){
    if (playerx != 15 && gameboard[playery][playerx + 1] != 'w'){
        gameboard[playery][playerx] = 'o';
        playerx += 1;
        gameboard[playery][playerx] = 'p';
    }
}

/*
 *Function: move_down
 *
 *Moves the player down on the gameboard if possible
 *
 *returns: void
 */
void move_down(){
    if (playery != 11 && gameboard[playery + 1][playerx] != 'w'){
        gameboard[playery][playerx] = 'o';
        playery += 1;
        gameboard[playery][playerx] = 'p';
    }
}

void handler(int signo){
	int input = fgetc(gamepad_driver);
	if(input == INPUT_DOWN){
		move_down();
		draw_maze();
	}
	else if(input == INPUT_LEFT){
		move_left();
		draw_maze();
	}
	else if(input == INPUT_RIGHT){
		move_right();
		draw_maze();
	}
	if(input == INPUT_UP){
		move_up();
		draw_maze();
	}
	if(check_win()){
		gameboard[goaly][goalx] = 'e';
		playerx = 3;
		playery = 3;
		gameboard[playerx][playery] = 'p';
		draw_maze();
	}
}

int main(int argc, char *argv[])
{
	
	gamepad_driver = fopen("/dev/gamepad", "r+");
	if (!gamepad_driver) {
        printf("Error while opening driver\n");
        return EXIT_FAILURE;
    }
    if (signal(SIGIO, &handler) == SIG_ERR) {
        printf("Error while registering signal handler\n");
        return EXIT_FAILURE;
    }
    if (fcntl(fileno(gamepad_driver), F_SETOWN, getpid()) == -1) {
        printf("Error while setting pid\n");
        return EXIT_FAILURE;
    }
    long oflags = fcntl(fileno(gamepad_driver), F_GETFL);
    if (fcntl(fileno(gamepad_driver), F_SETFL, oflags | FASYNC) == -1) {
        printf("Error while setting FASYNC flag\n");
        return EXIT_FAILURE;
}
	
	framebuffer_driver = fopen("/dev/fb0", "r+");
	framebuffer_fd = fileno(framebuffer_driver);
    if (framebuffer_fd == -1) {
        printf("Error while opening frame buffer device\n");
        return EXIT_FAILURE;
    }

    if (ioctl(framebuffer_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        printf("Error while getting screen info\n");
        return EXIT_FAILURE;
    }
    printf("Screeninfo: %d x %d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    screen.dx = 0;
    screen.dy = 0;
    screen.width = vinfo.xres;
    screen.height = vinfo.yres;

    screensize_pixels = vinfo.xres * vinfo.yres;
    screensize_bytes = screensize_pixels * vinfo.bits_per_pixel / 8;

    framebuffer_pointer = (uint16_t*)mmap(NULL, screensize_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, framebuffer_fd, 0);
    if ((int)framebuffer_pointer == (int)MAP_FAILED) {
        printf("Error while making memory map\n");
        return EXIT_FAILURE;
	}
	
	//Draw the maze for the first time
	draw_maze();
	//continyally put the program to sleep and wait for input from the gamepad driver
	while(1){
		pause();
	}
}

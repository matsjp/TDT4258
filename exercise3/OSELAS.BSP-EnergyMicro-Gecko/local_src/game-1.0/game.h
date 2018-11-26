#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <fcntl.h>

#define YELLOW 0xFFE0
#define BLACK 0x0000
#define WHITE 0xFFFF
#define CYAN 0x07FF
#define INPUT_DOWN 0xf7
#define INPUT_LEFT 0xfe
#define INPUT_RIGHT 0xfb
#define INPUT_UP 0xfd
#define SCREENSIZE_PIXELS 76800
#define SCREENSIZE_BYTES 153600
#define GAMEBOARD_TOP_EDGE 0
#define GAMEBOARD_LEFT_EDGE 0
#define GAMEBOARD_RIGHT_EDGE 15
#define GAMEBOARD_BOTTOM_EDGE 11

int get_row (int pixel);
int get_col (int pixel, int row);
void draw_maze ();
int check_win ();
int move_left ();
int move_right ();
int move_up ();
int move_down ();
void handler (int signo);
int init_gamepad_driver ();
int init_framebuffer_driver ();

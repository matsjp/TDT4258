#include "game.h";
/*
w = wall
o = open space
p = placer
e = goal
*/
char gameboard[12][17] = { {"wwwwwwwwwwwwwwew\0"},
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

FILE *gamepad_driver;
FILE *framebuffer_driver;

int playerx = 3;
int playery = 3;
int goalx = 14;
int goaly = 0;
int framebuffer_fd;
uint16_t *framebuffer_pointer;
struct fb_copyarea rect;

/*
 *Function: get_row
 *
 *this function determines which row on the gameboard a pixel belongs to
 *
 *int pixel: the pixel whos gameboard row you want
 *
 *returns: the row the pixel belongs to
 */
int
get_row (int pixel)
{
  return (int) pixel / 320;
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
int
get_col (int pixel, int row)
{
  return (int) (((int) (pixel - row * 320)) / 20);
}

/*
 *Function: draw_maze
 *
 *this function determines which pixel corresponds to which grid on the game board, and gives that pixel the correct color
 *
 *returns: void
 */
void
draw_maze ()
{
  int i;
  int color;
  //loop through each pixel and give it color based on which grid in the gameboard it belongs to
  for (i = 0; i < SCREENSIZE_PIXELS; i++)
    {
      int row = get_row (i);
      int col = get_col (i, row);
      row = (int) (row / 20);
      char grid = gameboard[row][col];
      if (grid == 'w')
	{
	  color = BLACK;
	}
      else if (grid == 'o')
	{
	  color = WHITE;
	}
      else if (grid == 'p')
	{
	  color = YELLOW;
	}
      else if (grid == 'e')
	{
	  color = CYAN;
	}
      framebuffer_pointer[i] = color;
    }
  //update screen
  ioctl (framebuffer_fd, 0x4680, &rect);
}

/*
 *Function: check_win
 *
 *Checks if the player is in a winning state or not
 *
 *returns: 1 if the player is in a winning state, and 0 if not
 */
int
check_win ()
{
  return ((playerx == goalx) && (playery == goaly));
}


/*
 *Function: move_left
 *
 *Moves the player left on the gameboard if possible
 *
 *returns: 1 if move is legal, else 0
 */
int
move_left ()
{
  if (playerx != GAMEBOARD_LEFT_EDGE
      && gameboard[playery][playerx - 1] != 'w')
    {
      gameboard[playery][playerx] = 'o';
      playerx -= 1;
      gameboard[playery][playerx] = 'p';
      return 1;
    }
  return 0;
}

/*
 *Function: move_up
 *
 *Moves the player up on the gameboard if possible
 *
 *returns: 1 if the move is legal, else 0
 */
int
move_up ()
{
  if (playery != GAMEBOARD_TOP_EDGE && gameboard[playery - 1][playerx] != 'w')
    {
      gameboard[playery][playerx] = 'o';
      playery -= 1;
      gameboard[playery][playerx] = 'p';
      return 1;
    }
  return 0;
}

/*
 *Function: move_right
 *
 *Moves the player right on the gameboard if possible
 *
 *returns: 1 if move is legal, else 0
 */
int
move_right ()
{
  if (playerx != GAMEBOARD_RIGHT_EDGE
      && gameboard[playery][playerx + 1] != 'w')
    {
      gameboard[playery][playerx] = 'o';
      playerx += 1;
      gameboard[playery][playerx] = 'p';
      return 1;
    }
  return 0;
}

/*
 *Function: move_down
 *
 *Moves the player down on the gameboard if possible
 *
 *returns: 1 if move is legal, else 0
 */
int
move_down ()
{
  if (playery != GAMEBOARD_BOTTOM_EDGE
      && gameboard[playery + 1][playerx] != 'w')
    {
      gameboard[playery][playerx] = 'o';
      playery += 1;
      gameboard[playery][playerx] = 'p';
      return 1;
    }
  return 0;
}

/*
 *Function: handler
 *
 *This is the handler function that runs every time the gamepad driver send a asyncronous notification
 *
 *signo: the signal sent by the driver
 *
 *returns: void
 */
void
handler (int signo)
{
  int input = fgetc (gamepad_driver);
  //Legal move is used to keep track of if a attenped move is legal or not. If the move is legal then the screen is updated
  int legal_move;
  if (input == INPUT_DOWN)
    {
      legal_move = move_down ();
    }
  else if (input == INPUT_LEFT)
    {
      legal_move = move_left ();
    }
  else if (input == INPUT_RIGHT)
    {
      legal_move = move_right ();
    }
  else if (input == INPUT_UP)
    {
      legal_move = move_up ();
    }
  else
    {
      legal_move = 0;
    }
  if (legal_move)
    {
      draw_maze ();
    }
  //if player is at goal, teleport them back to the start
  if (check_win ())
    {
      gameboard[goaly][goalx] = 'e';
      playerx = 3;
      playery = 3;
      gameboard[playerx][playery] = 'p';
      draw_maze ();
    }
}

/*
 *Function: init_gamepad_driver
 *
 *Initiates the gamepad driver
 *
 *returns: integer showing success or failure 
 */
int
init_gamepad_driver ()
{
  gamepad_driver = fopen ("/dev/gamepad", "r+");
  int gamepad_driver_fd = fileno (gamepad_driver);
  if (!gamepad_driver)
    {
      printf ("Error while opening driver\n");
      return EXIT_FAILURE;
    }
  if (signal (SIGIO, &handler) == SIG_ERR)
    {
      printf ("Error while registering signal handler\n");
      return EXIT_FAILURE;
    }
  if (fcntl (gamepad_driver_fd, F_SETOWN, getpid ()) == -1)
    {
      printf ("Error while setting pid\n");
      return EXIT_FAILURE;
    }
  long oflags = fcntl (gamepad_driver_fd, F_GETFL);
  if (fcntl (gamepad_driver_fd, F_SETFL, oflags | FASYNC) == -1)
    {
      printf ("Error while setting FASYNC flag\n");
      return EXIT_FAILURE;
    }
  return 0;
}

/*
 *Function: init_framebuffer_driver
 *
 *Initiates the framebuffer driver
 *
 *returns: integer to show success or failure
 */
int
init_framebuffer_driver ()
{
  framebuffer_driver = fopen ("/dev/fb0", "r+");
  framebuffer_fd = fileno (framebuffer_driver);
  if (framebuffer_fd == -1)
    {
      printf ("Error while opening frame buffer device\n");
      return EXIT_FAILURE;
    }

  rect.dx = 0;
  rect.dy = 0;
  rect.width = 320;
  rect.height = 240;

  framebuffer_pointer =
    (uint16_t *) mmap (NULL, SCREENSIZE_BYTES, PROT_READ | PROT_WRITE,
		       MAP_SHARED, framebuffer_fd, 0);
  if ((int) framebuffer_pointer == (int) MAP_FAILED)
    {
      printf ("Error while making memory map\n");
      return EXIT_FAILURE;
    }
  return 0;
}

int
main (int argc, char *argv[])
{
  if (init_gamepad_driver () != 0)
    {
      return EXIT_FAILURE;
    }

  if (init_framebuffer_driver () != 0)
    {
      return EXIT_FAILURE;
    }
  //Draw the maze for the first time
  draw_maze ();
  //continually put the program to sleep and wait for input from the gamepad driver
  while (1)
    {
      pause ();
    }
}

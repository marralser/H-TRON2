/**************************************************************
*    htron.c                                   	              *
*    H-Tron - VGA 640x480, centered 640x350 arena             *
*    written in Borland Turbo C 2.01                          *
*    Conceived by RobertK 2016 and expanded by Markus R 2026  *
*    VGA colours with centered EGA-sized playfield            *
*                                                             *
*    Controls:                                                *
*    P1/P2 turn left/right: A/D and Cursor left/right         *
*    2026 edition: levels, joystick, AdLib, high scores      *
*    Pause game: P                                            *
*    End game: Escape                                         *
**************************************************************/

#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <string.h>

#define FIELD_WIDTH 80
#define FIELD_HEIGHT 58
#define BLOCK_WIDTH 8
#define BLOCK_HEIGHT 6
#define PLAYFIELD_TOP 3
#define ARENA_Y_OFFSET 65  /* (480-350)/2 */

#define POINTS_TO_WIN 5
#define AI_MAX_DISTANCE_TO_CHECK 15

/* VGA colours */
#define P1_COLOR LIGHTBLUE
#define P2_COLOR YELLOW
#define WALL_COLOR WHITE
#define SCORE_COLOR WHITE

/* Challenge / Hall of Fame scoring */
#define SCORE_PER_SECOND 20L
#define SCORE_PER_TURN   60L
#define SCORE_SURVIVE    400L
#define LEVEL_MATCH_WIN  2   /* best of three = first to two rounds */
#define HIGHSCORE_COUNT  10
#define PLAYER_NAME_LEN  30

#define HIGHSCORE_FILE   "HIGHSCORE.TXT"

/*
   Embedded ANSI/CP437 welcome artwork supplied for the 2026 edition.
   Only simple SGR colour sequences are used. The renderer below
   interprets them directly, so ANSI.SYS is not required.
*/
static unsigned char welcomeAnsi[] =
{
  0x1B,0x5B,0x30,0x6D,0x1B,0x5B,0x31,0x6D,0xDA,0xC4,0xC4,0xC4,0xC4,0xBF,0xDA,0xC4,
  0xC4,0xC4,0xC4,0xBF,0x20,0xDA,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,
  0xBF,0x20,0xDA,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xBF,0x20,0xDA,
  0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xBF,0x20,0x20,0xDA,0xC4,0xC4,0xC4,
  0xC4,0xBF,0x20,0x20,0x20,0xDA,0xC4,0xC4,0xC4,0xC4,0xBF,0x20,0xDA,0xC4,0xC4,0xC4,
  0xC4,0xC4,0xBF,0x1B,0x5B,0x30,0x6D,0x0D,0x0A,0x1B,0x5B,0x31,0x6D,0xC0,0xBF,0x20,
  0x20,0xDA,0xD9,0xC0,0xBF,0x20,0x20,0xDA,0xD9,0x20,0xB3,0x20,0xDA,0xC4,0xBF,0x20,
  0x20,0xDA,0xC4,0xBF,0x20,0xB3,0x20,0xC0,0xBF,0x20,0x20,0xDA,0xC4,0xC4,0xC4,0xBF,
  0x20,0x20,0xB3,0x20,0xB3,0x20,0x20,0xDA,0xC4,0xC4,0xC4,0xBF,0x20,0x20,0xB3,0x20,
  0x20,0xB3,0x20,0x20,0x20,0x20,0xC0,0xBF,0x20,0x20,0xC0,0xBF,0x20,0x20,0xDA,0xD9,
  0x20,0xC0,0xC4,0xC4,0xC4,0xBF,0x20,0xB3,0x1B,0x5B,0x30,0x6D,0x0D,0x0A,0x1B,0x5B,
  0x31,0x6D,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0xC0,
  0xC4,0xD9,0x20,0xB3,0x20,0x20,0xB3,0x20,0xC0,0xC4,0xD9,0x20,0x20,0xB3,0x20,0x20,
  0xB3,0x20,0x20,0x20,0xB3,0x20,0x20,0xB3,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0x20,
  0xB3,0x20,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0xC3,0xBF,0x20,0xC0,0xBF,0x20,0x20,
  0xB3,0x20,0x20,0xB3,0x20,0x20,0xDA,0xC4,0xC4,0xC4,0xD9,0x20,0xB3,0x1B,0x5B,0x30,
  0x6D,0x0D,0x0A,0x1B,0x5B,0x31,0x6D,0x20,0xB3,0x20,0x20,0xC0,0xC4,0xC4,0xD9,0x20,
  0x20,0xB3,0x1B,0x5B,0x30,0x6D,0x20,0x20,0x20,0x20,0x20,0x20,0x1B,0x5B,0x31,0x6D,
  0xB3,0x20,0x20,0xB3,0x1B,0x5B,0x30,0x6D,0x20,0x20,0x20,0x20,0x20,0x20,0x1B,0x5B,
  0x31,0x6D,0xB3,0x20,0x20,0xC0,0xC4,0xC4,0xC4,0xD9,0x20,0x20,0xB3,0x20,0xB3,0x20,
  0x20,0xB3,0x20,0x20,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0xB3,0xC0,
  0xBF,0x20,0xC0,0xBF,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0xB3,0x20,0xDA,0xC4,0xC4,
  0xC4,0xD9,0x1B,0x5B,0x30,0x6D,0x0D,0x0A,0x1B,0x5B,0x31,0x6D,0x20,0xB3,0x20,0x20,
  0xDA,0xC4,0xC4,0xBF,0x20,0x20,0xB3,0x1B,0x5B,0x30,0x6D,0x20,0x20,0x20,0x20,0x20,
  0x20,0x1B,0x5B,0x31,0x6D,0xB3,0x20,0x20,0xB3,0x1B,0x5B,0x30,0x6D,0x20,0x20,0x20,
  0x20,0x20,0x20,0x1B,0x5B,0x31,0x6D,0xB3,0x20,0x20,0xDA,0xBF,0x20,0x20,0xDA,0xC4,
  0xC4,0xD9,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,
  0xB3,0x20,0x20,0xB3,0x20,0xC0,0xBF,0x20,0xC0,0xBF,0xB3,0x20,0x20,0xB3,0x20,0x20,
  0xB3,0x20,0xC0,0xC4,0xC4,0xC4,0xBF,0x1B,0x5B,0x30,0x6D,0x0D,0x0A,0x1B,0x5B,0x31,
  0x6D,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0xB3,0x1B,0x5B,0x30,0x6D,
  0x20,0x20,0x20,0x20,0x20,0x20,0x1B,0x5B,0x31,0x6D,0xB3,0x20,0x20,0xB3,0x1B,0x5B,
  0x30,0x6D,0x20,0x20,0x20,0x20,0x20,0x20,0x1B,0x5B,0x31,0x6D,0xB3,0x20,0x20,0xB3,
  0xC0,0xBF,0x20,0xC0,0xBF,0x20,0x20,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0x20,0xB3,
  0x20,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0xB3,0x20,0x20,0xC0,0xBF,0x20,0xC0,0xB4,
  0x20,0x20,0xB3,0x20,0x20,0xC0,0xC4,0xC4,0xC4,0xC4,0xC4,0xD9,0x1B,0x5B,0x30,0x6D,
  0x0D,0x0A,0x1B,0x5B,0x31,0x6D,0xDA,0xD9,0x20,0x20,0xC0,0xBF,0xDA,0xD9,0x20,0x20,
  0xC0,0xBF,0x20,0x20,0x20,0x20,0xDA,0xD9,0x20,0x20,0xC0,0xBF,0x20,0x20,0x20,0x20,
  0xDA,0xD9,0x20,0x20,0xC0,0xBF,0xC0,0xBF,0x20,0xC0,0xC4,0xBF,0x20,0xB3,0x20,0x20,
  0xC0,0xC4,0xC4,0xC4,0xD9,0x20,0x20,0xB3,0x20,0xDA,0xD9,0x20,0x20,0xC0,0xBF,0x20,
  0x20,0xC0,0xBF,0x20,0x20,0x20,0x20,0xB3,0x1B,0x5B,0x30,0x6D,0x0D,0x0A,0x1B,0x5B,
  0x31,0x6D,0xC0,0xC4,0xC4,0xC4,0xC4,0xD9,0xC0,0xC4,0xC4,0xC4,0xC4,0xD9,0x20,0x20,
  0x20,0x20,0xC0,0xC4,0xC4,0xC4,0xC4,0xD9,0x20,0x20,0x20,0x20,0xC0,0xC4,0xC4,0xC4,
  0xC4,0xD9,0x20,0xC0,0xC4,0xC4,0xC4,0xD9,0x20,0xC0,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,
  0xC4,0xC4,0xC4,0xD9,0x20,0xC0,0xC4,0xC4,0xC4,0xC4,0xD9,0x20,0x20,0x20,0xC0,0xC4,
  0xC4,0xC4,0xC4,0xD9,0x1B,0x5B,0x30,0x6D,0x0D,0x0A,0x0D,0x0A,0x1B,0x5B,0x31,0x6D,
  0x20,0x1B,0x5B,0x30,0x3B,0x33,0x32,0x6D,0xD2,0xC4,0xC4,0xBF,0x20,0xD2,0xC4,0xC4,
  0xBF,0x20,0xD6,0xC4,0xD2,0xC4,0xBF,0x20,0xD2,0x20,0x20,0xC2,0x20,0xD2,0xC4,0xC4,
  0xBF,0x20,0xD6,0xC4,0xC4,0xBF,0x20,0x20,0xD6,0xC4,0xC4,0xBF,0x20,0xD2,0xC4,0xC4,
  0xBF,0x20,0x20,0xD6,0xC4,0xD2,0xC4,0xBF,0x20,0xD2,0x20,0x20,0xC2,0x20,0xD2,0xC4,
  0xC4,0xBF,0x20,0x20,0xC4,0xB7,0x20,0x20,0x20,0xD6,0xC4,0x20,0xD6,0xC4,0xC4,0xD2,
  0xC4,0xC4,0xBF,0x1B,0x5B,0x33,0x37,0x6D,0x0D,0x0A,0x1B,0x5B,0x33,0x32,0x6D,0x20,
  0xC7,0xC4,0xC2,0xD9,0x20,0xC7,0xC4,0x1B,0x5B,0x33,0x37,0x6D,0x20,0x20,0x20,0x20,
  0x20,0x1B,0x5B,0x33,0x32,0x6D,0xBA,0x20,0x20,0x20,0xBA,0x20,0x20,0xB3,0x20,0xC7,
  0xC4,0xC2,0xD9,0x20,0xBA,0x20,0x20,0xB3,0x20,0x20,0xBA,0x20,0x20,0xB3,0x20,0xC7,
  0xC4,0x1B,0x5B,0x33,0x37,0x6D,0x20,0x20,0x20,0x20,0x20,0x20,0x1B,0x5B,0x33,0x32,
  0x6D,0xBA,0x20,0x20,0x20,0xC7,0xC4,0xC4,0xB4,0x20,0xC7,0xC4,0x1B,0x5B,0x33,0x37,
  0x6D,0x20,0x20,0x20,0x20,0x20,0x1B,0x5B,0x33,0x32,0x6D,0xD3,0xB7,0x20,0xD6,0xBD,
  0x1B,0x5B,0x33,0x37,0x6D,0x20,0x20,0x20,0x20,0x20,0x1B,0x5B,0x33,0x32,0x6D,0xBA,
  0x1B,0x5B,0x33,0x37,0x6D,0x0D,0x0A,0x1B,0x5B,0x33,0x32,0x6D,0x20,0xD0,0x20,0xC1,
  0x20,0x20,0xD0,0xC4,0xC4,0xD9,0x20,0x20,0x20,0xD0,0x20,0x20,0x20,0xD3,0xC4,0xC4,
  0xD9,0x20,0xD0,0x20,0xC1,0x20,0x20,0xD0,0x20,0x20,0xC1,0x20,0x20,0xD3,0xC4,0xC4,
  0xD9,0x20,0xD0,0x1B,0x5B,0x33,0x37,0x6D,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1B,
  0x5B,0x33,0x32,0x6D,0xD0,0x20,0x20,0x20,0xD0,0x20,0x20,0xC1,0x20,0xD0,0xC4,0xC4,
  0xD9,0x20,0x20,0x20,0x20,0xC7,0xC4,0xB6,0x1B,0x5B,0x33,0x37,0x6D,0x20,0x20,0x20,
  0x20,0x20,0x20,0x1B,0x5B,0x33,0x32,0x6D,0xBA,0x1B,0x5B,0x33,0x37,0x6D,0x0D,0x0A,
  0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
  0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
  0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
  0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1B,0x5B,
  0x33,0x32,0x6D,0xD6,0xBD,0x20,0xD3,0xB7,0x1B,0x5B,0x33,0x37,0x6D,0x20,0x20,0x20,
  0x20,0x20,0x1B,0x5B,0x33,0x32,0x6D,0xBA,0x1B,0x5B,0x33,0x37,0x6D,0x0D,0x0A,0x20,
  0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
  0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
  0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
  0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1B,0x5B,0x33,0x32,
  0x6D,0xC4,0xBD,0x20,0x20,0x20,0xD3,0xC4,0x20,0x20,0x20,0x20,0xD0,0x1B,0x5B,0x33,
  0x37,0x6D,0x0D,0x0A,
  0x00
};


/* AdLib / Yamaha OPL2 */
#define ADLIB_ADDR 0x388
#define ADLIB_DATA 0x389

int adlibTurnActive;

struct HighScoreEntry
{
  long score;
  char name[PLAYER_NAME_LEN+1];
};

struct HighScoreEntry hall[HIGHSCORE_COUNT];
long gameScore;
int gameTicks;

/* Level 2: bouncing ball hazard */
#define LEVEL2_BALL_RADIUS 6
#define LEVEL2_BALL_COLOR  WHITE
#define LEVEL2_BALL_DX     6
#define LEVEL2_BALL_DY     5

/* Levels 3 and 5: a new 5x5-cell obstacle every five seconds */
#define RANDOM_OBSTACLE_SIZE   5
#define RANDOM_OBSTACLE_TICKS  100  /* 100 * 50 ms = 5 seconds */
#define RANDOM_OBSTACLE_COLOR  GREEN

/* Levels 4 and 5: two additional computer Trons */
#define CPU2_COLOR LIGHTGREEN
#define CPU3_COLOR LIGHTMAGENTA

int gameLevel;
int ballX;
int ballY;
int ballDX;
int ballDY;
int ballVisible;
/*
   Colour stored for each occupied logical cell. Collision code still
   uses field[][]; this parallel array lets the fast ball restore the
   tiny patch underneath itself without copying VGA image memory.
*/
unsigned char fieldColor[FIELD_WIDTH][FIELD_HEIGHT];

/* Development shortcut: 0=normal campaign, 2..5=start at that level */
int forcedLevel;

/* Level 3/5 obstacle timer */
int obstacleTicks;

/* Extra computer Trons used in Levels 4 and 5 */
int p3x,p3y,p4x,p4y;
int p3dir,p4dir;
int p3Crashed,p4Crashed;

/* Amstrad PC1512/PC1640 joystick raw scan codes.
   The Amstrad BIOS normally translates joystick directions to
   cursor-key scan codes, but accepting the raw values as well
   makes the game tolerant of systems/configurations that expose them. */
#define AMSTRAD_JOY_FIRE_A       0x77
#define AMSTRAD_JOY_FIRE_B       0x78
#define AMSTRAD_JOY_RIGHT        0x79
#define AMSTRAD_JOY_LEFT         0x7A
#define AMSTRAD_JOY_DOWN         0x7B
#define AMSTRAD_JOY_UP           0x7C

#define AMSTRAD_JOY_FIRE_A_UP    0xF7
#define AMSTRAD_JOY_FIRE_B_UP    0xF8
#define AMSTRAD_JOY_RIGHT_UP     0xF9
#define AMSTRAD_JOY_LEFT_UP      0xFA
#define AMSTRAD_JOY_DOWN_UP      0xFB
#define AMSTRAD_JOY_UP_UP        0xFC

/* Playfield array that holds the information whether
   there is an obstacle at this x,y coordinate.
   VGA mode, EGA-sized active grid: 80 x 58
   x=0 to 79
   y=0 to 57
   Rows 0..2 are reserved for the score area;
   row 3 is the upper playfield wall, as in the original.
*/
int field[FIELD_WIDTH][FIELD_HEIGHT];

/* indicates whether the player has hit an obstacle */
int p1Crashed;
int p2Crashed;

/* player coordinate variables */
int p1x,p1y,p2x,p2y;

/* player direction variables in radian degrees */
/* 0 (or 360) = right, 90 = up, 180 = left, 270 = down */
int p1dir,p2dir;

/* player score variables */
int p1Score,p2Score;

/* One or two players */

int numberOfPlayers;

/* -------------------------------------------------------------
   Low-level keyboard / Amstrad joystick input

   The PC1512/PC1640 joystick produces raw scan codes on IRQ 1,
   but these codes are not placed in the BIOS keyboard buffer.
   Therefore bioskey() cannot see them.

   Confirmed on a real Amstrad:
       Left   = 7Ah
       Right  = 79h
       Fire B = 78h

   During game play we temporarily replace INT 09h and read port
   60h directly.  Normal keyboard controls are decoded here too.
   ------------------------------------------------------------- */

#define KBD_DATA_PORT 0x60
#define SYS_PORT_B    0x61
#define PIC_CMD_PORT  0x20
#define PIC_EOI       0x20

volatile int ev_p1_left  = 0;
volatile int ev_p1_right = 0;
volatile int ev_p2_left  = 0;
volatile int ev_p2_right = 0;
volatile int ev_fire     = 0;
volatile int ev_escape   = 0;
volatile int ev_pause    = 0;
volatile int ev_anykey   = 0;

volatile int joy_left_down   = 0;
volatile int joy_right_down  = 0;
volatile int joy_up_down     = 0;
volatile int joy_down_down   = 0;
volatile int joy_fire_a_down = 0;
volatile int joy_fire_b_down = 0;
volatile int ext_prefix      = 0;

void interrupt (*oldKeyboardISR)(void);

void ClearInputEvents()
{
  disable();

  ev_p1_left=0;
  ev_p1_right=0;
  ev_p2_left=0;
  ev_p2_right=0;
  ev_fire=0;
  ev_escape=0;
  ev_pause=0;
  ev_anykey=0;

  enable();
}

void interrupt GameKeyboardISR(void)
{
  unsigned char sc;

  unsigned char pb;

  sc=inp(KBD_DATA_PORT);

  /*
     AMSTRAD PC1512/PC1640 keyboard acknowledge.
     Pulse Port B bit 7, then restore it low.
  */
  pb=inp(SYS_PORT_B);
  outp(SYS_PORT_B,pb | 0x80);
  outp(SYS_PORT_B,pb & 0x7F);

  /* Extended-key prefix used by cursor keys on many AT keyboards */
  if (sc==0xE0)
  {
    ext_prefix=1;
    outp(PIC_CMD_PORT,PIC_EOI);
    return;
  }

  /* -------- Confirmed Amstrad joystick BREAK codes -------- */
  if (sc==AMSTRAD_JOY_LEFT_UP)   joy_left_down=0;
  if (sc==AMSTRAD_JOY_RIGHT_UP)  joy_right_down=0;
  if (sc==AMSTRAD_JOY_UP_UP)     joy_up_down=0;
  if (sc==AMSTRAD_JOY_DOWN_UP)   joy_down_down=0;
  if (sc==AMSTRAD_JOY_FIRE_A_UP) joy_fire_a_down=0;
  if (sc==AMSTRAD_JOY_FIRE_B_UP) joy_fire_b_down=0;

  /* Only act on MAKE codes below. */
  if ((sc & 0x80)==0)
  {
    ev_anykey=1;

    /*
       Confirmed on the user's real PC1512/PC1640 joystick:
         Left  7A / FA       Right 79 / F9
         Up    7C / FC       Down  7B / FB
         FireA 77 / F7       FireB 78 / F8

       HTRON uses Left and Right as relative turning controls.
       Up/Down are tracked so the handler fully understands the
       joystick, but they do not steer the light cycle.
    */
    if (sc==AMSTRAD_JOY_LEFT)
    {
      if (!joy_left_down)
      {
        ev_p2_left++;
        joy_left_down=1;
      }
    }
    else if (sc==AMSTRAD_JOY_RIGHT)
    {
      if (!joy_right_down)
      {
        ev_p2_right++;
        joy_right_down=1;
      }
    }
    else if (sc==AMSTRAD_JOY_UP)
    {
      joy_up_down=1;
    }
    else if (sc==AMSTRAD_JOY_DOWN)
    {
      joy_down_down=1;
    }
    else if (sc==AMSTRAD_JOY_FIRE_A)
    {
      if (!joy_fire_a_down)
      {
        ev_fire=1;
        joy_fire_a_down=1;
      }
    }
    else if (sc==AMSTRAD_JOY_FIRE_B)
    {
      if (!joy_fire_b_down)
      {
        ev_fire=1;
        joy_fire_b_down=1;
      }
    }

    /* Normal PC keyboard */
    else if (sc==0x1E) ev_p1_left++;   /* A */
    else if (sc==0x20) ev_p1_right++;  /* D */
    else if (sc==0x4B) ev_p2_left++;   /* Cursor Left */
    else if (sc==0x4D) ev_p2_right++;  /* Cursor Right */
    else if (sc==0x39) ev_fire=1;      /* Space */
    else if (sc==0x19) ev_pause=1;     /* P */
    else if (sc==0x01) ev_escape=1;    /* Escape */
  }

  ext_prefix=0;

  /* End of interrupt to the 8259 PIC */
  outp(PIC_CMD_PORT,PIC_EOI);
}

void InstallGameKeyboard()
{
  ClearInputEvents();

  joy_left_down=0;
  joy_right_down=0;
  joy_up_down=0;
  joy_down_down=0;
  joy_fire_a_down=0;
  joy_fire_b_down=0;
  ext_prefix=0;

  oldKeyboardISR=getvect(0x09);
  setvect(0x09,GameKeyboardISR);
}

void RestoreKeyboard()
{
  setvect(0x09,oldKeyboardISR);
}



void ApplyAnsiTextColor(int baseColor,int bright)
{
  if (bright)
    textcolor(baseColor+8);
  else
    textcolor(baseColor);
}


void ShowWelcomeAnsi()
{
  unsigned int i;
  int bright;
  int baseColor;
  int value;
  int haveValue;
  unsigned char c;

  clrscr();
  textbackground(BLACK);

  bright=0;
  baseColor=LIGHTGRAY;
  ApplyAnsiTextColor(baseColor,bright);

  i=0;
  while(welcomeAnsi[i]!=0)
  {
    c=welcomeAnsi[i++];

    if (c==27 && welcomeAnsi[i]=='[')
    {
      i++;
      value=0;
      haveValue=0;

      while(welcomeAnsi[i]!=0 && welcomeAnsi[i]!='m')
      {
        c=welcomeAnsi[i++];

        if (c>='0' && c<='9')
        {
          value=value*10+(c-'0');
          haveValue=1;
        }
        else if (c==';')
        {
          if (!haveValue) value=0;

          if (value==0) { bright=0; baseColor=LIGHTGRAY; }
          if (value==1) bright=1;
          if (value==32) baseColor=GREEN;
          if (value==37) baseColor=LIGHTGRAY;

          value=0;
          haveValue=0;
        }
      }

      if (welcomeAnsi[i]=='m') i++;

      if (!haveValue) value=0;

      if (value==0) { bright=0; baseColor=LIGHTGRAY; }
      if (value==1) bright=1;
      if (value==32) baseColor=GREEN;
      if (value==37) baseColor=LIGHTGRAY;

      ApplyAnsiTextColor(baseColor,bright);
    }
    else
    {
      putch(c);
    }
  }

  textcolor(LIGHTGRAY);
}




/* =============================================================
   ADLIB / OPL2 SOUND EFFECTS

   These effects are based on the sounds tested in ADLIBTEST3.C.
   There is no background music.

   Important:
     - Turn chirp is NON-BLOCKING during active gameplay.
     - Longer sounds only run during round/level transitions.
   ============================================================= */

void AdlibDelay1()
{
  int i;
  for(i=0;i<6;i++) inp(ADLIB_ADDR);
}

void AdlibDelay2()
{
  int i;
  for(i=0;i<35;i++) inp(ADLIB_ADDR);
}

void AdlibWrite(unsigned char reg,unsigned char value)
{
  outp(ADLIB_ADDR,reg);
  AdlibDelay1();

  outp(ADLIB_DATA,value);
  AdlibDelay2();
}

void AdlibReset()
{
  int i;

  for(i=1;i<0xF6;i++)
    AdlibWrite((unsigned char)i,0);

  adlibTurnActive=0;
}

void AdlibInstrument()
{
  /* Bright electronic voice on OPL2 channel 0 */
  AdlibWrite(0x20,0x01);
  AdlibWrite(0x23,0x01);

  AdlibWrite(0x40,0x18);
  AdlibWrite(0x43,0x00);

  AdlibWrite(0x60,0xF3);
  AdlibWrite(0x63,0xF2);

  AdlibWrite(0x80,0x74);
  AdlibWrite(0x83,0x74);

  AdlibWrite(0xE0,0x00);
  AdlibWrite(0xE3,0x00);

  AdlibWrite(0xC0,0x06);
}

void AdlibNoteOn(unsigned int fnum,unsigned char block)
{
  AdlibWrite(0xA0,(unsigned char)(fnum & 0xFF));

  AdlibWrite(0xB0,
    (unsigned char)(
      0x20 |
      ((block & 7) << 2) |
      ((fnum >> 8) & 3)
    )
  );
}

void AdlibNoteOff()
{
  AdlibWrite(0xB0,0);
  adlibTurnActive=0;
}

void AdlibTone(unsigned int fnum,unsigned char block,int milliseconds)
{
  AdlibNoteOn(fnum,block);
  delay(milliseconds);
  AdlibNoteOff();
}


/*
   Gameplay turn chirp.

   No delay() is used here. The note is started immediately and
   AdlibUpdateTurnSound() switches it off on the next game tick.
*/
void SoundTurn()
{
  AdlibNoteOn(0x220,4);
  adlibTurnActive=1;
}

void AdlibUpdateTurnSound()
{
  if (adlibTurnActive)
    AdlibNoteOff();
}


/* Crash / falling sweep */
void SoundCrash()
{
  int f;

  for(f=0x2F0;f>=0x100;f-=12)
  {
    AdlibNoteOn((unsigned int)f,3);
    delay(12);
  }

  AdlibNoteOff();
}


/* Round start */
void SoundRoundStart()
{
  AdlibTone(0x160,3,80);
  AdlibTone(0x1C0,3,80);
  AdlibTone(0x230,3,120);
}


/* Longer Level-Up sound selected during testing */
void SoundLevelUp()
{
  AdlibTone(0x150,3,100);
  AdlibTone(0x1B0,3,100);
  AdlibTone(0x210,3,100);
  AdlibTone(0x160,4,130);
  delay(60);
  AdlibTone(0x210,4,320);
}


/* Level-2 energy-ball warning */
void SoundBallWarning()
{
  int i;

  for(i=0;i<3;i++)
  {
    AdlibTone(0x180,4,35);
    delay(20);
    AdlibTone(0x240,4,35);
    delay(20);
  }
}


/* Low impact sound, used when the ball hits the human Tron */
void SoundImpact()
{
  AdlibTone(0x110,2,130);
}


/*
   WIN E - selected victory sound.
   Tonal C-major arpeggio: C - E - G - C
*/
void SoundWin()
{
  static unsigned int noteF[12] =
  {
    0x157,0x16B,0x181,0x198,0x1B0,0x1CA,
    0x1E5,0x202,0x220,0x241,0x263,0x287
  };

  AdlibTone(noteF[0],4,140);  /* C */
  AdlibTone(noteF[4],4,140);  /* E */
  AdlibTone(noteF[7],4,180);  /* G */
  delay(60);
  AdlibTone(noteF[0],5,650);  /* high C */
}


void DrawGameScore()
{
  char scoreText[30];

  if (numberOfPlayers!=1) return;

  /* Clear only the centre part of the score panel. */
  setfillstyle(SOLID_FILL,BLACK);
  bar(245,ARENA_Y_OFFSET+7,395,ARENA_Y_OFFSET+17);

  sprintf(scoreText,"POINTS: %ld",gameScore);
  setcolor(SCORE_COLOR);
  outtextxy(320-(textwidth(scoreText)/2),ARENA_Y_OFFSET+9,scoreText);
}


void InitHall()
{
  int i;

  for(i=0;i<HIGHSCORE_COUNT;i++)
  {
    hall[i].score=0L;
    hall[i].name[0]=0;
  }
}


void LoadHall()
{
  FILE *f;
  char line[100];
  char *sep;
  int i;

  InitHall();

  f=fopen(HIGHSCORE_FILE,"rt");
  if (f==NULL) return;

  i=0;
  while(i<HIGHSCORE_COUNT && fgets(line,sizeof(line),f)!=NULL)
  {
    /* Remove CR/LF. */
    sep=strchr(line,'\r');
    if (sep!=NULL) *sep=0;
    sep=strchr(line,'\n');
    if (sep!=NULL) *sep=0;

    sep=strchr(line,'|');
    if (sep!=NULL)
    {
      *sep=0;
      hall[i].score=atol(line);
      strncpy(hall[i].name,sep+1,PLAYER_NAME_LEN);
      hall[i].name[PLAYER_NAME_LEN]=0;
      i++;
    }
  }

  fclose(f);
}


void SaveHall()
{
  FILE *f;
  int i;

  f=fopen(HIGHSCORE_FILE,"wt");
  if (f==NULL) return;

  for(i=0;i<HIGHSCORE_COUNT;i++)
  {
    if (hall[i].score>0L)
      fprintf(f,"%ld|%s\n",hall[i].score,hall[i].name);
  }

  fclose(f);
}


int ScoreQualifies(long score)
{
  int i;

  if (score<=0L) return 0;

  for(i=0;i<HIGHSCORE_COUNT;i++)
    if (score>hall[i].score) return 1;

  return 0;
}


void InsertHighScore(long score,char *name)
{
  int i,j;

  for(i=0;i<HIGHSCORE_COUNT;i++)
  {
    if (score>hall[i].score)
    {
      for(j=HIGHSCORE_COUNT-1;j>i;j--)
        hall[j]=hall[j-1];

      hall[i].score=score;
      strncpy(hall[i].name,name,PLAYER_NAME_LEN);
      hall[i].name[PLAYER_NAME_LEN]=0;
      return;
    }
  }
}


void EnterPlayerName(char *name)
{
  int key;
  int len;
  char display[50];

  len=0;
  name[0]=0;

  cleardevice();
  setcolor(SCORE_COLOR);
  outtextxy(150,160,"NEW HALL OF FAME SCORE!");
  outtextxy(150,190,"Enter your name (max 30 chars):");
  outtextxy(150,235,"Press ENTER when finished.");

  while(1)
  {
    sprintf(display,"%-30s",name);

    setfillstyle(SOLID_FILL,BLACK);
    bar(145,208,500,226);
    setcolor(P2_COLOR);
    outtextxy(150,210,display);

    key=getch();

    if (key==13) break;              /* Enter */

    if (key==8)                      /* Backspace */
    {
      if (len>0)
      {
        len--;
        name[len]=0;
      }
    }
    else if (key>=32 && key<=126 && len<PLAYER_NAME_LEN)
    {
      /*
         '|' is the separator in HIGHSCORE.TXT, so replace it
         with a normal space.
      */
      if (key=='|') key=' ';

      name[len]=(char)key;
      len++;
      name[len]=0;
    }
  }

  if (len==0)
    strcpy(name,"PLAYER");
}


void ShowHallOfFame()
{
  int i;
  int y;
  char line[80];

  cleardevice();

  setcolor(SCORE_COLOR);
  outtextxy(245,40,"HALL OF FAME");

  y=75;
  for(i=0;i<HIGHSCORE_COUNT;i++)
  {
    if (hall[i].score>0L)
      sprintf(line,"%2i. %-30s %8ld",i+1,hall[i].name,hall[i].score);
    else
      sprintf(line,"%2i. ---",i+1);

    if (i==0) setcolor(YELLOW);
    else setcolor(WHITE);

    outtextxy(115,y,line);
    y+=28;
  }
}



void RestoreFastBallArea()
{
  int gx1,gx2,gy1,gy2;
  int gx,gy;
  int px,py;
  int color;

  if (!ballVisible) return;

  /*
     A radius-6 ball overlaps only a handful of 8x6 logical cells.
     Redraw those cells from fieldColor[] instead of copying VGA
     image memory. This is considerably cheaper on an 8088.
  */
  gx1=(ballX-LEVEL2_BALL_RADIUS-1)/BLOCK_WIDTH;
  gx2=(ballX+LEVEL2_BALL_RADIUS+1)/BLOCK_WIDTH;
  gy1=(ballY-ARENA_Y_OFFSET-LEVEL2_BALL_RADIUS-1)/BLOCK_HEIGHT;
  gy2=(ballY-ARENA_Y_OFFSET+LEVEL2_BALL_RADIUS+1)/BLOCK_HEIGHT;

  if (gx1<0) gx1=0;
  if (gx2>=FIELD_WIDTH) gx2=FIELD_WIDTH-1;
  if (gy1<PLAYFIELD_TOP) gy1=PLAYFIELD_TOP;
  if (gy2>=FIELD_HEIGHT) gy2=FIELD_HEIGHT-1;

  for(gx=gx1;gx<=gx2;gx++)
  {
    for(gy=gy1;gy<=gy2;gy++)
    {
      px=gx*BLOCK_WIDTH;
      py=ARENA_Y_OFFSET+gy*BLOCK_HEIGHT;

      /* Restore black cell background first. */
      setfillstyle(SOLID_FILL,BLACK);
      bar(px,py,px+BLOCK_WIDTH-1,py+BLOCK_HEIGHT-1);

      color=fieldColor[gx][gy];

      if (color!=0)
      {
        setcolor(color);
        rectangle(px,py,
                  px+BLOCK_WIDTH-1,
                  py+BLOCK_HEIGHT-1);
      }
    }
  }

  ballVisible=0;
}


void DrawFastBall()
{
  /*
     Precomputed scanline shape. No fillellipse() calculation is
     performed during gameplay.
  */
  static int halfWidth[11] =
    {2,4,5,6,6,6,6,6,5,4,2};

  int row;
  int yy;

  setcolor(LEVEL2_BALL_COLOR);

  for(row=0;row<11;row++)
  {
    yy=ballY-5+row;
    line(ballX-halfWidth[row],yy,
         ballX+halfWidth[row],yy);
  }

  ballVisible=1;
}


int InitLevel2Ball()
{
  ballX=320;
  ballY=ARENA_Y_OFFSET+185;
  ballDX=LEVEL2_BALL_DX;
  ballDY=LEVEL2_BALL_DY;
  ballVisible=0;

  DrawFastBall();
  return 1;
}


void RemoveLevel2Ball()
{
  RestoreFastBallArea();
  ballVisible=0;
}




int CircleHitsField(int cx,int cy)
{
  int gx1,gx2,gy1,gy2;
  int gx,gy;
  int left,right,top,bottom;
  int nearX,nearY;
  long dx,dy;

  gx1=(cx-LEVEL2_BALL_RADIUS)/BLOCK_WIDTH;
  gx2=(cx+LEVEL2_BALL_RADIUS)/BLOCK_WIDTH;
  gy1=(cy-ARENA_Y_OFFSET-LEVEL2_BALL_RADIUS)/BLOCK_HEIGHT;
  gy2=(cy-ARENA_Y_OFFSET+LEVEL2_BALL_RADIUS)/BLOCK_HEIGHT;

  if (gx1<0) gx1=0;
  if (gy1<0) gy1=0;
  if (gx2>=FIELD_WIDTH) gx2=FIELD_WIDTH-1;
  if (gy2>=FIELD_HEIGHT) gy2=FIELD_HEIGHT-1;

  for(gx=gx1;gx<=gx2;gx++)
  {
    for(gy=gy1;gy<=gy2;gy++)
    {
      if (field[gx][gy] &&
          !(!p1Crashed && gx==p1x && gy==p1y) &&
          !(!p2Crashed && gx==p2x && gy==p2y) &&
          !(LevelHasThreeComputers() &&
            !p3Crashed && gx==p3x && gy==p3y) &&
          !(LevelHasThreeComputers() &&
            !p4Crashed && gx==p4x && gy==p4y))
      {
        left=gx*BLOCK_WIDTH;
        right=left+BLOCK_WIDTH-1;
        top=ARENA_Y_OFFSET+gy*BLOCK_HEIGHT;
        bottom=top+BLOCK_HEIGHT-1;

        if (cx<left) nearX=left;
        else if (cx>right) nearX=right;
        else nearX=cx;

        if (cy<top) nearY=top;
        else if (cy>bottom) nearY=bottom;
        else nearY=cy;

        dx=(long)cx-(long)nearX;
        dy=(long)cy-(long)nearY;

        if (dx*dx+dy*dy <=
            (long)LEVEL2_BALL_RADIUS*(long)LEVEL2_BALL_RADIUS)
          return 1;
      }
    }
  }

  return 0;
}


void UpdateLevel2Ball()
{
  int nextX,nextY;
  int bounceX,bounceY;
  int leftLimit,rightLimit,topLimit,bottomLimit;

  leftLimit=8+LEVEL2_BALL_RADIUS;
  rightLimit=631-LEVEL2_BALL_RADIUS;
  topLimit=ARENA_Y_OFFSET+24+LEVEL2_BALL_RADIUS;
  bottomLimit=ARENA_Y_OFFSET+347-LEVEL2_BALL_RADIUS;

  nextX=ballX+ballDX;
  nextY=ballY+ballDY;
  bounceX=0;
  bounceY=0;

  if (nextX<=leftLimit || nextX>=rightLimit)
    bounceX=1;

  if (nextY<=topLimit || nextY>=bottomLimit)
    bounceY=1;

  if (!bounceX && CircleHitsField(nextX,ballY))
    bounceX=1;

  if (!bounceY && CircleHitsField(ballX,nextY))
    bounceY=1;

  if (!bounceX && !bounceY && CircleHitsField(nextX,nextY))
  {
    bounceX=1;
    bounceY=1;
  }

  if (bounceX) ballDX=-ballDX;
  if (bounceY) ballDY=-ballDY;

  nextX=ballX+ballDX;
  nextY=ballY+ballDY;

  if (!CircleHitsField(nextX,nextY) &&
      nextX>leftLimit && nextX<rightLimit &&
      nextY>topLimit && nextY<bottomLimit)
  {
    ballX=nextX;
    ballY=nextY;
  }
}




int BallTouchesBlock(int gridX,int gridY)
{
  int left;
  int right;
  int top;
  int bottom;
  int nearX;
  int nearY;
  long dx;
  long dy;

  left=gridX*BLOCK_WIDTH;
  right=left+BLOCK_WIDTH-1;
  top=ARENA_Y_OFFSET+gridY*BLOCK_HEIGHT;
  bottom=top+BLOCK_HEIGHT-1;

  if (ballX<left) nearX=left;
  else if (ballX>right) nearX=right;
  else nearX=ballX;

  if (ballY<top) nearY=top;
  else if (ballY>bottom) nearY=bottom;
  else nearY=ballY;

  dx=(long)ballX-(long)nearX;
  dy=(long)ballY-(long)nearY;

  return (dx*dx+dy*dy <=
          (long)LEVEL2_BALL_RADIUS*(long)LEVEL2_BALL_RADIUS);
}


void ShowLevelScreen(int level)
{
  cleardevice();

  setcolor(YELLOW);
  settextstyle(DEFAULT_FONT,HORIZ_DIR,3);

  if (level==2) outtextxy(245,135,"LEVEL 2");
  if (level==3) outtextxy(245,135,"LEVEL 3");
  if (level==4) outtextxy(245,135,"LEVEL 4");
  if (level==5) outtextxy(245,135,"LEVEL 5");

  settextstyle(DEFAULT_FONT,HORIZ_DIR,1);
  setcolor(WHITE);

  if (level==2)
  {
    outtextxy(145,205,"A fast bouncing energy ball has entered the arena.");
    outtextxy(155,230,"Touching the ball makes you lose the round.");
  }
  else if (level==3)
  {
    outtextxy(125,205,"Every five seconds a new 5x5 obstacle appears.");
    outtextxy(145,230,"The objects remain for the rest of the round.");
  }
  else if (level==4)
  {
    outtextxy(135,205,"You now face THREE computer-controlled Trons.");
    outtextxy(130,230,"Outlive all three computers to win the round.");
  }
  else if (level==5)
  {
    outtextxy(120,195,"FINAL LEVEL: ball, random obstacles and three CPUs.");
    outtextxy(120,220,"Everything from the previous levels is active at once.");
    outtextxy(145,245,"Outlive all three computers to win.");
  }

  outtextxy(190,290,"Press SPACE or joystick FIRE.");

  ClearInputEvents();
  while (!ev_fire && !ev_escape)
    delay(1);

  settextstyle(DEFAULT_FONT,HORIZ_DIR,1);
}


int LevelHasBall()
{
  return (numberOfPlayers==1 && (gameLevel==2 || gameLevel==5));
}

int LevelHasRandomObstacles()
{
  return (numberOfPlayers==1 && (gameLevel==3 || gameLevel==5));
}

int LevelHasThreeComputers()
{
  return (numberOfPlayers==1 && (gameLevel==4 || gameLevel==5));
}

int AllComputersCrashed()
{
  if (!LevelHasThreeComputers())
    return p1Crashed;

  return (p1Crashed && p3Crashed && p4Crashed);
}

int CampaignRoundOver()
{
  return (p2Crashed || AllComputersCrashed());
}


int ObstacleAreaFree(int ox,int oy)
{
  int x,y;
  int left,right,top,bottom;

  for(x=ox;x<ox+RANDOM_OBSTACLE_SIZE;x++)
    for(y=oy;y<oy+RANDOM_OBSTACLE_SIZE;y++)
      if (field[x][y]) return 0;

  /*
     The ball is not represented in field[][], so explicitly avoid
     materializing a square on top of it in Level 5.
  */
  if (LevelHasBall() && ballVisible)
  {
    left=ox*BLOCK_WIDTH;
    right=(ox+RANDOM_OBSTACLE_SIZE)*BLOCK_WIDTH-1;
    top=ARENA_Y_OFFSET+oy*BLOCK_HEIGHT;
    bottom=ARENA_Y_OFFSET+
           (oy+RANDOM_OBSTACLE_SIZE)*BLOCK_HEIGHT-1;

    if (ballX+LEVEL2_BALL_RADIUS>=left &&
        ballX-LEVEL2_BALL_RADIUS<=right &&
        ballY+LEVEL2_BALL_RADIUS>=top &&
        ballY-LEVEL2_BALL_RADIUS<=bottom)
      return 0;
  }

  return 1;
}


void SpawnRandomObstacle()
{
  int tries;
  int ox,oy;
  int x,y;
  int maxX,maxY;

  maxX=FIELD_WIDTH-RANDOM_OBSTACLE_SIZE-2;
  maxY=FIELD_HEIGHT-RANDOM_OBSTACLE_SIZE-2;

  for(tries=0;tries<80;tries++)
  {
    ox=2+random(maxX-1);
    oy=PLAYFIELD_TOP+2+random(maxY-PLAYFIELD_TOP-1);

    if (ObstacleAreaFree(ox,oy))
    {
      setcolor(RANDOM_OBSTACLE_COLOR);
      setfillstyle(SOLID_FILL,RANDOM_OBSTACLE_COLOR);
      bar(ox*BLOCK_WIDTH,
          ARENA_Y_OFFSET+oy*BLOCK_HEIGHT,
          (ox+RANDOM_OBSTACLE_SIZE)*BLOCK_WIDTH-1,
          ARENA_Y_OFFSET+
          (oy+RANDOM_OBSTACLE_SIZE)*BLOCK_HEIGHT-1);

      for(x=ox;x<ox+RANDOM_OBSTACLE_SIZE;x++)
        for(y=oy;y<oy+RANDOM_OBSTACLE_SIZE;y++)
          field[x][y]=1;
          fieldColor[x][y]=RANDOM_OBSTACLE_COLOR;

      return;
    }
  }
}



void DrawBlock(int x, int y, int color)
/* x,y: block coordinates */
/* x=0 to 79, y=0 to 79 */
{
  int xCoord;
  int yCoord;

  if (x<0 || y<0 || x>=FIELD_WIDTH || y>=FIELD_HEIGHT) return;

  /* Transform to graphics coordinates */
  xCoord=x*BLOCK_WIDTH;
  yCoord=ARENA_Y_OFFSET+y*BLOCK_HEIGHT;

  /* draw block in the player's colour */
  setcolor(color);
  rectangle(xCoord,yCoord,xCoord+BLOCK_WIDTH-1,yCoord+BLOCK_HEIGHT-1);

  /* set obstacle marker and remember its display colour */
  field[x][y]=1;
  fieldColor[x][y]=(unsigned char)color;
}


void InitPlayfield()
{
  int x,y;
  char textP1[25];
  char textP2[25];
  char far * textScore = "S C O R E";

  for(x=0;x<FIELD_WIDTH;x++)
    for(y=0;y<FIELD_HEIGHT;y++)
    {
      field[x][y]=0;
      fieldColor[x][y]=0;
    }

  /*
     VGA remains 640x480 for 16 colours, but the active arena is
     intentionally 640x350 and vertically centered in VGA mode.
  */
  setcolor(WALL_COLOR);
  rectangle(0,ARENA_Y_OFFSET,639,ARENA_Y_OFFSET+349);
  rectangle(7,ARENA_Y_OFFSET+6,632,ARENA_Y_OFFSET+18);
  rectangle(7,ARENA_Y_OFFSET+23,632,ARENA_Y_OFFSET+347);

  if (numberOfPlayers==0)
  {
    sprintf(textP1,"Computer One: %i",p1Score);
    sprintf(textP2,"Computer Two: %i",p2Score);
  }
  else if (numberOfPlayers==1)
  {
    if (LevelHasThreeComputers())
      sprintf(textP1,"Computers: %i",p1Score);
    else
      sprintf(textP1,"Computer: %i",p1Score);

    sprintf(textP2,"Human: %i",p2Score);
  }
  else
  {
    sprintf(textP1,"Player One: %i",p1Score);
    sprintf(textP2,"Player Two: %i",p2Score);
  }

  setcolor(P1_COLOR);
  outtextxy(30,ARENA_Y_OFFSET+9,textP1);

  setcolor(P2_COLOR);
  outtextxy(610-textwidth(textP2),ARENA_Y_OFFSET+9,textP2);

  setcolor(SCORE_COLOR);
  if (numberOfPlayers==1)
  {
    char levelText[12];

    DrawGameScore();

    if (forcedLevel)
      sprintf(levelText,"Level %i TEST",gameLevel);
    else
      sprintf(levelText,"Level %i",gameLevel);

    setcolor(SCORE_COLOR);
    outtextxy(185,ARENA_Y_OFFSET+9,levelText);
  }
  else
    outtextxy(320-(textwidth(textScore)/2),ARENA_Y_OFFSET+9,textScore);

  /* Arena border cells */
  for(x=0;x<FIELD_WIDTH;x++)
  {
    field[x][PLAYFIELD_TOP]=1;
    fieldColor[x][PLAYFIELD_TOP]=WALL_COLOR;
    field[x][FIELD_HEIGHT-1]=1;
    fieldColor[x][FIELD_HEIGHT-1]=WALL_COLOR;
  }

  for(y=PLAYFIELD_TOP;y<FIELD_HEIGHT;y++)
  {
    field[0][y]=1;
    fieldColor[0][y]=WALL_COLOR;
    field[FIELD_WIDTH-1][y]=1;
    fieldColor[FIELD_WIDTH-1][y]=WALL_COLOR;
  }

  /* Main CPU and human */
  p1x=13;
  p1y=30;
  p2x=66;
  p2y=30;
  p1dir=0;
  p2dir=180;
  p1Crashed=0;
  p2Crashed=0;

  /* Extra CPUs for Levels 4 and 5 */
  p3x=40;
  p3y=8;
  p3dir=270;
  p3Crashed=0;

  p4x=40;
  p4y=49;
  p4dir=90;
  p4Crashed=0;

  DrawBlock(p1x,p1y,P1_COLOR);
  DrawBlock(p2x,p2y,P2_COLOR);

  if (LevelHasThreeComputers())
  {
    DrawBlock(p3x,p3y,CPU2_COLOR);
    DrawBlock(p4x,p4y,CPU3_COLOR);
  }
}


int CellBlocked(int x,int y)
{
  if (x<0 || x>=FIELD_WIDTH ||
      y<PLAYFIELD_TOP || y>=FIELD_HEIGHT)
    return 1;

  return field[x][y];
}


void StepOne(int *x,int *y,int dir)
{
  switch(dir)
  {
    case 0:   (*x)++; break;
    case 90:  (*y)--; break;
    case 180: (*x)--; break;
    case 270: (*y)++; break;
  }
}


/* Original two-Tron movement for demo and two-human modes. */
void MovePlayers()
{
  StepOne(&p1x,&p1y,p1dir);
  StepOne(&p2x,&p2y,p2dir);

  if (CellBlocked(p1x,p1y))
  {
    p1Crashed=1;
    p2Score++;
  }

  if (CellBlocked(p2x,p2y))
  {
    p2Crashed=1;
    p1Score++;
  }

  if (!p1Crashed && !p2Crashed &&
      p1x==p2x && p1y==p2y)
  {
    p1Crashed=1;
    p2Crashed=1;
    p1Score++;
    p2Score++;
  }

  if (p1Crashed && p2Crashed)
  {
    p1Score--;
    p2Score--;
  }

  if (!p1Crashed) DrawBlock(p1x,p1y,P1_COLOR);
  if (!p2Crashed) DrawBlock(p2x,p2y,P2_COLOR);
}


/*
   One-player campaign movement. p1 is always CPU #1, p2 is human.
   Levels 4/5 additionally activate p3 and p4.
   Scoring is deliberately done after the round ends, not here.
*/
void MoveCampaignPlayers()
{
  if (!p1Crashed) StepOne(&p1x,&p1y,p1dir);
  if (!p2Crashed) StepOne(&p2x,&p2y,p2dir);

  if (LevelHasThreeComputers())
  {
    if (!p3Crashed) StepOne(&p3x,&p3y,p3dir);
    if (!p4Crashed) StepOne(&p4x,&p4y,p4dir);
  }

  if (!p1Crashed && CellBlocked(p1x,p1y)) p1Crashed=1;
  if (!p2Crashed && CellBlocked(p2x,p2y)) p2Crashed=1;

  if (LevelHasThreeComputers())
  {
    if (!p3Crashed && CellBlocked(p3x,p3y)) p3Crashed=1;
    if (!p4Crashed && CellBlocked(p4x,p4y)) p4Crashed=1;
  }

  /*
     Detect head-to-head collisions where multiple Trons enter the
     same previously empty cell on the same tick.
  */
  if (!p1Crashed && !p2Crashed &&
      p1x==p2x && p1y==p2y)
  {
    p1Crashed=1;
    p2Crashed=1;
  }

  if (LevelHasThreeComputers())
  {
    if (!p1Crashed && !p3Crashed &&
        p1x==p3x && p1y==p3y)
    {
      p1Crashed=1;
      p3Crashed=1;
    }

    if (!p1Crashed && !p4Crashed &&
        p1x==p4x && p1y==p4y)
    {
      p1Crashed=1;
      p4Crashed=1;
    }

    if (!p2Crashed && !p3Crashed &&
        p2x==p3x && p2y==p3y)
    {
      p2Crashed=1;
      p3Crashed=1;
    }

    if (!p2Crashed && !p4Crashed &&
        p2x==p4x && p2y==p4y)
    {
      p2Crashed=1;
      p4Crashed=1;
    }

    if (!p3Crashed && !p4Crashed &&
        p3x==p4x && p3y==p4y)
    {
      p3Crashed=1;
      p4Crashed=1;
    }
  }

  if (!p1Crashed) DrawBlock(p1x,p1y,P1_COLOR);
  if (!p2Crashed) DrawBlock(p2x,p2y,P2_COLOR);

  if (LevelHasThreeComputers())
  {
    if (!p3Crashed) DrawBlock(p3x,p3y,CPU2_COLOR);
    if (!p4Crashed) DrawBlock(p4x,p4y,CPU3_COLOR);
  }
}


void AIComputerPlayer(int *x, int *y, int *dir)
{
  /* Let the computer player decide whether to turn left or right,
     or continue straight on.
     The coordinate and direction variables are passed by reference,
     so we can use this function for either computer player one or two */

  /* number of empty grid blocks before an obstacle
     from the AI player's point of view */
  int distanceFront,distanceLeft,distanceRight;

  int maxDistanceToChk;
  maxDistanceToChk=AI_MAX_DISTANCE_TO_CHECK;

  /* To make the AI player's movements less predictable,
  we occasionally make him a little short-sighted */
  maxDistanceToChk=maxDistanceToChk-random(10);

  /* Check the distance from the AI player's position
     to the next obstacle in the front, left and right
     direction from the AI player's point of view. According
     to the players direction, we have to use the respective
     grid direction */
  switch(*dir)
  {
    case 0: /* right */
       distanceFront=AICheckDistanceEast(*x,*y,maxDistanceToChk);
       distanceLeft=AICheckDistanceNorth(*x,*y,maxDistanceToChk);
       distanceRight=AICheckDistanceSouth(*x,*y,maxDistanceToChk);
       break;
    case 90: /* up */
       distanceFront=AICheckDistanceNorth(*x,*y,maxDistanceToChk);
       distanceLeft=AICheckDistanceWest(*x,*y,maxDistanceToChk);
       distanceRight=AICheckDistanceEast(*x,*y,maxDistanceToChk);
       break;
    case 180: /* left */
       distanceFront=AICheckDistanceWest(*x,*y,maxDistanceToChk);
       distanceLeft=AICheckDistanceSouth(*x,*y,maxDistanceToChk);
       distanceRight=AICheckDistanceNorth(*x,*y,maxDistanceToChk);
       break;
    case 270: /* down */
       distanceFront=AICheckDistanceSouth(*x,*y,maxDistanceToChk);
       distanceLeft=AICheckDistanceEast(*x,*y,maxDistanceToChk);
       distanceRight=AICheckDistanceWest(*x,*y,maxDistanceToChk);
       break;
  }

  /* Now we decide what the AI player shall do */
  if ((distanceFront>=distanceLeft) && (distanceFront>=distanceRight))
  {
    /* clear sailing ahead, nothing to do */
  }
  else if ((distanceFront<distanceLeft) || (distanceFront<distanceRight))
  {
    /* now we know that it would be safer to turn either left or right */
    /* let us check which direction would be better */
    if (distanceLeft>distanceRight)
    {
    	/* turn left */
    	*dir = *dir+90;
    	if (*dir>=360) *dir=0;
    }
    else if (distanceLeft<distanceRight)
    {
        /* turn right */
	*dir = *dir-90;
	if (*dir<0) *dir=270;
    }
    else /* distanceLeft == distanceRight */
    {
	/* randomly turn either left or right */
	if (random(100)<50)
	{
	  /* turn left */
	  *dir = *dir+90;
	  if (*dir>=360) *dir=0;
	}
	else
	{
	  /* turn right */
	  *dir = *dir-90;
	  if (*dir<0) *dir=270;
	}
    }
  }
}

/* The following four functions calculate the distance from the
   given position to the next obstacle in grid map direction */
int AICheckDistanceEast(int x,int y,int maxDistanceToCheck)
{
  int distance;
  distance=1;
  while(distance<=maxDistanceToCheck &&
        x+distance<FIELD_WIDTH &&
        field[x+distance][y]==0)
    distance++;
  return distance;
}

int AICheckDistanceWest(int x,int y,int maxDistanceToCheck)
{
  int distance;
  distance=1;
  while(distance<=maxDistanceToCheck &&
        x-distance>=0 &&
        field[x-distance][y]==0)
    distance++;
  return distance;
}

int AICheckDistanceNorth(int x,int y,int maxDistanceToCheck)
{
  int distance;
  distance=1;
  while(distance<=maxDistanceToCheck &&
        y-distance>=PLAYFIELD_TOP &&
        field[x][y-distance]==0)
    distance++;
  return distance;
}

int AICheckDistanceSouth(int x,int y,int maxDistanceToCheck)
{
  int distance;
  distance=1;
  while(distance<=maxDistanceToCheck &&
        y+distance<FIELD_HEIGHT &&
        field[x][y+distance]==0)
    distance++;
  return distance;
}



int GetForcedLevelParameter(int argc,char *argv[])
{
  int i;

  for(i=1;i<argc;i++)
  {
    if (!strcmp(argv[i],"/2") || !strcmp(argv[i],"-2")) return 2;
    if (!strcmp(argv[i],"/3") || !strcmp(argv[i],"-3")) return 3;
    if (!strcmp(argv[i],"/4") || !strcmp(argv[i],"-4")) return 4;
    if (!strcmp(argv[i],"/5") || !strcmp(argv[i],"-5")) return 5;
  }

  return 0;
}


void main(int argc,char *argv[])
{
  int x,y,i,j,exitgame;
  int key;
  int ballHitThisRound;
  int notPlayAgain;
  int grd, grm;
  int gresult;
  char textFinalScore[22];
  char textChallengeScore[40];
  char playerName[PLAYER_NAME_LEN+1];

  notPlayAgain=0;
  randomize();
  LoadHall();

  forcedLevel=GetForcedLevelParameter(argc,argv);
  ballVisible=0;

  clrscr();
  if (!forcedLevel)
  {
    ShowWelcomeAnsi();

    cprintf("\r\nH-Tron was conceived by RobertK in 2016, and extended by MarkusR in 2026,\r\n");
    cprintf("on XT-class PCs and Borland's Turbo C.\r\n\r\n");

    cprintf("You drive a Tron. Steer it left or right using the keyboard or an Amstrad\r\n");
    cprintf("PC joystick. Win a best-of-three to advance to the next level.\r\n\r\n");

    cprintf("Please choose:  (0) Demo Mode  (1) One Player  (2) Battle w. 2 Players: ");

    do
    {
      key=getch();

      if (key=='0') numberOfPlayers=0;
      if (key=='1') numberOfPlayers=1;
      if (key=='2') numberOfPlayers=2;

      /*
         Hidden development shortcuts:
           D = Level 2
           E = Level 3
           F = Level 4
           G = Level 5
      */
      if (key=='d' || key=='D') { numberOfPlayers=1; forcedLevel=2; }
      if (key=='e' || key=='E') { numberOfPlayers=1; forcedLevel=3; }
      if (key=='f' || key=='F') { numberOfPlayers=1; forcedLevel=4; }
      if (key=='g' || key=='G') { numberOfPlayers=1; forcedLevel=5; }

      if (key==27)
      {
        clrscr();
        return;
      }
    }
    while(key!='0' && key!='1' && key!='2' &&
          key!='d' && key!='D' &&
          key!='e' && key!='E' &&
          key!='f' && key!='F' &&
          key!='g' && key!='G');
  }
  else
  {
    numberOfPlayers=1;
    printf("*** H-Tron - DEVELOPMENT MODE ***\n");
    printf("Starting directly in Level %i.\n",forcedLevel);
    printf("Press any key to enter graphics mode.\n");
    getch();
  }


  /* === switch to graphics mode: beginning of section === */

  /* Force the Borland BGI VGA driver and its 640 x 480 mode.
     EGAVGA.BGI must be available in the current directory, or
     replace the empty path below with the directory containing
     your BGI drivers (for example "C:\\TC\\BGI"). */
  grd = VGA;
  grm = VGAHI;

  initgraph(&grd, &grm, "");
  gresult = graphresult();
  if(gresult != grOk)
  {
    printf("%s",grapherrormsg(gresult));
    getch();
    return;
  }

  setbkcolor(BLACK);
  setcolor(WALL_COLOR);

  /*
     Initialize AdLib/OPL2 sound.
     On a machine without an AdLib-compatible card these port
     writes normally have no effect.
  */
  AdlibReset();
  AdlibInstrument();

  /* === switch to graphics mode: end of section === */



  /* game restart loop */
  while(notPlayAgain<1)
  {
    p1Score=0;
    p2Score=0;
    exitgame=0;
    gameScore=0L;
    gameTicks=0;
    /*
       One-player campaign:
       each level is first to two round wins (best of three).
       Development mode starts directly at the selected level and
       stops after that one match.
    */
    if (forcedLevel)
      gameLevel=forcedLevel;
    else
      gameLevel=1;

    while(exitgame<1)
    {
      if (numberOfPlayers==1)
      {
        if (p1Score>=LEVEL_MATCH_WIN || p2Score>=LEVEL_MATCH_WIN)
        {
          if (p2Score>=LEVEL_MATCH_WIN &&
              !forcedLevel &&
              gameLevel<5)
          {
            /*
               Celebrate winning the completed level, then announce
               the transition to the next one.
            */
            SoundWin();

            gameLevel++;
            p1Score=0;
            p2Score=0;

            SoundLevelUp();

            if (gameLevel==2 || gameLevel==5)
              SoundBallWarning();

            InstallGameKeyboard();
            ShowLevelScreen(gameLevel);

            if (ev_escape) exitgame=1;

            RestoreKeyboard();
            ClearInputEvents();

            if (exitgame) break;
          }
          else
            break;
        }
      }
      else
      {
        if (p1Score>=POINTS_TO_WIN || p2Score>=POINTS_TO_WIN)
          break;
      }

	cleardevice();
	InitPlayfield();
	ballHitThisRound=0;
	obstacleTicks=0;

  	/* workaround to avoid pause immediately after the game starts */
  	delay(10);

	/*
	   The Amstrad joystick is invisible to bioskey(), so install
	   our raw IRQ1 handler for the round.
	*/
	InstallGameKeyboard();

	if (numberOfPlayers>0)
	{
	  /* Wait for keyboard Space or either Amstrad joystick fire button. */
	  ClearInputEvents();
	  while (!ev_fire && !ev_escape)
	    delay(1);

	  if (ev_escape) exitgame=1;
	  ClearInputEvents();

	  if (!exitgame)
	  {
	    SoundRoundStart();
	    ClearInputEvents();
	  }

	  gameTicks=0;
	  DrawGameScore();

	  if (LevelHasBall())
	    InitLevel2Ball();
	  else
	    ballVisible=0;
	}
	else
	{
	  delay(1000); /* in demo mode, the round starts automatically */
	  ballVisible=0;
	}

	/* main game loop */
  	while(exitgame<1 &&
              ((numberOfPlayers==1 && !CampaignRoundOver()) ||
               (numberOfPlayers!=1 && !p1Crashed && !p2Crashed)))
  	{
	  /*
	     A turn chirp started during the previous frame is stopped
	     here. This keeps the sound short without delaying gameplay.
	  */
	  AdlibUpdateTurnSound();

	  /*
	     Process all input events collected by IRQ1.
	     The counters make very quick key/joystick presses reliable.
	  */

	  if (ev_escape)
	  {
	    ev_escape=0;
	    exitgame=1;
	  }

	  if (ev_pause)
	  {
	    ev_pause=0;
	    ev_anykey=0;

	    /* Original behaviour: pause until another key/control is pressed. */
	    while (!ev_anykey && !ev_escape)
	      delay(1);

	    ev_anykey=0;
	  }

	  while (ev_p2_left>0)
	  {
	    ev_p2_left--;
	    if (numberOfPlayers>0)
	    {
	      p2dir=p2dir+90;
	      if (p2dir>=360) p2dir=0;

	      SoundTurn();

	      if (numberOfPlayers==1)
	      {
	        gameScore+=SCORE_PER_TURN;
	        DrawGameScore();
	      }
	    }
	  }

	  while (ev_p2_right>0)
	  {
	    ev_p2_right--;
	    if (numberOfPlayers>0)
	    {
	      p2dir=p2dir-90;
	      if (p2dir<0) p2dir=270;

	      SoundTurn();

	      if (numberOfPlayers==1)
	      {
	        gameScore+=SCORE_PER_TURN;
	        DrawGameScore();
	      }
	    }
	  }

	  while (ev_p1_left>0)
	  {
	    ev_p1_left--;
	    if (numberOfPlayers==2)
	    {
	      p1dir=p1dir+90;
	      if (p1dir>=360) p1dir=0;
	      SoundTurn();
	    }
	  }

	  while (ev_p1_right>0)
	  {
	    ev_p1_right--;
	    if (numberOfPlayers==2)
	    {
	      p1dir=p1dir-90;
	      if (p1dir<0) p1dir=270;
	      SoundTurn();
	    }
	  }

	  /* Computer steering */
	  if (numberOfPlayers==1)
	  {
	    if (!p1Crashed) AIComputerPlayer(&p1x,&p1y,&p1dir);

	    if (LevelHasThreeComputers())
	    {
	      if (!p3Crashed) AIComputerPlayer(&p3x,&p3y,&p3dir);
	      if (!p4Crashed) AIComputerPlayer(&p4x,&p4y,&p4dir);
	    }
	  }
	  else
	  {
	    if (numberOfPlayers<2) AIComputerPlayer(&p1x,&p1y,&p1dir);
	    if (numberOfPlayers==0) AIComputerPlayer(&p2x,&p2y,&p2dir);
	  }

	  /*
	     Level 2 rendering order:

	       1. Redraw the few logical cells beneath the old ball.
	       2. Let the Trons update their trails.
	       3. Move/bounce the ball twice for extra speed.
	       4. Draw the ball from precomputed horizontal scanlines.

	     No getimage(), putimage(), malloc() or fillellipse() is used
	     in the active ball loop, which is much friendlier to 8088 PCs.
	  */
	  if (LevelHasBall() && ballVisible)
	    RestoreFastBallArea();

    	  if (numberOfPlayers==1)
	    MoveCampaignPlayers();
	  else
	    MovePlayers();

	  /*
	     Level 3 / 5: spawn one permanent 5x5-cell obstacle every
	     five seconds of active gameplay.
	  */
	  if (LevelHasRandomObstacles() && !CampaignRoundOver())
	  {
	    obstacleTicks++;
	    if (obstacleTicks>=RANDOM_OBSTACLE_TICKS)
	    {
	      obstacleTicks=0;
	      SpawnRandomObstacle();
	    }
	  }

	  if (LevelHasBall() &&
	      !p2Crashed && !exitgame)
	  {
	    /*
	       Two ball sub-steps per 50 ms Tron frame make the ball
	       noticeably faster while keeping the main game speed intact.
	    */
	    UpdateLevel2Ball();

	    if (!BallTouchesBlock(p2x,p2y))
	      UpdateLevel2Ball();

	    if (BallTouchesBlock(p2x,p2y))
	    {
	      p2Crashed=1;
	      ballHitThisRound=1;
	      SoundImpact();
	    }
	    else
	    {
	      DrawFastBall();
	    }
	  }

	  /*
	     One-player challenge timer.
	     One loop step is 50 ms; 20 active steps are approximately
	     one second. Paused time is not counted because the loop is
	     stopped inside the pause handler.
	  */
	  if (numberOfPlayers==1 && !CampaignRoundOver() && !exitgame)
	  {
	    gameTicks++;
	    if (gameTicks>=20)
	    {
	      gameTicks=0;
	      gameScore+=SCORE_PER_SECOND;
	      DrawGameScore();
	    }
	  }

	  /* Sleep for 50 milliseconds.
	     note: the delay() function was introduced in Turbo C 2.0
	     (it is not available in Turbo C 1.0) */
	  delay(50);
        }

	/* Restore the last saved background and release ball buffer. */
	if (LevelHasBall())
	  RemoveLevel2Ball();

	/* Restore the normal BIOS keyboard handler after each round. */
	RestoreKeyboard();

	if (numberOfPlayers==1)
	{
	  /*
	     Award one match point only after the whole campaign round
	     is resolved. In Levels 4/5 the human must outlive all 3 CPUs.
	  */
	  if (p2Crashed && AllComputersCrashed())
	  {
	    /* simultaneous elimination: no round point */
	  }
	  else if (p2Crashed)
	    p1Score++;
	  else if (AllComputersCrashed())
	    p2Score++;

	  if ((p2Crashed || AllComputersCrashed()) && !ballHitThisRound)
	    SoundCrash();

	  if (AllComputersCrashed() && !p2Crashed)
	  {
	    gameScore+=SCORE_SURVIVE;
	    DrawGameScore();
	  }
	}
	else
	{
	  if ((p1Crashed || p2Crashed) && !ballHitThisRound)
	    SoundCrash();
	}

	/* wait a quarter of a second after a crash has
	   happened before clearing the screen */
	if ((numberOfPlayers==1 && CampaignRoundOver()) ||
	    (numberOfPlayers!=1 && (p1Crashed || p2Crashed)))
	  delay(250);
    }

    /*
       Win E fanfare when the human completes the final Level 5.
       In development mode it also plays when the tested level is won.
    */
    if (numberOfPlayers==1 &&
        p2Score>=LEVEL_MATCH_WIN &&
        !exitgame &&
        (gameLevel==5 || forcedLevel))
      SoundWin();

    /*
       In one-player mode the challenge score can enter the
       persistent Hall of Fame.
    */
    if (numberOfPlayers==1)
    {
      LoadHall();

      if (!forcedLevel && ScoreQualifies(gameScore))
      {
        EnterPlayerName(playerName);
        InsertHighScore(gameScore,playerName);
        SaveHall();
      }

      if (!forcedLevel)
        ShowHallOfFame();
      else
      {
        cleardevice();
        setcolor(YELLOW);
        {
          char devText[30];
          sprintf(devText,"LEVEL %i TEST COMPLETE",gameLevel);
          outtextxy(215,80,devText);
        }
      }

      sprintf(textChallengeScore,"Your score: %ld",gameScore);
      setcolor(P2_COLOR);
      outtextxy(230,365,textChallengeScore);

      sprintf(textFinalScore,"Level reached: %i",gameLevel);
      outtextxy(245,390,textFinalScore);

      sprintf(textFinalScore,"Rounds: %i : %i",p1Score,p2Score);
      setcolor(SCORE_COLOR);
      outtextxy(245,415,textFinalScore);

      outtextxy(235,445,"Play again (y/n)?");
    }
    else
    {
      cleardevice();
      sprintf(textFinalScore,"Final Score: %i : %i",p1Score,p2Score);
      setcolor(SCORE_COLOR);
      outtextxy(40,20,textFinalScore);

      if (p1Score>p2Score)
      {
        if (numberOfPlayers==0)
          outtextxy(40,40,"Computer Player One wins!");
        else
          outtextxy(40,40,"Player One wins!");
      }

      if (p1Score<p2Score)
      {
        if (numberOfPlayers==0)
          outtextxy(40,40,"Computer Player Two wins!");
        else
          outtextxy(40,40,"Player Two wins!");
      }

      if (p1Score==p2Score)
        outtextxy(40,40,"A draw!");

      outtextxy(40,60,"Play again (y/n)?");
    }

    do
    {
      key=getch();
      if (key=='n' || key=='N') notPlayAgain=1;

    } while(key!='y' && key!='Y' && key!='n' && key!='N');

  }
  /* Silence OPL2 before returning to DOS. */
  AdlibReset();

  restorecrtmode(); /* switch back to text mode */
  closegraph(); /* to clean up memory */
  clrscr();

  return;
}
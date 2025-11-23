#include "gfx.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "tetris.h"
#include "hal.h"  // Per palReadPad()

// Definizione pin joystick
#define JOY_PORT_1      GPIOB
#define JOY_PORT_2      GPIOC
#define JOY_UP_PIN          0
#define JOY_DOWN_PIN        4
#define JOY_LEFT_PIN        6
#define JOY_RIGHT_PIN       0
#define JOY_CENTRE_PIN      7
#define SEVEN_SEG_HEIGHT            SEVEN_SEG_SIZE*3
#define SEVEN_SEG_WIDTH             SEVEN_SEG_HEIGHT*3
#define SEVEN_SEG_CHAR_WIDTH        ((SEVEN_SEG_SIZE*4)+(SEVEN_SEG_HEIGHT*2)+SEVEN_SEG_WIDTH)
#define SEVEN_SEG_CHAR_HEIGHT       ((SEVEN_SEG_SIZE*4)+(SEVEN_SEG_HEIGHT*3)+(SEVEN_SEG_WIDTH*2))
#define TETRIS_SEVEN_SEG_SCORE_X(i, pps_str_len)    ((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH) - (pps_str_len*SEVEN_SEG_CHAR_WIDTH) + (SEVEN_SEG_CHAR_WIDTH*i))

// Definizione pin joystick
#define JOY_PORT_1      GPIOB
#define JOY_PORT_2      GPIOC
//#define JOY_PORT_3      GPIOA
#define JOY_UP_PIN          0
#define JOY_DOWN_PIN        4
#define JOY_LEFT_PIN        6
#define JOY_RIGHT_PIN       0
#define JOY_CENTRE_PIN      7

/*
 * Inizializzazione pin joystick
 */
static void joystickInit(void) {
    palSetPadMode(JOY_PORT_2, JOY_UP_PIN, PAL_MODE_INPUT_PULLUP);            //UP
    palSetPadMode(JOY_PORT_1, JOY_DOWN_PIN, PAL_MODE_INPUT_PULLUP);          //DOWN
    palSetPadMode(JOY_PORT_1, JOY_LEFT_PIN, PAL_MODE_INPUT_PULLUP);          //LEFT
    palSetPadMode(JOY_PORT_1, JOY_RIGHT_PIN, PAL_MODE_INPUT_PULLUP);         //RIGHT
    palSetPadMode(JOY_PORT_2, JOY_CENTRE_PIN, PAL_MODE_INPUT_PULLUP);      //CENTRE
}



// bit0 = A
// bit1 = B
// bit2 = C
// bit3 = D
// bit4 = E
// bit5 = F
// bit6 = G
// 7segment number array, starting from 0 (array index matches number)
// 7segment number:
/*
   A
F     B
   G
E     C
   D
*/
const gU8   sevenSegNumbers[10]                                   = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // 0,1,2,3,4,5,6,7,8,9
const gColor   tetrisShapeColors[9]                                  = {GFX_BLACK, HTML2COLOR(0x009000), GFX_RED, GFX_BLUE, GFX_MAGENTA, GFX_SKYBLUE, GFX_ORANGE, HTML2COLOR(0xBBBB00), GFX_WHITE}; // shape colors
// default tetris shapes
const int       tetrisShapes[TETRIS_SHAPE_COUNT][4][2]                = {
                                                                          {{4, 17},{4, 16},{5, 16},{4, 15}},
                                                                          {{4, 16},{5, 16},{4, 15},{5, 15}},
                                                                          {{4, 17},{5, 16},{4, 16},{5, 15}},
                                                                          {{5, 17},{5, 16},{4, 16},{4, 15}},
                                                                          {{5, 17},{5, 16},{5, 15},{4, 15}},
                                                                          {{4, 17},{4, 16},{4, 15},{5, 15}},
                                                                          {{4, 18},{4, 17},{4, 16},{4, 15}}
                                                                        };

int             tetrisField[TETRIS_FIELD_HEIGHT][TETRIS_FIELD_WIDTH];        // main tetris field array
unsigned int    tetrisGameSpeed                                       = 400; // game auto-move speed in ms
unsigned int    tetrisKeySpeed                                        = 140; // game key repeat speed in ms
gTicks   tetrisPreviousGameTime                                = 0;
gTicks   tetrisPreviousKeyTime                                 = 0;
int             tetrisCurrentShape[4][2];
int             tetrisNextShape[4][2];
int             tetrisOldShape[4][2];
int             tetrisNextShapeNum, tetrisOldShapeNum;
unsigned long   tetrisLines                                           = 0;
unsigned long   tetrisScore                                           = 0;
gBool          tetrisKeysPressed[5]                                  = {gFalse, gFalse, gFalse, gFalse, gFalse}; // left/down/right/up/pause
gBool          tetrisPaused                                          = gFalse;
gBool          tetrisGameOver                                        = gFalse;
gFont          font16;
gFont          font12;

// GEventMouse     ev; // Non usata con i pulsanti fisici

static void initRng(void) {
  srand(gfxSystemTicks());
}

static int uitoa(unsigned int value, char * buf, int max) {
  int n = 0;
  int i = 0;
  int tmp = 0;

  if (!buf) return -3;
  if (2 > max) return -4;
  i=1;
  tmp = value;
  if (0 > tmp) {
    tmp *= -1;
    i++;
  }
  for (;;) {
    tmp /= 10;
    if (0 >= tmp) break;
    i++;
  }
  if (i >= max) {
    buf[0] = '?';
    buf[1] = 0x0;
    return 2;
  }
  n = i;
  tmp = value;
  if (0 > tmp) {
    tmp *= -1;
  }
  buf[i--] = 0x0;
  for (;;) {
    buf[i--] = (tmp % 10) + '0';
    tmp /= 10;
    if (0 >= tmp) break;
  }
  if (-1 != i) {
    buf[i--] = '-';
  }
  return n;
}

static void sevenSegDraw(int x, int y, gU8 number, gColor color) {
  if (number & 0x01) gdispFillArea(x+SEVEN_SEG_HEIGHT+SEVEN_SEG_SIZE, y, SEVEN_SEG_WIDTH, SEVEN_SEG_HEIGHT, color); // A
  if (number & 0x02) gdispFillArea(x+SEVEN_SEG_HEIGHT+(SEVEN_SEG_SIZE*2)+SEVEN_SEG_WIDTH, y+SEVEN_SEG_HEIGHT+SEVEN_SEG_SIZE, SEVEN_SEG_HEIGHT, SEVEN_SEG_WIDTH, color); // B
  if (number & 0x04) gdispFillArea(x+SEVEN_SEG_HEIGHT+(SEVEN_SEG_SIZE*2)+SEVEN_SEG_WIDTH, y+(SEVEN_SEG_HEIGHT*2)+SEVEN_SEG_WIDTH+(SEVEN_SEG_SIZE*3), SEVEN_SEG_HEIGHT, SEVEN_SEG_WIDTH, color); // C
  if (number & 0x08) gdispFillArea(x+SEVEN_SEG_HEIGHT+SEVEN_SEG_SIZE, y+(SEVEN_SEG_HEIGHT*2)+(SEVEN_SEG_WIDTH*2)+(SEVEN_SEG_SIZE*4), SEVEN_SEG_WIDTH, SEVEN_SEG_HEIGHT, color); // D
  if (number & 0x10) gdispFillArea(x, y+(SEVEN_SEG_HEIGHT*2)+SEVEN_SEG_WIDTH+(SEVEN_SEG_SIZE*3), SEVEN_SEG_HEIGHT, SEVEN_SEG_WIDTH, color); // E
  if (number & 0x20) gdispFillArea(x, y+SEVEN_SEG_HEIGHT+SEVEN_SEG_SIZE, SEVEN_SEG_HEIGHT, SEVEN_SEG_WIDTH, color); // F
  if (number & 0x40) gdispFillArea(x+SEVEN_SEG_HEIGHT+SEVEN_SEG_SIZE, y+SEVEN_SEG_HEIGHT+SEVEN_SEG_WIDTH+(SEVEN_SEG_SIZE*2), SEVEN_SEG_WIDTH, SEVEN_SEG_HEIGHT, color); // G
}

static void drawShape(gU8 color) {
  int i;
  for (i = 0; i <= 3; i++) {
    if (tetrisCurrentShape[i][1] <= 16 || tetrisCurrentShape[i][1] >= 19) {
      gdispFillArea((tetrisCurrentShape[i][0]*TETRIS_CELL_WIDTH)+2, gdispGetHeight()-TETRIS_CELL_HEIGHT-(tetrisCurrentShape[i][1]*TETRIS_CELL_HEIGHT)-3, TETRIS_CELL_WIDTH-2, TETRIS_CELL_HEIGHT-2, tetrisShapeColors[color]);
       if (color != 0) {
         gdispDrawBox((tetrisCurrentShape[i][0]*TETRIS_CELL_WIDTH)+2, gdispGetHeight()-TETRIS_CELL_HEIGHT-(tetrisCurrentShape[i][1]*TETRIS_CELL_HEIGHT)-3, TETRIS_CELL_WIDTH-1, TETRIS_CELL_HEIGHT-1, tetrisShapeColors[8]);
       } else {
         gdispDrawBox((tetrisCurrentShape[i][0]*TETRIS_CELL_WIDTH)+2, gdispGetHeight()-TETRIS_CELL_HEIGHT-(tetrisCurrentShape[i][1]*TETRIS_CELL_HEIGHT)-3, TETRIS_CELL_WIDTH-1, TETRIS_CELL_HEIGHT-1, tetrisShapeColors[0]);
       }
    }
  }
}

static gU32 randomInt(gU32 max) {
  return rand() % max;
}

static void createShape(void) {
  int i;
  memcpy(tetrisNextShape, tetrisShapes[tetrisNextShapeNum], sizeof(tetrisNextShape)); // assign from tetrisShapes arr;
  memcpy(tetrisCurrentShape, tetrisNextShape, sizeof(tetrisCurrentShape));            // tetrisCurrentShape = tetrisNextShape;
  memcpy(tetrisOldShape, tetrisNextShape, sizeof(tetrisOldShape));                    // tetrisOldShape = tetrisNextShape;
  for (i = 0; i <= 3; i++) {
    tetrisCurrentShape[i][0] += 7;
    tetrisCurrentShape[i][1] -= 4;
  }
  drawShape(0);
  tetrisOldShapeNum = tetrisNextShapeNum;
  tetrisNextShapeNum = randomInt(TETRIS_SHAPE_COUNT);
  memcpy(tetrisNextShape, tetrisShapes[tetrisNextShapeNum], sizeof(tetrisNextShape)); // assign from tetrisShapes arr;
  memcpy(tetrisCurrentShape, tetrisNextShape, sizeof(tetrisCurrentShape)); // tetrisCurrentShape = tetrisNextShape;
  for (i = 0; i <= 3; i++) {
    tetrisCurrentShape[i][0] += 7;
    tetrisCurrentShape[i][1] -= 4;
  }
  drawShape(tetrisNextShapeNum+1);
  memcpy(tetrisCurrentShape, tetrisOldShape, sizeof(tetrisCurrentShape)); // tetrisCurrentShape = tetrisOldShape;
}

static void tellScore(gU8 color) {
  char pps_str[12];
  gU8 i;
  uitoa(tetrisLines, pps_str, sizeof(pps_str));
  gdispFillArea((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+5, gdispGetHeight()-50, gdispGetStringWidth(pps_str, font16)+4,  gdispGetCharWidth('A', font16)+2, tetrisShapeColors[0]);
  gdispDrawString((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+5, gdispGetHeight()-50, pps_str, font16, tetrisShapeColors[color]);
  uitoa(tetrisScore, pps_str, sizeof(pps_str));
  gdispFillArea(0, 0, gdispGetWidth(),  gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-6, tetrisShapeColors[0]);
  int pps_str_len = strlen(pps_str);
  for (i = 0; i < pps_str_len; i++) {
    int x_pos = TETRIS_SEVEN_SEG_SCORE_X(i, pps_str_len);
    gU8 digit = pps_str[i] - '0';
    if (digit <= 9) {
        sevenSegDraw(x_pos, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-SEVEN_SEG_CHAR_HEIGHT-7, sevenSegNumbers[digit], GFX_LIME);
    }
  }
}

static void initField(void) {
  int i,j;
  tellScore(0); // clear score
  for (i = 0; i < TETRIS_FIELD_HEIGHT; i++) {
    for (j = 0; j < TETRIS_FIELD_WIDTH; j++) {
      tetrisField[i][j] = 0;
    }
  }
  createShape();
  drawShape(tetrisOldShapeNum+1);
  tetrisLines = 0;
  tetrisScore = 0;
  tellScore(8);
}

static void drawCell(int x, int y, gU8 color) {
  gdispFillArea((x*TETRIS_CELL_WIDTH)+2, gdispGetHeight()-TETRIS_CELL_HEIGHT-(y*TETRIS_CELL_HEIGHT)-3, TETRIS_CELL_WIDTH-2, TETRIS_CELL_HEIGHT-2, tetrisShapeColors[color]);
  if (color != 0) {
    gdispDrawBox((x*TETRIS_CELL_WIDTH)+2, gdispGetHeight()-TETRIS_CELL_HEIGHT-(y*TETRIS_CELL_HEIGHT)-3, TETRIS_CELL_WIDTH-1, TETRIS_CELL_HEIGHT-1, tetrisShapeColors[8]);
  } else {
    gdispDrawBox((x*TETRIS_CELL_WIDTH)+2, gdispGetHeight()-TETRIS_CELL_HEIGHT-(y*TETRIS_CELL_HEIGHT)-3, TETRIS_CELL_WIDTH-1, TETRIS_CELL_HEIGHT-1, tetrisShapeColors[0]);
  }
}

static void printText(gU8 color) {
  gdispDrawString((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+TETRIS_CELL_WIDTH, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT), "Next", font16, tetrisShapeColors[color]);
  gdispDrawString((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+5,   gdispGetHeight()-70, "Lines", font16, tetrisShapeColors[color]);
}

static void printPaused(void) {
  if (tetrisPaused) gdispDrawString((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+TETRIS_CELL_WIDTH, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)/2, "Paused!", font12, tetrisShapeColors[2]);
  else gdispFillArea((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+TETRIS_CELL_WIDTH, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)/2, gdispGetStringWidth("Paused!", font12)+4,  gdispGetCharWidth('A', font12)+2, tetrisShapeColors[0]);
}

static void printGameOver(void) {
  if (tetrisGameOver) gdispDrawString(((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)/2)-(gdispGetStringWidth("Game Over!", font12)/2), gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)/2, "Game Over!", font12, tetrisShapeColors[2]);
  else gdispFillArea(((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)/2)-(gdispGetStringWidth("Game Over!", font12)/2), gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)/2, gdispGetStringWidth("Game Over!", font12)+4,  gdispGetCharWidth('A', font12)+2, tetrisShapeColors[0]);
}

// Funzione non più necessaria in un ambiente non-touch
// static void printTouchAreas(void) {
//   gdispDrawStringBox(0, 0, gdispGetWidth(), gdispGetFontMetric(font16, gFontHeight), "Touch Area's", font16, GFX_WHITE, gJustifyCenter);
//   gdispDrawStringBox(0, 0, gdispGetWidth(), gdispGetHeight()/4, "Pause", font16, GFX_GRAY, gJustifyCenter);
//   gdispDrawStringBox(0, gdispGetHeight()/4, gdispGetWidth(), gdispGetHeight()/2, "Rotate", font16, GFX_GRAY, gJustifyCenter);
//   gdispDrawStringBox(0, gdispGetHeight()-(gdispGetHeight()/4), gdispGetWidth()/4, gdispGetHeight()/4, "Left", font16, GFX_GRAY, gJustifyCenter);
//   gdispDrawStringBox(gdispGetWidth()/4, gdispGetHeight()-(gdispGetHeight()/4), gdispGetWidth()/2, gdispGetHeight()/4, "Down", font16, GFX_GRAY, gJustifyCenter);
//   gdispDrawStringBox(gdispGetWidth()-(gdispGetWidth()/4), gdispGetHeight()-(gdispGetHeight()/4), gdispGetWidth()/4, gdispGetHeight()/4, "Right", font16, GFX_GRAY, gJustifyCenter);
//   gdispDrawLine(0, gdispGetHeight()/4, gdispGetWidth()-1, gdispGetHeight()/4, GFX_GRAY);
//   gdispDrawLine(0, gdispGetHeight()-gdispGetHeight()/4, gdispGetWidth()-1, gdispGetHeight()-gdispGetHeight()/4, GFX_GRAY);
//   gdispDrawLine(gdispGetWidth()/4, gdispGetHeight()-gdispGetHeight()/4, gdispGetWidth()/4, gdispGetHeight()-1, GFX_GRAY);
//   gdispDrawLine(gdispGetWidth()-(gdispGetWidth()/4), gdispGetHeight()-gdispGetHeight()/4, gdispGetWidth()-(gdispGetWidth()/4), gdispGetHeight()-1, GFX_GRAY);
// }

static gBool stay(gBool down) {
  int sk, k;
  if (down) sk = 1; else sk = 0;
  for (k = 0; k <= 3; k++) {
    if (tetrisCurrentShape[k][1] == 0) {
      return gTrue;
    }
  }
  for (k = 0; k <= 3; k++) {
    if ((tetrisCurrentShape[k][0] < 0) || (tetrisCurrentShape[k][0] > 9)) return gTrue;
    if (tetrisCurrentShape[k][1] <= 16)
      if (tetrisField[tetrisCurrentShape[k][1]-sk][tetrisCurrentShape[k][0]] != 0) return gTrue;
  }
  return gFalse; // Corretto, non serve la variabile stay
}

static void clearCompleteLines(void) {
  gBool t;
  gU8 reiz = 0;
  int l,k,j;
  l = 0;
  while (l <= 16) {
    t = gTrue;
    for (j = 0; j <= 9; j++)
      if (tetrisField[l][j] == 0) t = gFalse;
    if (t) {
      for (j = 0; j <= 4; j++) { // cheap & dirty line removal animation :D
        drawCell(j,l, 0);
        drawCell(9-j,l, 0);
        gfxSleepMilliseconds(40);
      }
      reiz++;
      for (k = 0; k <= 9; k++) {
        for (j = l; j < 16; j++) {
          tetrisField[j][k] = tetrisField[j+1][k];
          drawCell(k,j, tetrisField[j][k]);
        }
      }
      for (j = 0; j <= 9; j++) {
        tetrisField[16][j] = 0;
        drawCell(j,16,0);
      }
    } else {
      l++;
    }
  }
  if (reiz > 0) {
    tetrisLines += reiz;
    tetrisScore += (reiz*10)*reiz;
    tellScore(8);
  }
}

static void goDown(void) {
  int i;
  if (!stay(gTrue)) {
    drawShape(0);
    for (i = 0; i <= 3; i++) {
      tetrisCurrentShape[i][1]--;
    }
    drawShape(tetrisOldShapeNum+1);
  } else {
    for (i = 0; i <= 3; i++) {
      if (tetrisCurrentShape[i][1] >=17) {
        tetrisGameOver = gTrue;
        return;
      } else {
       tetrisField[tetrisCurrentShape[i][1]][tetrisCurrentShape[i][0]] = tetrisOldShapeNum+1;
      }
    }
    clearCompleteLines();
    createShape();
    if (stay(gFalse)) {
      tetrisGameOver = gTrue;
      return;
    }
    drawShape(tetrisOldShapeNum+1);
  }
}

static void clearField(void) {
  int j, k;
  for (k = 16; k >= 0; k--) {
    for (j = 0; j <= 9; j++) {
      drawCell(j,16-k, randomInt(8)+1);
      gfxSleepMilliseconds(10);
    }
  }
  for (k = 0; k <= 16; k++) {
    for (j = 0; j <= 9; j++) {
      drawCell(j,16-k, tetrisShapeColors[0]);
      gfxSleepMilliseconds(10);
    }
  }
}



static void rotateShape(void) {
  int i, ox, oy, tx, ty;
  ox = tetrisCurrentShape[1][0];
  oy = tetrisCurrentShape[1][1];
  memcpy(tetrisOldShape, tetrisCurrentShape, sizeof(tetrisOldShape)); // tetrisOldShape = tetrisCurrentShape;
  for (i = 0; i <= 3; i++) {
    tx = tetrisCurrentShape[i][0];
    ty = tetrisCurrentShape[i][1];
    tetrisCurrentShape[i][0] = ox+(round((tx-ox)*cos(90*(3.14/180))-(ty-oy)*sin(90*(3.14/180))));
    tetrisCurrentShape[i][1] = oy+(round((tx-ox)*sin(90*(3.14/180))+(ty-oy)*cos(90*(3.14/180))));
  }
  if (!stay(gFalse)) {
    memcpy(tetrisNextShape, tetrisCurrentShape, sizeof(tetrisNextShape)); // tetrisNextShape = tetrisCurrentShape;
    memcpy(tetrisCurrentShape, tetrisOldShape, sizeof(tetrisCurrentShape)); // tetrisCurrentShape = tetrisOldShape;
    drawShape(0);
    memcpy(tetrisCurrentShape, tetrisNextShape, sizeof(tetrisCurrentShape)); // tetrisCurrentShape = tetrisNextShape;
    drawShape(tetrisOldShapeNum+1);
  } else {
    memcpy(tetrisCurrentShape, tetrisOldShape, sizeof(tetrisCurrentShape)); // tetrisCurrentShape = tetrisOldShape;
  }
}




static gBool checkSides(gBool left) {
  int sk,k;
  if (left) sk = 1; else sk = -1;
  for (k = 0; k <= 3; k++) {
    if ((tetrisCurrentShape[k][0]+sk < 0) || (tetrisCurrentShape[k][0]+sk > 9)) return gTrue;
    if (tetrisCurrentShape[k][1] <= 16)
      if (tetrisField[tetrisCurrentShape[k][1]][tetrisCurrentShape[k][0]+sk] != 0) return gTrue;
  }
  return gFalse;
}

static void goRight(void) {
  int i;
  if (!checkSides(gTrue)) {
    drawShape(0);
    for (i = 0; i <= 3; i++) {
      tetrisCurrentShape[i][0]++;
    }
    drawShape(tetrisOldShapeNum+1);
  }
}

static void goLeft(void) {
  int i;
  if (!checkSides(gFalse)) {
    drawShape(0);
    for (i = 0; i <= 3; i++) {
      tetrisCurrentShape[i][0]--;
    }
    drawShape(tetrisOldShapeNum+1);
  }
}


static void resetGameFlags(void) {
    tetrisGameOver = gFalse;
    tetrisPaused = gFalse;
    for (int i = 0; i < 5; i++) {
        tetrisKeysPressed[i] = gFalse;
    }
}

static GFX_THREAD_FUNCTION(thdTetris, arg) {
    (void)arg;

    joystickInit();

    gBool lastKeyState[5] = {gTrue, gTrue, gTrue, gTrue, gTrue};
    gTicks lastKeyPressTime[5] = {0, 0, 0, 0, 0};
    const gTicks debounceTime = gfxMillisecondsToTicks(140);

    while (!tetrisGameOver) {
        gTicks now = gfxSystemTicks();

        // LEFT
        if (!palReadPad(JOY_PORT_1, JOY_LEFT_PIN)) {
            if (lastKeyState[0] == gTrue || (now - lastKeyPressTime[0] > debounceTime)) {
                if (!tetrisPaused) goLeft();
                lastKeyPressTime[0] = now;
                lastKeyState[0] = gFalse;
            }
        } else {
            lastKeyState[0] = gTrue;
        }

        // DOWN
        if (!palReadPad(JOY_PORT_1, JOY_DOWN_PIN)) {
            if (lastKeyState[1] == gTrue || (now - lastKeyPressTime[1] > debounceTime)) {
                if (!tetrisPaused) goDown();
                lastKeyPressTime[1] = now;
                lastKeyState[1] = gFalse;
            }
        } else {
            lastKeyState[1] = gTrue;
        }

        // RIGHT
        if (!palReadPad(JOY_PORT_1, JOY_RIGHT_PIN)) {
            if (lastKeyState[2] == gTrue || (now - lastKeyPressTime[2] > debounceTime)) {
                if (!tetrisPaused) goRight();
                lastKeyPressTime[2] = now;
                lastKeyState[2] = gFalse;
            }
        } else {
            lastKeyState[2] = gTrue;
        }

        // UP (typically rotate)
        if (!palReadPad(JOY_PORT_2, JOY_UP_PIN)) {
          gdispDrawString((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+TETRIS_CELL_WIDTH, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)/2, "Up!", font12, tetrisShapeColors[2]);
            if (lastKeyState[3] == gTrue || (now - lastKeyPressTime[3] > debounceTime)) {
                if (!tetrisPaused) {
                    // Uncomment and implement rotateShape() if needed
                    rotateShape();
                }
                lastKeyPressTime[3] = now;
                lastKeyState[3] = gFalse;
            }
        } else {
            lastKeyState[3] = gTrue;
        }

        // CENTER (Pause)
        if (!palReadPad(JOY_PORT_2, JOY_CENTRE_PIN)) {
           gdispDrawString((TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+TETRIS_CELL_WIDTH, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)/2, "Centro!!", font12, tetrisShapeColors[2]);
            if (lastKeyState[4] == gTrue || (now - lastKeyPressTime[4] > debounceTime)) {
                tetrisPaused = !tetrisPaused;
                lastKeyPressTime[4] = now;
                lastKeyState[4] = gFalse;
                printPaused();
            }
        } else {
            lastKeyState[4] = gTrue;
        }

        // Auto move shape down periodically
        if (!tetrisPaused && (now - tetrisPreviousGameTime) > gfxMillisecondsToTicks(tetrisGameSpeed)) {
            tetrisPreviousGameTime = now;
            goDown();
        }

        gfxSleepMilliseconds(10);
    }

    printGameOver();
    return 0;
}

static void tetrisDeinit(void) {
  gdispCloseFont(font16);
  gdispCloseFont(font12);
}

void tetrisStart(void) {
  // Show the help first
  gdispClear(GFX_BLACK);
  gfxSleepMilliseconds(3000);

  // Draw the board
  gdispClear(GFX_BLACK);
  gdispDrawBox(0, gdispGetHeight()-(TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)-5, (TETRIS_FIELD_WIDTH*TETRIS_CELL_WIDTH)+3, (TETRIS_FIELD_HEIGHT*TETRIS_CELL_HEIGHT)+3, GFX_WHITE);
  printText(8);

  // Away we go
  initField();
  tetrisGameOver = gFalse;
  printGameOver(); // removes "Game Over!" if tetrisGameOver == gFalse
  tetrisPreviousGameTime = gfxSystemTicks();
  gfxThreadCreate(0, 1024, gThreadpriorityNormal, thdTetris, 0);
  while (!tetrisGameOver) {
    gfxSleepMilliseconds(1000);
  }
  clearField();
  printGameOver();
  tetrisDeinit();
}

void tetrisInit(void) {
  initRng();
  tetrisNextShapeNum = randomInt(TETRIS_SHAPE_COUNT);
  font16 = gdispOpenFont("DejaVuSans16");
  font12 = gdispOpenFont("DejaVuSans12");
}



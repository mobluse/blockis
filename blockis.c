/////////////////////////////////////////////////////////////////////
//
// Blockis by M.O.B. as nCurses-program in C for Linux.
// Copyright (C) 2007, 2016 by Mikael O. Bonnier, Lund, Sweden.
// License: GNU GPL v3 or later, http://www.gnu.org/licenses/gpl-3.0.txt
// Donations are welcome to PayPal mikael.bonnier@gmail.com.
// The source code is at <http://www.df.lth.se.orbin.se/~mikaelb/wap/>.
//
// Blockis is a game similar to Tetris(R). I didn't read any other tetris code before
// writing this. I only played and studied the behaviour of a hand held tetris-type game I
// bought in 1999 (238A, E-8238, All in One, 238 in One, Super King for 238 Game, ...it
// had many names on the package) and studied the information on rules in Swedish
// Wikipedia. In 238A Tetris is called Game I, Original Brick Game. My implementation
// doesn't behave exactly like the one in 238A. E.g. you cannot change speed, level or
// number of game (i.e. variations). Another difference is that rotation cannot move the
// block sideways or downwards in my current implementation. There is no sound.
//
// It was originally developed in J2ME (Java) using WTK-2.2 and JDK 1.5 on Windows 2000.
// This C-program was translated from the Java-app into C using nCurses on a Raspberry Pi 2
// running Raspbian Jessie (Debian) Linux.
//
// Revision history:
// 2007-Jan-15:     v.0.0.2 Java
// 2007-Jan-17:     v.0.0.3 Java
// 2007-Jan-19:     v.0.1.0 Java
// 2007-Jan-20:     v.0.1.1 Java
// 2007-Feb-21:     v.0.1.2 Java
// 2008-May-23:     v.0.1.3 Java
// 2016-Jan-10:     v.0.1.4 C (Work in progress)
//
// Suggestions, improvements, and bug-reports
// are always welcome to:
//                  Mikael Bonnier
//                  Osten Undens gata 88
//                  SE-227 62  LUND
//                  SWEDEN
//
// Or use my electronic addresses:
//     Web: http://www.df.lth.se.orbin.se/~mikaelb/
//     E-mail/MSN: mikael.bonnier@gmail.com
//     ICQ # 114635318
//     Skype: mikael4u
//              _____
//             /   / \
// ***********/   /   \***********
//           /   /     \
// *********/   /       \*********
//         /   /   / \   \
// *******/   /   /   \   \*******
//       /   /   / \   \   \
// *****/   /   /***\   \   \*****
//     /   /__ /_____\   \   \
// ***/               \   \   \***
//   /_________________\   \   \
// **\                      \  /**
//    \______________________\/
//
// Mikael O. Bonnier
/////////////////////////////////////////////////////////////////////
/**/
// It is compiled on my system by:
// gcc blockis.c -o blockis -lncurses -std=c99

#include <ncurses.h>

void drawBlock();
void moveOn();
void hitGround();
bool blockCollides();
void resetBlock();
void setBlock();
void render();
void blit();

#define MATRIX_ROWS (20)
#define MATRIX_COLS (10)
const int _nDelay = 100; // ms
const int INTERLEAVE = 8;
char _nMatrix[MATRIX_ROWS][MATRIX_COLS];
char _nMatrixPrev[MATRIX_ROWS][MATRIX_COLS];
const bool _nBI[2][4] = {{0,0,0,0},
                         {1,1,1,1}};
const bool _nBT[2][3] = {{1,1,1},
                         {0,1,0}};
const bool _nBO[2][2] = {{1,1},
                         {1,1}};
const bool _nBL[2][3] = {{1,1,1},
                         {1,0,0}};
const bool _nBJ[2][3] = {{1,1,1},
                         {0,0,1}};
const bool _nBS[2][3] = {{0,1,1},
                         {1,1,0}};
const bool _nBZ[2][3] = {{1,1,0},
                         {0,1,1}};
const bool *_blocks[7] = {(bool *)_nBI, (bool *)_nBT, (bool *)_nBO,
                          (bool *)_nBL, (bool *)_nBJ, (bool *)_nBS, (bool *)_nBZ};
bool _bRunning = TRUE;
int _iBlockNext, _iBlock, _iBlockPrev;
int _nRot, _nRotPrev;
int _nMRow, _nMCol, _nMRowPrev, _nMColPrev;
char _iColorNext;
char _iColor;
bool _bNewBlock;
static int _nScaleX, _nScaleY;
static int _nLevel = 0;
static int _nScore, _nHiScore = 0;

int main()
{
    int loopCount = 1;

    initscr();
    cbreak();
    noecho();
    // scrollok(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
    getyx(stdscr, _nMRow, _nMCol);
    render();
    chtype ks;
    while(_bRunning) {
        switch(ks = getch()) {
        case KEY_LEFT:
        case 'a':
            --_nMCol;
            break;
        case KEY_RIGHT:
        case 'd':
            ++_nMCol;
            break;
        case KEY_UP:
        case 'w':
            _nRot = _nRot >= 3 ? 0 : _nRot + 1;
            break;
        case 'Q':
            endwin();
            return 0;
            break;
        }
        flushinp();
        drawBlock();

        render();
        refresh();
        napms(_nDelay);
        if(ks == KEY_DOWN || ks == 's' || loopCount % INTERLEAVE == 0) {
            ++_nMRow;
        }
        ++loopCount; // loopCount will wrap around at Integer.MAX_VALUE.
    }
}

/**
 * This method calls other methods which draws the falling block on the matrix. If there
 * would be a collision with walls or the ground it tries to prevent it by prohibiting
 * rotation and motion or restricting motion. This method also detects Game Over.
 */
void drawBlock()
{
    if(!_bNewBlock)
        resetBlock();
    if(blockCollides()) {
        if(!_bNewBlock) {
            if(_nRot != _nRotPrev) {
                _nRot = _nRotPrev;
                if(!blockCollides()) {
                    moveOn();
                    return;
                }
            }
            if(_nMRow != _nMRowPrev && _nMCol == _nMColPrev) {
                hitGround();
            }
            else if(_nMCol != _nMColPrev && _nMRow != _nMRowPrev) {
                _nMCol = _nMColPrev;
                if(blockCollides()) {
                    hitGround();
                }
                else {
                    moveOn();
                }
            }
            else {
                _nMCol = _nMColPrev;
                moveOn();
            }
        }
        else {
            //Graphics g = getGraphics();
            //g.setGrayScale(0x00);
            //g.drawString(String.valueOf(_nHiScore), getWidth(), (MATRIX_ROWS-8)*_nScaleY,
            //   Graphics.BASELINE | Graphics.RIGHT);
            //_nHiScore = Math.max(_nHiScore, _nScore);
            //g.setGrayScale(0xFF);
            //g.drawString(String.valueOf(_nHiScore), getWidth(), (MATRIX_ROWS-8)*_nScaleY,
            //   Graphics.BASELINE | Graphics.RIGHT);
            //g.drawString("GAME OVER", (MATRIX_COLS+1)*_nScaleX, 0, Graphics.TOP | Graphics.LEFT);
            //stop();
            //_mobt.gameOver();
        }
    }
    else {
        moveOn();
    }
}

/**
 * This method is called when there is no collision or a collision that has been avoided.
 * It allows the block to continue its fall.
 */
void moveOn()
{
    _bNewBlock = false;
    setBlock();
    _iBlockPrev = _iBlock;
    _nRotPrev = _nRot;
    _nMRowPrev = _nMRow; _nMColPrev = _nMCol;
}

/**
 * This method is called when a block hits the ground and it checks for full rows
 * and compacts according to the rules of standard tetris. Points calculation
 * is done here.
 */
void hitGround()
{
    _bNewBlock = true;
    _nMRow = _nMRowPrev; _nMCol = _nMColPrev;
    setBlock();
    int r, c, rMove;

    bool fullLines[MATRIX_ROWS];
    bool existsFull, isFull;
    existsFull = false;
    int noOfLines = 0;
    for(r = MATRIX_ROWS - 1; r >= 0; --r) {
        isFull = true;
        for(c = 0; c < MATRIX_COLS; ++c)
            if(_nMatrix[r][c] == 0) {
                isFull = false;
                break;
            }
        if(fullLines[r] = isFull) {
            existsFull = true;
            ++noOfLines;
            for(c = 0; c < MATRIX_COLS; ++c)
                _nMatrix[r][c] = 0;
        }
    }

    const int scores[] = {40, 100, 300, 1200};
    if(existsFull) {
        //Graphics g = getGraphics();
        //g.setGrayScale(0x00);
        //g.drawString(String.valueOf(_nScore), getWidth(), MATRIX_ROWS*_nScaleY,
        //   Graphics.BASELINE | Graphics.RIGHT);
        _nScore += (_nLevel+1)*scores[noOfLines-1];
        //g.setGrayScale(0xFF);
        //g.drawString(String.valueOf(_nScore), getWidth(), MATRIX_ROWS*_nScaleY,
        //   Graphics.BASELINE | Graphics.RIGHT);

        render();
        refresh();
        napms(_nDelay);
        for(r = MATRIX_ROWS - 1; r >= 0; --r)
            while(fullLines[r]) {
                for(rMove = r; rMove >= 1; --rMove) {
                    fullLines[rMove] = fullLines[rMove-1];
                    for(c = 0; c < MATRIX_COLS; ++c)
                        _nMatrix[rMove][c] = _nMatrix[rMove-1][c];
                }
                fullLines[0] = false;
                for(c = 0; c < MATRIX_COLS; ++c)
                    _nMatrix[0][c] = 0;
                render();
                refresh();
                napms(INTERLEAVE*_nDelay);
            }
    }
}

/**
 * This method checks if a block will collide with the walls or the ground.
 * Notice that exception is used to detect collisions with the walls of the container.
 * Motivation of design: It's faster to use the built in array index check than writing your own redu$
 * @return It returns true if collision, false otherwise.
 */
bool blockCollides()
{
    //try {
    //   _bcb.blit(_iBlock, _nRot, _nMRow, _nMCol);
    //} catch(ArrayIndexOutOfBoundsException ae) {
    //   return true;
    //}
    return false;
}

/**
 * This method wipes out the previously drawn block from the matrix.
 */
void resetBlock()
{
    //_rbb.blit(_iBlockPrev, _nRotPrev, _nMRowPrev, _nMColPrev);
}

/**
 * This method draws the current block on the matrix.
 */
void setBlock()
{
    //_sbb.blit(_iBlock, _nRot, _nMRow, _nMCol);
}

/**
 * This method renders the tetris matrix on the Graphics with the current block and
 * the next block.
 * Motivation of design: Graphics.fillRect is costly and should be avoided as much as possible.
 * Therefore the previous matrix is stored and only the changes are plotted.
 */
void render()
{
    blit();
    // Render the matrix.
    //Graphics g = getGraphics();
    //g.translate(2, 2);
    // g.setColor(_nMColors[0]);
    // g.fillRect(0, 0, MATRIX_COLS*_nScaleX - 1, MATRIX_ROWS*_nScaleY - 1);
    for(int r = 0; r < MATRIX_ROWS; ++r)
        for(int c = 0; c < MATRIX_COLS; ++c)
            if(_nMatrix[r][c] != _nMatrixPrev[r][c]) {
                //g.setColor(_nMColors[_nMatrix[r][c]]);
                //g.fillRect(c*_nScaleX, r*_nScaleY, _nScaleX-1, _nScaleY-1);
                _nMatrixPrev[r][c] = _nMatrix[r][c];
            }
    //g.translate(-2, -2);

    // Render the next block.
    //g.translate((MATRIX_COLS+1)*_nScaleX + 2, 4*_nScaleY + 2);
    //g.setColor(_nMColors[0]);
    const int scaleNBX = (3*_nScaleX)/4, scaleNBY = (3*_nScaleY)/4;
    //g.fillRect(0, 0, 4*scaleNBX, 2*scaleNBY);
    //g.setColor(_nMColors[_iColorNext]);
    // TODO: This might use Blitter.
    const int BLOCK_ROWS = 2; //_blocks[_iBlockNext].length;
    const int BLOCK_COLS = 3; // _blocks[_iBlockNext][0].length;
    for(int r = 0; r < BLOCK_ROWS; ++r)
        for(int c = 0; c < BLOCK_COLS; ++c)
            if(_blocks[_iBlockNext][r*BLOCK_COLS+c] != 0)
                //g.fillRect(c*scaleNBX, r*scaleNBY, scaleNBX-1, scaleNBY-1)
                ;
    //g.translate(-((MATRIX_COLS+1)*_nScaleX + 2), -(4*_nScaleY + 2));
}

void blit() {
    int i, j;
    clear();
    for(i = 0; i < 2; ++i)
        for(j = 0; j < 3; ++j) {
            chtype ch = _nBT[i][j] == 1 ? ACS_CKBOARD : ' ';
            mvaddch(_nMRow+i, _nMCol+j, ch);
        }
}

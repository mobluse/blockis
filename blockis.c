/////////////////////////////////////////////////////////////////////
//
// Blockis by M.O.B. as nCurses-program in C for Linux.
// Copyright (C) 2007, 2016 by Mikael O. Bonnier, Lund, Sweden.
// License: GNU GPL v3 or later, http://www.gnu.org/licenses/gpl-3.0.txt
// Donations are welcome to PayPal mikael.bonnier@gmail.com.
// The source code is at <http://www.df.lth.se.orbin.se/~mikaelb/wap/>.
//
// Blockis is a game similar to Tetris(R). I didn't read any other tetris
// code before writing this. I only played and studied the behaviour of a
// hand held tetris-type game I bought in 1999 (238A, E-8238, All in One,
// 238 in One, Super King for 238 Game, ...it had many names on the
// package) and studied the information on rules in Swedish Wikipedia. In
// 238A Tetris is called Game I, Original Brick Game. My implementation
// doesn't behave exactly like the one in 238A. E.g. you cannot change
// speed, level or number of game (i.e. variations). Another difference is
// that rotation cannot move the block sideways or downwards in my current
// implementation. There is no sound.
//
// It was originally developed in J2ME (Java) using WTK-2.2 and JDK 1.5 on
// Windows 2000. This C-program was translated from the Java-app into C
// using nCurses on a Raspberry Pi 2 running Raspbian Jessie (Debian)
// Linux.
//
// Revision history:
// 2007-Jan-15:     v.0.0.2   Java
// 2007-Jan-17:     v.0.0.3   Java
// 2007-Jan-19:     v.0.1.0   Java
// 2007-Jan-20:     v.0.1.1   Java
// 2007-Feb-21:     v.0.1.2   Java
// 2008-May-23:     v.0.1.3   Java
// 2016-Jan-10:     v.0.1.3a  C
// 2016-Jan-11:     v.0.1.3b  C (Work in progress)
//
// Suggestions, improvements, and bug-reports
// are always welcome to:
//                  Mikael O. Bonnier
//                  Osten Undens gata 88
//                  SE-227 62  LUND
//                  SWEDEN
//
// Or use my electronic addresses:
//     Web: http://www.df.lth.se.orbin.se/~mikaelb/
//     Twitter: https://twitter.com/mobluse
//     Scratch: https://scratch.mit.edu/users/mobluse/
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

// It is compiled on my system by:
// p=blockis; gcc $p.c -o $p -lncurses -std=c99
//
/* indent -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i4 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1 --no-tabs blockis.c  */
#include <ncurses.h>
#include <sys/param.h>
#include <time.h>
#include <stdlib.h>
#include <setjmp.h>
void init();
void clearGraphics();
void start();
void resume();
void stop();
void drawBlock();
void moveOn();
void hitGround();
bool blockCollides();
void resetBlock();
void setBlock();
void render();
void blit(void (*doCell) (int, int), int block, int rot, int row, int col);
void doCellBlockCollides(int r, int c);
void doCellResetBlock(int r, int c);
void doCellSetBlock(int r, int c);

#define _MATRIX_ROWS (20)
#define _MATRIX_COLS (10)
#define _CA (2)
const int _nDelay = 100;        // ms
const int _INTERLEAVE = 8;
#define _ROT0 (0)
#define _ROT90 (1)
#define _ROT180 (2)
#define _ROT270 (3)
char _nMatrix[_MATRIX_ROWS][_MATRIX_COLS];
char _nMatrixPrev[_MATRIX_ROWS][_MATRIX_COLS];
const bool _nBI[2][4] = { {0, 0, 0, 0},
{1, 1, 1, 1}
};
const bool _nBT[2][3] = { {1, 1, 1},
{0, 1, 0}
};
const bool _nBO[2][2] = { {1, 1},
{1, 1}
};
const bool _nBL[2][3] = { {1, 1, 1},
{1, 0, 0}
};
const bool _nBJ[2][3] = { {1, 1, 1},
{0, 0, 1}
};
const bool _nBS[2][3] = { {0, 1, 1},
{1, 1, 0}
};
const bool _nBZ[2][3] = { {1, 1, 0},
{0, 1, 1}
};

#define _NOOFBLOCKS (7)
#define _NOOFCOLORS (7)

const bool *_blocks[_NOOFBLOCKS] =
    { (bool *) _nBI, (bool *) _nBT, (bool *) _nBO,
    (bool *) _nBL, (bool *) _nBJ, (bool *) _nBS, (bool *) _nBZ
};

const struct {
    int rows;
    int cols;
} _blockInfo[_NOOFBLOCKS] = {
    {
    2, 4}, {
    2, 3}, {
    2, 2}, {
    2, 3}, {
    2, 3}, {
    2, 3}, {
2, 3}};

bool _bRunning;
int _iBlockNext, _iBlock, _iBlockPrev;
int _nRot, _nRotPrev;
int _nMRow, _nMCol, _nMRowPrev, _nMColPrev;
char _iColorNext;
char _iColor;
bool _bNewBlock;
static int _nLevel = 0;
static int _nScore, _nHiScore = 0;
static jmp_buf _exceptionEnv;

int main(int argc, char* argv[])
{
    int loopCount = 1;

    init();
    start();

    for (;;) {
        while (_bRunning) {
            chtype ks = getch();
            flushinp();
            switch (ks) {
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
            case KEY_DC:
            case 'e':
                _nRot = _nRot <= 0 ? 3 : _nRot - 1;
                break;
            case KEY_END:
            case 'q':
            case 'Q':
                stop();
                break;
            }
            if (_bNewBlock) {
                _nMRow = _nMRowPrev = 0;
                _nMCol = _nMColPrev = _MATRIX_COLS/2 - 2;
                _iBlock = _iBlockNext;
                if (_iBlock == 0)
                    _nMRow = _nMRowPrev = -1;
                _iBlockNext = rand() % _NOOFBLOCKS;
                _iColor = _iColorNext;
                _iColorNext = (char)(rand() % (_NOOFCOLORS - 1) + 1);
                _nRot = 0;
            }
            drawBlock();

            render();
            refresh();
            napms(_nDelay);

            if (ks == KEY_DOWN || ks == 's' || loopCount % _INTERLEAVE == 0) {
                ++_nMRow;
            }
            ++loopCount;        // loopCount will wrap around at INT_MAX.
        }
        chtype ks = getch();
        flushinp();
        switch (ks) {
        case KEY_DOWN:
        case 's':
            start();
            break;
        case KEY_DC:
        case 'e':
            resume();
            break;
        case KEY_END:
        case 'q':
        case 'Q':
            endwin();
            return 0;
            break;
        }
        napms(_nDelay);
    }
}

void init()
{
    srand(time(NULL));
    initscr();
    cbreak();
    noecho();
    // scrollok(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(1, COLOR_RED, COLOR_RED);
    init_pair(2, COLOR_GREEN, COLOR_GREEN);
    init_pair(3, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(4, COLOR_BLUE, COLOR_BLUE);
    init_pair(5, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(6, COLOR_CYAN, COLOR_CYAN);
}

/**
 * This method clears the graphics.
 */
void clearGraphics()
{
    // Clear the Graphics.
    clear();
    for (int c = 1 + _CA; c <= _MATRIX_COLS + _CA; ++c) {
        mvaddch(0, c, ACS_HLINE);
        mvaddch(_MATRIX_ROWS + 1, c, ACS_HLINE);
    }
    for (int r = 1; r <= _MATRIX_ROWS; ++r) {
        mvaddch(r, _CA, ACS_VLINE);
        mvaddch(r, _MATRIX_COLS + 1 + _CA, ACS_VLINE);
    }
    mvaddch(0, _CA, ACS_ULCORNER);
    mvaddch(0, _MATRIX_COLS + 1 + _CA, ACS_URCORNER);
    mvaddch(_MATRIX_ROWS + 1, _CA, ACS_LLCORNER);
    mvaddch(_MATRIX_ROWS + 1, _MATRIX_COLS + 1 + _CA, ACS_LRCORNER);
    mvprintw(_MATRIX_ROWS, _MATRIX_COLS + 3 + _CA, "%d", _nScore);
    mvprintw(_MATRIX_ROWS - 6, _MATRIX_COLS + 3 + _CA, "%d", _nHiScore);
    mvaddstr(_MATRIX_ROWS + 3, 1 + _CA, "W A S D E Q");
}

/**
 * This method starts the game.
 */
void start()
{
    _nScore = 0;
    clearGraphics();
    // Reset the matrix.
    for (int r = 0; r < _MATRIX_ROWS; ++r)
        for (int c = 0; c < _MATRIX_COLS; ++c)
            _nMatrix[r][c] = _nMatrixPrev[r][c] = 0;
    _bNewBlock = true;
    _iBlockNext = rand() % _NOOFBLOCKS;
    _iColorNext = (char)(rand() % (_NOOFCOLORS - 1) + 1);
    _bRunning = true;
}

/**
 * This resumes animation after a pause.
 */
void resume()
{
    _bRunning = true;
}

/**
 * This is called to stop the animation when pausing, destroying or game over.
 */
void stop()
{
    _bRunning = false;
}

/**
 * This method calls other methods which draws the falling block on the
 * matrix. If there would be a collision with walls or the ground it tries
 * to prevent it by prohibiting rotation and motion or restricting motion.
 * This method also detects Game Over.
 */
void drawBlock()
{
    if (!_bNewBlock)
        resetBlock();
    if (blockCollides()) {
        if (!_bNewBlock) {
            if (_nRot != _nRotPrev) {
                _nRot = _nRotPrev;
                if (!blockCollides()) {
                    moveOn();
                    return;
                }
            }
            if (_nMRow != _nMRowPrev && _nMCol == _nMColPrev) {
                hitGround();
            } else if (_nMCol != _nMColPrev && _nMRow != _nMRowPrev) {
                _nMCol = _nMColPrev;
                if (blockCollides()) {
                    hitGround();
                } else {
                    moveOn();
                }
            } else {
                _nMCol = _nMColPrev;
                moveOn();
            }
        } else {
            _nHiScore = MAX(_nHiScore, _nScore);
            mvprintw(_MATRIX_ROWS - 6, _MATRIX_COLS + 3 + _CA, "%d", _nHiScore);
            mvaddstr(1, _MATRIX_COLS + 3 + _CA, "GAME OVER");
            stop();
        }
    } else {
        moveOn();
    }
}

/**
 * This method is called when there is no collision or a collision that has
 * been avoided. It allows the block to continue its fall.
 */
void moveOn()
{
    _bNewBlock = false;
    setBlock();
    _iBlockPrev = _iBlock;
    _nRotPrev = _nRot;
    _nMRowPrev = _nMRow;
    _nMColPrev = _nMCol;
}

/**
 * This method is called when a block hits the ground and it checks for full
 * rows and compacts according to the rules of standard tetris. Points
 * calculation is done here.
 */
void hitGround()
{
    _bNewBlock = true;
    _nMRow = _nMRowPrev;
    _nMCol = _nMColPrev;
    setBlock();
    int r, c, rMove;

    bool fullLines[_MATRIX_ROWS];
    bool existsFull, isFull;
    existsFull = false;
    int noOfLines = 0;
    for (int r = _MATRIX_ROWS - 1; r >= 0; --r) {
        isFull = true;
        for (int c = 0; c < _MATRIX_COLS; ++c)
            if (_nMatrix[r][c] == 0) {
                isFull = false;
                break;
            }
        if (fullLines[r] = isFull) {
            existsFull = true;
            ++noOfLines;
            for (c = 0; c < _MATRIX_COLS; ++c)
                _nMatrix[r][c] = 0;
        }
    }

    const int scores[] = { 40, 100, 300, 1200 };
    if (existsFull) {
        _nScore += (_nLevel + 1) * scores[noOfLines - 1];
        mvprintw(_MATRIX_ROWS, _MATRIX_COLS + 3 + _CA, "%d", _nScore);

        render();
        refresh();
        napms(_nDelay);
        for (int r = _MATRIX_ROWS - 1; r >= 0; --r)
            while (fullLines[r]) {
                for (int rMove = r; rMove >= 1; --rMove) {
                    fullLines[rMove] = fullLines[rMove - 1];
                    for (int c = 0; c < _MATRIX_COLS; ++c)
                        _nMatrix[rMove][c] = _nMatrix[rMove - 1][c];
                }
                fullLines[0] = false;
                for (c = 0; c < _MATRIX_COLS; ++c)
                    _nMatrix[0][c] = 0;
                render();
                refresh();
                napms(_INTERLEAVE * _nDelay);
            }
    }
}

/**
 * This method checks if a block will collide with the walls or the ground.
 * @return It returns true if collision, false otherwise.
 */
bool blockCollides()
{
    if (setjmp(_exceptionEnv)) {
        return true;
    }
    blit(doCellBlockCollides, _iBlock, _nRot, _nMRow, _nMCol);
    return false;
}

/**
 * This method wipes out the previously drawn block from the matrix.
 */
void resetBlock()
{
    blit(doCellResetBlock, _iBlockPrev, _nRotPrev, _nMRowPrev, _nMColPrev);
}

/**
 * This method draws the current block on the matrix.
 */
void setBlock()
{
    blit(doCellSetBlock, _iBlock, _nRot, _nMRow, _nMCol);
}

/**
 * This method renders the tetris matrix on the Graphics with the current
 * block and the next block.
 * Motivation of design: Graphics.fillRect is costly and should be avoided as
 * much as possible.
 * Therefore the previous matrix is stored and only the changes are plotted.
 */
void render()
{
    // Render the matrix.
    for (int r = 0; r < _MATRIX_ROWS; ++r)
        for (int c = 0; c < _MATRIX_COLS; ++c)
            if (_nMatrix[r][c] != _nMatrixPrev[r][c]) {
                chtype ch = _nMatrix[r][c] != 0 ? ACS_CKBOARD : ' ';
                attron(COLOR_PAIR(_nMatrix[r][c]));     // TODO: Optimize!
                mvaddch(r + 1, c + 1 + _CA, ch);
                attroff(COLOR_PAIR(_nMatrix[r][c]));
                _nMatrixPrev[r][c] = _nMatrix[r][c];
            }
    // Render the next block.
    // TODO: This might use Blitter.
    const int BLOCK_ROWS = _blockInfo[_iBlockNext].rows,
        BLOCK_COLS = _blockInfo[_iBlockNext].cols;
    mvaddstr(3, _MATRIX_COLS + 3 + _CA, "      ");
    mvaddstr(4, _MATRIX_COLS + 3 + _CA, "      ");
    attron(COLOR_PAIR(_iColorNext));
    for (int r = 0; r < BLOCK_ROWS; ++r)
        for (int c = 0; c < BLOCK_COLS; ++c)
            if (_blocks[_iBlockNext][r * BLOCK_COLS + c]) {
                mvaddch(r + 3, c + _MATRIX_COLS + 3 + _CA, ACS_CKBOARD);
            }
    attroff(COLOR_PAIR(_iColorNext));
}

/**
 * This method can blit the block on to the matrix, but exactly what it does
 * depend on the implementation of doCell() in the child class.
 * TODO: If r and c is outside of the matrix or if collision.
 */
void blit(void (*doCell) (int, int), int block, int rot, int row, int col)
{
    const int BLOCK_ROWS = _blockInfo[block].rows,
        BLOCK_COLS = _blockInfo[block].cols;

    switch (rot) {
    case _ROT0:
        for (int r = 0; r < BLOCK_ROWS; ++r)
            for (int c = 0; c < BLOCK_COLS; ++c)
                if (_blocks[block][r * BLOCK_COLS + c] != 0)
                    (*doCell) (row + r, col + c);
        break;
    case _ROT90:
        for (int r = 0; r < BLOCK_ROWS; ++r)
            for (int c = 0; c < BLOCK_COLS; ++c)
                if (_blocks[block][r * BLOCK_COLS + c] != 0)
                    (*doCell) (row + c, col + (BLOCK_COLS - 1 - r));
        break;
    case _ROT180:
        for (int r = 0; r < BLOCK_ROWS; ++r)
            for (int c = 0; c < BLOCK_COLS; ++c)
                if (_blocks[block][r * BLOCK_COLS + c] != 0)
                    (*doCell)
                        (row + (BLOCK_COLS - 1 - r),
                         col + (BLOCK_COLS - 1 - c));
        break;
    case _ROT270:
        for (int r = 0; r < BLOCK_ROWS; ++r)
            for (int c = 0; c < BLOCK_COLS; ++c)
                if (_blocks[block][r * BLOCK_COLS + c] != 0)
                    (*doCell) (row + (BLOCK_COLS - 1 - c), col + r);
        break;
    }
}

/**
 * This doCell has block collision detection.
 */
void doCellBlockCollides(int r, int c)
{
    if (_nMatrix[r][c] != 0 ||
        !(0 <= r && r < _MATRIX_ROWS && 0 <= c && c < _MATRIX_COLS))
        longjmp(_exceptionEnv, TRUE);
}

/**
 * This doCell has block reset code.
 */
void doCellResetBlock(int r, int c)
{
    _nMatrix[r][c] = 0;
}

/**
 * This doCell has block set code.
 */
void doCellSetBlock(int r, int c)
{
    _nMatrix[r][c] = _iColor;
}

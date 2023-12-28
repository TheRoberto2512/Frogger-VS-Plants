#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <locale.h>
#include <stddef.h>
#include <time.h>

#include "colors.h"
#include "sprite.h"

int main()
{
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(0);

    start_color();
    istanziaColori();

    bool tane[5] = {true, true, false, true, true};

    WINDOW *scoreBoard = newwin(3, (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+2, 0, 1);

    WINDOW *gameArea = newwin( ROWS_PER_MAP +2, (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+2, 2, 1);

    printScoreBoard(scoreBoard, 2, 2512, 30, 30); 

    printMap(gameArea, true, true, tane, true);

    wrefresh(gameArea); wrefresh(scoreBoard);

    endwin();
    return 0;
}
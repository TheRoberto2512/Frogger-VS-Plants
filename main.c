#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <locale.h>
#include <stddef.h>
#include <time.h>
#include <fcntl.h>

#include "colors.h"
#include "sprite.h"
#include "structs.h"
#include "functionsP.h"

int main()
{
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(0);
    cbreak(); //input della tastiera istantaneo

    start_color();
    istanziaColori();

    // pipes
    int mainToFrog[2]; pipe(mainToFrog); fcntl(mainToFrog[READ], F_SETFL, O_NONBLOCK); fcntl(mainToFrog[WRITE], F_SETFL, O_NONBLOCK);
    int frogToMain[2]; pipe(frogToMain); fcntl(frogToMain[READ], F_SETFL, O_NONBLOCK); fcntl(frogToMain[WRITE], F_SETFL, O_NONBLOCK);
    int FPHToMain[2]; pipe(FPHToMain); fcntl(FPHToMain[READ], F_SETFL, O_NONBLOCK); fcntl(FPHToMain[WRITE], F_SETFL, O_NONBLOCK);

    __pid_t frog = fork();

    if(frog == 0)
    {
        frogHandler(frogToMain, mainToFrog, FPHToMain);
    }
    else if(frog > 0)
    {
        WINDOW *scoreBoard = newwin(3, (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+2, 0, 1);
        WINDOW *gameArea = newwin( ROWS_PER_MAP + 2, (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+2, 2, 1);

        keypad(gameArea, TRUE); //attiva i tasti speciali (le frecce)
        nodelay(gameArea, TRUE); // nessun delay per la getch()
        printScoreBoard(scoreBoard, 3, 0, 30, 30);
        bool a[5] = {true, true, true, true, true};

        mainManager(gameArea, scoreBoard, frogToMain, mainToFrog);
    }

    endwin();
    return 0;
}
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
    start_color(); istanziaColori();
    cbreak();
    nodelay(stdscr, TRUE); // nessun delay per la getch()
    keypad(stdscr, TRUE); //attiva i tasti speciali (le frecce)
    
    // pipes
    int mainToFrog[2]; pipe(mainToFrog);
    fcntl(mainToFrog[READ], F_SETFL, O_NONBLOCK);
    //fcntl(mainToFrog[WRITE], F_SETFL, O_NONBLOCK);
    int frogToMain[2]; pipe(frogToMain);
    fcntl(frogToMain[READ], F_SETFL, O_NONBLOCK);
    //fcntl(frogToMain[WRITE], F_SETFL, O_NONBLOCK);
    int FPHToMain[2]; pipe(FPHToMain);
    fcntl(FPHToMain[READ], F_SETFL, O_NONBLOCK); //fcntl(FPHToMain[WRITE], F_SETFL, O_NONBLOCK);

    __pid_t frog = fork();

    if(frog == 0)
    {
        close(frogToMain[READ]); // pipe dove scrive le coordinate
        close(mainToFrog[WRITE]); // pipe dove legge le coordinate aggiornate
        close(FPHToMain[READ]);  // pipe dove comunica la creazione di un proiettile rana
        frogHandler(frogToMain, mainToFrog, FPHToMain);
    }
    else if(frog > 0)
    {
        close(frogToMain[WRITE]);
        close(mainToFrog[READ]);
        mainManager(frogToMain, mainToFrog);
    }

    endwin();
    return 0;
}
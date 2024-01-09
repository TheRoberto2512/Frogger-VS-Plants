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
#include "settings.h"

void game(GameRules rules);

int main()
{
    setlocale(LC_ALL, "");
    initscr(); noecho(); curs_set(0);
    start_color(); istanziaColori(); cbreak();
    nodelay(stdscr, TRUE); // nessun delay per la getch()
    keypad(stdscr, TRUE); // attiva i tasti speciali (le frecce)
    mousemask(ALL_MOUSE_EVENTS, NULL); // attiva gli eventi del mouse
    
    // ... 

    GameRules regole = getRules(EASY);

    game(regole);

    endwin();
    return 0;
}

void game(GameRules rules)
{
    // pipes
    int mainToFrog[2]; pipe(mainToFrog); fcntl(mainToFrog[READ], F_SETFL, O_NONBLOCK); // main comunica alla rana
    int frogToMain[2]; pipe(frogToMain); fcntl(frogToMain[READ], F_SETFL, O_NONBLOCK); // rana comunica al main
    int FPHToMain[2];  pipe(FPHToMain);  fcntl(FPHToMain[READ], F_SETFL, O_NONBLOCK);  // gestore proiettili rana comunica al main
    int mainToFPH[2];  pipe(mainToFPH);  fcntl(mainToFPH[READ], F_SETFL, O_NONBLOCK);  // main comunica al gestore proiettili rana 

    int crocToMain[2]; pipe(crocToMain); fcntl(crocToMain[READ], F_SETFL, O_NONBLOCK); // coccodrillo comunica al main
    int mainToRivH[2]; pipe(mainToRivH); fcntl(mainToRivH[READ], F_SETFL, O_NONBLOCK); // main comunica al riverHandler

    __pid_t frog = fork();

    if(frog == 0)
    {
        close(frogToMain[READ]);  // pipe dove scrive le coordinate
        close(mainToFrog[WRITE]); // pipe dove legge le coordinate aggiornate
        close(FPHToMain[READ]);   // pipe dove comunica la creazione di un proiettile rana
        frogHandler(frogToMain, mainToFrog, FPHToMain);
    }
    else if(frog > 0)
    {
        __pid_t croc = fork();

        if(croc == 0)
        {
            riverHandler(crocToMain, mainToRivH, rules);
        }
        else if(croc > 0)
        {
            close(frogToMain[WRITE]);
            close(mainToFrog[READ]);
            mainManager(rules.time, frogToMain, mainToFrog, crocToMain, mainToRivH);
        }
    }
}
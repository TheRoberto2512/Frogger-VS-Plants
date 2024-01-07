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
    keypad(stdscr, TRUE); //attiva i tasti speciali (le frecce)
    
    GameRules regole = getRules(1);
    
    game(regole);

    endwin();
    return 0;
}

void game(GameRules rules)
{
    // pipes
    int mainToFrog[2]; pipe(mainToFrog); fcntl(mainToFrog[READ], F_SETFL, O_NONBLOCK);
    int frogToMain[2]; pipe(frogToMain); fcntl(frogToMain[READ], F_SETFL, O_NONBLOCK);
    int FPHToMain[2];  pipe(FPHToMain);  fcntl(FPHToMain[READ], F_SETFL, O_NONBLOCK);

    int crocToMain[2]; pipe(crocToMain); fcntl(crocToMain[READ], F_SETFL, O_NONBLOCK); // coccodrillo comunica al main
    int mainToRivH[2]; pipe(mainToRivH); fcntl(mainToRivH[READ], F_SETFL, O_NONBLOCK); // main comunica al riverHandler
    int crocToRivH[2]; pipe(crocToRivH); fcntl(crocToRivH[READ], F_SETFL, O_NONBLOCK); // coccodrillo comunica al riverHandler

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
            Crocodile crocc;
            crocc.y = 20;
            crocc.splash = -10;
            crocc.x = 50;
            crocc.direction = 0;
            crocc.speed = 2;
            close(crocToMain[READ]);
            close(crocToRivH[READ]);
            singleCrocodileHandler(crocToMain, crocToRivH, crocc);
        }
        else if(croc > 0)
        {
            close(frogToMain[WRITE]);
            close(mainToFrog[READ]);
            mainManager(rules.time, frogToMain, mainToFrog, crocToMain, mainToRivH);
        }
    }
}
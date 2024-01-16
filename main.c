#include <unistd.h>
#include <curses.h>
#include <locale.h>
#include <stddef.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#include "colors.h"
#include "sprite.h"
#include "structs.h"
#include "functionsP.h"
#include "settings.h"

void game(GameRules *rules, GameUpdates *thisGame);

int main()
{  
    setlocale(LC_ALL, ""); // attiva i caratteri speciali dell'UNICODE

    GameRules regole = getRules(MEDIUM);
    GameUpdates thisGame; thisGame.lives = LIVES; thisGame.score = 0;

    game(&regole, &thisGame);

    return 0;
}

void game(GameRules *rules, GameUpdates *thisGame)
{
    int status; // servira' dopo per la waitpid()

    // pipes
    int mainToFrog[2]; pipe(mainToFrog); fcntl(mainToFrog[READ], F_SETFL, O_NONBLOCK); // main comunica alla rana
    int frogToMain[2]; pipe(frogToMain); fcntl(frogToMain[READ], F_SETFL, O_NONBLOCK); // rana comunica al main
    int frogToFPH[2];  pipe(frogToFPH);  fcntl(frogToFPH[READ],  F_SETFL, O_NONBLOCK);
    int PHToMain[2];   pipe(PHToMain);   fcntl(PHToMain[READ],   F_SETFL, O_NONBLOCK);  // gestore proiettili rana comunica al main
    int mainToFPH[2];  pipe(mainToFPH);  fcntl(mainToFPH[READ],  F_SETFL, O_NONBLOCK);  // main comunica al gestore proiettili rana 

    int crocToMain[2]; pipe(crocToMain); fcntl(crocToMain[READ], F_SETFL, O_NONBLOCK); // coccodrillo comunica al main
    int mainToRivH[2]; pipe(mainToRivH); fcntl(mainToRivH[READ], F_SETFL, O_NONBLOCK); // main comunica al riverHandler

    int enHToMain[2]; pipe(enHToMain); fcntl(enHToMain[READ], F_SETFL, O_NONBLOCK); // enemiesHandler comunica al main
    int mainToEnH[2]; pipe(mainToEnH); fcntl(mainToEnH[READ], F_SETFL, O_NONBLOCK); // main comunica all'enemiesHandler

    pid_t croc = fork();

    if(croc == 0)
    {
        riverHandler(crocToMain, mainToRivH, rules);
    }
    else if(croc > 0)
    {
        pid_t frogPRJH = fork();

        if(frogPRJH == 0)
        {
            frogProjectilesHandler(frogToFPH, PHToMain, mainToFPH, rules->speed);
        }
        else
        {

            pid_t enH = fork();

            if(enH == 0)
            {
                enemiesHandler(enHToMain, mainToEnH);
            }
            else
            {
                initscr(); noecho(); curs_set(0);
                start_color(); istanziaColori(); cbreak();
                nodelay(stdscr, TRUE); // nessun delay per la getch()
                keypad(stdscr, TRUE); // attiva i tasti speciali (le frecce)
                mousemask(ALL_MOUSE_EVENTS, NULL); // attiva gli eventi del mouse   

                pid_t frog = fork();

                if(frog == 0)
                {
                    
                    close(frogToMain[READ]);  // pipe dove scrive le coordinate
                    close(mainToFrog[WRITE]); // pipe dove legge le coordinate aggiornate
                    close(PHToMain[READ]);   // pipe dove comunica la creazione di un proiettile rana
                    frogHandler(frogToMain, mainToFrog, frogToFPH);
                }
                else if(frog > 0)
                {
                    close(frogToMain[WRITE]);
                    close(mainToFrog[READ]);
                    mainManager(rules, thisGame, frogToMain, mainToFrog, crocToMain, mainToRivH, PHToMain, mainToFPH, enHToMain, mainToEnH);

                    endwin();
                    kill(frog, SIGTERM);        // uccide la rana
                    waitpid(frog, &status, 0); 
                }
            }

        }
    }

    kill(croc, SIGTERM);        // uccide il riverHandler
    waitpid(croc, &status, 0); 

    usleep(FRAME_UPDATE); Crocodile crocc;

    ssize_t bytes_read = -1;
    do{
        bytes_read = read(crocToMain[READ], &croc, sizeof(croc)); // read(...) restituisce il numero di byte letti (-1 se non legge nulla) 
        if(bytes_read != -1)
        {
            kill(crocc.PID, SIGTERM);
            waitpid(crocc.PID, &status, 0); 
        }
    } while (bytes_read != -1); // controlla che non sia rimasto nessun coccodrillo in vita, in caso li uccide
}
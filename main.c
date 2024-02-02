#include <unistd.h>
#include <stdlib.h>
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

void game();

int main()
{  
    setlocale(LC_ALL, ""); // attiva i caratteri speciali dell'UNICODE

    if(GENERAL_DEBUG)
        system("echo -e \"\\e[8;53;160t\""); // imposta la dimensione del terminale qualora sia troppo piccolo
    else
        system("echo -e \"\\e[8;53;108t\"");

    GameRules regole = getRules(MEDIUM);

    game(&regole);

    // execlp("pkill", "pkill", "-f", "./game", (char *)NULL);
    // system("pkill -f ./game");
    return 0;
}

void game()
{
    // PIPES (tutte impostate in modalita' non bloccante per la lettura)
    int mainToFrog[2]; pipe(mainToFrog); fcntl(mainToFrog[READ], F_SETFL, O_NONBLOCK); // main comunica alla rana
    int frogToMain[2]; pipe(frogToMain); fcntl(frogToMain[READ], F_SETFL, O_NONBLOCK); // rana comunica al main
    int frogToFPH[2];  pipe(frogToFPH);  fcntl(frogToFPH[READ],  F_SETFL, O_NONBLOCK); // rana comunica al gestore proiettili
    int PHToMain[2];   pipe(PHToMain);   fcntl(PHToMain[READ],   F_SETFL, O_NONBLOCK); // gestore proiettili comunica al main
    int mainToFPH[2];  pipe(mainToFPH);  fcntl(mainToFPH[READ],  F_SETFL, O_NONBLOCK); // main comunica al gestore proiettili rana 

    int crocToMain[2]; pipe(crocToMain); fcntl(crocToMain[READ], F_SETFL, O_NONBLOCK); // coccodrillo comunica al main
    int mainToRivH[2]; pipe(mainToRivH); fcntl(mainToRivH[READ], F_SETFL, O_NONBLOCK); // main comunica al riverHandler

    int enHToMain[2];  pipe(enHToMain);  fcntl(enHToMain[READ], F_SETFL, O_NONBLOCK);  // enemiesHandler comunica al main
    int mainToEnH[2];  pipe(mainToEnH);  fcntl(mainToEnH[READ], F_SETFL, O_NONBLOCK);  // main comunica all'enemiesHandler

    pid_t croc = fork();

    if(croc == 0)
    {
        kill(getpid(), SIGSTOP); // bisogna per forza fare SIGCONT prima di chiudere il programma
        riverHandler(crocToMain, mainToRivH);
    }
    else if(croc > 0)
    {
        pid_t frogPRJH = fork();

        if(frogPRJH == 0)
        {
            kill(getpid(), SIGSTOP);
            frogProjectilesHandler(frogToFPH, PHToMain, mainToFPH);
        }
        else
        {
            pid_t enH = fork();

            if(enH == 0)
            {
                kill(getpid(), SIGSTOP); // bisogna per forza fare SIGCONT prima di chiudere il programma
                enemiesHandler(enHToMain, mainToEnH, PHToMain);
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
                    close(PHToMain[READ]);    // pipe dove comunica la creazione di un proiettile rana

                    frogHandler(frogToMain, mainToFrog, frogToFPH);
                }
                else if(frog > 0)
                {
                    /*
                    close(frogToMain[WRITE]); // pipe dove legge le coordinate dalla rana
                    close(mainToFrog[READ]);  // pipe dove scrive le coordinate per la rana
                    close(crocToMain[WRITE]); // pipe dove legge le coordinate dei coccodrilli
                    close(mainToRivH[READ]);  // pipe dove scrive al riverHandler
                    close(PHToMain[WRITE]);   // pipe dove legge le coordinate dei proiettili
                    close(mainToFPH[READ]);   // pipe dove scrive al frogProjectilesHandler
                    close(enHToMain[WRITE]);  // pipe dove legge le coordinate dei nemici
                    close(mainToEnH[READ]);   // pipe dove scrive all'enemiesHandler */

                    short difficult = EASY;

                    write(mainToEnH[WRITE], &difficult, sizeof(difficult));
                    write(mainToRivH[WRITE], &difficult, sizeof(difficult));
                    write(mainToFPH[WRITE], &difficult, sizeof(difficult));

                    kill(enH, SIGCONT); kill(croc, SIGCONT); kill(frogPRJH, SIGCONT);

                    mainManager(difficult, frogToMain, mainToFrog, crocToMain, mainToRivH, PHToMain, mainToFPH, enHToMain, mainToEnH);

                    endwin();
                    easyKill(frog); // uccide la rana
                }
                easyKill(enH);
            }
            easyKill(frogPRJH);
        }
    }
    kill(croc, SIGCONT);

    easyKill(croc);

    usleep(FRAME_UPDATE); Crocodile crocc;

    ssize_t bytes_read = -1;
    do{
        bytes_read = read(crocToMain[READ], &croc, sizeof(croc)); // read(...) restituisce il numero di byte letti (-1 se non legge nulla) 
        if(bytes_read != -1)
        {
            easyKill(crocc.PID);
        }
    } while (bytes_read != -1); // controlla che non sia rimasto nessun coccodrillo in vita, in caso li uccide
}
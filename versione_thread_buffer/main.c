#include <unistd.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>
#include <stddef.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#include "colors.h"
#include "sprite.h"
#include "structs.h"
#include "buffers.h"
#include "functionsT.h"
#include "settings.h"
#include "menu.h"

volatile short difficult;                               pthread_mutex_t semDifficult;
volatile GameUpdates currentGame;                       pthread_mutex_t semCurses;
volatile bool frogProj[MAX_FROG_PROJ];                  pthread_mutex_t semFrogProj;
volatile bool frogAtStart;                              pthread_mutex_t semFrogger;

Buffer mainBuffer;

bool setStartingVariables();
bool game();

int main()
{  
    setlocale(LC_ALL, ""); // attiva i caratteri speciali dell'UNICODE

    if(GENERAL_DEBUG)
        system("echo -e \"\\e[8;53;160t\""); // imposta la dimensione del terminale qualora sia troppo piccolo
    else
        system("echo -e \"\\e[8;53;108t\"");

    if(GENERAL_DEBUG)
    {
        FILE *fp = fopen("debug.txt", "w"); // cancella il file di debug!
        fclose(fp);  
    }      

    bool playAgain = game(); 

    if(playAgain)
        execl("./game", "./game", NULL);

    return 0;
}

bool setStartingVariables()
{
    // PER NCURSES ==========================================================================
    pthread_mutex_init(&semDifficult, NULL);
    pthread_mutex_init(&semCurses, NULL);
    // ======================================================================================

    // PER IL BUFFER ========================================================================
    pthread_mutex_init(&mainBuffer.mutex, NULL);
    sem_init(&mainBuffer.sem_free_space, false, BUFFER_SIZE);
    // ======================================================================================

    // PER LA RANA ==========================================================================
    pthread_mutex_init(&semFrogProj, NULL);
    pthread_mutex_init(&semFrogger, NULL);
    frogAtStart = false;

    for(short r = 0; r < MAX_FROG_PROJ; r++)
        frogProj[r] = false;
}

bool game()
{
    initscr(); noecho(); curs_set(0);
    start_color(); istanziaColori(); cbreak();
    nodelay(stdscr, TRUE); // nessun delay per la getch()
    keypad(stdscr, TRUE); // attiva i tasti speciali (le frecce)
    mousemask(ALL_MOUSE_EVENTS, NULL); // attiva gli eventi del mouse   

    pthread_t tFrog;
    bool playAgain = false; bool continua = true;

    difficult = MEDIUM; //Menu();
    
    if(difficult != -1) // se non e' stato scelto exit
    {
        setStartingVariables();

        // CREAZIONE THREADS
        pthread_create(&tFrog, NULL, frogHandler, NULL);

        mainManager();

        // FINE PARTITA (cancellazione threads e mutex)
        pthread_cancel(tFrog);
        pthread_join(tFrog, NULL);

        clear(); 
        CHANGE_COLOR(DEFAULT);
        if(currentGame.lives == 0) // se e' a zero vuol dire che e' morto esaurendo le vite
            mvprintw(0,0, "Game over!%lc Il tuo punteggio e' %d!", L'💀', currentGame.score);
        else
            mvprintw(0,0, "Hai vinto!%lc Il tuo punteggio e' %d!", L'🐸', currentGame.score);

        mvprintw(2,0, "Vuoi giocare ancora? (Y/N)"); refresh();

        char easy[POINT_NUM], medium[POINT_NUM], hard[POINT_NUM];


        GetScore(easy, medium, hard);
        int ch;
        do {
            if((ch = getch()) != ERR)
            {
                switch(ch)
                {
                    case 'y':
                    case 'Y':
                        continua = false;
                        playAgain = true;
                        break;

                    case 'N':
                    case 'n':
                        continua = false;
                        break;
                }
            }
        } while (continua);


        char printToFile[POINT_NUM+1];
        sprintf(printToFile, "%04d", currentGame.score);
        short prevScore;
        switch (difficult)
        {
            case EASY:
                prevScore = charToShort(easy);
                if(currentGame.score > prevScore)
                {
                    SetScore(printToFile, medium, hard);
                }
                break;

            case MEDIUM:
                prevScore = charToShort(medium);
                if(currentGame.score > prevScore)
                {
                    SetScore(easy, printToFile, hard);
                }
                break;

            case HARD:
                prevScore = charToShort(hard);
                if(currentGame.score > prevScore)
                {
                    SetScore(easy, medium, printToFile);
                }
                break;
        }
    }
    endwin();    
    return playAgain;
}
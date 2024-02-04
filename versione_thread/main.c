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
#include "functionsT.h"
#include "settings.h"
#include "menu.h"

volatile short difficult;
volatile GameUpdates currentGame;

volatile Frog Frogger;
pthread_mutex_t semFrogger;
pthread_mutex_t semCurses;

bool game();

int main()
{  
    setlocale(LC_ALL, ""); // attiva i caratteri speciali dell'UNICODE

    difficult = EASY;

    bool playAgain = game(); 

    return 0;
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

    // VALORI DI DEFAULT
    Frogger.x = (BLOCK_PER_MAP_ROWS / 2) * COLUMNS_PER_BLOCK +1; // x iniziale (centro mappa)
    Frogger.y = ROWS_PER_MAP - 1; // y iniziale (ultima riga)

    // SEMAFORI
    pthread_mutex_init(&semFrogger, NULL);
    pthread_mutex_init(&semCurses, NULL);

    pthread_create(&tFrog, NULL, frogHandler, NULL);

    mainManager();

    pthread_cancel(tFrog);
    pthread_join(tFrog, NULL);

    // FINE PARTITA
    pthread_mutex_destroy(&semFrogger);
    pthread_mutex_destroy(&semCurses);

    clear(); 
    if(currentGame.lives == 0) // se e' a zero vuol dire che e' morto esaurendo le vite
        mvprintw(0,0, "Game over!%lc Il tuo punteggio e' %d!", L'💀', currentGame.score);
    else
        mvprintw(0,0, "Hai vinto!%lc Il tuo punteggio e' %d!", L'🐸', currentGame.score);

    mvprintw(2,0, "Vuoi giocare ancora? (Y/N)"); refresh();

    int ch;
    do
    {
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

    endwin();
    return playAgain;
}
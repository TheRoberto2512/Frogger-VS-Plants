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

volatile short difficult;                       pthread_mutex_t semDifficult;
volatile GameUpdates currentGame;               pthread_mutex_t semCurses;

volatile Frog Frogger;                          pthread_mutex_t semFrogger;

Projectile frogProjectiles[MAX_FROG_PROJ];      pthread_mutex_t semFrogProjectiles;
bool doProjectileExist[MAX_FROG_PROJ];          pthread_mutex_t semDoProjectileExist;
bool genFrogProj;                               pthread_mutex_t semGenFrogProj;

Enemy allEnemies[MAX_ENEMIES];                  pthread_mutex_t semAllEnemies;
bool aliveEnemies[MAX_ENEMIES];                 pthread_mutex_t semAliveEnemies;
short rowsY[RIVERSIDE_ROWS];                    

Projectile enemyProjectiles[MAX_ENEMIES];       pthread_mutex_t semEmenyProjectiles;
bool enemyProjectilesAlive[MAX_ENEMIES];        pthread_mutex_t semEnemyProjectilesAlive;

RiverRow river[RIVER_ROWS];                     pthread_mutex_t riverMutex[RIVER_ROWS];

bool setStartingVariables();
bool game();

int main()
{  
    setlocale(LC_ALL, ""); // attiva i caratteri speciali dell'UNICODE

    FILE *fp = fopen("debug.txt", "w"); // cancella il file di debug!
    fclose(fp);    

    difficult = HARD;

    bool playAgain = game(); 

    return 0;
}

bool setStartingVariables()
{
    // PER NCURSES ==========================================================================
    pthread_mutex_init(&semDifficult, NULL);
    pthread_mutex_init(&semCurses, NULL);
    // ======================================================================================

    GameRules rules = getRules(difficult);

    // PER GESTIRE LA RANA ==================================================================
    Frogger.x = (BLOCK_PER_MAP_ROWS / 2) * COLUMNS_PER_BLOCK +1; // x iniziale (centro mappa)
    Frogger.y = ROWS_PER_MAP - 1;                                // y iniziale (ultima riga)

    pthread_mutex_init(&semFrogger, NULL);
    // ======================================================================================


    // PER I PROIETTILI DELLA RANA ==========================================================
    for(short i = 0; i < MAX_FROG_PROJ; i++) 
    {
        frogProjectiles[i].x = -1; frogProjectiles[i].y = -1;
        doProjectileExist[i] = false;
    }  
    genFrogProj = false;

    pthread_mutex_init(&semFrogProjectiles, NULL);
    pthread_mutex_init(&semDoProjectileExist, NULL);
    pthread_mutex_init(&semGenFrogProj, NULL);
    // ======================================================================================

    // I NEMICI =============================================================================
    for(short e = 0; e < MAX_ENEMIES; e++)
    {
        aliveEnemies[e] = false;
        allEnemies[e].x = -1; allEnemies[e].y = -1;
    }

    rowsY[0] = (LILY_PADS_ROWS * ROWS_PER_BLOCK) + SCOREBOARD_ROWS;
    for(short r = 1; r < RIVERSIDE_ROWS; r++)
    {   
        rowsY[r] = rowsY[r-1] + ROWS_PER_BLOCK;
    }

    pthread_mutex_init(&semAllEnemies, NULL);
    pthread_mutex_init(&semAliveEnemies, NULL);
    //pthread_mutex_init(&semRowsY, NULL);
    // ======================================================================================

    // PROIETTILI NEMICI ====================================================================
    for(short t = 0; t < MAX_ENEMIES; t++)
    {
        enemyProjectilesAlive[t] = false;
        enemyProjectiles[t].x=-1; enemyProjectiles[t].y=-1;
        enemyProjectiles[t].speed = rules.speed;
    }
    // ======================================================================================

    // COCCODRILLI ==========================================================================
    short ra = rand() % 2;
    for(short r = 0; r < RIVER_ROWS; r++)
    {
        river[r].direction = ra;
        ra = !ra;
        for(short rr = 0; rr < MAX_CROCODILE_PER_ROW; rr++)
        {
            river[r].crocs[rr].x = STOPPED_CROCODILE;
            river[r].crocs[rr].y = STOPPED_CROCODILE;  
            river[r].crocs[rr].direction = river[r].direction;
            river[r].crocs[rr].speed = 2;
            river[r].speed=2;
            river[r].crocs[rr].splash = -10;
        }
        pthread_mutex_init(&riverMutex[r], NULL);
    }
    // ======================================================================================
}

bool game()
{
    initscr(); noecho(); curs_set(0);
    
    start_color(); istanziaColori(); cbreak();
    nodelay(stdscr, TRUE); // nessun delay per la getch()
    keypad(stdscr, TRUE); // attiva i tasti speciali (le frecce)
    mousemask(ALL_MOUSE_EVENTS, NULL); // attiva gli eventi del mouse   

    pthread_t tFrog, tFrogProjectilesHandler, crocOne,tRiverHandler;
    bool playAgain = false; bool continua = true;

    setStartingVariables();

    // CREAZIONE THREADS
    pthread_create(&tFrog, NULL, frogHandler, NULL);
    pthread_create(&tFrogProjectilesHandler, NULL, frogProjectilesHandler, NULL);

    CrocPos crocCoords[RIVER_ROWS * MAX_CROCODILE_PER_ROW]; short coordsCount = 0;
    for(short r = 0; r < RIVER_ROWS; r++)
    {
        for(short rr = 0; rr < MAX_CROCODILE_PER_ROW; rr++)
        {
            crocCoords[coordsCount].row = r; crocCoords[coordsCount].ID = rr;
            pthread_create(&crocOne, NULL, singleCrocodileHandler, (void *)&crocCoords[coordsCount]);
            coordsCount++;
        }
    }
    pthread_create(&tRiverHandler, NULL, riverHandler, NULL);

    mainManager();

    pthread_cancel(tFrog);
    pthread_join(tFrog, NULL);
    pthread_cancel(tFrogProjectilesHandler);
    pthread_join(tFrogProjectilesHandler, NULL);
    pthread_cancel(tRiverHandler);
    pthread_join(tRiverHandler, NULL);

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

    endwin();
    return playAgain;
}
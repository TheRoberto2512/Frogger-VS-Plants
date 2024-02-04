#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#include "functionsT.h"
#include "sprite.h"
#include "structs.h"
#include "settings.h"
#include "crocList.h"
#include "collisionDetector.h"

extern volatile short difficult;
extern GameUpdates currentGame;

extern volatile Frog Frogger;
extern pthread_mutex_t semFrogger;
extern pthread_mutex_t semCurses;


/********************\
*  FUNZIONI THREAD  *
\********************/

void *frogHandler(void *arg)
{
    int ch;
    do {
        pthread_mutex_lock(&semCurses); 
        if((ch = getch()) != ERR && ch != KEY_MOUSE) // se c'e' stato un input non proveniente dal mouse
        {
            pthread_mutex_lock(&semFrogger); 
            switch(ch) //qui dentro modifichiamo x e y
            {
                case 'd':
                case 'D':
                case KEY_RIGHT:
                    Frogger.x += COLUMNS_PER_BLOCK;
                    if (Frogger.x > COLUMNS_PER_MAP - COLUMNS_PER_BLOCK+1)
                    {
                        Frogger.x = COLUMNS_PER_MAP - COLUMNS_PER_BLOCK+1;
                    }
                    break;

                case 'a':
                case 'A':
                case KEY_LEFT:
                    Frogger.x -= COLUMNS_PER_BLOCK;
                    if (Frogger.x < 1)
                    {
                        Frogger.x = 1;
                    }
                    break;

                case 'w':
                case 'W':
                case KEY_UP:
                    Frogger.y -= ROWS_PER_BLOCK;
                    if(Frogger.y < SCOREBOARD_ROWS)
                    {
                        Frogger.y = SCOREBOARD_ROWS;
                    }
                    break;

                case 's':
                case 'S':
                case KEY_DOWN:
                    Frogger.y += ROWS_PER_BLOCK;
                    if (Frogger.y > (ROWS_PER_MAP - 3))
                    {
                        Frogger.y = ROWS_PER_MAP - 1;
                    }
                    break;

                case ' ': // generazione di un proietile
                    break;
            }
            pthread_mutex_unlock(&semFrogger); 
        } 
        pthread_mutex_unlock(&semCurses); 
        usleep(FRAME_UPDATE); 
    } while (true);
}

void *mainManager(void *args)
{
    // GESTIONE DELLA PARTITA CORRENTE
    currentGame.lives = LIVES; currentGame.score = 0;
    GameRules rules = getRules(difficult);
    rules.time = 2;

    Frog frogger;
    
    // FLAG BOOLEANE PER GESTIRE LA PARTITA
    bool endManche = false; bool keepPlaying = true;                                 

    short fps = 0, seconds = 0;                     // per gestire il numero di aggiornamenti (fps) e di secondi passati (seconds)
    short DebugLine = 0;                            // per la stampa delle schermate aggiuntive di debug
    short minRow = ROWS_PER_MAP - 1;                // salva la riga piu' alta raggiunta dalla rana per attribuire i punti

    bool emptyLilyPads[5] = {true, true, true, true, true};
    double r = BLOCK_PER_MAP_ROWS / 5;
    short lilyPadsX[5] = { -1, -1, -1, -1, -1};

    for(short i = 0; i < 5; i++)
    {
        lilyPadsX[i] = COLUMNS_PER_BLOCK * (1 + (i * r));
    }

    do{
        if(fps < 5 && seconds == 0) // evita casi di schermo che non si aggiorna 
        {
            pthread_mutex_lock(&semCurses); clear(); customBorder(0, 0, COLUMNS_PER_MAP + 2, ROWS_PER_MAP + SCOREBOARD_ROWS, true); pthread_mutex_unlock(&semCurses);
        } 

        // INIZIO STAMPE
    pthread_mutex_lock(&semCurses);
        customBorder(0, 0, COLUMNS_PER_MAP + 2, ROWS_PER_MAP + SCOREBOARD_ROWS, true);              // stampa i bordi
        printScoreBoard(currentGame.lives, currentGame.score, (rules.time - seconds), FULL_TIME);  // aggiorna la scoreboard
        printMap(true, emptyLilyPads, (fps == 0 && seconds == 0) ? true : false);                   // stampa la mappa                                                                         // stampa i coccodrilli
    pthread_mutex_unlock(&semCurses);

        // LEGGE LA POSIZIONE DELLA RANA
        pthread_mutex_lock(&semFrogger); frogger = Frogger; pthread_mutex_unlock(&semFrogger);
        if(frogger.y < minRow)
        {
            minRow = frogger.y;
            currentGame.score += ROW_UP;
        }  
    

        // STAMPA LA RANA
        pthread_mutex_lock(&semCurses);  printFrog(frogger.x, frogger.y);  pthread_mutex_unlock(&semCurses);

        // SCHERMATE DI DEBUG
        if(GENERAL_DEBUG)
        {
            DebugLine = 0;
            if(FROG_DEBUG)                                      
            {
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, 3, false);
                mvprintw(DebugLine, DEBUG_COLUMNS+1, "FROGGER");
                mvprintw(DebugLine+1, DEBUG_COLUMNS, "%03d : %03d", frogger.x, frogger.y);
                DebugLine += (2 + 1 + 1); // 2 (bordi) + 1 (righe) + 1 (spazio)
            }
            if(FROG_PROJECTILES_DEBUG)
            {

            }
            if(RIVER_DEBUG)
            {
            }
            if(ENEMIES_DEBUG)
            {
            }
            if(ENEMIES_PROJECTILES_DEBUG)
            {

            }
            if(COLLISION_DEBUG)
            {
                
            }
        }  
           
        // SE IL TEMPO E' SCADUTO
        if((rules.time-seconds) <= -1 && !GODMODE)
        {
            currentGame.lives = currentGame.lives - 1; endManche = true;
        }

        if(currentGame.lives == 0)
            keepPlaying = false;

        if(endManche)
        {
            endManche = false; seconds = 0; fps = 0; 

            frogger.x = (BLOCK_PER_MAP_ROWS / 2) * COLUMNS_PER_BLOCK +1;    // x iniziale (centro mappa)
            frogger.y = ROWS_PER_MAP - 1;                                   // y iniziale (ultima riga)
            pthread_mutex_lock(&semFrogger); Frogger = frogger; pthread_mutex_unlock(&semFrogger);
        }

        // UPDATE SCHERMO E TEMPO
        pthread_mutex_lock(&semCurses); refresh(); pthread_mutex_unlock(&semCurses);        
        fps++;               // aggiorna e incrementa fps        
        if(fps != 0 && fps % 30 == 0)   // ogni 30 update passa un secondo
            seconds++;

        usleep(FRAME_UPDATE);           // riposa
    
    } while(keepPlaying);
}
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

extern volatile short difficult;                    extern pthread_mutex_t semDifficult;
extern GameUpdates currentGame;                     extern pthread_mutex_t semCurses;

extern volatile Frog Frogger;                       extern pthread_mutex_t semFrogger;

extern Projectile frogProjectiles[MAX_FROG_PROJ];   extern pthread_mutex_t semFrogProjectiles;
extern bool doProjectileExist[MAX_FROG_PROJ];       extern pthread_mutex_t semDoProjectileExist;
extern bool genFrogProj;                            extern pthread_mutex_t semGenFrogProj;


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
                    pthread_mutex_lock(&semGenFrogProj); genFrogProj = true; pthread_mutex_unlock(&semGenFrogProj);
                    break;
            }
            pthread_mutex_unlock(&semFrogger); 
        } 
        pthread_mutex_unlock(&semCurses); 
        usleep(FRAME_UPDATE); 
    } while (true);
}

void *frogProjectilesHandler(void *arg)
{
    pthread_mutex_lock(&semDifficult);
    GameRules rules = getRules(difficult);
    pthread_mutex_unlock(&semDifficult);
    
    short updatesCounter = 0; 

    bool proiettiliEsistenti[MAX_FROG_PROJ];

    for(short i = 0; i < MAX_FROG_PROJ; i++)
    {
        proiettiliEsistenti[i] = false;
    }

    Frog actual;

    do{
        bool generate = false;
        pthread_mutex_lock(&semGenFrogProj); generate = genFrogProj; pthread_mutex_unlock(&semGenFrogProj);

        pthread_mutex_lock(&semDoProjectileExist);
        for(short i = 0; i < MAX_FROG_PROJ; i++)
        {
            proiettiliEsistenti[i] = doProjectileExist[i];
        }
        pthread_mutex_unlock(&semDoProjectileExist);

        // SE LA RANA HA CHIESTO DI GENERARE UN PROIETTILE
        if(generate)
        {
            pthread_mutex_lock(&semFrogger); actual = Frogger; pthread_mutex_unlock(&semFrogger);
            for(short i = 0; i < MAX_FROG_PROJ; i++)
            {
                if(proiettiliEsistenti[i] == false)
                {
                    proiettiliEsistenti[i] = true;

                    pthread_mutex_lock(&semFrogProjectiles);
                    frogProjectiles[i].PTID = 0;             // perche' e' la rana (i proiettili nemici useranno 1-2-3)
                    frogProjectiles[i].ID = i;               // salviamo un ID provvisorio
                    frogProjectiles[i].x = actual.x + 3;
                    frogProjectiles[i].y = actual.y - 1;
                    frogProjectiles[i].speed = rules.speed;
                    pthread_mutex_unlock(&semFrogProjectiles);
                    pthread_mutex_lock(&semGenFrogProj); genFrogProj = false; pthread_mutex_unlock(&semGenFrogProj);
                    break;
                }
            }

            pthread_mutex_lock(&semDoProjectileExist);
            for(short i = 0; i < MAX_FROG_PROJ; i++)
            {
                doProjectileExist[i] = proiettiliEsistenti[i];
            }
            pthread_mutex_unlock(&semDoProjectileExist);
        }

        // FA AVANZARE I PROIETTILI IN BASE ALLA LORO VELOCITA' E COMUNICA LE NUOVE COORDINATE
        pthread_mutex_lock(&semFrogProjectiles);
        for(short i = 0; i < MAX_FROG_PROJ; i++)
        {
            if(proiettiliEsistenti[i])
            {
                if(updatesCounter % rules.speed == 0)
                {
                    frogProjectiles[i].y -= 1;
                    if(frogProjectiles[i].y < 3)
                    {
                        proiettiliEsistenti[i] = false; // smette di esistere
                        pthread_mutex_lock(&semDoProjectileExist);
                        doProjectileExist[i] = false;
                        pthread_mutex_unlock(&semDoProjectileExist);
                    }                    
                }
            }
        }
        pthread_mutex_unlock(&semFrogProjectiles);

        usleep(FRAME_UPDATE);

    } while (true);
}

void *mainManager(void *args)
{
    // GESTIONE DELLA PARTITA CORRENTE
    currentGame.lives = LIVES; currentGame.score = 0;
    pthread_mutex_lock(&semDifficult);
    GameRules rules = getRules(difficult);
    pthread_mutex_unlock(&semDifficult);

    Frog frogger;
    Projectile proiettiliRana[MAX_FROG_PROJ];
    bool doFrogProjectileExist[MAX_FROG_PROJ];

    for(short i = 0; i < MAX_FROG_PROJ; i++)
    {
        doFrogProjectileExist[i] = false;
    }
    
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
    
        pthread_mutex_lock(&semDoProjectileExist);
        for(short i = 0; i < MAX_FROG_PROJ; i++)
        {
            doFrogProjectileExist[i] = doProjectileExist[i];
        }
        pthread_mutex_unlock(&semDoProjectileExist);

        pthread_mutex_lock(&semFrogProjectiles);
        for(short i = 0; i < MAX_FROG_PROJ; i++)
        {
            proiettiliRana[i] = frogProjectiles[i];
        }
        pthread_mutex_unlock(&semFrogProjectiles);

        // STAMPA LA RANA
        pthread_mutex_lock(&semCurses);  printFrog(frogger.x, frogger.y);  pthread_mutex_unlock(&semCurses);

        // STAMPA TUTTI I PROIETTILI DELLA RANA (SE DENTRO L'AREA DI GIOCO)
        for(short p = 0; p < MAX_FROG_PROJ; p++)
        {
            if(doFrogProjectileExist[p])
            {
                pthread_mutex_lock(&semCurses);
                printProjectile(proiettiliRana[p].x, proiettiliRana[p].y, true);
                pthread_mutex_unlock(&semCurses);
            
                if(proiettiliRana[p].y <= 3)
                {
                    doFrogProjectileExist[p] = false;
                    pthread_mutex_lock(&semDoProjectileExist);
                    doProjectileExist[p] = false;
                    pthread_mutex_unlock(&semDoProjectileExist);
                }
            }
        }

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
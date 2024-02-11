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
#include "collisionDetector.h"

extern volatile short difficult;                    extern pthread_mutex_t semDifficult;
extern GameUpdates currentGame;                     extern pthread_mutex_t semCurses;

extern volatile Frog Frogger;                       extern pthread_mutex_t semFrogger;

extern Projectile frogProjectiles[MAX_FROG_PROJ];   extern pthread_mutex_t semFrogProjectiles;
extern bool doProjectileExist[MAX_FROG_PROJ];       extern pthread_mutex_t semDoProjectileExist;
extern bool genFrogProj;                            extern pthread_mutex_t semGenFrogProj;

extern Enemy allEnemies[MAX_ENEMIES];               extern pthread_mutex_t semAllEnemies;
extern bool aliveEnemies[MAX_ENEMIES];              extern pthread_mutex_t semAliveEnemies;
extern short rowsY[RIVERSIDE_ROWS];                 

extern Projectile enemyProjectiles[MAX_ENEMIES];    extern pthread_mutex_t semEmenyProjectiles;
extern bool enemyProjectilesAlive[MAX_ENEMIES];     extern pthread_mutex_t semEnemyProjectilesAlive;

extern RiverRow river[RIVER_ROWS];                  extern pthread_mutex_t riverMutex[RIVER_ROWS];

short spawnTimers[RIVER_ROWS] = {32000, 32000, 32000, 32000, 32000, 32000, 32000, 32000};

/********************\
*  FUNZIONI THREAD  *
\********************/

void *frogHandler(void *arg) // semCurses, semFrogger, semGenFrogProj
{
    int ch;
    do {
        pthread_mutex_lock(&semCurses); ch = getch(); pthread_mutex_unlock(&semCurses);
        if(ch != ERR && ch != KEY_MOUSE) // se c'e' stato un input non proveniente dal mouse
        {
            switch(ch) //qui dentro modifichiamo x e y
            {
                case 'd':
                case 'D':
                case KEY_RIGHT:
                    pthread_mutex_lock(&semFrogger); 
                    Frogger.x += COLUMNS_PER_BLOCK;
                    if (Frogger.x > COLUMNS_PER_MAP - COLUMNS_PER_BLOCK+1)
                    {
                        Frogger.x = COLUMNS_PER_MAP - COLUMNS_PER_BLOCK+1;
                    }
                    pthread_mutex_unlock(&semFrogger);
                    break;

                case 'a':
                case 'A':
                case KEY_LEFT:
                    pthread_mutex_lock(&semFrogger); 
                    Frogger.x -= COLUMNS_PER_BLOCK;
                    if (Frogger.x < 1)
                    {
                        Frogger.x = 1;
                    }
                    pthread_mutex_unlock(&semFrogger);
                    break;

                case 'w':
                case 'W':
                case KEY_UP:
                    pthread_mutex_lock(&semFrogger); 
                    Frogger.y -= ROWS_PER_BLOCK;
                    if(Frogger.y < SCOREBOARD_ROWS)
                    {
                        Frogger.y = SCOREBOARD_ROWS;
                    }
                    pthread_mutex_unlock(&semFrogger);
                    break;

                case 's':
                case 'S':
                case KEY_DOWN:
                    pthread_mutex_lock(&semFrogger); 
                    Frogger.y += ROWS_PER_BLOCK;
                    if (Frogger.y > (ROWS_PER_MAP - 3))
                    {
                        Frogger.y = ROWS_PER_MAP - 1;
                    }
                    pthread_mutex_unlock(&semFrogger);
                    break;

                case ' ': // generazione di un proietile
                    pthread_mutex_lock(&semGenFrogProj); genFrogProj = true; pthread_mutex_unlock(&semGenFrogProj);
                    break;
            }
        } 
        usleep(FRAME_UPDATE); 
    } while (true);
}

void *frogProjectilesHandler(void *arg) // semGenFrogProj, semDoProjectileExist, semFrogProjectiles
{
    pthread_mutex_lock(&semDifficult);
    GameRules rules = getRules(difficult);
    pthread_mutex_unlock(&semDifficult);
    
    short updatesCounter = 0; Frog actual;

    bool proiettiliEsistenti[MAX_FROG_PROJ];

    setToFalse(proiettiliEsistenti, MAX_FROG_PROJ);

    do{
        bool generate = false;
        pthread_mutex_lock(&semGenFrogProj); generate = genFrogProj; pthread_mutex_unlock(&semGenFrogProj);

        pthread_mutex_lock(&semDoProjectileExist);  // scarichiamo in locale
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

                    pthread_mutex_lock(&semDoProjectileExist);
                    doProjectileExist[i] = true;
                    pthread_mutex_unlock(&semDoProjectileExist);

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
                    }                    
                }
            }
        }
        pthread_mutex_unlock(&semFrogProjectiles);

        pthread_mutex_lock(&semDoProjectileExist);  // scarichiamo in locale
        for(short i = 0; i < MAX_FROG_PROJ; i++)
        {
            doProjectileExist[i] = proiettiliEsistenti[i];
        }
        pthread_mutex_unlock(&semDoProjectileExist);

        usleep(FRAME_UPDATE);

    } while (true);
}

void *mainManager(void *args)
{
    bool n = true;
    // GESTIONE DELLA PARTITA CORRENTE
    currentGame.lives = LIVES; currentGame.score = 0;

    pthread_mutex_lock(&semDifficult);
    GameRules rules = getRules(difficult);
    pthread_mutex_unlock(&semDifficult);   

    Frog frogger;
    Projectile proiettiliRana[MAX_FROG_PROJ];
    bool doFrogProjectileExist[MAX_FROG_PROJ];
    setToFalse(doFrogProjectileExist, MAX_FROG_PROJ);

    bool printEnemies[MAX_ENEMIES];                 // FLAG per decidere se stampare o no i nemici
    Enemy AllEnemies[MAX_ENEMIES]; 
    setToFalse(printEnemies, MAX_ENEMIES);

    bool printAllEnemyProjectiles[MAX_ENEMIES];   
    Projectile AllProjectilesEnemies[MAX_ENEMIES];
    
    RiverRow backupRiver[RIVER_ROWS];

    // FLAG BOOLEANE PER GESTIRE LA PARTITA
    bool endManche = false; bool keepPlaying = true;                                 

    short fps = 0, seconds = 0;                     // per gestire il numero di aggiornamenti (fps) e di secondi passati (seconds)
    short DebugLine = 0;                            // per la stampa delle schermate aggiuntive di debug
    short minRow = ROWS_PER_MAP - 1;                // salva la riga piu' alta raggiunta dalla rana per attribuire i punti

    bool emptyLilyPads[5] = {true, true, true, true, true};
    double r = BLOCK_PER_MAP_ROWS / 5;
    short lilyPadsX[5] = { -1, -1, -1, -1, -1};

    randomSeed();

    pthread_mutex_lock(&semAllEnemies);
    newEnemiesScene(rowsY, allEnemies);
    pthread_mutex_unlock(&semAllEnemies);

    pthread_mutex_lock(&semAliveEnemies);
    for(short i = 0; i < MAX_ENEMIES; i++)
    {
        aliveEnemies[i] = true;
    }
    pthread_mutex_unlock(&semAliveEnemies);

    for(short k=0;k<MAX_ENEMIES;k++)
    {
        spawnEnemy(allEnemies[k].ID);
    }

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
    
        // LEGGE I NEMICI
        pthread_mutex_lock(&semAliveEnemies); 
        for(short i = 0; i < MAX_ENEMIES; i++)
        {
            printEnemies[i] = aliveEnemies[i];
        }
        pthread_mutex_unlock(&semAliveEnemies); 

        pthread_mutex_lock(&semAllEnemies);
        for(short i = 0; i < MAX_ENEMIES; i++)
        {
            AllEnemies[i] = allEnemies[i];
        }
        pthread_mutex_unlock(&semAllEnemies);

        // SALVA I PROIETTILI ESISTENTI (RANA)
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

        // SALVA I PROIETTILI ESISTENTI (NEMICI)
        pthread_mutex_lock(&semEmenyProjectiles); 
        for(short i = 0; i < MAX_ENEMIES; i++)
        {
            AllProjectilesEnemies[i] = enemyProjectiles[i];
        }
        pthread_mutex_unlock(&semEmenyProjectiles); 

        pthread_mutex_lock(&semEnemyProjectilesAlive);
        for(short i = 0; i < MAX_ENEMIES; i++)
        {
            printAllEnemyProjectiles[i] = enemyProjectilesAlive[i]; 
        }
        pthread_mutex_unlock(&semEnemyProjectilesAlive);

        // SALVA TUTTI I COCCODRILLI
        for(short c = 0; c < RIVER_ROWS; c++)
        {
            pthread_mutex_lock(&riverMutex[c]);
            backupRiver[c] = river[c];
            pthread_mutex_unlock(&riverMutex[c]);
        }

        // COLLISIONI
        bool frogPrjsEnemiesCollided = false;
        for(short fr = 0; fr < MAX_FROG_PROJ; fr++)
        {
            if(doFrogProjectileExist[fr]) // se il proiettile esiste
            {
                short ROW = yToRowNumber(proiettiliRana[fr].y);

                if(ROW >= 1 && ROW <= RIVERSIDE_ROWS)
                {
                    // controlliamo la collisione tra i proiettili rana e i nemici solo se l'altezza dei proiettili rientra nelle righe oltre il fiume
                    for(short e = 0; e < MAX_ENEMIES && !frogPrjsEnemiesCollided; e++)
                    {
                        if(AllEnemies[e].genTime == 0 && printEnemies[e])
                        {
                            frogPrjsEnemiesCollided = enemyFrogProjCD(AllEnemies[e].x, AllEnemies[e].y, proiettiliRana[fr].x, proiettiliRana[fr].y);
                            if(frogPrjsEnemiesCollided)
                            {
                                currentGame.score = currentGame.score + ENEMY_KILLED; 
                                doFrogProjectileExist[fr] = false;

                                pthread_mutex_lock(&semDoProjectileExist);
                                doProjectileExist[fr] = false;
                                pthread_mutex_unlock(&semDoProjectileExist);

                                pthread_mutex_lock(&semAliveEnemies);
                                aliveEnemies[e] = false;
                                pthread_mutex_unlock(&semAliveEnemies);

                                printEnemies[e] = false;
                            }
                        }
                    }
                }
            }
        }

        // COLLISIONI RANA - tane / nemici/ coccodrilli
        bool frogEnemyCollided = false;                         // rana scontrata con un nemico
        bool froggerEnteredLilypads = false;                    // rana dentro una tana
        bool frogOnCrocodile = false;                           // rana sul coccodrillo
        
        short frogRow = yToRowNumber(frogger.y);
        if(frogRow == 0) // se la rana e' all'altezza delle tane
        {
            for(short t = 0; t < 5 && !froggerEnteredLilypads; t++)
            {
                if(emptyLilyPads[t])
                {
                    froggerEnteredLilypads = isFrogEnteredInside(frogger.x, FROG_COLUMNS, lilyPadsX[t], LILY_PADS_COLUMNS);
                    if(froggerEnteredLilypads)
                    {
                        emptyLilyPads[t] = false;

                        // SCOREBOARD
                        currentGame.score += FILLED_LILYPAD; // aggiorno il punteggio
                        currentGame.score += (rules.time - seconds) * POINTS_PER_SECOND;

                        bool allFull = true;
                        for(short f = 0; f < 5; f++)
                        {   
                            if(emptyLilyPads[f])
                            {
                                allFull = false;
                                break;
                            }     
                        }
                        if(allFull)
                        {
                            keepPlaying = false;
                            currentGame.score += VICTORY;
                        }
                        else   
                            endManche = true;                     
                    }
                }
            }
            if(!froggerEnteredLilypads && !GODMODE)
            {
                currentGame.lives = currentGame.lives - 1; endManche = true;
            }
        }
        else if(frogRow >= 1 && frogRow <= RIVERSIDE_ROWS) // se la rana e' all'altezza dell'argine superiore
        {
            for(short e = 0; e < MAX_ENEMIES && !frogEnemyCollided; e++)          
            {
                if(AllEnemies[e].genTime == 0) 
                {
                    frogEnemyCollided = frogEnemyCD(frogger.x, frogger.y, AllEnemies[e].x, AllEnemies[e].y);
                }
            }
            if(frogEnemyCollided && !GODMODE)
            {
                currentGame.lives = currentGame.lives - 1; endManche = true;
            }
        }
        
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

        // STAMPA TUTTI I NEMICI
        for(short e = 0; e < MAX_ENEMIES; e++)
        {
            if(printEnemies[e])
            {
                pthread_mutex_lock(&semCurses); printEnemy(AllEnemies[e].x, AllEnemies[e].y, AllEnemies[e].genTime); pthread_mutex_unlock(&semCurses);
                if(AllEnemies[e].genTime > 0)
                {
                    AllEnemies[e].genTime--;
                    
                    pthread_mutex_lock(&semAllEnemies);
                    allEnemies[e].genTime--;
                    pthread_mutex_unlock(&semAllEnemies);
                }                
            }
        }
        
        // STAMPA TUTTI I COCCODRILLI
        for(short cc = 0; cc < RIVER_ROWS; cc++)
        {
            for(short ccc = 0; ccc < MAX_CROCODILE_PER_ROW; ccc++)
            {
                if(backupRiver[cc].crocs[ccc].x != STOPPED_CROCODILE && backupRiver[cc].crocs[ccc].y != STOPPED_CROCODILE)
                {
                    pthread_mutex_lock(&semCurses);
                    if(backupRiver[cc].crocs[ccc].splash == -10)
                        printCrocodile(backupRiver[cc].crocs[ccc].x, backupRiver[cc].crocs[ccc].y, backupRiver[cc].direction);
                    else
                        printBadCrocodile(backupRiver[cc].crocs[ccc].x, backupRiver[cc].crocs[ccc].y, backupRiver[cc].direction);
                    pthread_mutex_unlock(&semCurses);
                }
            }
        }

        // STAMPA TUTTI I PROIETTILI NEMICI
        for(short p = 0; p < MAX_ENEMIES; p++)
        {
            if(printAllEnemyProjectiles[p] && aliveEnemies)
            {
                if(seconds < 1)
                {
                    printAllEnemyProjectiles[p] = false;
                    pthread_mutex_lock(&semEnemyProjectilesAlive);
                    enemyProjectilesAlive[p] = false;
                    pthread_mutex_unlock(&semEnemyProjectilesAlive);
                }
                else
                {
                    if(AllProjectilesEnemies[p].y >= (ROWS_PER_MAP + ROWS_PER_BLOCK -1))
                    {
                        printAllEnemyProjectiles[p] = false;
                        pthread_mutex_lock(&semEnemyProjectilesAlive);
                        enemyProjectilesAlive[p] = false;
                        easyKill(enemyProjectiles[p].PTID);
                        pthread_mutex_unlock(&semEnemyProjectilesAlive);
                    }
                    else
                    {
                        pthread_mutex_lock(&semCurses);
                        printProjectile(AllProjectilesEnemies[p].x, AllProjectilesEnemies[p].y, false);
                        pthread_mutex_unlock(&semCurses);
                    }
                }
            }
        }

        // STAMPA LA RANA
        pthread_mutex_lock(&semCurses);  printFrog(frogger.x, frogger.y);  pthread_mutex_unlock(&semCurses);

        // SCHERMATE DI DEBUG
        if(GENERAL_DEBUG)
        {
            DebugLine = 0;
            if(FROG_DEBUG)                                      
            {
                pthread_mutex_lock(&semCurses); 
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, 3, false);
                mvprintw(DebugLine, DEBUG_COLUMNS+1, "FROGGER");
                mvprintw(DebugLine+1, DEBUG_COLUMNS, "%03d : %03d", frogger.x, frogger.y);
                DebugLine += (2 + 1 + 1); // 2 (bordi) + 1 (righe) + 1 (spazio)
                pthread_mutex_unlock(&semCurses);
            }
            if(FROG_PROJECTILES_DEBUG)
            {
                pthread_mutex_lock(&semCurses);
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, 2+MAX_FROG_PROJ, false);
                mvprintw(DebugLine, DEBUG_COLUMNS+1, "FR-PROJ");
                for(short s = 0; s < MAX_FROG_PROJ; s++)
                    if(doFrogProjectileExist[s])
                        mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "%03d : %03d", proiettiliRana[s].x, proiettiliRana[s].y);
                    else
                        mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "  false  ");
                DebugLine += 2 + (MAX_FROG_PROJ) + 1;  // 2 (bordi) + MAX_FROG_PROJ (righe) + 1 (spazio)
                pthread_mutex_unlock(&semCurses);
            }
            if(RIVER_DEBUG)
            {
              
            }
            if(ENEMIES_DEBUG)
            {
                pthread_mutex_lock(&semCurses);
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, 2+MAX_ENEMIES, false);
                mvprintw(DebugLine, DEBUG_COLUMNS+1, "ENEMIES");
                for(short s = 0; s < MAX_ENEMIES; s++)
                    if(printEnemies[s])
                    {
                        mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "%03d : %03d", AllEnemies[s].x, AllEnemies[s].y);
                        mvprintw(DebugLine+1+s, DEBUG_COLUMNS+11, "%03d", AllEnemies[s].shot);
                    }
                    else
                        mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "  false  ");
                DebugLine += 2 + (MAX_ENEMIES) + 1;  // 2 (bordi) + MAX_FROG_PROJ (righe) + 1 (spazio)
                pthread_mutex_unlock(&semCurses);
            }
            if(ENEMIES_PROJECTILES_DEBUG)
            {
                pthread_mutex_lock(&semCurses);
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, MAX_ENEMIES+2, false);
                mvprintw(DebugLine, DEBUG_COLUMNS+1, "EN-PROJ");
                for(short s = 0; s < MAX_ENEMIES; s++)
                    if(printAllEnemyProjectiles[s])
                        mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "%03d : %03d", enemyProjectiles[s].x,enemyProjectiles[s].y);
                    else
                        mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "  false  ");
                DebugLine += 2 + MAX_ENEMIES + 1;
                pthread_mutex_unlock(&semCurses);
            }
            if(COLLISION_DEBUG)
            {
                
            }
        }  

        // AGGIORNA LO STATO DEI NEMICI
        pthread_mutex_lock(&semAllEnemies);
        for(short e = 0; e < MAX_ENEMIES; e++)
        {
            if(allEnemies[e].genTime == 0)
            {
                if(allEnemies[e].shot < -1)
                {
                    allEnemies[e].shot = randomNumber(30 * 4, 30 * 8);
                }
                else
                {
                    allEnemies[e].shot--;
                }
            }
            else
            {
                allEnemies[e].genTime--;
            }
        }
        pthread_mutex_unlock(&semAllEnemies);

        // GENERA NUOVI NEMICI DOPO LA LORO MORTE
        if(fps%10==0 &&fps>0)
        {
            for(short e = 0; e < MAX_ENEMIES; e++)
            {
                if(aliveEnemies[e] == false)
                {
                    pthread_mutex_lock(&semAliveEnemies);
                    aliveEnemies[e]=true;
                    pthread_mutex_unlock(&semAliveEnemies);

                    pthread_mutex_lock(&semAllEnemies);
                    bool collided = false; 
                    do{
                        collided = false;
                        allEnemies[e].x = randomNumber(1, COLUMNS_PER_MAP-ENEMY_COLUMNS-1);
                        allEnemies[e].y = rowsY[rand() % RIVERSIDE_ROWS];
                        allEnemies[e].shot = randomNumber(30, 100);
                        allEnemies[e].ID = e;
                        allEnemies[e].genTime = randomNumber(15, 90); // da mezzo a tre secondi
                        
                        for(short o = 0; o < MAX_ENEMIES; o++)
                        {
                            if(o != e)
                            {
                                collided = enemyEnemyCD(allEnemies[e].x, allEnemies[e].y, allEnemies[o].x, allEnemies[o].y);
                            }
                            if(collided)
                            {
                                o = 30000;
                            }    
                        }
                    
                    } while (collided);
                    pthread_mutex_unlock(&semAllEnemies);
                    break;
                }
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

            for(short p = 0; p < MAX_ENEMIES; p++)
            {
                pthread_mutex_lock(&semAllEnemies);
                newEnemiesScene(rowsY, allEnemies);   
                pthread_mutex_unlock(&semAllEnemies); 

                pthread_mutex_lock(&semEmenyProjectiles);
                if(enemyProjectilesAlive[p]==true)
                    easyKill(enemyProjectiles[p].PTID);
                pthread_mutex_unlock(&semEmenyProjectiles);

                pthread_mutex_lock(&semEnemyProjectilesAlive);
                enemyProjectilesAlive[p]=false;
                pthread_mutex_unlock(&semEnemyProjectilesAlive);               
            }

            newCrocodileScene(river,spawnTimers);

        }

        // UPDATE SCHERMO E TEMPO
        pthread_mutex_lock(&semCurses); refresh(); pthread_mutex_unlock(&semCurses);        
        fps++;                          // aggiorna e incrementa fps        
        if(fps != 0 && fps % 30 == 0)   // ogni 30 update passa un secondo
            seconds++;

        usleep(FRAME_UPDATE);           // riposa
    
        if(seconds == 2 && n && false)
        {
            n = false;
            pthread_mutex_lock(&riverMutex[2]);
            river[2].crocs[0].x = 1;
            river[2].crocs[0].y = 16;
            pthread_mutex_unlock(&riverMutex[2]);
            pthread_mutex_lock(&riverMutex[1]);
            river[1].crocs[0].x = 100;
            river[1].crocs[0].y = 20;
            river[1].crocs[0].splash = 60;
            pthread_mutex_unlock(&riverMutex[1]);
        }

    } while(keepPlaying);
}

void *riverHandler(void *args)
{
    short spawns[2] = {COLUMNS_PER_MAP, 1-CROCODILE_COLUMNS};
    bool keepGenerating = true;
    newCrocodileScene(river,spawnTimers);
    
    do
    {
        for(short i=0; i<RIVER_ROWS;i++)
        {
            if(spawnTimers[i]==0)
            {
                spawnCrocodile(i,spawns[river[i].direction],computeY(i));

                if(river[i].direction) // se va a destra
                    spawnTimers[i] = (crocodileSpace() + (CROCODILE_COLUMNS*2)) * river[i].speed;
                else
                    spawnTimers[i] = (crocodileSpace() + CROCODILE_COLUMNS) * river[i].speed; 
            }
            else
            {
                spawnTimers[i]--;
            }
        }

        usleep(FRAME_UPDATE);
    } while (true);
    
}

void *singleEnemyHandler(void *arg)
{
    bool nowGenerated = false;
    short enemyID= *((short*)arg);

    pthread_mutex_lock(&semDifficult);
    GameRules rules = getRules(difficult);
    pthread_mutex_unlock(&semDifficult);

    randomSeed();

    pthread_t proj; Projectile newBorn; 

    Enemy myself;

    do {
        pthread_mutex_lock(&semAllEnemies);         // scarica in locale
        myself = allEnemies[enemyID];
        pthread_mutex_unlock(&semAllEnemies);

        if(myself.genTime == 0)
        {
            if(myself.shot == 0 && nowGenerated == false)
            {      
                newBorn.x = myself.x+3; newBorn.y = myself.y+2;
                newBorn.ID = myself.ID;

                pthread_create(&proj, NULL, singleEnemyProjectileHandler, (void *)&newBorn.ID);

                newBorn.PTID = proj;

                pthread_mutex_lock(&semEmenyProjectiles);
                enemyProjectiles[newBorn.ID].x = newBorn.x;
                enemyProjectiles[newBorn.ID].y = newBorn.y;
                enemyProjectiles[newBorn.ID].ID = newBorn.ID;
                enemyProjectiles[newBorn.ID].PTID = proj;
                pthread_mutex_unlock(&semEmenyProjectiles);

                pthread_mutex_lock(&semEnemyProjectilesAlive);
                enemyProjectilesAlive[newBorn.ID]=true;
                pthread_mutex_unlock(&semEnemyProjectilesAlive);
                nowGenerated = true;
            }
            else
            {
                nowGenerated = false;
            }
        }
        usleep(FRAME_UPDATE);
    } while (true);
}

void *singleEnemyProjectileHandler(void *arg)
{
    short limit = ROWS_PER_MAP + ROWS_PER_BLOCK - 1; // 1 per il bordo inferiore
    short updates = 0;
    short id = *((short*)arg);
    bool continua = true;

    pthread_mutex_lock(&semDifficult);
    GameRules rules = getRules(difficult);
    pthread_mutex_unlock(&semDifficult);

    do{
        pthread_mutex_lock(&semEmenyProjectiles);
        if(updates != 0 && updates % enemyProjectiles[id].speed == 0)
        {
            if(enemyProjectiles[id].y < limit)
            {                
                enemyProjectiles[id].y++;
            }
            else
            {
                continua=false;
            }
        }
        pthread_mutex_unlock(&semEmenyProjectiles);
        
        updates++;
        usleep(FRAME_UPDATE);
    } while (continua);
    pthread_exit(NULL);
}

void *singleCrocodileHandler(void *arg1)
{
    CrocPos input = *((CrocPos *) arg1);
    
    short myRow = input.row;
    short myID = input.ID;
    short update = 1;

    Crocodile myself;

    pthread_mutex_lock(&riverMutex[myRow]);
    myself = river[myRow].crocs[myID];
    river[myRow].crocs[myID].PTID = pthread_self();
    pthread_mutex_unlock(&riverMutex[myRow]);

    do{
        if(update % myself.speed == 0)
        {
            // scarica aggiornamenti
            pthread_mutex_lock(&riverMutex[myRow]);
            myself = river[myRow].crocs[myID];
            pthread_mutex_unlock(&riverMutex[myRow]);

            if(myself.x != STOPPED_CROCODILE && myself.y != STOPPED_CROCODILE) // se esisto ancora
            {
                if(myself.direction) // se deve andare a destra
                {
                    pthread_mutex_lock(&riverMutex[myRow]);
                    river[myRow].crocs[myID].x++;
                    if(river[myRow].crocs[myID].x >= COLUMNS_PER_MAP + 1)
                    {
                        river[myRow].crocs[myID].x = STOPPED_CROCODILE;
                        river[myRow].crocs[myID].y = STOPPED_CROCODILE;
                    }
                    pthread_mutex_unlock(&riverMutex[myRow]);
                }
                else
                {
                    pthread_mutex_lock(&riverMutex[myRow]);
                    river[myRow].crocs[myID].x--;
                    if(river[myRow].crocs[myID].x <= 0 - CROCODILE_COLUMNS)
                    {
                        river[myRow].crocs[myID].x = STOPPED_CROCODILE;
                        river[myRow].crocs[myID].y = STOPPED_CROCODILE;
                    }
                    pthread_mutex_unlock(&riverMutex[myRow]);
                }

                pthread_mutex_lock(&riverMutex[myRow]);
                if(river[myRow].crocs[myID].splash > 0)
                    river[myRow].crocs[myID].splash--;
                if(river[myRow].crocs[myID].splash == 0)
                {
                    river[myRow].crocs[myID].x = STOPPED_CROCODILE;
                    river[myRow].crocs[myID].y = STOPPED_CROCODILE;
                }
                pthread_mutex_unlock(&riverMutex[myRow]);
            }
        }
        update++;
        usleep(FRAME_UPDATE);
    } while (true);
    
}


/*********************\
*  FUNZIONI UTILITA'  *
\*********************/

void easyKill(pthread_t PTID)
{
    pthread_cancel(PTID);
    pthread_join(PTID, NULL);
}

short computeY(short n)
{
    short y = n * ROWS_PER_BLOCK;
    y += (ROWS_PER_BLOCK * (1 + LILY_PADS_ROWS + RIVERSIDE_ROWS)); // considera la scoreaboard, le tane e le righe del fiume.
    return y;
}

short reverseComputeY(short y)
{
    return ((y - (y % ROWS_PER_BLOCK ) ) / ROWS_PER_BLOCK ) - LILY_PADS_ROWS - RIVERSIDE_ROWS - 1; // 1 per la scoreboard
}

Crocodile buildCrocodile(short x, short y, short direction, short speed, short splashP)
{
    Crocodile croc;
    croc.x = x;
    croc.y = y;
    croc.direction = direction;
    croc.speed = speed;
    croc.splash = GOOD_CROC_FLAG;
    if(splashP)
    {
        short prob = rand() % splashP;
        if(prob == 0)
        {
            short min = ((CROCODILE_COLUMNS) + 5) * speed;
            short max = (((CROCODILE_COLUMNS * 2) + COLUMNS_PER_MAP)) * speed ;
            croc.splash = randomNumber(min, max);
        }
    }
    return croc;
}

void randomSeed()
{
    pthread_t ptid = pthread_self();

    struct timespec CurrentTime;

    clock_gettime(CLOCK_REALTIME, &CurrentTime);

    srand(time(NULL) % ptid * CurrentTime.tv_nsec);
    // prendiamo il numero di secondi trascorsi dal 1° gennaio 1970, facciamo il modulo per il pid e moltiplichiamo i nanosecondi correnti
}

short randomNumber(short min, short max)
{
    randomSeed();
    short n = min + (rand() % (max - min + 1));
    return n;
}

short spawnCrocodile(short lane, short x,short y )
{
    pthread_mutex_lock(&semDifficult);
    GameRules rules = getRules(difficult);
    pthread_mutex_unlock(&semDifficult);

    short splash = rules.BadCrocodile;
    
    pthread_mutex_lock(&riverMutex[lane]);
    for(short i = 0; i < MAX_CROCODILE_PER_ROW; i++)
    {
        if(river[lane].crocs[i].x == STOPPED_CROCODILE && river[lane].crocs[i].y == STOPPED_CROCODILE)
        {
            river[lane].crocs[i].x = x;
            river[lane].crocs[i].y = y;
            river[lane].crocs[i].direction = river[lane].direction;
            river[lane].crocs[i].speed = river[lane].speed;
            river[lane].crocs[i].splash = GOOD_CROC_FLAG;
            if(splash)
            {
                short prob = rand() % splash;
                if(prob == 0)
                {
                    short min = ((CROCODILE_COLUMNS) + 5) * river[lane].speed;
                    short max = (((CROCODILE_COLUMNS * 2) + COLUMNS_PER_MAP)) * river[lane].speed ;
                    river[lane].crocs[i].splash = randomNumber(min, max);
                }
            }    
            break;      
        }
    }
    pthread_mutex_unlock(&riverMutex[lane]);
}

void riverSpeeds()
{
    pthread_mutex_lock(&semDifficult);
    GameRules rules = getRules(difficult);
    pthread_mutex_unlock(&semDifficult);
    river[0].speed = (rand() % 2) + rules.speed;
    river[1].speed = (rand() % 2) + rules.speed;

    for(short i = 2; i<RIVER_ROWS;)
    {
        short r = (rand() % 2) + rules.speed;
        if(river[i - 2].speed != river[i - 1].speed)
        {
            river[i].speed = r;
            i++;
        }
        else if(r != river[i-1].speed)
        {
            river[i].speed = r;
            i++;
        } 
    }
}

void riversDirections()
{
    river[0].direction = (rand() % 2);

    for(short i = 1; i < RIVER_ROWS; i++)
    {
        if(river[i-1].direction) // se e' 1
            river[i].direction = 0;
        else
            river[i].direction = 1;
    }
}

short crocodileSpace()
{
    short min = (short) CROC_SPACE_MIN;
    short max = (short) CROC_SPACE_MAX;
    return randomNumber(min, max);
}

void setCrocSpeeds()
{
    for(short i = 0; i<RIVER_ROWS;i++)
    {
        for(short c = 0; c<MAX_CROCODILE_PER_ROW;c++)
        {
            pthread_mutex_lock(&riverMutex[i]);
            river[i].crocs[c].speed=river[i].speed;
            pthread_mutex_unlock(&riverMutex[i]);
        }
    }
}

void resetCrocCord()
{
    for(short i = 0; i<RIVER_ROWS;i++)
    {
        for(short c = 0; c<MAX_CROCODILE_PER_ROW;c++)
        {
            pthread_mutex_lock(&riverMutex[i]);
            river[i].crocs[c].x=STOPPED_CROCODILE;
            river[i].crocs[c].y=STOPPED_CROCODILE;
            pthread_mutex_unlock(&riverMutex[i]);
        }
    }
}

void newCrocodileScene(RiverRow rivRow[],short spawnTimers[])
{
    resetCrocCord();
    riverSpeeds();
    riversDirections();
    for(short i = 0; i < RIVER_ROWS; i++)
    {
        if(rivRow[i].direction) // se la direzione e' destra
        {
            short min = COLUMNS_PER_MAP - (COLUMNS_PER_BLOCK * 5);
            short max = COLUMNS_PER_MAP - COLUMNS_PER_BLOCK;
            short currentX = randomNumber(min, max);

            spawnCrocodile(i,currentX,computeY(i));// Genera il thread di un coccodrillo

            short generated = 1;
            do{
                currentX -= crocodileSpace();  // spazio tra un coccodrillo e l'altro
                currentX -= CROCODILE_COLUMNS; // la x e' la prima colonna
                spawnCrocodile(i,currentX,computeY(i));// Genera il thread di un coccodrillo
                generated++;
            }while(currentX - CROCODILE_COLUMNS > COLUMNS_PER_BLOCK && generated < 3);

            spawnTimers[i] = ((CROCODILE_COLUMNS + crocodileSpace()) - currentX) * rivRow[i].speed;
        }
        else
        {
            short min = COLUMNS_PER_BLOCK;
            short max = COLUMNS_PER_BLOCK * 5;
            short currentX = randomNumber(min, max);

            spawnCrocodile(i,currentX,computeY(i));// Genera il thread di un coccodrillo

            short generated = 1;
            do
            {
                currentX += crocodileSpace(); // spazio tra un coccodrillo e l'altro
                currentX += CROCODILE_COLUMNS; // la x e' la prima colonna

                spawnCrocodile(i,currentX,computeY(i));// Genera il thread di un coccodrillo
                generated++;
            } while (currentX + CROCODILE_COLUMNS > (COLUMNS_PER_MAP - COLUMNS_PER_BLOCK) && generated < 3);
            
            currentX += CROCODILE_COLUMNS;

            spawnTimers[i] = ((CROCODILE_COLUMNS + crocodileSpace()) - (COLUMNS_PER_MAP - currentX)) * rivRow[i].speed;
        }
    }
    setCrocSpeeds();
}

short yToRowNumber(short y)
{
    return ((y - (y % ROWS_PER_BLOCK ) ) / ROWS_PER_BLOCK ) - LILY_PADS_ROWS;
}

void setToFalse(bool array[], short size)
{
    for(short i = 0; i < size; i++)
        array[i] = false;
}

void newEnemiesScene(short rowsY[], Enemy allEnemies[])
{
    for(short i = 0; i < MAX_ENEMIES;)
    {
        allEnemies[i].x = randomNumber(1, COLUMNS_PER_MAP-ENEMY_COLUMNS-1);
        allEnemies[i].y = rowsY[rand() % RIVERSIDE_ROWS];
        allEnemies[i].shot = randomNumber(30, 100);
        allEnemies[i].ID = i;
        allEnemies[i].genTime = randomNumber(0, 30); // da 0 a 1 secondi

        bool collided = false;
        for(short c = 0; c < i; c++)
        {
            collided = enemyEnemyCD(allEnemies[i].x, allEnemies[i].y, allEnemies[c].x, allEnemies[c].y);
            if(collided)
            break;
        }

        if(collided == false)
            i++;
    }
}

void spawnEnemy(short enemyID)
{
    pthread_t enemy;
    pthread_mutex_lock(&semAllEnemies);
    pthread_create(&enemy, NULL, singleEnemyHandler, (void *)&allEnemies[enemyID].ID);    
    allEnemies[enemyID].PTID=enemy;
    pthread_mutex_unlock(&semAllEnemies);
}

void logChat(char *msg)
{
    FILE *fp = fopen("debug.txt", "a");
    fprintf(fp, "%s", msg);
    fprintf(fp, "\n");
    fclose(fp);
}
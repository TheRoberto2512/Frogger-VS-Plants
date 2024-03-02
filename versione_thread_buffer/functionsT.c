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
#include "buffers.h"

extern volatile short difficult;                            extern pthread_mutex_t semDifficult;
extern GameUpdates currentGame;                             extern pthread_mutex_t semCurses;

extern Buffer mainBuffer;

extern volatile bool frogAtStart;                           extern pthread_mutex_t semFrogger;
extern volatile bool frogProj[MAX_FROG_PROJ];               extern pthread_mutex_t semFrogProj;

short spawnTimers[RIVER_ROWS] = {32000, 32000, 32000, 32000, 32000, 32000, 32000, 32000};

extern short WhatShouldIDo[RIVER_ROWS][MAX_CROCS];          extern pthread_mutex_t crocActions;
// 0 se il coccodrillo esiste
// 1 se deve killarsi da solo
// 2 se deve diventare buono
// -1 dopo che si e' killato (bisogna fare il pthread_join)

/********************\
*  FUNZIONI THREAD  *
\********************/

void *frogHandler(void *arg)
{
    Frog frogger;
    frogger.x = (BLOCK_PER_MAP_ROWS / 2) * COLUMNS_PER_BLOCK +1; // x iniziale (centro mappa)
    frogger.y = ROWS_PER_MAP - 1; // y iniziale (ultima riga)
    
    pthread_mutex_lock(&semDifficult);
    GameRules rules = getRules(difficult);
    pthread_mutex_unlock(&semDifficult);  

    bool update; int ch; bool fStart = false;
    do {
        update = false;
        
        pthread_mutex_lock(&semFrogger);
        fStart = frogAtStart;
        pthread_mutex_unlock(&semFrogger);

        if(fStart == true)
        {
            pthread_mutex_lock(&semFrogger);
            frogAtStart = false;
            pthread_mutex_unlock(&semFrogger);

            frogger.x = (BLOCK_PER_MAP_ROWS / 2) * COLUMNS_PER_BLOCK +1; // x iniziale (centro mappa)
            frogger.y = ROWS_PER_MAP - 1; // y iniziale (ultima riga)

            sem_wait(&mainBuffer.sem_free_space);
            pthread_mutex_lock(&mainBuffer.mutex);
            if(mainBuffer.index < BUFFER_SIZE)
            {
                mainBuffer.data[mainBuffer.index].type = 'F';
                mainBuffer.data[mainBuffer.index].value.frogger = frogger;
                mainBuffer.index++;
            }
            pthread_mutex_unlock(&mainBuffer.mutex);
        }
        else
        {
            pthread_mutex_lock(&semCurses); ch = getch(); pthread_mutex_unlock(&semCurses);
            if(ch != ERR && ch != KEY_MOUSE) // se c'e' stato un input non proveniente dal mouse
            {
                switch(ch) //qui dentro modifichiamo x e y
                {
                    case 'd':
                    case 'D':
                    case KEY_RIGHT:
                        update = true;
                        frogger.x += COLUMNS_PER_BLOCK;
                        if (frogger.x > COLUMNS_PER_MAP - COLUMNS_PER_BLOCK+1)
                        {
                            frogger.x = COLUMNS_PER_MAP - COLUMNS_PER_BLOCK+1;
                        }
                        break;

                    case 'a':
                    case 'A':
                    case KEY_LEFT:
                        update = true;
                        frogger.x -= COLUMNS_PER_BLOCK;
                        if (frogger.x < 1)
                        {
                            frogger.x = 1;
                        }
                        break;

                    case 'w':
                    case 'W':
                    case KEY_UP:
                        update = true;
                        frogger.y -= ROWS_PER_BLOCK;
                        if(frogger.y < SCOREBOARD_ROWS)
                        {
                            frogger.y = SCOREBOARD_ROWS;
                        }
                        break;

                    case 's':
                    case 'S':
                    case KEY_DOWN:
                        update = true;
                        frogger.y += ROWS_PER_BLOCK;
                        if (frogger.y > (ROWS_PER_MAP - 3))
                        {
                            frogger.y = ROWS_PER_MAP - 1;
                        }
                        break;

                    case ' ': // generazione di un proietile
                        if(rules.enemies)
                        {
                            pthread_mutex_lock(&semFrogProj);
                            for(short p = 0; p < MAX_FROG_PROJ; p++)
                            {
                                if(frogProj[p] == false)
                                {
                                    static Projectile newBorn;
                                    newBorn.ID = p;
                                    newBorn.x = frogger.x + 3;
                                    newBorn.y = frogger.y - 1;
                                    newBorn.speed = rules.speed;
                                    newBorn.fromFrog = true;
                                    pthread_t A;
                                    pthread_create(&A, NULL, singleFrogProjectileHandler, (void*)&newBorn);
                                    frogProj[p] = true;
                                    break;
                                }
                            }
                            pthread_mutex_unlock(&semFrogProj);
                        }
                        break;
                }
                if(update)
                {
                    sem_wait(&mainBuffer.sem_free_space);
                    pthread_mutex_lock(&mainBuffer.mutex);
                    if(mainBuffer.index < BUFFER_SIZE)
                    {
                        mainBuffer.data[mainBuffer.index].type = 'F';
                        mainBuffer.data[mainBuffer.index].value.frogger = frogger;
                        mainBuffer.index++;
                    }
                    pthread_mutex_unlock(&mainBuffer.mutex);
                }
            } 
        }

        usleep(FRAME_UPDATE); 
    } while (true);
}

void *singleFrogProjectileHandler(void *arg)
{
    Projectile myself = *((Projectile*)arg);

    myself.PTID = pthread_self();

    short updates = 0;

    do{
        if(updates != 0 && updates % myself.speed == 0)
        {
            if(myself.y >= 3)
            {
                myself.y--;
                sem_wait(&mainBuffer.sem_free_space);
                pthread_mutex_lock(&mainBuffer.mutex);
                if(mainBuffer.index < BUFFER_SIZE)
                {
                    mainBuffer.data[mainBuffer.index].type = 'P';
                    mainBuffer.data[mainBuffer.index].value.proj = myself;
                    mainBuffer.index++;
                }
                pthread_mutex_unlock(&mainBuffer.mutex);
            }
            else
            {
                break;
            }
        }
        updates++;
        usleep(FRAME_UPDATE);
    } while (true);

    pthread_exit(NULL);
}

void *mainManager(void *args)
{
    // GESTIONE DELLA PARTITA CORRENTE
    currentGame.lives = LIVES; currentGame.score = 0; bool keepGenerating = true;

    pthread_mutex_lock(&semDifficult);
    GameRules rules = getRules(difficult);
    pthread_mutex_unlock(&semDifficult);   

    // VARIABILI PER LE ENTITA' 
    Frog frogger;
    frogger.x = (BLOCK_PER_MAP_ROWS / 2) * COLUMNS_PER_BLOCK +1;    // x iniziale (centro mappa)
    frogger.y = ROWS_PER_MAP - 1;                                   // y iniziale (ultima riga)

    Crocodile collidedCroc; collidedCroc.PTID = 0;                  // per salvare il coccodrillo su cui la rana sale
    short xDiff; short oldFrogX; short xFDiff;

    // FLAG BOOLEANE PER GESTIRE LA PARTITA
    bool endManche = false; bool keepPlaying = true;                                 

    short fps = 0, seconds = 0;                                     // per gestire il numero di aggiornamenti (fps) e di secondi passati (seconds)
    short DebugLine = 0;                                            // per la stampa delle schermate aggiuntive di debug
    short minRow = ROWS_PER_MAP - 1;                                // salva la riga piu' alta raggiunta dalla rana per attribuire i punti

    short yDanger = -1;                                             // coordinata dove verra' stampato il segnale d'allerta
    short dirDanger = -1;                                           // direzione dove verra' stampato il segnale d'allerta
    bool printDanger = false;                                       // FLAG per gestire la stampa del segnale d'allerta

    bool emptyLilyPads[5] = {true, true, true, true, true};
    double r = BLOCK_PER_MAP_ROWS / 5;
    short lilyPadsX[5] = { -1, -1, -1, -1, -1};

    randomSeed();

    for(short i = 0; i < 5; i++)
    {
        lilyPadsX[i] = COLUMNS_PER_BLOCK * (1 + (i * r));
    }

    // PRE-PARTITA: Coccodrilli
    Crocodile allCrocs[RIVER_ROWS][MAX_CROCS];
    bool aliveCrocs[RIVER_ROWS][MAX_CROCS];

    for(short r = 0; r < RIVER_ROWS; r++)
    {
        for(short c = 0; c < MAX_CROCS; c++)
        {
            aliveCrocs[r][c] = false;
            WhatShouldIDo[r][c]  = 0;
        }
    }

    short spawns[2] = {COLUMNS_PER_MAP, 1-CROCODILE_COLUMNS};
    short directions[RIVER_ROWS];                                                               // salva le direzioni delle corsie
    short speeds[RIVER_ROWS];                                                                   // salva le velocita' delle corsie
    short spawnTimers[RIVER_ROWS] = {32000, 32000, 32000, 32000, 32000, 32000, 32000, 32000};   // inizializzati a valori altissimi
    randomSeed();

    directions[0] = rand() % 2; // puo' essere 0 o 1

    newCrocodileScene(speeds, directions, spawnTimers, &rules);

    // PRE-PARTITA: Nemici
    Enemy allEnemies[MAX_ENEMIES];          // salva tutti i nemici correnti
    bool aliveEnemies[MAX_ENEMIES];         // true quando il nemico corrispondente e' vivo
    pthread_t A;
    short rowsY[RIVERSIDE_ROWS];            // salva le coordinate Y delle righe

    if(rules.enemies)
    {
        rowsY[0] = (LILY_PADS_ROWS * ROWS_PER_BLOCK) + SCOREBOARD_ROWS;
        for(short r = 1; r < RIVERSIDE_ROWS; r++)
        {   
            rowsY[r] = rowsY[r-1] + ROWS_PER_BLOCK;
        }

        newEnemiesScene(rowsY, allEnemies);
        for(short i = 0; i < MAX_ENEMIES; i++)
        {
            aliveEnemies[i] = true;
        }
    }


    // PRE-PARTITA: Proiettili
    Projectile enemyProjectiles[MAX_ENEMIES];
    bool printEnemyProjectiles[MAX_ENEMIES];
    Projectile frogProjectiles[MAX_FROG_PROJ];
    bool printFrogProjectiles[MAX_FROG_PROJ];

    setToFalse(printEnemyProjectiles, MAX_ENEMIES);
    setToFalse(printFrogProjectiles, MAX_FROG_PROJ);

    // INIZIO PARTITA
    do{
        if(fps < 5 && seconds == 0) // evita casi di schermo che non si aggiorna 
        {
            pthread_mutex_lock(&semCurses); clear(); customBorder(0, 0, COLUMNS_PER_MAP + 2, ROWS_PER_MAP + SCOREBOARD_ROWS, true); pthread_mutex_unlock(&semCurses);
        } 

        // INIZIO STAMPE
        pthread_mutex_lock(&semCurses);
        customBorder(0, 0, COLUMNS_PER_MAP + 2, ROWS_PER_MAP + SCOREBOARD_ROWS, true);             // stampa i bordi
        printScoreBoard(currentGame.lives, currentGame.score, (rules.time - seconds), FULL_TIME);  // aggiorna la scoreboard
        printMap(true, emptyLilyPads, (fps == 0 && seconds == 0) ? true : false);                  // stampa la mappa
        pthread_mutex_unlock(&semCurses);

        // LETTURE
        pthread_mutex_lock(&mainBuffer.mutex);
        if(mainBuffer.index > 0)
        {
            for (short i = 0; i < mainBuffer.index; i++)
            {
                switch(mainBuffer.data[i].type)
                {   
                    case 'F':
                        frogger = mainBuffer.data[i].value.frogger;
                        break;

                    case 'C':
                        Crocodile croc = mainBuffer.data[i].value.croc;
                        allCrocs[croc.row][croc.ID] = croc;
                        aliveCrocs[croc.row][croc.ID] = true;
                        break;
                    
                    case 'E':
                        Enemy en = mainBuffer.data[i].value.enemy;
                        allEnemies[en.ID] = en;
                        aliveEnemies[en.ID] = true;
                        break;

                    case 'P':
                        Projectile proj = mainBuffer.data[i].value.proj;
                        if(proj.fromFrog)
                        {
                            pthread_mutex_lock(&semFrogProj);
                            frogProj[proj.ID] = true;
                            pthread_mutex_unlock(&semFrogProj);
                            frogProjectiles[proj.ID] = proj;
                            printFrogProjectiles[proj.ID] = true;
                        }
                        else
                        {
                            enemyProjectiles[proj.ID] = proj;
                            printEnemyProjectiles[proj.ID] = true;
                        }
                        break;

                } 
                sem_post(&mainBuffer.sem_free_space);
            }
            mainBuffer.index = 0;
        }
        pthread_mutex_unlock(&mainBuffer.mutex);

        // COLLISIONI
        bool frogEnPrjsCollided = false;                                                                        // proiettili nemici - rana
        for(short f = 0; f < MAX_ENEMIES && !frogEnPrjsCollided; f++)    
        {
            if(printEnemyProjectiles[f])
            {
                frogEnPrjsCollided = frogEnemyProjCD(frogger.x, frogger.y, enemyProjectiles[f].x, enemyProjectiles[f].y);
                if(frogEnPrjsCollided && !GODMODE)
                {
                    currentGame.lives = currentGame.lives - 1; endManche = true;
                    break;
                }
            }
        }

        // COLLISIONI PROIETTILI RANA - nemici / coccodrilli / proiettili nemici
        bool twoProjectilesCollided = false;                    // proiettile rana - proiettile nemico
        bool frogPrjsEnemiesCollided = false;                   // proeittile rana - nemico
        bool frogPrjsCrocodileCollided = false;                 // proiettile rana - coccodrillo
        for(short fr = 0; fr < MAX_FROG_PROJ; fr++)
        {
            if(printFrogProjectiles[fr]) // se il proiettile esiste
            {
                short ROW = yToRowNumber(frogProjectiles[fr].y); // calcolo la sua riga

                if(ROW >= 1 && ROW <= RIVERSIDE_ROWS) 
                {
                    // controlliamo la collisione tra i proiettili rana e i nemici solo se l'altezza dei proiettili rientra nelle righe oltre il fiume
                    for(short e = 0; e < MAX_ENEMIES && !frogPrjsEnemiesCollided; e++)
                    {
                        if(allEnemies[e].genTime == 0 && aliveEnemies[e]) // se il nemico e' gia' stato completamente generato
                        {
                            frogPrjsEnemiesCollided = enemyFrogProjCD(allEnemies[e].x, allEnemies[e].y, frogProjectiles[fr].x, frogProjectiles[fr].y);
                            if(frogPrjsEnemiesCollided)
                            {
                                currentGame.score = currentGame.score + ENEMY_KILLED;                 // aggiungiamo i punti

                                pthread_mutex_lock(&mainBuffer.mutex);
                                easyKill(allEnemies[e].PTID); 
                                pthread_mutex_unlock(&mainBuffer.mutex);

                                aliveEnemies[e] = false;
                                            
                                pthread_mutex_lock(&mainBuffer.mutex);
                                easyKill(frogProjectiles[fr].PTID);
                                pthread_mutex_unlock(&mainBuffer.mutex);            
                                printFrogProjectiles[fr] = false;

                                pthread_mutex_lock(&semFrogProj);         
                                frogProj[fr] = false;                      
                                pthread_mutex_unlock(&semFrogProj);
                            }
                        }
                    }

                } else if(ROW > RIVERSIDE_ROWS && ROW <= (RIVERSIDE_ROWS+RIVER_ROWS))
                {

                }

                for(short en = 0; en < MAX_ENEMIES && !twoProjectilesCollided; en++)             
                {
                    if(printEnemyProjectiles[en])
                    {
                        twoProjectilesCollided = frogProjectileEnemyProjectileCollisionDetector(frogProjectiles[fr].x, frogProjectiles[fr].y, enemyProjectiles[en].x, enemyProjectiles[en].y);
                        if(twoProjectilesCollided)
                        {
                            pthread_mutex_lock(&mainBuffer.mutex);
                            easyKill(frogProjectiles[fr].PTID);
                            pthread_mutex_unlock(&mainBuffer.mutex);            
                            printFrogProjectiles[fr] = false; 

                            pthread_mutex_lock(&semFrogProj);         
                            frogProj[fr] = false;                      
                            pthread_mutex_unlock(&semFrogProj);

                            pthread_mutex_lock(&mainBuffer.mutex);
                            easyKill(enemyProjectiles[en].PTID);
                            pthread_mutex_unlock(&mainBuffer.mutex);
                            printEnemyProjectiles[en] = false;
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
        }
        else if(frogRow >= 1 && frogRow <= RIVERSIDE_ROWS) // se la rana e' all'altezza dell'argine superiore
        {
            for(short e = 0; e < MAX_ENEMIES && !frogEnemyCollided; e++)          
            {
                if(allEnemies[e].genTime == 0 && aliveEnemies[e]) 
                {
                    frogEnemyCollided = frogEnemyCD(frogger.x, frogger.y, allEnemies[e].x, allEnemies[e].y);
                }
            }
            if(frogEnemyCollided && !GODMODE)
            {
                currentGame.lives = currentGame.lives - 1; endManche = true;
            }
        }
        else if(frogRow > RIVERSIDE_ROWS && frogRow <= (RIVERSIDE_ROWS+RIVER_ROWS)) // se la rana e' all'altezza del fiume
        {

        }

        // STAMPA TUTTI I COCCODRILLI
        for(short r = 0; r < RIVER_ROWS; r++)
        {
            for(short c = 0; c < MAX_CROCS; c++)
            {
                if(aliveCrocs[r][c])
                {
                    bool stillAlive = true;

                    if(allCrocs[r][c].direction) // se e' 1 va a destra
                    {
                        if(allCrocs[r][c].x >= COLUMNS_PER_MAP + 1) // se e' appena uscito dallo schermo
                            stillAlive = false;
                    }
                    else // se e' 0 va a sinistra
                    {
                        if(allCrocs[r][c].x <= 0 - CROCODILE_COLUMNS) // se e' appena uscito dallo schermo
                            stillAlive = false;
                    }

                    if(stillAlive)
                    {
                        if(allCrocs[r][c].splash == GOOD_CROC_FLAG)
                        {
                            printCrocodile(allCrocs[r][c].x, allCrocs[r][c].y, allCrocs[r][c].direction);
                        }
                        else
                        {
                            if(allCrocs[r][c].splash <= 0)
                            {
                                aliveCrocs[r][c] = false;
                                pthread_mutex_lock(&mainBuffer.mutex);
                                easyKill(allCrocs[r][c].PTID);
                                pthread_mutex_unlock(&mainBuffer.mutex);
                            }
                            else
                                printBadCrocodile(allCrocs[r][c].x, allCrocs[r][c].y, allCrocs[r][c].direction);
                        }
                    }

                }
            }
        }

        // STAMPA TUTTI I NEMICI E RELATIVI PROIETTILI
        for(short e = 0; e < MAX_ENEMIES; e++)
        {   
            if(aliveEnemies[e])
            {
                pthread_mutex_lock(&semCurses);
                printEnemy(allEnemies[e].x, allEnemies[e].y, allEnemies[e].genTime);
                pthread_mutex_unlock(&semCurses);
            }
            if(printEnemyProjectiles[e])
            {
                if(enemyProjectiles[e].y >= (ROWS_PER_MAP + ROWS_PER_BLOCK - 1))
                {
                    pthread_mutex_lock(&mainBuffer.mutex);
                    easyKill(enemyProjectiles[e].PTID);
                    pthread_mutex_unlock(&mainBuffer.mutex);
                    printEnemyProjectiles[e] = false;
                }
                else
                {
                    pthread_mutex_lock(&semCurses);
                    printProjectile(enemyProjectiles[e].x, enemyProjectiles[e].y, false);
                    pthread_mutex_unlock(&semCurses);
                }
            }   
        }

        pthread_mutex_lock(&semCurses);
        printFrog(frogger.x, frogger.y);
        pthread_mutex_unlock(&semCurses);

        // STAMPA I PROIETTILI DELLA RANA
        for(short p = 0; p < MAX_FROG_PROJ; p++)
        {
            if(printFrogProjectiles[p])
            {
                if(frogProjectiles[p].y < 3)
                {
                    pthread_mutex_lock(&mainBuffer.mutex);
                    easyKill(frogProjectiles[p].PTID);
                    pthread_mutex_unlock(&mainBuffer.mutex);
                    printFrogProjectiles[p] = false;
                    pthread_mutex_lock(&semFrogProj);
                    frogProj[p] = false;
                    pthread_mutex_unlock(&semFrogProj);
                }
                else
                {
                    pthread_mutex_lock(&semCurses);
                    printProjectile(frogProjectiles[p].x, frogProjectiles[p].y, true);
                    pthread_mutex_unlock(&semCurses);
                }
            }
        }

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
                pthread_mutex_unlock(&semCurses);
                DebugLine += (2 + 1 + 1); // 2 (bordi) + 1 (righe) + 1 (spazio)
            }
            if(FROG_PROJECTILES_DEBUG)
            {
                pthread_mutex_lock(&semCurses);
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, 2+MAX_FROG_PROJ, false);
                mvprintw(DebugLine, DEBUG_COLUMNS+1, "FR-PROJ");
                for(short s = 0; s < MAX_FROG_PROJ; s++)
                    if(printFrogProjectiles[s])
                        mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "%03d : %03d", frogProjectiles[s].x, frogProjectiles[s].y);
                    else
                        mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "  false  ");
                DebugLine += 2 + (MAX_FROG_PROJ) + 1;  // 2 (bordi) + MAX_FROG_PROJ (righe) + 1 (spazio)
                pthread_mutex_unlock(&semCurses);
            }
            if(RIVER_DEBUG)
            {
                pthread_mutex_lock(&semCurses);
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, RIVER_ROWS+2, false);
                mvprintw(DebugLine, DEBUG_COLUMNS+2, "RIVER");
                
                for(short r = 0; r < RIVER_ROWS; r++)
                {
                    short nCroc = 0;
                    for(short c = 0; c < MAX_CROCS; c++)
                    {
                        if(aliveCrocs[r][c])
                            nCroc++;
                    }
                    if(nCroc <= 0)
                        CHANGE_COLOR(RED_DEBUG);
                    mvprintw(DebugLine+1+r, DEBUG_COLUMNS, " [%d] : %-2d", r, nCroc);
                    CHANGE_COLOR(DEFAULT);
                }
                DebugLine += 2 + (RIVER_ROWS) + 1;
                pthread_mutex_unlock(&semCurses);
            }
            if(ENEMIES_DEBUG)
            {
                pthread_mutex_lock(&semCurses);
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, MAX_ENEMIES+2, false);
                mvprintw(DebugLine, DEBUG_COLUMNS+1, "ENEMIES");
                for(short s = 0; s < MAX_ENEMIES; s++)
                    mvprintw(DebugLine+s+1, DEBUG_COLUMNS, "%03d : %03d", allEnemies[s].x, allEnemies[s].y); 
                pthread_mutex_unlock(&semCurses); 
                DebugLine += 2 + MAX_ENEMIES + 1; // 2 (bordi) + MAX_ENEMIES (righe) + 1 (spazio)
            }
            if(ENEMIES_PROJECTILES_DEBUG)
            {
                pthread_mutex_lock(&semCurses);
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, MAX_ENEMIES+2, false);
                mvprintw(DebugLine, DEBUG_COLUMNS+1, "EN-PROJ");
                for(short s = 0; s < MAX_ENEMIES; s++)
                    if(printEnemyProjectiles[s])
                        mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "%03d : %03d", enemyProjectiles[s].x, enemyProjectiles[s].y);
                    else
                        mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "  false  ");
                pthread_mutex_unlock(&semCurses);
                DebugLine += 2 + MAX_ENEMIES + 1; // 2 (bordi) + MAX_ENEMIES (righe) + 1 (spazio)
            }
            if(COLLISION_DEBUG)
            {
                pthread_mutex_lock(&semCurses);
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, 7, false);
                mvprintw(DebugLine, DEBUG_COLUMNS, "COLLISION");
                if(false)   // FROG - ENEMIES
                {
                    CHANGE_COLOR(RED_DEBUG);
                    mvprintw(DebugLine+1, DEBUG_COLUMNS, "  true   ");
                    CHANGE_COLOR(DEFAULT);
                } else {
                    CHANGE_COLOR(GREEN_DEBUG);
                    mvprintw(DebugLine+1, DEBUG_COLUMNS, "  false  ");
                    CHANGE_COLOR(DEFAULT);
                }
                    
                if(frogEnPrjsCollided)  // FROG - ENEMY PROJECTILES
                {
                    CHANGE_COLOR(RED_DEBUG);
                    mvprintw(DebugLine+2, DEBUG_COLUMNS, "  true   ");
                    CHANGE_COLOR(DEFAULT);
                } else {
                    CHANGE_COLOR(GREEN_DEBUG);
                    mvprintw(DebugLine+2, DEBUG_COLUMNS, "  false  ");
                    CHANGE_COLOR(DEFAULT);
                }  

                if(false)  // FROG - CROCODILE
                {
                    CHANGE_COLOR(GREEN_DEBUG);
                    mvprintw(DebugLine+3, DEBUG_COLUMNS, "%03d : %03d", collidedCroc.x, collidedCroc.y);
                    CHANGE_COLOR(DEFAULT);
                } else {
                    short fRow = yToRowNumber(frogger.y);
                    if(fRow > RIVERSIDE_ROWS && fRow <= (RIVERSIDE_ROWS+RIVER_ROWS))
                        CHANGE_COLOR(RED_DEBUG);
                    else
                        CHANGE_COLOR(GREEN_DEBUG);
                    mvprintw(DebugLine+3, DEBUG_COLUMNS, "  false  ");
                    CHANGE_COLOR(DEFAULT);
                } 

                if(twoProjectilesCollided)  // FROG PROJECTILES - ENEMY PROJECTILES
                {
                    CHANGE_COLOR(RED_DEBUG);
                    mvprintw(DebugLine+4, DEBUG_COLUMNS, "  true   ");
                    CHANGE_COLOR(DEFAULT);
                } else {
                    CHANGE_COLOR(GREEN_DEBUG);
                    mvprintw(DebugLine+4, DEBUG_COLUMNS, "  false  ");
                    CHANGE_COLOR(DEFAULT);
                } 

                if(frogPrjsEnemiesCollided)  // ENEMY - FROG PROJECTILES
                {
                    CHANGE_COLOR(GREEN_DEBUG);
                    mvprintw(DebugLine+5, DEBUG_COLUMNS, "  true   ");
                    CHANGE_COLOR(DEFAULT);
                } else {
                    CHANGE_COLOR(RED_DEBUG);
                    mvprintw(DebugLine+5, DEBUG_COLUMNS, "  false  ");
                    CHANGE_COLOR(DEFAULT);
                } 

                if(COLLISION_DEBUG_INFO)
                {
                    mvprintw(DebugLine+1, DEBUG_COLUMNS + 11, "FROG - ENEMIES");
                    mvprintw(DebugLine+2, DEBUG_COLUMNS + 11, "FROG - ENEMY PROJECTILES");
                    mvprintw(DebugLine+3, DEBUG_COLUMNS + 11, "FROG - CROCODILES");
                    mvprintw(DebugLine+4, DEBUG_COLUMNS + 11, "FROG PROJECTILES - ENEMY PROJECTILES");
                    mvprintw(DebugLine+5, DEBUG_COLUMNS + 11, "ENEMY - FROG PROJECTILES");
                }
                DebugLine += (2 + 1 + 1);
                pthread_mutex_unlock(&semCurses);
            }
        }  

        // GENERA NUOVI NEMICI DOPO LA LORO MORTE
        if(fps % 10 == 0 && fps > 0 && rules.enemies)
        {
            for(short e = 0; e < MAX_ENEMIES; e++)
            {
                if(aliveEnemies[e] == false)
                {
                    bool collided = false;
                    do{
                        collided = false;
                        allEnemies[e].x = randomNumber(1, COLUMNS_PER_MAP-ENEMY_COLUMNS-1);
                        allEnemies[e].y = rowsY[rand() % RIVERSIDE_ROWS];
                        allEnemies[e].shot = randomNumber(30, 100);
                        allEnemies[e].ID = e;
                        allEnemies[e].genTime = randomNumber(15, 90); // da mezzo a tre secondi

                        for(short en = 0; en < MAX_ENEMIES; en++)
                        {
                            if(en != e)
                                collided = enemyEnemyCD(allEnemies[e].x, allEnemies[e].y, allEnemies[en].x, allEnemies[en].y);
                            if(collided)
                                break;
                        }

                    } while (collided);

                    pthread_t A;
                    pthread_create(&A, NULL, singleEnemyHandler, (void*)&allEnemies[e]);
                    aliveEnemies[e] = true;
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
            endManche = false; seconds = 0; fps = 0; minRow = ROWS_PER_MAP - 1;

            pthread_mutex_lock(&mainBuffer.mutex);
            for(short b = 0; b < BUFFER_SIZE; b++)
            {
                mainBuffer.data[b].type = 'Z';
            }
            pthread_mutex_unlock(&mainBuffer.mutex);

            pthread_mutex_lock(&semFrogger);
            frogAtStart = true;
            pthread_mutex_unlock(&semFrogger);

            frogger.x = (BLOCK_PER_MAP_ROWS / 2) * COLUMNS_PER_BLOCK +1; // x iniziale (centro mappa)
            frogger.y = ROWS_PER_MAP - 1; // y iniziale (ultima riga)

            // NEMICI E PROIETTILI
            for(short en = 0; en < MAX_ENEMIES; en++)
            {
                if(aliveEnemies[en])
                {
                    pthread_mutex_lock(&mainBuffer.mutex);
                    easyKill(allEnemies[en].PTID); 
                    pthread_mutex_unlock(&mainBuffer.mutex);
                    aliveEnemies[en] = false;
                }
                if(printEnemyProjectiles[en])
                {
                    pthread_mutex_lock(&mainBuffer.mutex);
                    easyKill(enemyProjectiles[en].PTID);
                    pthread_mutex_unlock(&mainBuffer.mutex);
                    printEnemyProjectiles[en] = false;
                }
            }

            // COCCODRILLI
            pthread_mutex_lock(&mainBuffer.mutex);
            for(short r = 0; r < RIVER_ROWS; r++)
            {
                for(short c = 0; c < MAX_CROCS; c++)
                {
                    if(aliveCrocs[r][c])
                    {
                        WhatShouldIDo[r][c] = 1;
                        aliveCrocs[r][c] = false;
                    }
                }
            }
            pthread_mutex_unlock(&mainBuffer.mutex);
            
            newEnemiesScene(rowsY, allEnemies);
            for(short i = 0; i < MAX_ENEMIES; i++)
            {
                aliveEnemies[i] = true;
            }
        }

        // UPDATE SCHERMO E TEMPO
        pthread_mutex_lock(&semCurses); refresh(); pthread_mutex_unlock(&semCurses);        
        fps++;                          // aggiorna e incrementa fps        
        if(fps != 0 && fps % 30 == 0)   // ogni 30 update passa un secondo
            seconds++;

        usleep(FRAME_UPDATE);           // riposa

    } while(keepPlaying);
}

void *singleEnemyHandler(void *arg)
{
    pthread_mutex_lock(&semDifficult);
    GameRules rules = getRules(difficult);
    pthread_mutex_unlock(&semDifficult); 

    Enemy myself = *((Enemy*)arg);

    myself.PTID = pthread_self();

    randomSeed();

    short speed = rules.speed;
    bool newBorn = true;

    do {
        if(myself.genTime == 0)
        {
            if(newBorn)
            {
                newBorn = false;
                sem_wait(&mainBuffer.sem_free_space);
                pthread_mutex_lock(&mainBuffer.mutex);
                if(mainBuffer.index < BUFFER_SIZE)
                {
                    mainBuffer.data[mainBuffer.index].type = 'E';
                    mainBuffer.data[mainBuffer.index].value.enemy = myself;
                    mainBuffer.index++;
                }
                pthread_mutex_unlock(&mainBuffer.mutex);
            }
            if(myself.shot == 0)
            {
                static Projectile proj;
                static pthread_t AAA;
                proj.ID = myself.ID;
                proj.speed = speed; proj.fromFrog = false;
                proj.x = myself.x+3; proj.y = myself.y+2;

                pthread_create(&AAA, NULL, singleEnemyProjectileHandler, (void*)&proj);

                myself.shot = randomNumber(30 * 4, 30 * 8);
            }
            else
            {
                myself.shot--;
            }
        }
        else
        {
            myself.genTime--;
        }

        usleep(FRAME_UPDATE);
    } while (true);
}

void *singleEnemyProjectileHandler(void *arg)
{
    Projectile myself = *((Projectile*)arg);

    myself.PTID = pthread_self();

    short limit = ROWS_PER_MAP + ROWS_PER_BLOCK - 1; // 1 per il bordo inferiore
    short updates = 0;

    do{
        if(updates != 0 && updates % myself.speed == 0)
        {
            if(myself.y < limit)
            {
                myself.y++;
                sem_wait(&mainBuffer.sem_free_space);
                pthread_mutex_lock(&mainBuffer.mutex);
                if(mainBuffer.index < BUFFER_SIZE)
                {
                    mainBuffer.data[mainBuffer.index].type = 'P';
                    mainBuffer.data[mainBuffer.index].value.proj = myself;
                    mainBuffer.index++;
                }
                pthread_mutex_unlock(&mainBuffer.mutex);
            }
            else
            {
                break;
            }
        }
        updates++;
        usleep(FRAME_UPDATE);
    } while (true);

    pthread_exit(NULL);
}

void *singleCrocodileHandler(void *arg1)
{
    bool keepGoing = true; int aliveTime = 1;
    Crocodile me = *((Crocodile *) arg1);
    me.PTID = pthread_self();

    sem_wait(&mainBuffer.sem_free_space);
    pthread_mutex_lock(&mainBuffer.mutex);
    if(mainBuffer.index < BUFFER_SIZE)
    {
        mainBuffer.data[mainBuffer.index].type = 'C';
        mainBuffer.data[mainBuffer.index].value.croc = me;
        mainBuffer.index++;
    }
    pthread_mutex_unlock(&mainBuffer.mutex);
    aliveTime++;
    usleep(FRAME_UPDATE);

    do{
        if(aliveTime % me.speed == 0) // se deve aggiornarsi
        {
            if(me.direction) // se e' 1 va a destra
            {
                me.x++;
                if(me.x >= COLUMNS_PER_MAP + 1) // se e' appena uscito dallo schermo
                    keepGoing = false;
            }
            else // se e' 0 va a sinistra
            {
                me.x--;
                if(me.x <= 0 - CROCODILE_COLUMNS) // se e' appena uscito dallo schermo
                    keepGoing = false;
            }
        }
        if(me.splash != GOOD_CROC_FLAG)
        {
            me.splash--;
            if(me.splash == 0)
            {
                keepGoing = false;
            }
        }
        switch(WhatShouldIDo[me.row][me.ID])
        {
            case 0:
                sem_wait(&mainBuffer.sem_free_space);
                pthread_mutex_lock(&mainBuffer.mutex);
                if(mainBuffer.index < BUFFER_SIZE)
                {
                    mainBuffer.data[mainBuffer.index].type = 'C';
                    mainBuffer.data[mainBuffer.index].value.croc = me;
                    mainBuffer.index++;
                }
                pthread_mutex_unlock(&mainBuffer.mutex);
                break;

            case 1:
                WhatShouldIDo[me.row][me.ID] = -1;
                pthread_exit(NULL);
                break;

            case 2:
                if(me.splash != GOOD_CROC_FLAG)
                {
                    me.splash != GOOD_CROC_FLAG;
                }
                sem_wait(&mainBuffer.sem_free_space);
                pthread_mutex_lock(&mainBuffer.mutex);
                if(mainBuffer.index < BUFFER_SIZE)
                {
                    mainBuffer.data[mainBuffer.index].type = 'C';
                    mainBuffer.data[mainBuffer.index].value.croc = me;
                    mainBuffer.index++;
                }
                pthread_mutex_unlock(&mainBuffer.mutex);
                break;
        }

        aliveTime++;
        usleep(FRAME_UPDATE);
    } while(keepGoing);

    // ...
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

Crocodile buildCrocodile(short x, short y, short direction, short speed, short splashP, short row, short ID)
{
    randomSeed();
    Crocodile croc;
    croc.row = row;
    croc.ID = ID;
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

    clock_gettime(CLOCK_REALTIME, &CurrentTime); // l'IDE potrebbe riportare un errore ma la costante e' inclusa in time.h

    srand(time(NULL) % ptid * CurrentTime.tv_nsec);
    // prendiamo il numero di secondi trascorsi dal 1° gennaio 1970, facciamo il modulo per il pid e moltiplichiamo i nanosecondi correnti
}

short randomNumber(short min, short max)
{
    randomSeed();
    short n = min + (rand() % (max - min + 1));
    return n;
}

void spawnCrocodile(Crocodile croc)
{
    static Crocodile lastFive[5];
    static short lastOne = 0;
    lastFive[lastOne] = croc;
    
    pthread_t PTID;
    pthread_create(&PTID, NULL, singleCrocodileHandler, (void*)&lastFive[lastOne]);

    lastOne++;
    if(lastOne == 5)
        lastOne = 0;
}

void riverSpeeds(short speeds[], short rulesSpeed)
{
    speeds[0] = (rand() % 2) + rulesSpeed;
    speeds[1] = (rand() % 2) + rulesSpeed;
    for(short i = 2; i < RIVER_ROWS;)
    {
        short r = (rand() % 2) + rulesSpeed;
        if(speeds[i - 2] != speeds[i - 1])
        {
            speeds[i] = r;
            i++;
        }
        else if(r != speeds[i - 1])
        {
            speeds[i] = r;
            i++;
        }      
    }
}

short crocodileSpace()
{
    short min = (short) CROC_SPACE_MIN;
    short max = (short) CROC_SPACE_MAX;
    return randomNumber(min, max);
}

void newCrocodileScene(short speeds[], short directions[], short spawnTimers[], GameRules *rules)
{
    riverSpeeds(speeds, rules->speed);
    for(short i = 1; i < RIVER_ROWS; i++)
    {
        if(directions[i-1]) // se e' 1
            directions[i] = 0;
        else
            directions[i] = 1;
    }
    
    for(int i = 0; i < RIVER_ROWS; i++)
    {
        if(directions[i]) // se la direzione e' destra
        { 
            short min = COLUMNS_PER_MAP - (COLUMNS_PER_BLOCK * 5);
            short max = COLUMNS_PER_MAP - COLUMNS_PER_BLOCK;
            short currentX = randomNumber(min, max);
            
            short generated = 0;
            spawnCrocodile(buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile, i, generated));
            generated++;            

            do{
                currentX -= crocodileSpace(); // spazio tra un coccodrillo e l'altro
                currentX -= CROCODILE_COLUMNS; // la x e' la prima colonna
                spawnCrocodile(buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile, i, generated));
                generated++;
            } while( currentX - CROCODILE_COLUMNS > COLUMNS_PER_BLOCK && generated < 2 );

            spawnTimers[i] = ((CROCODILE_COLUMNS + crocodileSpace()) - currentX) * speeds[i];
        }
        else // se la direzione e' sinistra
        {   
            short min = COLUMNS_PER_BLOCK;
            short max = COLUMNS_PER_BLOCK * 5;
            short currentX = randomNumber(min, max);
            
            short generated = 0;
            spawnCrocodile(buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile, i, generated));
            generated++;

            do{
                currentX += crocodileSpace(); // spazio tra un coccodrillo e l'altro
                currentX += CROCODILE_COLUMNS; // la x e' la prima colonna
                spawnCrocodile(buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile, i, generated));
                generated++;
            } while( currentX + CROCODILE_COLUMNS > (COLUMNS_PER_MAP - COLUMNS_PER_BLOCK) && generated < 2 );

            currentX += CROCODILE_COLUMNS;

            spawnTimers[i] = ((CROCODILE_COLUMNS + crocodileSpace()) - (COLUMNS_PER_MAP - currentX)) * speeds[i];
        }
    }
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
    for(short i = 0; i < MAX_ENEMIES; i++)
    {
        pthread_t A;
        pthread_create(&A, NULL, singleEnemyHandler, (void*)&allEnemies[i]);
    }

}

void logChat(char *msg)
{
    FILE *fp = fopen("debug.txt", "a");
    fprintf(fp, "%s", msg);
    fprintf(fp, "\n");
    fclose(fp);
}
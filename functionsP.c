#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#include "functionsP.h"
#include "sprite.h"
#include "structs.h"
#include "settings.h"
#include "crocList.h"
#include "collisionDetector.h"


/*********************\
*  FUNZIONI PROCESSI  *
\*********************/

void frogHandler(int frogToMain[], int mainToFrog[], int frogToFPH[])
{
    Frog frogger;
    frogger.x = (BLOCK_PER_MAP_ROWS / 2) * COLUMNS_PER_BLOCK +1; // x iniziale (centro mappa)
    frogger.y = ROWS_PER_MAP - 1; // y iniziale (ultima riga)

    write(frogToMain[WRITE], &frogger, sizeof(frogger));

    usleep(FRAME_UPDATE); 
    bool update; int ch;
    do {
        read(mainToFrog[READ], &frogger, sizeof(frogger)); // prova a leggere le nuove coordinate della rana (in caso sia su un coccodrillo)
        update = false;
        if((ch = getch()) != ERR && ch != KEY_MOUSE) // se c'e' stato un input non proveniente dal mouse
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
                    write(frogToFPH[WRITE], &frogger, sizeof(frogger)); // comunichiamo la posizione corrente al processo che gestira' i proiettili
                    break;
            }

            if(update) // se la rana si e' mossa in qualche direzione
                write(frogToMain[WRITE], &frogger, sizeof(frogger));  // comunica le nuove coordinate
        } 
        usleep(FRAME_UPDATE); 
    } while (true);
}

void frogProjectilesHandler(int frogToFPH[], int PHToMain[], int mainToFPH[], short speed)
{
    Frog frogger; // per le letture dalla rana (la rana comunica le sue coordinate)
    short updatesCounter = 0; 
    Projectile frogProjectiles[MAX_FROG_PROJ]; Projectile readed;
    bool doProjectileExist[MAX_FROG_PROJ];

    for(short i = 0; i < MAX_FROG_PROJ; i++)    // operazione preliminare per assicurarci siano tutti impostati a false
        doProjectileExist[i] = false;

    ssize_t bytesR = -1;
    do{
        bytesR = read(frogToFPH[READ], &frogger, sizeof(Frog));

        // SE LA RANA HA CHIESTO DI GENERARE UN PROIETTILE
        if(bytesR > -1)
        {
            for(short i = 0; i < MAX_FROG_PROJ; i++)
            {
                if(doProjectileExist[i] == false)
                {
                    doProjectileExist[i] = true;

                    frogProjectiles[i].PID = 0;             // perche' e' la rana (i proiettili nemici useranno 1-2-3)
                    frogProjectiles[i].ID = i;              // salviamo un ID provvisorio
                    frogProjectiles[i].x = frogger.x + 3;
                    frogProjectiles[i].y = frogger.y - 1;
                    frogProjectiles[i].speed = speed;
                    break;
                }
            }
        }

        // LEGGE DAL MAIN PER CONTROLLARE SE QUALCHE PROIETTILE E' ENTRATO IN COLLISIONE (E QUINDI SCOMPARSO)
        do{
            short justRead;
            bytesR = read(mainToFPH[READ], &justRead, sizeof(justRead));
            if(bytesR > -1)
            {
                doProjectileExist[justRead] = false;
            }
        } while (bytesR > -1);

        // FA AVANZARE I PROIETTILI IN BASE ALLA LORO VELOCITA' E COMUNICA LE NUOVE COORDINATE
        for(short i = 0; i < MAX_FROG_PROJ; i++)
        {
            if(doProjectileExist[i])
            {
                if(updatesCounter % speed == 0)
                {
                    frogProjectiles[i].y -= 1;
                    if(frogProjectiles[i].y < 3)
                        doProjectileExist[i] = false; // smette di esistere
                    else
                        write(PHToMain[WRITE], &frogProjectiles[i], sizeof(Projectile));
                }
            }
        }

        usleep(FRAME_UPDATE);

    } while (true);
}

void mainManager(GameRules *rules, GameUpdates *currentGame, int frogToMain[], int mainToFrog[], int crocToMain[], int mainToRivH[], int PHToMain[], int mainToFPH[], int enHToMain[], int mainToEnH[])
{
    // ARRAY E VARIABILI PER LETTURE E "CACHE" 
    Frog frogger; CrocList cList; Crocodile croc;   // per salvare la rana, la lista dei coccodrilli e un coccodrillo dove effettuare le letture
    Enemy allEnemies[MAX_ENEMIES]; Enemy en;        // per salvare tutti i nemici e un nemico dove effettuare le letture

    Projectile frogPrjs[MAX_FROG_PROJ];             // per salvare tutti i proiettili della rana
    bool printFProj[MAX_FROG_PROJ];                 // FLAG per decidere se stampare o no i proiettili rana
    Projectile readed;                              // per effettuare le letture di tipo proiettile
    Projectile enemPrjs[MAX_ENEMIES];               // per salvare tutti i proiettili dei nemici (2 ciascuno)
    bool printEnProj[MAX_ENEMIES];                  // FLAG per decidere se stampare o no i proiettili nemici                                     

    short fps = 0, seconds = 0;                     // per gestire il numero di aggiornamenti (fps) e di secondi passati (seconds)
    short DebugLine = 0;                            // per la stampa delle schermate aggiuntive di debug
    short minRow = ROWS_PER_MAP - 1;                // salva la riga piu' alta raggiunta dalla rana per attribuire i punti
    ssize_t bytes_read = -1;                        // variabile da usare per verificare le letture dalle pipes non bloccanti

    // OPERAZIONI PRELIMINARI
    for(short i = 0; i < RIVER_ROWS; i++)           // evita SIGSEGV impostando i puntatori a NULL
        cList.lanes[i] = NULL;

    for(short p = 0; p < MAX_FROG_PROJ; p++)           
        printFProj[p] = false;

    for(short p = 0; p < MAX_ENEMIES; p++)
        printEnProj[p] = false;

    bool tane[5] = {true, true, true, true, true};

    do{
        // LEGGE LA POSIZIONE DELLA RANA
        bytes_read = read(frogToMain[READ], &frogger, sizeof(frogger)); // read(...) restituisce il numero di byte letti (-1 se non legge nulla)
        if(bytes_read != -1 && frogger.y < minRow)
        {
            minRow = frogger.y;
            currentGame->score += ROW_UP;
        }

        if(fps < 3 && seconds == 0) // evita casi di schermo che non si aggiorna 
        {
            clear(); customBorder(0, 0, COLUMNS_PER_MAP + 2, ROWS_PER_MAP + SCOREBOARD_ROWS, true);
        }                        
        
        //customBorder(0, 0, COLUMNS_PER_MAP + 2, ROWS_PER_MAP + SCOREBOARD_ROWS, true);
        printScoreBoard(currentGame->lives, currentGame->score, rules->time - seconds, FULL_TIME);  // aggiorna la scoreboard
        printMap(true, tane, (fps == 0 && seconds == 0) ? true : false);                            // stampa la mappa

        // LEGGE TUTTI I COCCODRILLI
        bytes_read = -1;
        do{
            bytes_read = read(crocToMain[READ], &croc, sizeof(croc));  
            if(bytes_read != -1)
                Update(&cList, reverseComputeY(croc.y), croc, fps);
        } while (bytes_read != -1);             
        DeleteUnnecessary(&cList, fps); // elimina i coccodrilli che non ricevono piu' aggiornamenti (usciti dallo schermo)
        printList(&cList);              // stampa i coccodrilli

        // LEGGE TUTTI I NEMICI
        bytes_read = -1;
        do{
            bytes_read = read(enHToMain[READ], &en, sizeof(Enemy));
            if(bytes_read != -1)
            {
                allEnemies[en.ID] = en;
            }
        } while (bytes_read != -1); 

        // LEGGE TUTTI I PROIETTILI
        bytes_read = -1;
        do{
            bytes_read = read(PHToMain[READ], &readed, sizeof(Projectile));
            if(bytes_read != -1)
            {
                if(readed.PID == 0) // se proviene dalla rana
                {
                    frogPrjs[readed.ID] = readed;
                    printFProj[readed.ID] = true;
                }
                else // se proviene da una pianta
                {
                    enemPrjs[readed.ID] = readed;
                    printEnProj[readed.ID] = true;
                }
            }
        } while (bytes_read != -1);

        /* TEST PER IL DE-SPAWN DEI PROIETTILI RANA
        for(short p = 0; p < MAX_FROG_PROJ; p++)
        {
            if(printFProj[p])
            {
                if(frogPrjs[p].y == 15)
                {
                    write(mainToFPH[WRITE], &frogPrjs[p].ID, sizeof(frogPrjs[p].ID));
                    printFProj[p] = false;
                }
            }
        } */
        
        // STAMPA TUTTI I NEMICI
        for(short e = 0; e < MAX_ENEMIES; e++)
        {
            printEnemy(allEnemies[e].x, allEnemies[e].y, allEnemies[e].genTime);
            if(allEnemies[e].genTime > 0)
                allEnemies[e].genTime--;
        }

        // STAMPA TUTTI I PROIETTILI DELLA RANA (SE DENTRO L'AREA DI GIOCO)
        for(short p = 0; p < MAX_FROG_PROJ; p++)
        {
            if(printFProj[p])
            {
                printProjectile(frogPrjs[p].x, frogPrjs[p].y, true);

                if(frogPrjs[p].y <= 3)
                    printFProj[p] = false;
            }
        }

        // STAMPA TUTTI I PROIETTILI NEMICI
        for(short f = 0; f < MAX_ENEMIES; f++)
        {
            if(printEnProj[f])
            {
                if(enemPrjs[f].y >= (ROWS_PER_MAP + ROWS_PER_BLOCK - 1))
                {
                    printEnProj[f] = false;
                    easyKill(enemPrjs[f].PID);
                }  
                else
                    printProjectile(enemPrjs[f].x, enemPrjs[f].y, false);

            }
        }

        // STAMPA LA RANA
        printFrog(frogger.x, frogger.y);

        // COLLISIONI
        bool frogEnPrjsCollided = false;                                                                        // proiettili nemici - rana
        for(short f = 0; f < (MAX_ENEMIES) && !frogEnPrjsCollided; f++)    
        {
            if(printEnProj[f])
            {
                frogEnPrjsCollided = frogEnemyProjCD(frogger.x, frogger.y, enemPrjs[f].x, enemPrjs[f].y);
            }
        } 

        bool frogEnemyCollided = false;                                                                         // rana - nemici
        for(short e = 0; e < MAX_ENEMIES && !frogEnemyCollided; e++)          
        {
            frogEnemyCollided = frogEnemyCD(frogger.x, frogger.y, allEnemies[e].x, allEnemies[e].y);
        }

        bool twoProjectilesCollided = false;
        bool frogPrjsEnemiesCollided = false;
        for(short fr = 0; fr < MAX_FROG_PROJ; fr++)                                                             // proiettili rana - proiettili nemici && proiettili rana - nemici
        {
            if(printFProj[fr])
            {
                for(short en = 0; en < (MAX_ENEMIES ) && !twoProjectilesCollided; en++)
                {
                    if(printEnProj[en])
                    {
                        twoProjectilesCollided = frogProjectileEnemyProjectileCollisionDetector(frogPrjs[fr].x, frogPrjs[fr].y, enemPrjs[en].x, enemPrjs[en].y);
                        if(twoProjectilesCollided)
                        {
                            write(mainToFPH[WRITE], &frogPrjs[fr].ID, sizeof(frogPrjs[fr].ID));     // comunichiamo al FrogProjectilesHandler la scomparsa del proiettile rana
                            printFProj[fr] = false;                                                 // disattiviamo la flag per stamparlo
                            easyKill(enemPrjs[en].PID);                                             // killiamo il processo proiettile
                            printEnProj[en] = false;                                                // disattiviamo la plag per stamparlo
                        }
                    }
                }

                for(short e = 0; e < MAX_ENEMIES && !frogPrjsEnemiesCollided; e++)
                {
                    if(allEnemies[e].genTime == 0)
                        frogPrjsEnemiesCollided = enemyFrogProjCD(allEnemies[e].x, allEnemies[e].y, frogPrjs[fr].x, frogPrjs[fr].y);
                        if(frogPrjsEnemiesCollided)
                        {
                            write(mainToEnH[WRITE], &allEnemies[e].ID, sizeof(allEnemies[e].ID));   // comunichiamo la morte all'EnemiesHandler
                            easyKill(allEnemies[e].PID);                                            // killiamo il processo nemico
                            write(mainToFPH[WRITE], &frogPrjs[fr].ID, sizeof(frogPrjs[fr].ID));     // comunichiamo al FrogProjectilesHandler la scomparsa del proiettile rana
                            printFProj[fr] = false;                                                 // disattiviamo la flag per stamparlo
                        }
                }
            }
        }

        // SCHERMATE DI DEBUG
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
            customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, MAX_FROG_PROJ+2, false);
            mvprintw(DebugLine, DEBUG_COLUMNS+1, "FR-PROJ");
            for(short s = 0; s < MAX_FROG_PROJ; s++)
                if(printFProj[s])
                    mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "%03d : %03d", frogPrjs[s].x, frogPrjs[s].y);
                else
                    mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "  false  ");
            DebugLine += 2 + (MAX_FROG_PROJ) + 1;  // 2 (bordi) + MAX_FROG_PROJ (righe) + 1 (spazio)
        }
        if(ENEMIES_DEBUG)
        {
            customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, MAX_ENEMIES+2, false);
            mvprintw(DebugLine, DEBUG_COLUMNS+1, "ENEMIES");
            for(short s = 0; s < MAX_ENEMIES; s++)
                mvprintw(DebugLine+s+1, DEBUG_COLUMNS, "%03d : %03d", allEnemies[s].x, allEnemies[s].y);  
            DebugLine += 2 + MAX_ENEMIES + 1; // 2 (bordi) + MAX_ENEMIES (righe) + 1 (spazio)
        }
        if(ENEMIES_PROJECTILES_DEBUG)
        {
            customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, MAX_ENEMIES+2, false);
            mvprintw(DebugLine, DEBUG_COLUMNS+1, "EN-PROJ");
            for(short s = 0; s < MAX_ENEMIES; s++)
                if(printEnProj[s])
                    mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "%03d : %03d", enemPrjs[s].x, enemPrjs[s].y);
                else
                    mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "  false  ");
            DebugLine += 2 + MAX_ENEMIES + 1; // 2 (bordi) + MAX_ENEMIES (righe) + 1 (spazio)
        }
        if(COLLISION_DEBUG)
        {
            customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, 6, false);
            mvprintw(DebugLine, DEBUG_COLUMNS, "COLLISION");

            if(frogEnemyCollided)   // FROG - ENEMIES
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

            if(twoProjectilesCollided)  // FROG PROJECTILES - ENEMY PROJECTILES
            {
                CHANGE_COLOR(RED_DEBUG);
                mvprintw(DebugLine+3, DEBUG_COLUMNS, "  true   ");
                CHANGE_COLOR(DEFAULT);
            } else {
                CHANGE_COLOR(GREEN_DEBUG);
                mvprintw(DebugLine+3, DEBUG_COLUMNS, "  false  ");
                CHANGE_COLOR(DEFAULT);
            } 

            if(frogPrjsEnemiesCollided)  // ENEMY - FROG PROJECTILES
            {
                CHANGE_COLOR(GREEN_DEBUG);
                mvprintw(DebugLine+4, DEBUG_COLUMNS, "  true   ");
                CHANGE_COLOR(DEFAULT);
            } else {
                CHANGE_COLOR(RED_DEBUG);
                mvprintw(DebugLine+4, DEBUG_COLUMNS, "  false  ");
                CHANGE_COLOR(DEFAULT);
            } 

            if(COLLISION_DEBUG_INFO)
            {
                mvprintw(DebugLine+1, DEBUG_COLUMNS + 11, "FROG - ENEMIES");
                mvprintw(DebugLine+2, DEBUG_COLUMNS + 11, "FROG - ENEMY PROJECTILES");
                mvprintw(DebugLine+3, DEBUG_COLUMNS + 11, "FROG PROJECTILES - ENEMY PROJECTILES");
                mvprintw(DebugLine+4, DEBUG_COLUMNS + 11, "ENEMY - FROG PROJECTILES");
            }
            DebugLine += (2 + 1 + 1);
        }
        
        refresh(); fps++;               // aggiorna e incrementa fps        
        if(fps != 0 && fps % 30 == 0)   // ogni 30 update passa un secondo
            seconds++;

        usleep(FRAME_UPDATE);           // riposa

        // SE IL TEMPO E' SCADUTO
        if((rules->time-seconds) <= -1)
        {
            currentGame->lives = currentGame->lives - 1; break;
        }

    } while(true);
    
    killAll(&cList);
}

void riverHandler(int crocToMain[], int mainToRivH[], GameRules *rules)
{
    short spawns[2] = {COLUMNS_PER_MAP, 1-CROCODILE_COLUMNS};   // [i] dove deve spawnare il coccodrillo se la sua direzione e' i
    
    short directions[RIVER_ROWS];                                                               // salva le direzioni delle corsie
    short speeds[RIVER_ROWS];                                                                   // salva le velocita' delle corsie
    short spawnTimers[RIVER_ROWS] = {30000, 30000, 30000, 30000, 30000, 30000, 30000, 30000};   // inizializzati a valori altissimi

    randomSeed(); // impostiamo un seme random

    riverSpeeds(speeds, rules->speed);
    directions[0] = rand() % 2; // puo' essere 0 o 1

    newCrocodileScene(crocToMain, directions, speeds, spawnTimers, rules);
        
    ssize_t bytes_read = -1;
    while (true)
    {
        do{
            Crocodile regen;
            read(mainToRivH[READ], &regen, sizeof(Crocodile));
            if(bytes_read != -1)
            {
                regen.splash = -10; // il coccodrillo diventa buono
                spawnCrocodile(crocToMain, regen); // viene generato un nuovo processo per il coccodrillo appena diventato buono
            }
        } while(bytes_read != -1);

        for(short i = 0; i < RIVER_ROWS; i++)
        {
            if(spawnTimers[i] == 0)
            {
                spawnCrocodile(crocToMain, buildCrocodile(spawns[directions[i]], computeY(i), directions[i], speeds[i], rules->BadCrocodile));
                if(directions[i]) // se va a destra
                    spawnTimers[i] = (crocodileSpace() + (CROCODILE_COLUMNS*2)) * speeds[i];
                else
                    spawnTimers[i] = (crocodileSpace() + CROCODILE_COLUMNS) * speeds[i]; 
            }
            else
                spawnTimers[i]--;
        }

        usleep(FRAME_UPDATE);
    }
}

void singleCrocodileHandler(int crocToMain[], Crocodile me)
{
    bool keepGoing = true; int aliveTime = 1;
    me.PID = getpid();
    write(crocToMain[WRITE], &me, sizeof(me)); // scrive le sue coordinate
    usleep(FRAME_UPDATE);
    aliveTime++;

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
        if(me.splash != -10)
        {
            me.splash--;
            if(me.splash == 0)
            {
                keepGoing = false;
            }
        }
        write(crocToMain[WRITE], &me, sizeof(me)); // comunica le cordinate al manager
        aliveTime++;
        usleep(FRAME_UPDATE);

    } while (keepGoing);

    usleep(FRAME_UPDATE * 10); // va in pausa
    exit(0);
}

void enemiesHandler(int enHToMain[], int mainToEnH[], int PHToMain[], short speed)
{
    randomSeed();
    Enemy allEnemies[MAX_ENEMIES];      // salva tutti i nemici correnti
    bool aliveEnemies[MAX_ENEMIES];     // true quando il nemico corrispondente e' vivo
    short rowsY[RIVERSIDE_ROWS];        // salva le coordinate Y delle righe

    // OPERAZIONI PRELIMINARI
    rowsY[0] = (LILY_PADS_ROWS * ROWS_PER_BLOCK) + SCOREBOARD_ROWS;
    for(short r = 1; r < RIVERSIDE_ROWS; r++)
    {   
        rowsY[r] = rowsY[r-1] + ROWS_PER_BLOCK;
    }

    // GENERA I NEMICI
    for(short i = 0; i < MAX_ENEMIES;)
    {
        //if(i == 0)
            //allEnemies[i].x = 1;
        //else
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

    // CREA I PROCESSI NEMICI
    for(short p = 0; p < MAX_ENEMIES; p++)
    {
        pid_t a = fork();
        if(a == 0)
        {
            singleEnemyHandler(allEnemies[p], PHToMain, speed);
        } 
        else
            allEnemies[p].PID = a;
    }

    // COMUNICA I NEMICI AL MANAGER
    for(short k = 0; k < MAX_ENEMIES; k++)
        write(enHToMain[WRITE], &allEnemies[k], sizeof(Enemy));

    short readed; ssize_t bytes_read = -1;
    do{
        do{
            bytes_read = read(mainToEnH[READ], &readed, sizeof(short));
            if(bytes_read != -1)
            {
                bool collided = false;
                do{
                    allEnemies[readed].x = randomNumber(1, COLUMNS_PER_MAP-ENEMY_COLUMNS-1);
                    allEnemies[readed].y = rowsY[rand() % RIVERSIDE_ROWS];
                    allEnemies[readed].shot = randomNumber(30, 100);
                    allEnemies[readed].ID = readed;
                    allEnemies[readed].genTime = randomNumber(15, 90); // da mezzo a tre secondi

                    for(short e = 0; e < MAX_ENEMIES; e++)
                    {
                        if(e != readed)
                            collided = enemyEnemyCD(allEnemies[readed].x, allEnemies[readed].y, allEnemies[e].x, allEnemies[e].y);
                        if(collided)
                            break;
                    }

                } while (collided);

                pid_t a = fork();
                if(a == 0)
                {
                    singleEnemyHandler(allEnemies[readed], PHToMain, speed);
                }
                allEnemies[readed].PID = a;
                write(enHToMain[WRITE], &allEnemies[readed], sizeof(Enemy));
            }
        } while(bytes_read != -1);

        usleep(FRAME_UPDATE);
    } while(true);
}

void singleEnemyHandler(Enemy myself, int PHToMain[], short speed)
{
    randomSeed();
    do {
        if(myself.genTime == 0)
        {
            if(myself.shot == 0)
            {
                pid_t a = fork();
                if(a == 0)
                {
                    Projectile newBorn;                         

                    newBorn.x = myself.x+3; newBorn.y = myself.y+2;
                    newBorn.ID = myself.ID;
                    newBorn.speed = speed;

                    singleEnemyProjectileHandler(PHToMain, newBorn);
                }
                else
                {
                    myself.shot = randomNumber(30 * 4, 30 * 8);
                }
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

void singleEnemyProjectileHandler(int PHToMain[], Projectile myself)
{
    short limit = ROWS_PER_MAP + ROWS_PER_BLOCK - 1; // 1 per il bordo inferiore
    short updates = 0;
    myself.PID = getpid();
    do{
        if(updates != 0 && updates % myself.speed == 0)
        {
            if(myself.y < limit)
            {
                myself.y++;
                write(PHToMain[WRITE], &myself, sizeof(Projectile));
            }
        }
        updates++;
        usleep(FRAME_UPDATE);
    } while (true);
}


/*********************\
*  FUNZIONI UTILITA'  *
\*********************/

int easyKill(pid_t PID)
{
    int status;
    kill(PID, SIGTERM);
    waitpid(PID, &status, 0);
    return status;
}

short computeY(short n)
{
    short y = n * ROWS_PER_BLOCK;
    y +=  (ROWS_PER_BLOCK * (1 + 1 + RIVERSIDE_ROWS)); // considera la scoreaboard, le tane e le righe del fiume.
    return y;
}

short reverseComputeY(short n)
{
    short y = (ROWS_PER_BLOCK * (1 + 1 + RIVERSIDE_ROWS));
    y /= (n * ROWS_PER_BLOCK);
    return y;
}

Crocodile buildCrocodile(short x, short y, short direction, short speed, short splashP)
{
    Crocodile croc;
    croc.x = x;
    croc.y = y;
    croc.direction = direction;
    croc.speed = speed;
    croc.splash = -10;
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
    pid_t pid = getpid();

    struct timespec CurrentTime;

    clock_gettime(CLOCK_REALTIME, &CurrentTime);

    srand(time(NULL) % pid * CurrentTime.tv_nsec);
    // prendiamo il numero di secondi trascorsi dal 1° gennaio 1970, facciamo il modulo per il pid e moltiplichiamo i nanosecondi correnti
}

short randomNumber(short min, short max)
{
    randomSeed();
    short n = min + (rand() % (max - min + 1));
    return n;
}

short spawnCrocodile(int crocToMain[], Crocodile croc)
{
    pid_t pid = fork();

    if(pid == 0)
    {
        singleCrocodileHandler(crocToMain, croc);
    }
    else if(pid < 0)
    {
        return -1;
    }
    else // con pid > 0
    {
        return pid;
    }
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

void newCrocodileScene(int crocToMain[], short directions[], short speeds[], short spawnTimers[], GameRules *rules)
{
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
            
            spawnCrocodile(crocToMain, buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile));

            do{
                currentX -= crocodileSpace(); // spazio tra un coccodrillo e l'altro
                currentX -= CROCODILE_COLUMNS; // la x e' la prima colonna
                spawnCrocodile(crocToMain, buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile));
            } while( currentX - CROCODILE_COLUMNS > COLUMNS_PER_BLOCK);

            spawnTimers[i] = ((CROCODILE_COLUMNS + crocodileSpace()) - currentX) * speeds[i];
        }
        else // se la direzione e' sinistra
        {   
            short min = COLUMNS_PER_BLOCK;
            short max = COLUMNS_PER_BLOCK * 5;
            short currentX = randomNumber(min, max);
            
            spawnCrocodile(crocToMain, buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile));

            do{
                currentX += crocodileSpace(); // spazio tra un coccodrillo e l'altro
                currentX += CROCODILE_COLUMNS; // la x e' la prima colonna
                spawnCrocodile(crocToMain, buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile));
            } while( currentX + CROCODILE_COLUMNS > (COLUMNS_PER_MAP - COLUMNS_PER_BLOCK));

            currentX += CROCODILE_COLUMNS;

            spawnTimers[i] = ((CROCODILE_COLUMNS + crocodileSpace()) - (COLUMNS_PER_MAP - currentX)) * speeds[i];
        }

    }
}

short yToRowNumber(short y)
{
    return ((y - (y % ROWS_PER_BLOCK ) ) / ROWS_PER_BLOCK )- LILY_PADS_ROWS;
}
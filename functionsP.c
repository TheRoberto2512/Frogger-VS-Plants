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

void frogProjectilesHandler(int frogToFPH[], int PHToMain[], int mainToFPH[])
{
    short difficult = -1;

    do{
        read(mainToFPH[READ], &difficult, sizeof(difficult));
    } while(difficult == -1);
    
    GameRules rules = getRules(difficult);

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
        if(bytesR > -1 && difficult>EASY)
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
                    frogProjectiles[i].speed = rules.speed;
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
                if(updatesCounter % rules.speed == 0)
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

GameUpdates mainManager(short difficult, int frogToMain[], int mainToFrog[], int crocToMain[], int mainToRivH[], int PHToMain[], int mainToFPH[], int enHToMain[], int mainToEnH[])
{
    // GESTIONE DELLA PARTITA CORRENTE
    GameUpdates currentGame; currentGame.lives = LIVES; currentGame.score = 0;
    GameRules rules = getRules(difficult);
    
    // FLAG BOOLEANE PER GESTIRE LA PARTITA
    bool endManche = false; bool keepPlaying = true;

    // ARRAY E VARIABILI PER LETTURE E "CACHE" 
    Frog frogger; CrocList cList; Crocodile croc;   // per salvare la rana e la lista dei coccodrilli e per salvare un coccodrillo dove effettuare le letture

    Crocodile collidedCroc; collidedCroc.PID = 0;   // per salvare il coccodrillo su cui la rana sale
    short xDiff; short oldFrogX; short xFDiff;      // variabili d'appoggio per i calcoli relativi alla rana sul coccodrilllo
    short nullCounters[RIVER_ROWS];

    Enemy allEnemies[MAX_ENEMIES]; Enemy en;        // per salvare tutti i nemici e un nemico dove effettuare le letture
    bool printEnemies[MAX_ENEMIES];                 // FLAG per decidere se stampare o no i nemici

    Projectile frogPrjs[MAX_FROG_PROJ];             // per salvare tutti i proiettili della rana
    bool printFProj[MAX_FROG_PROJ];                 // FLAG per decidere se stampare o no i proiettili rana
    Projectile readed;                              // per effettuare le letture di tipo proiettile
    Projectile enemPrjs[MAX_ENEMIES];               // per salvare tutti i proiettili dei nemici (2 ciascuno)
    bool printEnProj[MAX_ENEMIES];                  // FLAG per decidere se stampare o no i proiettili nemici                                     

    short fps = 0, seconds = 0;                     // per gestire il numero di aggiornamenti (fps) e di secondi passati (seconds)
    short DebugLine = 0;                            // per la stampa delle schermate aggiuntive di debug
    short minRow = ROWS_PER_MAP - 1;                // salva la riga piu' alta raggiunta dalla rana per attribuire i punti
    ssize_t bytes_read = -1;                        // variabile da usare per verificare le letture dalle pipes non bloccanti

    short yDanger = -1;                             // coordinata dove verra' stampato il segnale d'allerta
    short dirDanger = -1;                           // direzione dove verra' stampato il segnale d'allerta
    bool printDanger = false;                       // FLAG per gestire la stampa del segnale d'allerta

    // OPERAZIONI PRELIMINARI
    for(short i = 0; i < RIVER_ROWS; i++)           
    {
        cList.lanes[i] = NULL;                      // evita SIGSEGV impostando i puntatori a NULL
        cList.counts[i] = 0;                        // ci assicuriamo che siano tutti settati a zero
        nullCounters[i] = 0;
    }        

    setToFalse(printFProj,MAX_FROG_PROJ);
    setToFalse(printEnemies,MAX_ENEMIES);
    setToFalse(printEnProj,MAX_ENEMIES);

    bool emptyLilyPads[5] = {true, true, true, true, true};
    double r = BLOCK_PER_MAP_ROWS / 5;
    short lilyPadsX[5] = { -1, -1, -1, -1, -1};

    for(short i = 0; i < 5; i++)
    {
        lilyPadsX[i] = COLUMNS_PER_BLOCK * (1 + (i * r));
    }

    do{
        // LEGGE LA POSIZIONE DELLA RANA
        bytes_read = read(frogToMain[READ], &frogger, sizeof(frogger)); // read(...) restituisce il numero di byte letti (-1 se non legge nulla)
        if(bytes_read != -1 && frogger.y < minRow)
        {
            minRow = frogger.y;
            currentGame.score += ROW_UP;
        }

        if(fps < 3 && seconds == 0) // evita casi di schermo che non si aggiorna 
        {
            clear(); customBorder(0, 0, COLUMNS_PER_MAP + 2, ROWS_PER_MAP + SCOREBOARD_ROWS, true);
        }                        
        
        // LEGGE TUTTI I COCCODRILLI
        bytes_read = -1;
        do{
            bytes_read = read(crocToMain[READ], &croc, sizeof(croc));  
            if(bytes_read != -1)
                Update(&cList, reverseComputeY(croc.y), croc, fps);
        } while (bytes_read != -1);             
        DeleteUnnecessary(&cList, fps); // elimina i coccodrilli che non ricevono piu' aggiornamenti (usciti dallo schermo)

        for(short i = 0; i < RIVER_ROWS && seconds > 1; i++)
        {
            if(cList.counts[i] >= 5)
            {
                FixLane(&cList, i);
            }

            if(cList.lanes[i] == NULL)
            {
                if(nullCounters[i] % 30 == 0)
                {
                    nullCounters[i] = 0;
                    Crocodile alert; alert.x = CROC_NOW; alert.y = i; 
                    write(mainToRivH[WRITE], &alert, sizeof(Crocodile));
                }
                else
                    nullCounters[i]++;
            }
        }

        // LEGGE TUTTI I NEMICI
        bytes_read = -1;
        do{
            bytes_read = read(enHToMain[READ], &en, sizeof(Enemy));
            if(bytes_read != -1)
            {
                allEnemies[en.ID] = en;
                printEnemies[en.ID] = true;
            }
        } while (bytes_read != -1); 

        // CHIEDE LA GENERAZIONE DI NUOVI NEMICI
        if(fps % 10 == 0 && fps > 0)
            for(short i = 0; i < MAX_ENEMIES; i++)
            {
                if(printEnemies[i] == false)
                {
                    write(mainToEnH[WRITE], &i, sizeof(i));
                    break;
                }
            }

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
        
        // COLLISIONI
        bool frogEnPrjsCollided = false;                                                                        // proiettili nemici - rana
        for(short f = 0; f < MAX_ENEMIES && !frogEnPrjsCollided; f++)    
        {
            if(printEnProj[f])
            {
                frogEnPrjsCollided = frogEnemyProjCD(frogger.x, frogger.y, enemPrjs[f].x, enemPrjs[f].y);
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
            if(printFProj[fr]) // se il proiettile esiste
            {
                short ROW = yToRowNumber(frogPrjs[fr].y); // calcolo la sua riga

                if(ROW >= 1 && ROW <= RIVERSIDE_ROWS) 
                {
                    // controlliamo la collisione tra i proiettili rana e i nemici solo se l'altezza dei proiettili rientra nelle righe oltre il fiume
                    for(short e = 0; e < MAX_ENEMIES && !frogPrjsEnemiesCollided; e++)
                    {
                        if(allEnemies[e].genTime == 0) // se il nemico e' gia' stato completamente generato
                        {
                            frogPrjsEnemiesCollided = enemyFrogProjCD(allEnemies[e].x, allEnemies[e].y, frogPrjs[fr].x, frogPrjs[fr].y);
                            if(frogPrjsEnemiesCollided)
                            {
                                currentGame.score = currentGame.score + ENEMY_KILLED;                 // aggiungiamo i punti
                                easyKill(allEnemies[e].PID);                                            // killiamo il processo nemico
                                printEnemies[e] = false;                                                 
                                write(mainToFPH[WRITE], &frogPrjs[fr].ID, sizeof(frogPrjs[fr].ID));     // comunichiamo al FrogProjectilesHandler la scomparsa del proiettile rana
                                printFProj[fr] = false;                                                 // disattiviamo la flag per stamparlo
                            }
                        }
                    }

                } else if(ROW > RIVERSIDE_ROWS && ROW <= (RIVERSIDE_ROWS+RIVER_ROWS))
                {
                    ROW = ROW - (RIVERSIDE_ROWS + 1);           // calcoliamo la riga del fiume corrispondente

                    CrocElement *current = cList.lanes[ROW];    // current punta al primo elemento di quella riga

                    while(current != NULL)
                    {
                        frogPrjsCrocodileCollided = crocFrogProjCD(current->croc.x, current->croc.y, frogPrjs[fr].x, frogPrjs[fr].y);
                        if(frogPrjsCrocodileCollided) // se c'e' stata una collisione
                        {
                            write(mainToFPH[WRITE], &frogPrjs[fr].ID, sizeof(frogPrjs[fr].ID));     // comunichiamo al FrogProjectilesHandler la scomparsa del proiettile rana
                            printFProj[fr] = false;                                                 // disattiviamo la flag per stamparlo
                            if(current->croc.splash != GOOD_CROC_FLAG) // se e' cattivo
                            {
                                write(mainToRivH[WRITE], &current->croc, sizeof(croc));             // comunichiamo al RiverHandler le info sul coccodrillo
                                Kill(&cList, reverseComputeY(current->croc.y), current->croc.PID);  // lo uccidiamo (verra' rigenerato buono dal RiverHandler)
                            } 
                            break;
                        }
                        current = current->next; // passiamo al prossimo
                    }
                }

                for(short en = 0; en < MAX_ENEMIES&& !twoProjectilesCollided; en++)             
                {
                    if(printEnProj[en])
                    {
                        twoProjectilesCollided = frogProjectileEnemyProjectileCollisionDetector(frogPrjs[fr].x, frogPrjs[fr].y, enemPrjs[en].x, enemPrjs[en].y);
                        if(twoProjectilesCollided)
                        {
                            write(mainToFPH[WRITE], &frogPrjs[fr].ID, sizeof(frogPrjs[fr].ID));     // comunichiamo al FrogProjectilesHandler la scomparsa del proiettile rana
                            printFProj[fr] = false;                                                 // disattiviamo la flag per stamparlo
                            easyKill(enemPrjs[en].PID);                                             // killiamo il processo proiettile
                            printEnProj[en] = false;                                                // disattiviamo la flag per stamparlo
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
                if(allEnemies[e].genTime == 0) 
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
            frogRow -= (RIVERSIDE_ROWS + 1);

            CrocElement *current = cList.lanes[frogRow];    // current punta al primo elemento di quella riga

            while(current != NULL)
            {
                frogOnCrocodile = frogCrocodileCD(frogger.x, current->croc.x);
                if(frogOnCrocodile) // se c'e' stata una collisione con un coccodrillo
                {
                    if(collidedCroc.PID != current->croc.PID)   // se la rana e' appena salita su quel coccodrillo
                    {
                        collidedCroc = current->croc;           // salviamo i dati sul coccodrillo
                        xDiff = frogger.x - collidedCroc.x;     // differenza rana-coccodrillo
                        oldFrogX = frogger.x;                   // salviamo le coordinate della rana
                    }
                    else                                        // se la rana e' rimasta sullo stesso coccodrillo
                    {
                        bool updateFrog = false; short xDiff2;

                        if(collidedCroc.x != current->croc.x || frogger.x != oldFrogX) 
                        {
                            xDiff2 = frogger.x - current->croc.x;   // calcoliamo la differenza attuale
                            updateFrog = true;

                            short realDiff = (xDiff - xDiff2);

                            if(realDiff != 1 && realDiff != -1)         // si e' spostata anche la rana
                            {
                                if(realDiff > 1)
                                    realDiff -= COLUMNS_PER_BLOCK;
                                else if(realDiff < -1)
                                    realDiff += COLUMNS_PER_BLOCK;
                            }
                            frogger.x += realDiff; // cosi' la rana si e' mossa mantenendo la differenza iniziale
                        }

                        if(updateFrog)
                        {
                            if (frogger.x < 1)
                                frogger.x = 1;
                            else if (frogger.x > COLUMNS_PER_MAP - COLUMNS_PER_BLOCK+1)
                                frogger.x = COLUMNS_PER_MAP - COLUMNS_PER_BLOCK+1;

                            oldFrogX = frogger.x;

                            write(mainToFrog[WRITE], &frogger, sizeof(frogger));
                        }
                    }

                    collidedCroc = current->croc;               // salvataggio
                    xDiff = frogger.x - collidedCroc.x;

                    if(collidedCroc.splash > 0 && collidedCroc.splash < 30)
                    {
                        printDanger = true;
                        yDanger = collidedCroc.y;
                        dirDanger = collidedCroc.direction;
                    }
                    break;
                }
                current = current->next; // passiamo al prossimo
            }
            if(!frogOnCrocodile && !GODMODE)
            {
                collidedCroc.PID = 0;
                currentGame.lives = currentGame.lives - 1; endManche = true;
            }
        }

        // INIZIO STAMPE
        customBorder(0, 0, COLUMNS_PER_MAP + 2, ROWS_PER_MAP + SCOREBOARD_ROWS, true);              // stampa i bordi
        printScoreBoard(currentGame.lives, currentGame.score, rules.time - seconds, FULL_TIME);  // aggiorna la scoreboard
        printMap(true, emptyLilyPads, (fps == 0 && seconds == 0) ? true : false);                   // stampa la mappa
        printList(&cList);                                                                          // stampa i coccodrilli

        // STAMPA TUTTI I NEMICI
        for(short e = 0; e < MAX_ENEMIES; e++)
        {   
            if(printEnemies[e])
            {
                printEnemy(allEnemies[e].x, allEnemies[e].y, allEnemies[e].genTime);
                if(allEnemies[e].genTime > 0)
                    allEnemies[e].genTime--;
            }   
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

        // STAMPA LA RANA
        printFrog(frogger.x, frogger.y);

        // STAMPA TUTTI I PROIETTILI NEMICI
        for(short f = 0; f < MAX_ENEMIES; f++)
        {
            if(printEnProj[f])
            {
                if(seconds < 1)
                {
                    printEnProj[f] = false;
                }
                else
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
        }

        // STAMPA IL SEGNALE DI PERICOLO
        if(printDanger)
        {
            printDangerSign(dirDanger, yDanger); printDanger = false;
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
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, MAX_FROG_PROJ+2, false);
                mvprintw(DebugLine, DEBUG_COLUMNS+1, "FR-PROJ");
                for(short s = 0; s < MAX_FROG_PROJ; s++)
                    if(printFProj[s])
                        mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "%03d : %03d", frogPrjs[s].x, frogPrjs[s].y);
                    else
                        mvprintw(DebugLine+1+s, DEBUG_COLUMNS, "  false  ");
                DebugLine += 2 + (MAX_FROG_PROJ) + 1;  // 2 (bordi) + MAX_FROG_PROJ (righe) + 1 (spazio)
            }
            if(RIVER_DEBUG)
            {
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, RIVER_ROWS+2, false);
                mvprintw(DebugLine, DEBUG_COLUMNS+2, "RIVER");
                for(short r = 0; r < RIVER_ROWS; r++)
                {
                    if(cList.counts[r] == 0)
                        CHANGE_COLOR(RED_DEBUG);
                    mvprintw(DebugLine+1+r, DEBUG_COLUMNS, " [%d] : %-2d", r, cList.counts[r]);
                    if(cList.lanes[r] == NULL)
                    {
                        mvprintw(DebugLine+1+r, DEBUG_COLUMNS + 11, "  NULL  ");
                    }
                    else
                    {
                        mvprintw(DebugLine+1+r, DEBUG_COLUMNS + 11, "NOT NULL");
                    }
                    CHANGE_COLOR(DEFAULT);
                }
                DebugLine += 2 + (RIVER_ROWS) + 1;
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
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, 7, false);
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

                if(frogOnCrocodile)  // FROG - CROCODILE
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
            }
        }          

        // SE IL TEMPO E' SCADUTO
        if((rules.time-seconds) <= -1 && !GODMODE)
        {
            currentGame.lives = currentGame.lives - 1; endManche = true;
        }

        if(currentGame.lives == 0)
            keepPlaying = false;

        // COSA FARE PER RICOMINCIARE LA MANCHE
        if(endManche)
        {
            endManche = false;

            seconds = 0; fps = 0; 

            // LA RANA RITORNA ALLA POSIZIONE INIZIALE 
            frogger.x = (BLOCK_PER_MAP_ROWS / 2) * COLUMNS_PER_BLOCK +1;    // x iniziale (centro mappa)
            frogger.y = ROWS_PER_MAP - 1;                                   // y iniziale (ultima riga)
            write(mainToFrog[WRITE], &frogger, sizeof(frogger));
            minRow = ROWS_PER_MAP - 1;

            // NUOVA SCENA PER IL FIUME (TUTTI COCCODRILLI UCCISI E RIFATTI)
            Crocodile killAllFlag;
            if(keepPlaying)
            {
                killAllFlag.x = CROCS_APOCALYPSE; killAllFlag.y = CROCS_APOCALYPSE;         // nuova scena per il fiume
                write(mainToRivH[WRITE], &killAllFlag, sizeof(Crocodile));
            }
            else
            {
                killAllFlag.x = CROC_STOP; killAllFlag.y = CROC_STOP;                       // niente piu' coccodrilli
                write(mainToRivH[WRITE], &killAllFlag, sizeof(Crocodile));                  
            }

            killAll(&cList);                                                                // uccidiamo tutti i coccodrilli

            // NEMICI E PROIETTILI
            for(short en = 0; en < MAX_ENEMIES; en++)
            {
                if(printEnemies[en])                                                        // se il nemico esiste...
                    easyKill(allEnemies[en].PID);                                           // lo uccidiamo
                if(printEnProj[en])                                                         // se il proiettile esiste...
                    easyKill(enemPrjs[en].PID);                                             // uccidiamo il suo proiettile
            }
            
            short newEnemies = (short) NEW_ENEMIES_FLAG;
            if(keepPlaying)                                                             // se il gioco deve continuare
                write(mainToEnH[WRITE], &newEnemies, sizeof(newEnemies));               // chiediamo la loro rigenerazione
            
            setToFalse(printEnProj, MAX_ENEMIES);      
            setToFalse(printEnemies, MAX_ENEMIES);  
            setToFalse(printFProj, MAX_FROG_PROJ);
            
            for(short fp = 0; fp < MAX_FROG_PROJ; fp++)
                if(printFProj[fp])
                    write(mainToFPH[WRITE], &frogPrjs[fp].ID, sizeof(frogPrjs[fp].ID));  
            
        }

        // UPDATE SCHERMO E TEMPO
        refresh(); fps++;               // aggiorna e incrementa fps        
        if(fps != 0 && fps % 30 == 0)   // ogni 30 update passa un secondo
            seconds++;

        usleep(FRAME_UPDATE);           // riposa
    
    } while(keepPlaying);
    
    killAll(&cList);

    return currentGame;
}

void riverHandler(int crocToMain[], int mainToRivH[])
{
    short difficult = -1;

    do{
        read(mainToRivH[READ], &difficult, sizeof(difficult));
    } while(difficult == -1);
    
    GameRules rules = getRules(difficult);

    short spawns[2] = {COLUMNS_PER_MAP, 1-CROCODILE_COLUMNS};   // [i] dove deve spawnare il coccodrillo se la sua direzione e' i
    bool keepGenerating = true;
    short directions[RIVER_ROWS];                                                               // salva le direzioni delle corsie
    short speeds[RIVER_ROWS];                                                                   // salva le velocita' delle corsie
    short spawnTimers[RIVER_ROWS] = {32000, 32000, 32000, 32000, 32000, 32000, 32000, 32000};   // inizializzati a valori altissimi

    randomSeed(); // impostiamo un seme random

    riverSpeeds(speeds, rules.speed);
    directions[0] = rand() % 2; // puo' essere 0 o 1

    newCrocodileScene(crocToMain, directions, speeds, spawnTimers, &rules);
        
    ssize_t bytes_read = -1;
    while (true)
    {
        do{
            Crocodile regen;
            bytes_read = read(mainToRivH[READ], &regen, sizeof(Crocodile));
            if(bytes_read != -1)
            {
                if(regen.x == CROCS_APOCALYPSE && regen.y == CROCS_APOCALYPSE) // APOCALISSE DEI COCCODRILLI
                {
                    usleep(FRAME_UPDATE);
                    do{
                        bytes_read = read(mainToRivH[READ], &regen, sizeof(Crocodile));
                        if (bytes_read != -1)
                            easyKill(regen.PID);
                    } while (bytes_read != -1);

                    riverSpeeds(speeds, rules.speed);
                    directions[0] = rand() % 2; // puo' essere 0 o 1
                    newCrocodileScene(crocToMain, directions, speeds, spawnTimers, &rules);
                }
                else if(regen.x == CROC_STOP && regen.y == CROC_STOP)   // basta con la generazione
                {
                    keepGenerating = false;
                }
                else if(regen.x == CROC_NOW) 
                {
                    short row = regen.y;
                    spawnCrocodile(crocToMain, buildCrocodile(spawns[directions[row]], computeY(row), directions[row], speeds[row], rules.BadCrocodile));

                    if(directions[row]) // se va a destra
                        spawnTimers[row] = (crocodileSpace() + (CROCODILE_COLUMNS*2)) * speeds[row];
                    else
                        spawnTimers[row] = (crocodileSpace() + CROCODILE_COLUMNS) * speeds[row]; 
                }
                else
                {
                    regen.splash = GOOD_CROC_FLAG; // il coccodrillo diventa buono
                    spawnCrocodile(crocToMain, regen); // viene generato un nuovo processo per il coccodrillo appena diventato buono
                }
                regen.x = 0;
            }
        } while(bytes_read != -1);

        for(short i = 0; i < RIVER_ROWS && keepGenerating; i++)
        {
            if(spawnTimers[i] <= 0)
            {
                spawnCrocodile(crocToMain, buildCrocodile(spawns[directions[i]], computeY(i), directions[i], speeds[i], rules.BadCrocodile));
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
        if(me.splash != GOOD_CROC_FLAG)
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

void enemiesHandler(int enHToMain[], int mainToEnH[], int PHToMain[])
{
    short difficult = -1;

    do{
        read(mainToEnH[READ], &difficult, sizeof(difficult));
    } while(difficult == -1);
    
    if(difficult == EASY)
    {
        exit(0);
    }

    GameRules rules = getRules(difficult);

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
    newEnemiesScene(rowsY, allEnemies);

    // CREA I PROCESSI NEMICI
    for(short p = 0; p < MAX_ENEMIES; p++)
    {
        spawnEnemy(enHToMain, PHToMain, allEnemies, p, rules.speed);
    }

    short readed; ssize_t bytes_read = -1;
    do{
        do{
            bytes_read = read(mainToEnH[READ], &readed, sizeof(readed));
            if(bytes_read != -1)
            {
                if(readed == NEW_ENEMIES_FLAG)
                {
                    newEnemiesScene(rowsY, allEnemies);

                    for(short e = 0; e < MAX_ENEMIES; e++)
                    {
                        spawnEnemy(enHToMain, PHToMain, allEnemies, e, rules.speed);
                    }
                } else {
                    bool collided = false;
                    do{
                        collided = false;
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

                    spawnEnemy(enHToMain, PHToMain, allEnemies, readed, rules.speed);
                }
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

            short generated = 1;

            do{
                currentX -= crocodileSpace(); // spazio tra un coccodrillo e l'altro
                currentX -= CROCODILE_COLUMNS; // la x e' la prima colonna
                spawnCrocodile(crocToMain, buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile));
                generated++;
            } while( currentX - CROCODILE_COLUMNS > COLUMNS_PER_BLOCK && generated <= 3 );

            spawnTimers[i] = ((CROCODILE_COLUMNS + crocodileSpace()) - currentX) * speeds[i];
        }
        else // se la direzione e' sinistra
        {   
            short min = COLUMNS_PER_BLOCK;
            short max = COLUMNS_PER_BLOCK * 5;
            short currentX = randomNumber(min, max);
            
            spawnCrocodile(crocToMain, buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile));
            short generated = 1;

            do{
                currentX += crocodileSpace(); // spazio tra un coccodrillo e l'altro
                currentX += CROCODILE_COLUMNS; // la x e' la prima colonna
                spawnCrocodile(crocToMain, buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile));
                generated++;
            } while( currentX + CROCODILE_COLUMNS > (COLUMNS_PER_MAP - COLUMNS_PER_BLOCK) && generated <= 3 );

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
}

void spawnEnemy(int enHToMain[], int PHToMain[], Enemy allEnemies[], short enemyID, short speed)
{
    pid_t a = fork();
    if(a == 0)
    {
        singleEnemyHandler(allEnemies[enemyID], PHToMain, speed);       // chiamiamo la funzione che gestira' il processo
    }
    else if(a > 0)
    {
        allEnemies[enemyID].PID = a;                                    // salviamo il PID del nemico appena creato
        write(enHToMain[WRITE], &allEnemies[enemyID], sizeof(Enemy));   // lo salviamo dell'array con tutti i nemici
    }
}
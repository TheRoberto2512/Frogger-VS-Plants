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
#include "crocList.h"

extern volatile short difficult;                            extern pthread_mutex_t semDifficult;
extern GameUpdates currentGame;                             extern pthread_mutex_t semCurses;

extern Buffer mainBuffer;

extern volatile Frog Frogger;                               extern pthread_mutex_t semFrogger;

short spawnTimers[RIVER_ROWS] = {32000, 32000, 32000, 32000, 32000, 32000, 32000, 32000};


/********************\
*  FUNZIONI THREAD  *
\********************/

void *frogHandler(void *arg) // semCurses, semFrogger, semGenFrogProj
{
    Frog frogger;
    frogger.x = (BLOCK_PER_MAP_ROWS / 2) * COLUMNS_PER_BLOCK +1; // x iniziale (centro mappa)
    frogger.y = ROWS_PER_MAP - 1; // y iniziale (ultima riga)
    bool update;
    int ch;
    do {
        update = false;
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
        usleep(FRAME_UPDATE); 
    } while (true);
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

    CrocList cList;

    for(short i = 0; i < RIVER_ROWS; i++)           
    {
        cList.lanes[i] = NULL;                      // evita SIGSEGV impostando i puntatori a NULL
        cList.counts[i] = 0;                        // ci assicuriamo che siano tutti settati a zero
    } 

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

    // PRE-PARTITA
    short spawns[2] = {COLUMNS_PER_MAP, 1-CROCODILE_COLUMNS};
    short directions[RIVER_ROWS];                                                               // salva le direzioni delle corsie
    short speeds[RIVER_ROWS];                                                                   // salva le velocita' delle corsie
    short spawnTimers[RIVER_ROWS] = {32000, 32000, 32000, 32000, 32000, 32000, 32000, 32000};   // inizializzati a valori altissimi
    randomSeed();

    directions[0] = rand() % 2; // puo' essere 0 o 1
    newCrocodileScene(speeds, directions, spawnTimers, &rules);


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
                        Update(&cList, reverseComputeY(croc.y), croc, fps);

                } 
                sem_post(&mainBuffer.sem_free_space);
            }
            mainBuffer.index = 0;
        }
        pthread_mutex_unlock(&mainBuffer.mutex);

        DeleteUnnecessary(&cList, fps);
        pthread_mutex_lock(&semCurses);
        printList(&cList);
        pthread_mutex_unlock(&semCurses);

        pthread_mutex_lock(&semCurses);
        printFrog(frogger.x, frogger.y);
        pthread_mutex_unlock(&semCurses);

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
            if(RIVER_DEBUG)
            {
                pthread_mutex_lock(&semCurses);
                customBorder(COLUMNS_PER_MAP+SCOREBOARD_ROWS, DebugLine, DEBUG_TOP, RIVER_ROWS+2, false);
                mvprintw(DebugLine, DEBUG_COLUMNS+2, "RIVER");
                for(short r = 0; r < RIVER_ROWS; r++)
                {
                    if(cList.counts[r] == 0)
                        CHANGE_COLOR(RED_DEBUG);
                    mvprintw(DebugLine+1+r, DEBUG_COLUMNS, " [%d] : %-2d", r, cList.counts[r]);
                    CHANGE_COLOR(DEFAULT);
                }
                pthread_mutex_unlock(&semCurses);
                DebugLine += 2 + (RIVER_ROWS) + 1;
            }
        }  

        // GENERA NUOVI NEMICI DOPO LA LORO MORTE
        if(fps % 10 == 0 && fps > 0)
        {
            for(short e = 0; e < MAX_ENEMIES; e++)
            {

            }
        }

        // GENERA NUOVI COCCODRILLI
        for(short i = 0; i < RIVER_ROWS && keepGenerating; i++)
        {
            if(spawnTimers[i] <= 0)
            {
                spawnCrocodile(buildCrocodile(spawns[directions[i]], computeY(i), directions[i], speeds[i], rules.BadCrocodile));
                if(directions[i]) // se va a destra
                    spawnTimers[i] = (crocodileSpace() + (CROCODILE_COLUMNS*2)) * speeds[i];
                else
                    spawnTimers[i] = (crocodileSpace() + CROCODILE_COLUMNS) * speeds[i]; 
            }
            else
                spawnTimers[i]--;
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
    
}

void *singleEnemyProjectileHandler(void *arg)
{
    
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

Crocodile buildCrocodile(short x, short y, short direction, short speed, short splashP)
{
    randomSeed();
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

void riversDirections()
{
    /*
    river[0].direction = (rand() % 2);

    for(short i = 1; i < RIVER_ROWS; i++)
    {
        if(river[i-1].direction) // se e' 1
            river[i].direction = 0;
        else
            river[i].direction = 1;
    }*/
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
            
            spawnCrocodile(buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile));

            short generated = 1;

            do{
                currentX -= crocodileSpace(); // spazio tra un coccodrillo e l'altro
                currentX -= CROCODILE_COLUMNS; // la x e' la prima colonna
                spawnCrocodile(buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile));
                generated++;
            } while( currentX - CROCODILE_COLUMNS > COLUMNS_PER_BLOCK && generated <= 3 );

            spawnTimers[i] = ((CROCODILE_COLUMNS + crocodileSpace()) - currentX) * speeds[i];
        }
        else // se la direzione e' sinistra
        {   
            short min = COLUMNS_PER_BLOCK;
            short max = COLUMNS_PER_BLOCK * 5;
            short currentX = randomNumber(min, max);
            
            spawnCrocodile(buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile));
            short generated = 1;

            do{
                currentX += crocodileSpace(); // spazio tra un coccodrillo e l'altro
                currentX += CROCODILE_COLUMNS; // la x e' la prima colonna
                spawnCrocodile(buildCrocodile(currentX, computeY(i), directions[i], speeds[i], rules->BadCrocodile));
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

void newEnemiesScene(short rowsY[], volatile Enemy allEnemies[])
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

void logChat(char *msg)
{
    FILE *fp = fopen("debug.txt", "a");
    fprintf(fp, "%s", msg);
    fprintf(fp, "\n");
    fclose(fp);
}
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>

#include "functionsP.h"
#include "sprite.h"
#include "structs.h"
#include "settings.h"


/*********************\
*  FUNZIONI PROCESSI  *
\*********************/

void frogHandler(int frogToMain[], int mainToFrog[], int FPHToMain[])
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
                    if(frogger.y < 4)
                    {
                        frogger.y = 4;
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
                    write(FPHToMain[WRITE], &frogger, sizeof(frogger)); // comunichiamo la posizione corrente al processo che gestira' i proiettili
                    break;
            }

            if(update) // se la rana si e' mossa in qualche direzione
                write(frogToMain[WRITE], &frogger, sizeof(frogger));  // comunica le nuove coordinate
        } 
        usleep(FRAME_UPDATE); 
    } while (true);
}

void mainManager(short startTime, int frogToMain[], int mainToFrog[], int crocToMain[], int mainToRivH[])
{
    short punteggio = 0, vite = 3, fps = 0, seconds = 0;
    Frog frogger; Crocodile croc;
    bool tane[5] = {true, true, true, true, true};
    do
    {
        read(frogToMain[READ], &frogger, sizeof(frogger));

        if(fps < 3 && seconds == 0)
            clear();
        
        customBorder(0, 0, COLUMNS_PER_MAP+2, ROWS_PER_MAP + 4, true);
        printScoreBoard(vite, punteggio, startTime-seconds, FULL_TIME); 
        printMap(true, tane, (fps == 0 && seconds == 0) ? true : false);

        ssize_t bytes_read = -1;
        do{
            bytes_read = read(crocToMain[READ], &croc, sizeof(croc));
            if(bytes_read != -1)
            {
                if(croc.splash == -10)
                {
                    printCrocodile(croc.x, croc.y, croc.direction); 
                }
                else
                {
                    printBadCrocodile(croc.x, croc.y, croc.direction); 
                }
            }
        } while (bytes_read != -1);


        printFrog(frogger.x, frogger.y);
        if(FROG_DEBUG)
        {
            customBorder(COLUMNS_PER_MAP+4, 0, 11, 3, false);
            CHANGE_COLOR(DEFAULT);
            mvprintw(1, COLUMNS_PER_MAP+5, "%03d : %03d", frogger.x, frogger.y);
        }
        
        refresh();
        fps++;
        if(fps != 0 && fps % 30 == 0)
        {
            fps = 0;
            seconds++;
        }
        usleep(FRAME_UPDATE);

    } while(true);
}

void riverHandler(int crocToMain[], int mainToRivH[], GameRules rules)
{
    short spawns[2] = {COLUMNS_PER_MAP, 1-CROCODILE_COLUMNS};   // [i] dove deve spawnare il coccodrillo se la sua direzione e' i
    short directions[RIVER_ROWS];                               // salva le direzioni delle corsie
    short speeds[RIVER_ROWS];                                   // salva le velocita' delle corsie
    short spawnTimers[RIVER_ROWS] = {30000, 30000, 30000, 30000, 30000, 30000, 30000, 30000};

    randomSeed(); // impostiamo un seme random
    riverSpeeds(speeds, rules.speed);
    directions[0] = rand() % 2; // puo' essere 0 o 1

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
            
            spawnCrocodile(crocToMain, buildCrocodile(currentX, computeY(i), directions[i], speeds[i], -10));

            do{
                currentX -= crocodileSpace(); // spazio tra un coccodrillo e l'altro
                currentX -= CROCODILE_COLUMNS; // la x e' la prima colonna
                spawnCrocodile(crocToMain, buildCrocodile(currentX, computeY(i), directions[i], speeds[i], -10));
            } while( currentX - CROCODILE_COLUMNS > COLUMNS_PER_BLOCK);

            spawnTimers[i] = ((CROCODILE_COLUMNS + crocodileSpace()) - currentX) * speeds[i];
        }
        else // se la direzione e' sinistra
        {   
            short min = COLUMNS_PER_BLOCK;
            short max = COLUMNS_PER_BLOCK * 5;
            short currentX = randomNumber(min, max);
            
            spawnCrocodile(crocToMain, buildCrocodile(currentX, computeY(i), directions[i], speeds[i], -10));

            do{
                currentX += crocodileSpace(); // spazio tra un coccodrillo e l'altro
                currentX += CROCODILE_COLUMNS; // la x e' la prima colonna
                spawnCrocodile(crocToMain, buildCrocodile(currentX, computeY(i), directions[i], speeds[i], -10));
            } while( currentX + CROCODILE_COLUMNS > (COLUMNS_PER_MAP - COLUMNS_PER_BLOCK));

            currentX += CROCODILE_COLUMNS;

            spawnTimers[i] = ((CROCODILE_COLUMNS + crocodileSpace()) - (COLUMNS_PER_MAP - currentX)) * speeds[i];
        }

    }
        
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
                spawnCrocodile(crocToMain, buildCrocodile(spawns[directions[i]], computeY(i), directions[i], speeds[i], -10));
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

    usleep(1000 * 1000 * 1000); // va in pausa in attesa che il padre esegua kill e wait
}


/*********************\
*  FUNZIONI UTILITA'  *
\*********************/

short computeY(short n)
{
    short y = n * ROWS_PER_BLOCK;
    y +=  (ROWS_PER_BLOCK * (1 + 1 + RIVERSIDE_ROWS)); // considera la scoreaboard, le tane e le righe del fiume.
}

Crocodile buildCrocodile(short x, short y, short direction, short speed, short splash)
{
    Crocodile croc;
    croc.x = x;
    croc.y = y;
    croc.direction = direction;
    croc.speed = speed;
    croc.splash = splash;
    return croc;
}

void randomSeed()
{
    __pid_t pid = getpid();

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
    __pid_t pid = fork();

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
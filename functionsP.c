#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>

#include "functionsP.h"
#include "sprite.h"
#include "structs.h"
#include "settings.h"

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
        if((ch = getch()) != ERR)
        {
            switch(ch) //qui dentro modifichiamo x e y
            {
                case 'd':
                case KEY_RIGHT:
                    update = true;
                    frogger.x += COLUMNS_PER_BLOCK;
                    if (frogger.x > COLUMNS_PER_MAP - COLUMNS_PER_BLOCK+1)
                    {
                        frogger.x = COLUMNS_PER_MAP - COLUMNS_PER_BLOCK+1;
                    }
                    break;

                case 'a':
                case KEY_LEFT:
                    update = true;
                    frogger.x -= COLUMNS_PER_BLOCK;
                    if (frogger.x < 1)
                    {
                        frogger.x = 1;
                    }
                    break;

                case 'w':
                case KEY_UP:
                    update = true;
                    frogger.y -= ROWS_PER_BLOCK;
                    if(frogger.y < 4)
                    {
                        frogger.y = 4;
                    }
                    break;

                case 's':
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

        ssize_t bytes_read = read(crocToMain[READ], &croc, sizeof(croc));
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

        printFrog(frogger.x, frogger.y);
        if(FROG_DEBUG)
        {
            customBorder(COLUMNS_PER_MAP+4, 0, 11, 3, false);
            CHANGE_COLOR(DEFAULT);
            mvprintw(1, COLUMNS_PER_MAP+5, "%03d : %03d", frogger.x, frogger.y);
            customBorder(COLUMNS_PER_MAP+4, 4, 11, 3, false); 
            mvprintw(5, COLUMNS_PER_MAP+5, "%03d : %03d", croc.x, croc.y);
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

void riverHandler()
{

}

void singleCrocodileHandler(int crocToMain[], int crocToRivH[], Crocodile me)
{
    bool keepGoing = true; int aliveTime = 1;
    me.PID = getpid();
    write(crocToMain[WRITE], &me, sizeof(me)); // scrive le sue coordinate
    usleep(FRAME_UPDATE);
    aliveTime++;

    do
    {
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

    write(crocToRivH[WRITE], &me.PID, sizeof(me.PID)); // comunica il suo PID al padre

    usleep(1000 * 1000 * 1000); // va in pausa in attesa che il padre esegua kill e wait

}
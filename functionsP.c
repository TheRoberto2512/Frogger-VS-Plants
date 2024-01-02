#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <curses.h>

#include "functionsP.h"
#include "sprite.h"
#include "structs.h"

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
        update = true;
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
                    //write(FPHToMain[WRITE], &frogger, sizeof(frogger)); // comunichiamo la posizione corrente al processo che gestira' i proiettili
                    break;
            }

            if(update) // se la rana si e' mossa in qualche direzione
                write(frogToMain[WRITE], &frogger, sizeof(frogger));  // comunica le nuove coordinate
        } 
        usleep(FRAME_UPDATE); 
    } while (true);
}

void mainManager(int frogToMain[], int mainToFrog[])
{
    bool BGCroc = true;
    Frog frogger; bool tane[5] = {true, true, true, true, true};
    int c = 0;
    do
    {
        read(frogToMain[READ], &frogger, sizeof(frogger));

        if(c < 3)
        {
            clear();
            c++;
        }
        customBorder(0, 0, COLUMNS_PER_MAP+2, ROWS_PER_MAP + 4, true);
        printScoreBoard(2, 0, 30, 30); 
        printMap(true, tane, BGCroc);
        if(BGCroc)
            BGCroc = false;
        printFrog(frogger.x, frogger.y);
        if(FROG_DEBUG)
        {
            customBorder(COLUMNS_PER_MAP+4, 0, 11, 3, false);
            CHANGE_COLOR(DEFAULT);
            mvprintw(1, COLUMNS_PER_MAP+5, "%03d : %03d", frogger.x, frogger.y); 
        }
        
        refresh();

        usleep(FRAME_UPDATE);

    } while(true);
}
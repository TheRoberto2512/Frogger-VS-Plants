#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <curses.h>

#include "functionsP.h"
#include "sprite.h"
#include "structs.h"

void frogHandler(int frogToMain[2], int mainToFrog[2], int FPHToMain[2])
{
    close(frogToMain[READ]); // pipe dove scrive le coordinate
    close(mainToFrog[WRITE]); // pipe dove legge le coordinate aggiornate
    close(FPHToMain[READ]);  // pipe dove comunica la creazione di un proiettile rana

    Frog frogger;
    frogger.x = (BLOCK_PER_MAP_ROWS / 2) * COLUMNS_PER_BLOCK +1; // x iniziale (centro mappa)
    frogger.y = ROWS_PER_MAP - 3; // y iniziale (ultima riga)

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
                    break;

                case 's':
                case KEY_DOWN:
                    update = true;
                    frogger.y += ROWS_PER_BLOCK;
                    if (frogger.y > (ROWS_PER_MAP - 3))
                    {
                        frogger.y = ROWS_PER_MAP - 3;
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

void mainManager(WINDOW *gameArea, WINDOW *scoreBoard, int frogToMain[2], int mainToFrog[2])
{
    close(frogToMain[WRITE]);
    close(mainToFrog[READ]);
    bool BGCroc = true;
    Frog frogger; bool tane[5] = {true, true, true, true, true};

    do
    {
        read(frogToMain[READ], &frogger, sizeof(frogger));

        printScoreBoard(scoreBoard, 2, 0, 30, 30); 
        printMap(gameArea, true, tane, BGCroc);
        if(BGCroc)
            BGCroc = false;
        printFrog(gameArea, frogger.x, frogger.y);
        wrefresh(gameArea); wrefresh(scoreBoard);
        refresh();

        usleep(FRAME_UPDATE);

    } while(true);
}
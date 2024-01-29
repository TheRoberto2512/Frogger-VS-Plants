#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "sprite.h"
#include "colors.h"


/**********************\
*  FUNZIONI PER MAPPA  *
\**********************/

void customBorder(short x, short y, short top, short side, bool separator)
{
    CHANGE_COLOR(DEFAULT); move(y, x);
    printw("%lc", L'┌'); 
    for(short i = 0; i < top - 2; i++)
    {
        printw("%lc", L'─');
    }
    printw("%lc", L'┐'); move(y+1, x);
    for (short i = 0; i < side - 2; i++)
    {
        printw("%lc", L'│');
        move(y+1+i, x+top-1);
        printw("%lc", L'│');
        move(y+2+i, x);
    }
    printw("%lc", L'└');
    for(short i = 0; i < top - 2; i++)
    {
        printw("%lc", L'─');
    }
    printw("%lc", L'┘');

    if(separator)
    {
        move(y+2, x); printw("%lc", L'├');
        for(short i = 0; i < top - 2; i++)
        {
            printw("%lc", L'─');
        } printw("%lc", L'┤');
    }

    refresh();
}

void printScoreBoard(short lives, short score, short time, short fullTime)
{
    CHANGE_COLOR(DEFAULT);
    short padding = 2;  // caratteri lasciati dai bordi verticali
    short spaces = (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS - (padding * 2)- 25 - 15 - 47)/2;
    short x = 1, y = 1; // 0, 0 saranno occupati dal bordo
    x += padding; //aggiungiamo del padding
    mvprintw(y, x, "Vite rimanenti: "); x += 16;

    for(short i = 0; i < 3; i++)
    {
        if(i < lives)
        {
            mvprintw(y, x, "%lc  ", L'🐸'); x += 3;
        }
        else
        {
            mvprintw(y, x, "%lc  ", L'💀'); x += 3;
        } 
    }
    
    x += spaces;
    mvprintw(y, x, "Punteggio: "); x += 11;
    mvprintw(y, x, "%04hd", score); x += 4;
    x += spaces;

    mvprintw(y, x, "Tempo rimanente: "); x += 17;

    if(time > (fullTime - fullTime/3))
        CHANGE_COLOR(SC_HIGH);
    else if (time > (fullTime/3))
        CHANGE_COLOR(SC_MID);
    else
        CHANGE_COLOR(SC_LOW);

    for(short i = 0; i < 30; i++)
    {
        if(i < time)
        {
            mvprintw(y, x, "█"); x++;
        }
        else
        {
            CHANGE_COLOR(DEFAULT);
            mvprintw(y, x, "█"); x++;
        }
    }
}

void printMap(bool bottom, bool lilyPads[], bool newBG)
{
    static short crocX = -500, direction, speed = 1, crocUpdate;

    if(crocX != -500) // se esiste un coccodrillo
    {
        if(direction) // in base alla direzione capisce se viene visualizzato o no
        {
            if(crocX > (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+2)
               newBG = true; 
        }
        else
        {
            if(crocX < 0 - BC_CROC_COLUMNS -2)
                newBG = true; 
        }
    }

    if(newBG)
    {
        crocUpdate = 0;
        direction = (rand() % 2); // 0 va a sinistra, 1 va a destra
        if(direction)
        {
            crocX = 1 - BC_CROC_COLUMNS;
        }
        else
        {
            crocX = (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS) + BC_CROC_COLUMNS;
        }
    }
    else
    {
        if(crocUpdate % speed == 0)
        {
            if(direction)
                crocX++;
            else
                crocX--;
        }
        crocUpdate++;
    }

    short x = 1, y = 3;  // 0, 0 saranno occupati dal bordo
    printLilyPads(x, y, lilyPads, crocX, direction);
    y += ROWS_PER_BLOCK * LILY_PADS_ROWS;
    y += ROWS_PER_SEPARATOR; // conta anche il separatore

    // stampa le righe della sponda dopo il fiume
    for(short i = 0; i < RIVERSIDE_ROWS; i++)
    {
        printGroundLine(x, y);
        y += ROWS_PER_BLOCK;
    } 

    // stampa il fiume
    for(short i = 0; i < RIVER_ROWS; i++)
    {
        printRiverLine(x, y);
        y += ROWS_PER_BLOCK;
    }  

    if(bottom)
        printGroundLine(x, y);
}

void printGroundLine(short x, short y)
{
    short startX = x;
    CHANGE_COLOR(PRATO);
    for(short i = 0; i < ROWS_PER_BLOCK; i++)
    {
        for(short j = 0; j < COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS; j++)
        {
            mvprintw(y, x, " ");
            x++;
        }
        y++;
        x = startX;
    } 
}

void printRiverLine(short x, short y)
{
    short startX = x;
    CHANGE_COLOR(FIUME);
    for(short i = 0; i < ROWS_PER_BLOCK; i++)
    {
        for(short j = 0; j < COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS; j++)
        {
            mvprintw(y, x, " ");
            x++;
        }
        y++;
        x = startX;
    } 
}

void printLilyPads(short x, short y, bool lilyPads[], short crocX, short direction)
{
    double r = BLOCK_PER_MAP_ROWS / 5;
    static short coords[5] = { -1, -1, -1, -1, -1};

    if(coords[0] == -1)
        for(short i = 0; i < 5; i++)
        {
            coords[i] = COLUMNS_PER_BLOCK * (1 + (i * r));
        }
    
    printRiverLine(x, y);
    if(crocX != -500)
    {
        printBGCrocodile(crocX, 3, direction);
    }
    
    CHANGE_COLOR(PRATO);
    for(short j = 0; j < COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS; j++)
    {
        mvprintw(y + ROWS_PER_BLOCK, x+j, " ");
    }

    for(short i = 0; i < 5; i++)
    {
        printSingleLilyPad(coords[i], y, lilyPads[i]);
        CHANGE_COLOR(PRATO_E_ACQUA);
        for(short z = 0; z < 11; z++)
            mvprintw(y + ROWS_PER_BLOCK, coords[i]-1+z, "▄");
    }
}

void printSingleLilyPad(short x, short y, bool empty)
{
    wchar_t sprite[ROWS_PER_BLOCK][LILY_PADS_COLUMNS] =
    { 
        { L'n', L'▄', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄', L'n' },
        { L'█', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'█' },
        { L'█', L' ', L' ', L' ', L'▄', L' ', L' ', L' ', L'█' },
        { L'n', L'▀', L'▄', L'▀', L'n', L'▀', L'▄', L'▀', L'n' }
    };

    short colors[ROWS_PER_BLOCK][LILY_PADS_COLUMNS] =
    { 
        { 0, ACQUA_E_BORDO, NINFEA, NINFEA, NINFEA, NINFEA, NINFEA, ACQUA_E_BORDO, 0 },
        { ACQUA_E_BORDO, NINFEA,        NINFEA,        NINFEA,        NINFEA,        NINFEA,        NINFEA, NINFEA, ACQUA_E_BORDO },
        { ACQUA_E_BORDO, NINFEA,        NINFEA,        NINFEA, NINFEA,        NINFEA,        NINFEA, NINFEA, ACQUA_E_BORDO },
        { 0, ACQUA_E_BORDO, NINFEA, ACQUA_E_BORDO, 0, ACQUA_E_BORDO, NINFEA, ACQUA_E_BORDO, 0 }
    };

    for(short i = 0; i < ROWS_PER_BLOCK; i++)
    {
        for(short j = 0; j < LILY_PADS_COLUMNS; j++)
        {
            if(sprite[i][j] != 'n')
            {
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i, x+j, "%lc", sprite[i][j]);
            }
        }
    }

    if(!empty)
    {
        wchar_t spriteF[ROWS_PER_BLOCK][FROG_COLUMNS] =
        {
            { L' ', L'n', L'n', L'n', L'n', L'n', L' ' },
            { L'n', L' ', L' ', L' ', L' ', L' ', L'n' },
            { L' ', L' ', L' ', L' ', L' ', L' ', L' ' },
            { L' ', L'n', L'▄', L' ', L'▄', L'n', L' ' }
        };
        CHANGE_COLOR(RANA);
        for(short i = 0; i < ROWS_PER_BLOCK; i++)
        {
            for(short j = 0; j < FROG_COLUMNS; j++)
            {
                if(spriteF[i][j] != L'n')
                {
                    mvprintw(y+i, x+j+1, "%lc", spriteF[i][j]);
                }
                
            }
        }
    }
}


/************************\
*  FUNZIONI PER ENTITA'  *
\************************/

void printCrocodile(short x, short y, short direction)
{
    wchar_t sprite[ROWS_PER_BLOCK][CROCODILE_COLUMNS] =
    {
        { L'n', L'n', L'n', L'n', L'n', L'n', L'▄', L'▄', L'▄', L'▄', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n' },
        { L'▄', L'▄', L' ', L'▄', L' ', L'▄', L' ', L'▀', L'▀', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'▄' },
        { L'▀', L' ', L'▀', L' ', L'▀', L' ', L'▀', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'▀' },
        { L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'▀', L'▀', L'▀', L'n', L'n', L'n', L'n', L'▀', L'▀', L'▀', L'n', L'n', L'n', L'n' }
    };

    short colors[ROWS_PER_BLOCK][CROCODILE_COLUMNS] =
    {
        {             0,           0,           0,           0,           0,           0, ACQUA_E_CROCC, ACQUA_E_CROCC, ACQUA_E_CROCC, ACQUA_E_CROCC,           0,           0,           0,           0,             0,             0,             0,           0,           0,           0,             0 },
        { ACQUA_E_CROCC, DENTI_CROCC, DENTI_CROCC, DENTI_CROCC, DENTI_CROCC, DENTI_CROCC,   DENTI_CROCC, PUPILLA_CROCC,   IRIDE_CROCC,   DENTI_CROCC, DENTI_CROCC, DENTI_CROCC, DENTI_CROCC, DENTI_CROCC,   DENTI_CROCC,   DENTI_CROCC,   DENTI_CROCC, DENTI_CROCC, DENTI_CROCC, DENTI_CROCC, ACQUA_E_CROCC },
        { ACQUA_E_CROCC, DENTI_CROCC, DENTI_CROCC, DENTI_CROCC, DENTI_CROCC, DENTI_CROCC,   DENTI_CROCC,   DENTI_CROCC,   DENTI_CROCC,   DENTI_CROCC, DENTI_CROCC, DENTI_CROCC, DENTI_CROCC, DENTI_CROCC,   DENTI_CROCC,   DENTI_CROCC,   DENTI_CROCC, DENTI_CROCC, DENTI_CROCC, DENTI_CROCC, ACQUA_E_CROCC },
        {             0,           0,           0,           0,           0,            0,            0, ACQUA_E_CROCC, ACQUA_E_CROCC, ACQUA_E_CROCC,           0,           0,           0,           0, ACQUA_E_CROCC, ACQUA_E_CROCC, ACQUA_E_CROCC,           0,           0,           0,             0 }
    };


    if(direction) // vanno a destra
        for(short i = 0; i < ROWS_PER_BLOCK; i++)
        {
            for(short j = (CROCODILE_COLUMNS-1); j >= 0; j--)
            {
                short xx = x+(CROCODILE_COLUMNS - j);
                if(sprite[i][j] != L'n' && (xx >= 1 && xx < (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+1))
                {
                    CHANGE_COLOR(colors[i][j]);
                    mvprintw(y+i, xx, "%lc", sprite[i][j]);
                }
            }
        }
    else // vanno a sinistra
        for(short i = 0; i < ROWS_PER_BLOCK; i++)
        {
            for(short j = 0; j < CROCODILE_COLUMNS; j++)
            {
                short xx = x+j+1;
                if(sprite[i][j] != L'n' && (xx >= 1 && xx < (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+1))
                {
                    CHANGE_COLOR(colors[i][j]);
                    mvprintw(y+i, xx, "%lc", sprite[i][j]);
                }
            }
        }
}

void printBadCrocodile(short x, short y, short direction)
{
    wchar_t sprite[ROWS_PER_BLOCK][CROCODILE_COLUMNS] =
    {
        { L'n', L'n', L'n', L'n', L'n', L'n', L'▄', L'▄', L'▄', L'▄', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n' },
        { L'▄', L'▄', L' ', L'▄', L' ', L'▄', L' ', L'▀', L'▀', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'▄' },
        { L'▀', L' ', L'▀', L' ', L'▀', L' ', L'▀', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'▀' },
        { L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'▀', L'▀', L'▀', L'n', L'n', L'n', L'n', L'▀', L'▀', L'▀', L'n', L'n', L'n', L'n' }
    };

    short colors[ROWS_PER_BLOCK][CROCODILE_COLUMNS] =
    {
        {             0,           0,           0,           0,           0,           0, ACQUA_E_DARK_CROCC, ACQUA_E_DARK_CROCC, ACQUA_E_DARK_CROCC, ACQUA_E_DARK_CROCC,           0,           0,           0,           0,             0,             0,             0,           0,           0,           0,             0 },
        { ACQUA_E_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC,   DENTI_DARK_CROCC, PUPILLA_DARK_CROCC,   IRIDE_DARK_CROCC,   DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC,   DENTI_DARK_CROCC,   DENTI_DARK_CROCC,   DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, ACQUA_E_DARK_CROCC },
        { ACQUA_E_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC,   DENTI_DARK_CROCC,   DENTI_DARK_CROCC,   DENTI_DARK_CROCC,   DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC,   DENTI_DARK_CROCC,   DENTI_DARK_CROCC,   DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, DENTI_DARK_CROCC, ACQUA_E_DARK_CROCC },
        {             0,           0,           0,           0,           0,            0,            0, ACQUA_E_DARK_CROCC, ACQUA_E_DARK_CROCC, ACQUA_E_DARK_CROCC,           0,           0,           0,           0, ACQUA_E_DARK_CROCC, ACQUA_E_DARK_CROCC, ACQUA_E_DARK_CROCC,           0,           0,           0,             0 }
    };


    if(direction) // vanno a destra
        for(short i = 0; i < ROWS_PER_BLOCK; i++)
        {
            for(short j = (CROCODILE_COLUMNS-1); j >= 0; j--)
            {
                short xx = x+(CROCODILE_COLUMNS - j);
                if(sprite[i][j] != L'n' && (xx >= 1 && xx < (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+1))
                {
                    CHANGE_COLOR(colors[i][j]);
                    mvprintw(y+i, xx, "%lc", sprite[i][j]);
                }
            }
        }
    else // vanno a sinistra
        for(short i = 0; i < ROWS_PER_BLOCK; i++)
        {
            for(short j = 0; j < CROCODILE_COLUMNS; j++)
            {
                short xx = x+j+1;
                if(sprite[i][j] != L'n' && (xx >= 1 && xx < (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+1))
                {
                    CHANGE_COLOR(colors[i][j]);
                    mvprintw(y+i, xx, "%lc", sprite[i][j]);
                }
            }
        }
}

void printEnemy(short x, short y, short genTime)
{
    // FULL ENEMY
    wchar_t sprite[ROWS_PER_BLOCK][ENEMY_COLUMNS] =
    {
        { L'█', L'▄', L'▀', L'▄', L'▀', L'▄', L'█' },
        { L'n', L'█', L'▸', L'•', L'◂', L'█', L'n' },
        { L'n', L'n', L'▀', L'▀', L'▀', L'n', L'n' },
        { L'n', L'n', L'▄', L'█', L'▄', L'n', L'n' }
    };

    short colors[ROWS_PER_BLOCK][ENEMY_COLUMNS] =
    {
        { PETALI_E_PRATO, PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_PRATO, PETALI_E_PRATO },
        {              0, PETALI_E_PRATO,   FACCIA_CENTRO,  FACCIA_CENTRO,   FACCIA_CENTRO, PETALI_E_PRATO,              0 },
        {              0,              0,  PETALI_E_PRATO, PETALI_E_GAMBO,  PETALI_E_PRATO,              0,              0 },
        {              0,              0,   GAMBO_E_PRATO,  GAMBO_E_PRATO,   GAMBO_E_PRATO,              0,              0 }
    };

    // HALF ENEMY
    wchar_t halfSprite[ROWS_PER_BLOCK/2][ENEMY_COLUMNS] =
    {
        { L'n', L'n', L'n', L'█', L'n', L'n', L'n' },
        { L'n', L'n', L'▄', L'█', L'▄', L'n', L'n' }
    };

    if(genTime)
    {
        CHANGE_COLOR(GAMBO_E_PRATO);
        for(short i = 0; i < ROWS_PER_BLOCK/2; i++)
            for(short j = 0; j < ENEMY_COLUMNS; j++)
                if(halfSprite[i][j] != L'n')
                    mvprintw(y+i+2, x+j, "%lc", halfSprite[i][j]);
    }
    else
    {
        for(short i = 0; i < ROWS_PER_BLOCK; i++)
        {
            for(short j = 0; j < ENEMY_COLUMNS; j++)
            {
                if(sprite[i][j] != L'n')
                {
                    CHANGE_COLOR(colors[i][j]);
                    mvprintw(y+i, x+j, "%lc", sprite[i][j]);
                }
                
            }
        }
    }
}

void printFrog(short x, short y)
{
    wchar_t sprite[ROWS_PER_BLOCK][FROG_COLUMNS] =
    {
        { L' ', L'n', L'▀', L' ', L'▀', L'n', L' ' },
        { L' ', L' ', L' ', L' ', L' ', L' ', L' ' },
        { L'n', L' ', L' ', L' ', L' ', L' ', L'n' },
        { L' ', L'n', L'n', L'n', L'n', L'n', L' ' },
    };

    CHANGE_COLOR(RANA);
    for(short i = 0; i < ROWS_PER_BLOCK; i++)
    {
        for(short j = 0; j < FROG_COLUMNS; j++)
        {
            if(sprite[i][j] != L'n')
            {
                mvprintw(y+i, x+j, "%lc", sprite[i][j]);
            }
            
        }
    }
}

void printBGCrocodile(short x, short y, short direction)
{
    static short tails = 0;
    wchar_t sprite0[ROWS_PER_BLOCK][BC_CROC_COLUMNS] =
    {
        { L'n', L'n', L'n', L'n', L'n', L'▄', L'▄', L'▄', L'n', L'n', L'▄', L'▄', L'▄', L'n', L'n', L'n', L'n', L'n', L'n' },
        { L'▄', L'▄', L'▄', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'▄', L'▄', L'▄', L'▄', L'▄' },
        { L'▀', L'▀', L'▀', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'▀', L'▀', L'▀', L'▀', L'n' },
        { L'n', L'n', L'n', L'n', L'n', L'▀', L'▀', L'▀', L'n', L'n', L'▀', L'▀', L'▀', L'n', L'n', L'n', L'n', L'n', L'n' }
    };

    wchar_t sprite1[ROWS_PER_BLOCK][BC_CROC_COLUMNS] =
    {
        { L'n', L'n', L'n', L'n', L'n', L'▄', L'▄', L'▄', L'n', L'n', L'▄', L'▄', L'▄', L'n', L'n', L'n', L'n', L'n', L'n' },
        { L'▄', L'▄', L'▄', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'▄', L'▄', L'▄', L'▄', L'n' },
        { L'▀', L'▀', L'▀', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'█', L'▀', L'▀', L'▀', L'▀', L'▀' },
        { L'n', L'n', L'n', L'n', L'n', L'▀', L'▀', L'▀', L'n', L'n', L'▀', L'▀', L'▀', L'n', L'n', L'n', L'n', L'n', L'n' }
    };

    if(tails)
        tails = 0;
    else
        tails = 1;

    CHANGE_COLOR(FIUME);
    if(direction) // vanno a destra
    {
        for(short i = 0; i < ROWS_PER_BLOCK; i++)
        {
            for(short j = (BC_CROC_COLUMNS-1); j >= 0; j--)
            {
                short xx = x+(BC_CROC_COLUMNS - j);
                if(tails)
                {
                    if(sprite1[i][j] != L'n' && (xx >= 1 && xx < (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+1))
                    {
                        mvprintw(y+i, xx, "%lc", sprite1[i][j]);
                    }
                }
                else
                {
                    if(sprite0[i][j] != L'n' && (xx >= 1 && xx < (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+1))
                    {
                        mvprintw(y+i, xx, "%lc", sprite0[i][j]);
                    }
                }
            }
        }
    }
    else // vanno a sinistra
    {
        for(short i = 0; i < ROWS_PER_BLOCK; i++)
        {
            for(short j = 0; j < BC_CROC_COLUMNS; j++)
            {
                short xx = x+j+1;
                if(tails)
                {
                    if(sprite1[i][j] != L'n' && (xx >= 1 && xx < (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+1))
                    {
                        mvprintw(y+i, xx, "%lc", sprite1[i][j]);
                    }
                }
                else
                {
                    if(sprite0[i][j] != L'n' && (xx >= 1 && xx < (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+1))
                    {
                        mvprintw(y+i, xx, "%lc", sprite0[i][j]);
                    }
                }
            }
        }
    }
}

void printProjectile(short x, short y, bool fromFrog)
{
    CHANGE_COLOR(PROIETTILI);

    if(fromFrog)
        mvprintw(y, x, " ");
    else
        mvprintw(y, x, "█");
}

void printDangerSign(short direction, short y)
{
    static short lastY = -7; static short printCount = 0; // variabili statiche per l'animazione

    if(lastY != y)
    {
        lastY = y; printCount = 0;
    }

    short x[2]; 
    x[0] = COLUMNS_PER_MAP-DANGER_SIGN_COLUMNS;     // direzione = 0 significa che il coccodrillo sta andando a sinistra
    x[1] = 1+1;                                     // direzione = 1 significa che il coccodrillo sta andando a destra
    // il segnale verra' stampato all'estremo opposto

    wchar_t sprite[ROWS_PER_BLOCK][DANGER_SIGN_COLUMNS] =
    {
        { L'n', L'n', L'▄', L'▀', L'▄', L'n', L'n' },
        { L'n', L'█', L' ', L'█', L' ', L'█', L'n' },
        { L'█', L' ', L' ', L'▄', L' ', L' ', L'█' },
        { L'n', L'▀', L'▀', L'▀', L'▀', L'▀', L'n' }
    };

    short colors[ROWS_PER_BLOCK][DANGER_SIGN_COLUMNS] =
    {
        {               0,               0, ROSSO_E_CELESTE,  ROSSO_E_BIANCO, ROSSO_E_CELESTE,               0,               0 },
        {               0, ROSSO_E_CELESTE,   NERO_E_BIANCO,   NERO_E_BIANCO,   NERO_E_BIANCO, ROSSO_E_CELESTE,               0 },
        { ROSSO_E_CELESTE,   NERO_E_BIANCO,   NERO_E_BIANCO,   NERO_E_BIANCO,   NERO_E_BIANCO,   NERO_E_BIANCO, ROSSO_E_CELESTE },
        {               0, ROSSO_E_CELESTE, ROSSO_E_CELESTE, ROSSO_E_CELESTE, ROSSO_E_CELESTE, ROSSO_E_CELESTE,               0 }
    };

    if(printCount >= 0)
    {
        for(short i = 0; i < ROWS_PER_BLOCK; i++)
        {
            for(short j = 0; j < DANGER_SIGN_COLUMNS; j++)
            {
                if(sprite[i][j] != L'n')
                {
                    CHANGE_COLOR(colors[i][j]);
                    mvprintw(y+i, x[direction]+j, "%lc", sprite[i][j]);
                }
            }
        }
        printCount++;
        if(printCount == 5)
            printCount = -1;
    }
    else
    {
        printCount--;
        if(printCount == -5)
            printCount = -0;
    }
}

// ▀ █ ▄
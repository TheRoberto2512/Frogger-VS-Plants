#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include "sprite.h"
#include "colors.h"

void printScoreBoard(WINDOW *win, short lives, short score, short time, short fullTime)
{
    CHANGE_COLOR(win, DEFAULT);
    wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LTEE, ACS_RTEE);
    short padding = 2;  // caratteri lasciati dai bordi verticali
    short spaces = (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS - (padding * 2)- 25 - 15 - 47)/2;
    short x = 1, y = 1; // 0, 0 saranno occupati dal bordo
    x += padding; //aggiungiamo del padding
    mvwprintw(win, y, x, "Vite rimanenti: "); x += 16;

    for(short i = 0; i < 3; i++)
    {
        if(i < lives)
        {
            mvwprintw(win, y, x, "%lc  ", L'🐸'); x += 3;
        }
        else
        {
            mvwprintw(win, y, x, "%lc  ", L'💀'); x += 3;
        } 
    }
    
    x += spaces;
    mvwprintw(win, y, x, "Punteggio: "); x += 11;
    mvwprintw(win, y, x, "%04hd", score); x += 4;
    x += spaces;

    mvwprintw(win, y, x, "Tempo rimanente: "); x += 17;

    if(time > (fullTime - fullTime/3))
        CHANGE_COLOR(win, SC_HIGH);
    else if (time > (fullTime/3))
        CHANGE_COLOR(win, SC_MID);
    else
        CHANGE_COLOR(win, SC_LOW);

    for(short i = 0; i < 30; i++)
    {
        if(i < time)
        {
            mvwprintw(win, y, x, "█"); x++;
        }
        else
        {
            CHANGE_COLOR(win, DEFAULT);
            mvwprintw(win, y, x, "█"); x++;
        }
    }
}

void printMap(WINDOW *win, bool top, bool bottom, bool lilyPads[], bool newBG)
{
    CHANGE_COLOR(win, DEFAULT);
    wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_LTEE, ACS_RTEE, ACS_LLCORNER, ACS_LRCORNER);

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

    short x = 1, y = 1;  // 0, 0 saranno occupati dal bordo
    if(top)
    {
        printLilyPads(win, x, y, lilyPads, crocX, direction);
        y += ROWS_PER_BLOCK * LILY_PADS_ROWS;
    }
    y += ROWS_PER_SEPARATOR; // conta anche il separatore

    // stampa le righe della sponda dopo il fiume
    for(short i = 0; i < RIVERSIDE_ROWS; i++)
    {
        printGroundLine(win, x, y);
        y += ROWS_PER_BLOCK;
    } 

    // stampa il fiume
    for(short i = 0; i < RIVER_ROWS; i++)
    {
        printRiverLine(win, x, y);
        y += ROWS_PER_BLOCK;
    }  

    if(bottom)
        printGroundLine(win, x, y);
}

void printLilyPads(WINDOW *win, short x, short y, bool lilyPads[], short crocX, short direction)
{
    double r = BLOCK_PER_MAP_ROWS / 5;
    static short coords[5] = { -1, -1, -1, -1, -1};

    if(coords[0] == -1)
        for(short i = 0; i < 5; i++)
        {
            coords[i] = COLUMNS_PER_BLOCK * (1 + (i * r));
        }
    
    printRiverLine(win, x, y);
    if(crocX != -500)
    {
        printBGCrocodile(win, crocX, 1, direction);
    }
    
    CHANGE_COLOR(win, PRATO);
    for(short j = 0; j < COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS; j++)
    {
        mvwprintw(win, y + ROWS_PER_BLOCK, x+j, " ");
    }

    for(short i = 0; i < 5; i++)
    {
        printSingleLilyPad(win, coords[i], y, lilyPads[i]);
        CHANGE_COLOR(win, PRATO_E_ACQUA);
        for(short z = 0; z < 11; z++)
            mvwprintw(win, y + ROWS_PER_BLOCK, coords[i]-1+z, "▄");
    }
}

void printGroundLine(WINDOW *win, short x, short y)
{
    short startX = x;
    CHANGE_COLOR(win, PRATO);
    for(short i = 0; i < ROWS_PER_BLOCK; i++)
    {
        for(short j = 0; j < COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS; j++)
        {
            mvwprintw(win, y, x, " ");
            x++;
        }
        y++;
        x = startX;
    } 
}

void printRiverLine(WINDOW *win, short x, short y)
{
    short startX = x;
    CHANGE_COLOR(win, FIUME);
    for(short i = 0; i < ROWS_PER_BLOCK; i++)
    {
        for(short j = 0; j < COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS; j++)
        {
            mvwprintw(win, y, x, " ");
            x++;
        }
        y++;
        x = startX;
    } 
}

void printSingleLilyPad(WINDOW *win, short x, short y, bool empty)
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
                CHANGE_COLOR(win, colors[i][j]);
                mvwprintw(win, y+i, x+j, "%lc", sprite[i][j]);
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
        CHANGE_COLOR(win, RANA);
        for(short i = 0; i < ROWS_PER_BLOCK; i++)
        {
            for(short j = 0; j < FROG_COLUMNS; j++)
            {
                if(spriteF[i][j] != L'n')
                {
                    mvwprintw(win, y+i, x+j+1, "%lc", spriteF[i][j]);
                }
                
            }
        }
    }
}

void printCrocodile(WINDOW *win, short x, short y, short direction)
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
                    CHANGE_COLOR(win, colors[i][j]);
                    mvwprintw(win, y+i, xx, "%lc", sprite[i][j]);
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
                    CHANGE_COLOR(win, colors[i][j]);
                    mvwprintw(win, y+i, xx, "%lc", sprite[i][j]);
                }
            }
        }
}

void printBadCrocodile(WINDOW *win, short x, short y, short direction)
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
                    CHANGE_COLOR(win, colors[i][j]);
                    mvwprintw(win, y+i, xx, "%lc", sprite[i][j]);
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
                    CHANGE_COLOR(win, colors[i][j]);
                    mvwprintw(win, y+i, xx, "%lc", sprite[i][j]);
                }
            }
        }
}

void printEnemy(WINDOW *win, short x, short y)
{
    wchar_t sprite[ROWS_PER_BLOCK][ENEMY_COLUMNS] =
    {
        { L'█', L'▄', L'▄', L'▄', L'▄', L'▄', L'█' },
        { L'n', L'█', L'▸', L'•', L'◂', L'█', L'n' },
        { L'n', L'n', L'▀', L'▀', L'▀', L'n', L'n' },
        { L'n', L'n', L'▄', L'█', L'▄', L'n', L'n' },
    };

    short colors[ROWS_PER_BLOCK][ENEMY_COLUMNS] =
    {
        { PETALI_E_PRATO, PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_PRATO, PETALI_E_PRATO },
        {              0, PETALI_E_PRATO,   FACCIA_CENTRO,  FACCIA_CENTRO,   FACCIA_CENTRO, PETALI_E_PRATO,              0 },
        {              0,              0,  PETALI_E_PRATO, PETALI_E_GAMBO,  PETALI_E_PRATO,              0,              0 },
        {              0,              0,   GAMBO_E_PRATO,  GAMBO_E_PRATO,   GAMBO_E_PRATO,              0,              0 }
    };

    for(short i = 0; i < ROWS_PER_BLOCK; i++)
    {
        for(short j = 0; j < ENEMY_COLUMNS; j++)
        {
            if(sprite[i][j] != L'n')
            {
                CHANGE_COLOR(win, colors[i][j]);
                mvwprintw(win, y+i, x+j, "%lc", sprite[i][j]);
            }
            
        }
    }
}

void printFrog(WINDOW *win, short x, short y)
{
    wchar_t sprite[ROWS_PER_BLOCK][FROG_COLUMNS] =
    {
        { L' ', L'n', L'▀', L' ', L'▀', L'n', L' ' },
        { L' ', L' ', L' ', L' ', L' ', L' ', L' ' },
        { L'n', L' ', L' ', L' ', L' ', L' ', L'n' },
        { L' ', L'n', L'n', L'n', L'n', L'n', L' ' },
    };

    CHANGE_COLOR(win, RANA);
    for(short i = 0; i < ROWS_PER_BLOCK; i++)
    {
        for(short j = 0; j < FROG_COLUMNS; j++)
        {
            if(sprite[i][j] != L'n')
            {
                mvwprintw(win, y+i, x+j, "%lc", sprite[i][j]);
            }
            
        }
    }
}

void printBGCrocodile(WINDOW *win, short x, short y, short direction)
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

    CHANGE_COLOR(win, FIUME);
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
                        mvwprintw(win, y+i, xx, "%lc", sprite1[i][j]);
                    }
                }
                else
                {
                    if(sprite0[i][j] != L'n' && (xx >= 1 && xx < (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+1))
                    {
                        mvwprintw(win, y+i, xx, "%lc", sprite0[i][j]);
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
                        mvwprintw(win, y+i, xx, "%lc", sprite1[i][j]);
                    }
                }
                else
                {
                    if(sprite0[i][j] != L'n' && (xx >= 1 && xx < (COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS)+1))
                    {
                        mvwprintw(win, y+i, xx, "%lc", sprite0[i][j]);
                    }
                }
            }
        }
    }
}

// ▀ █ ▄
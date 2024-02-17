#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "sprite.h"
#include "colors.h"
#include "menu.h"


/************\
*  FUNZIONI  *
\************/

short Menu()
{
    short pointerPos = 0; int ch = -1; short i = 0;
    customBorder(0,0,COLUMNS_PER_MAP+2,ROWS_PER_MAP+4,false);
    printMainMenu(1,1, pointerPos);
    short scene=1;short difficulty=-1;
    do{
        
        switch (scene)
        {
            case 1:
            printMainMenu(1,1, pointerPos);
            break;
            case 2:
            printDifficultyMenu(1,1, pointerPos);
            break;
            case 3:
            printScoreMenu(1,1);
            break;        
        }

        if((ch = getch()) != ERR)
        {
            switch (ch)
            {
                case KEY_DOWN:
                case 's':
                case 'S':
                    pointerPos++;
                    if(pointerPos > 2)
                    {
                        pointerPos = 0; 
                    }                     
                break;
                
                case KEY_UP:
                case 'w':
                case 'W':
                    pointerPos--;
                    if(pointerPos < 0)
                    {
                        pointerPos = 2;
                    }
                break;
                
                case KEY_ENTER:
                case '\n':
                case ' ':
                    if(scene==1)
                    {
                        switch (pointerPos)
                        {
                            case 0:
                                scene=2;
                                break;
                            case 1:
                                scene=3;
                                break;
                            case 2:
                                return -1;   
                        }                 
                        break;
                    }
                    else if(scene==2)
                    {
                        switch (pointerPos)
                        {
                        case 0:
                            difficulty=1;
                            break;
                        case 1:
                            difficulty=2;                    
                            break;
                        case 2:
                            difficulty=3;                    
                            break;
                        }  
                    }
                    else
                    {
                        pointerPos=0;
                    }
                break;
                case KEY_BACKSPACE:
                case KEY_LEFT:
                    scene=1;
                    pointerPos = 0;                
                break;
            }
        }    
        refresh();
        usleep(50 * 1000);
    } while (difficulty<=0);
    return difficulty;
}

// Menu principale di avvio.
void printMainMenu(short x, short y, short pointerY)
{   
    short startX = x; short cursorYs[3] = {22, 31, 40};// Cordinate per il cursore
    printBackMenu(x,y,20,28,3);
    CHANGE_COLOR(ERBA);

    // CREDITI    
    mvprintw(ROWS_PER_MAP,COLUMNS_PER_MAP-33,"Roberto A. Usai (60/79/00029)");         
    mvprintw(ROWS_PER_MAP+1,COLUMNS_PER_MAP-33,"              &");                     
    mvprintw(ROWS_PER_MAP+2,COLUMNS_PER_MAP-33,"Riccardo Secchi (60/79/00074)");       
    mvprintw(ROWS_PER_MAP,6, "    Università degli Studi di Cagliari");                
    mvprintw(ROWS_PER_MAP+1,6,"  Informatica Applicata e Data Analytics");             
    mvprintw(ROWS_PER_MAP+2,6,"Sistemi Operativi e Programmazione di Rete ");          

    printFROGGER(2,3);
    printVS(6,14);
    printPLANTS(28,14);
    printCursor(7, cursorYs[pointerY]);    
    printInterfaceMenu(29,22,1);
}

// Interfaccia menu principale di avvio.
void printInterfaceMenu(short x, short y,short z)
{   // Scritta PLAY
    short tmpx=x;
    printInterfacciaP(x,y);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaL(x,y);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaA(x,y,z);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaY(x,y,z);
    x=tmpx;
    y+=INTERFACE_ROWS+1;
    // Scritta SCORE
    printInterfacciaS(x,y,z);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaC(x,y);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaO(x,y);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaR(x,y,z);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaE(x,y,z);
    x=tmpx;
    y+=INTERFACE_ROWS+1;
    // Scritta EXIT
    printInterfacciaE(x,y,z);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaX(x,y);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaI(x,y,z);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaT(x,y);
} 

// Menu difficoltà.
void printDifficultyMenu(short x, short y,short pointerY)
{
    short startX = x; short cursorYs[3] = {21, 31, 41};short cursorXs[3] = {21, 11, 21};    
    printBackMenu(x,y,19,30,2); 
    
    //TITOLO
    printFROGGER(2,2);
    printVS(6,13);
    printPLANTS(28,13);

    printDifficultyInterface(29,21);
    printCursor(cursorXs[pointerY], cursorYs[pointerY]);
}

// Menu Score.
void printScoreMenu(short x, short y)
{
    short cursorYs[3] = {22, 31, 40};
    printBackMenu(x,y,14,34,3);    
    printSCORE(10,3);
    printDifficultyInterface(3,18);
    char easy[4]; char medium[4]; char hard[4];
    GetScore(easy,medium,hard);  
    //Punteggi
    printPoint(65,18,easy,2);    
    printPoint(65,18+SCORE_ROWS+LOW_SPACE,medium,3);
    printPoint(65,18+SCORE_ROWS+SCORE_ROWS+LOW_SPACE+LOW_SPACE,hard,4);
}

void printSCORE(short x, short y)
{
    printS(x,y);
    x+=FROGGER_COLUMNS+FROGGER_SPACE;
    printC(x,y);
    x+=FROGGER_COLUMNS+FROGGER_SPACE;
    printO(x,y);
    x+=FROGGER_COLUMNS+FROGGER_SPACE;
    printR(x,y);   
    x+=FROGGER_COLUMNS+FROGGER_SPACE;
    printE(x,y);
    x+=FROGGER_COLUMNS+FROGGER_SPACE;
    printS(x,y);
}

// Interfaccia menu delle difficoltà
void printDifficultyInterface(short x, short y)
{
    //EASY
    short tmpx=x;
    short z=2;
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaE(x,y,z);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaA(x,y,z);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaS(x,y,z);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaY(x,y,z);

    x=tmpx;
    y+=INTERFACE_ROWS+2;
    z++;
    //MEDIUM
    printInterfacciaM(x,y);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaE(x,y,z);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaD(x,y,z);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaI(x,y,z);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaU(x,y);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaM(x,y);

    //HARD
    x=tmpx;
    y+=INTERFACE_ROWS+2;
    z++;
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaH(x,y);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaA(x,y,z);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaR(x,y,z);
    x+=INTERFACE_COLUMNS+1;
    printInterfacciaD(x,y,z);
}

// Punteggi.
void printPoint(short x, short y, char P[], short z)
{   
    for(int i=0; i<POINT_NUM;i++)
    {           
        switch (P[i])
        {
            case '0':
                printScore0(x,y,z);
            break;
            case '1':
                printScore1(x,y,z);
            break;
            case '2':
                printScore2(x,y,z);
            break;
            case '3':
                printScore3(x,y,z);
            break;
            case '4':
                printScore4(x,y,z);
            break;
            case '5':
                printScore5(x,y,z);
            break;
            case '6':
                printScore6(x,y,z);
            break;
            case '7':
                printScore7(x,y,z);
            break;
            case '8':
                printScore8(x,y,z);
            break;
            case '9':
                printScore9(x,y,z);
        }
        x+=INTERFACE_COLUMNS+1;
    }
}

// FROGGER
void printF(short x, short y)
{   
    CHANGE_COLOR(TITOLOS_E_TITOLOC);
    wchar_t sprite[FROGGER_ROWS][FROGGER_COLUMNS] =
    { 
        {L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'},
        {L'█', L' ', L' ', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'█'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█', L'n', L'n'},
        {L'█', L' ', L' ', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'█', L'n', L'n'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L'▄', L'▄', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}        
    }; 

    for(short i=0; i<FROGGER_ROWS;i++)
    {
        for(short j = 0; j<FROGGER_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);

        }
    }
}

void printR(short x, short y)
{   
    wchar_t sprite[FROGGER_ROWS][FROGGER_COLUMNS] =
    { 
        {L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄', L'n'},  
        {L'█', L' ', L' ', L' ', L'▄', L'▄', L'▄', L'▄', L' ', L' ', L' ', L'█'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█'},  
        {L'█', L' ', L' ', L' ', L'▀', L'▀', L'▀', L'▀', L' ', L' ', L' ', L'█'}, 
        {L'█', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'▄', L'▀', L'▀', L'n'}, 
        {L'█', L' ', L' ', L'▄', L'▄', L'▄', L' ', L' ', L' ', L'▀', L'▄', L'n'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'▀', L'▄', L' ', L' ', L'▀', L'▄'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█'}, 
        {L'█', L'▄', L'▄', L'█', L'n', L'n', L'n', L'n', L'█', L'▄', L'▄', L'█'}
    };

    short colors[FROGGER_ROWS][FROGGER_COLUMNS] =
    {
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,    PRATO_E_TITOLO, 0                },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0, TITOLOS_E_TITOLOC,    PRATO_E_TITOLO,    PRATO_E_TITOLO, 0                },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,    PRATO_E_TITOLO, 0                },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,    PRATO_E_TITOLO, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC}
    };

    for(short i=0; i<FROGGER_ROWS;i++)            
    {
       for(short j = 0; j<FROGGER_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    }  
}

void printO(short x, short y)
{   
    wchar_t sprite[FROGGER_ROWS][FROGGER_COLUMNS] =
    { 
        {L'▄', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄'},         
        {L'█', L' ', L' ', L' ', L'▄', L'▄', L'▄', L'▄', L' ', L' ', L' ', L'█'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█'}, 
        {L'█', L' ', L' ', L' ', L'▀', L'▀', L'▀', L'▀', L' ', L' ', L' ', L'█'}, 
        {L'▀', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀'}           
    };

    short colors[FROGGER_ROWS][FROGGER_COLUMNS] =
    {
        {   PRATO_E_TITOLO, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                  0,                 0,                0,                 0, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                  0,                 0,                0,                 0, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                  0,                 0,                0,                 0, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                  0,                 0,                0,                 0, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                  0,                 0,                0,                 0, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                  0,                 0,                0,                 0, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {   PRATO_E_TITOLO, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   }
    };

    for(short i=0; i<FROGGER_ROWS;i++)            
    {
       for(short j = 0; j<FROGGER_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    }    
}

void printBigG(short x, short y)
{
    wchar_t sprite[FROGGER_ROWS][FROGGER_COLUMNS+1] =
    { 
        {L'▄', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄'},  
        {L'█', L' ', L' ', L' ', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'▄', L'▀', L'▀', L'▀', L'▀', L'▄'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'▀', L'▄', L'▄', L' ', L' ', L'█'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█'}, 
        {L'█', L' ', L' ', L' ', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L' ', L' ', L'█'}, 
        {L'▀', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄',  L'▄',L'▄', L'▄', L'▄', L'▄', L'▀'}             
    };

    short colors[FROGGER_ROWS][FROGGER_COLUMNS+1] =
    {
        {   PRATO_E_TITOLO, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,    PRATO_E_TITOLO, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,    PRATO_E_TITOLO, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {   PRATO_E_TITOLO, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   }
    };

    for(short i=0; i<FROGGER_ROWS;i++)            
    {
       for(short j = 0; j<FROGGER_COLUMNS+1;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    }
}

void printG(short x, short y)
{
    wchar_t sprite[FROGGER_ROWS][FROGGER_COLUMNS] =
    { 
        {L'▄', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄'},  
        {L'█', L' ', L' ', L' ', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'▄', L'▀', L'▀', L'▀', L'▀', L'▄'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'▀', L'▄', L'▄', L' ', L' ', L'█'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█'}, 
        {L'█', L' ', L' ', L' ', L'▀', L'▀', L'▀', L'▀', L'▀', L' ', L' ', L'█'}, 
        {L'▀', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀'}             
    };

    short colors[FROGGER_ROWS][FROGGER_COLUMNS] =
    {
        {   PRATO_E_TITOLO, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,    PRATO_E_TITOLO, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,    PRATO_E_TITOLO, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {   PRATO_E_TITOLO, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   }
    };

    for(short i=0; i<FROGGER_ROWS;i++)            
    {
       for(short j = 0; j<FROGGER_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printE(short x, short y)
{   
    CHANGE_COLOR(TITOLOS_E_TITOLOC);
    wchar_t sprite[FROGGER_ROWS][FROGGER_COLUMNS] =
    { 
        {L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'},
        {L'█', L' ', L' ', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'█'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█', L'n', L'n'},
        {L'█', L' ', L' ', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'█', L'n', L'n'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'},
        {L'█', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'█'}        
    };

    for(short i=0; i<FROGGER_ROWS;i++)
    {
        for(short j = 0; j<FROGGER_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);

        }
    }
}

void printC(short x, short y)
{
    wchar_t sprite[FROGGER_ROWS][FROGGER_COLUMNS] =
    { 
        {L'▄', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄'},  
        {L'█', L' ', L' ', L' ', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L' ', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'}, 
        {L'▀', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀'}             
    };

    short colors[FROGGER_ROWS][FROGGER_COLUMNS] =
    {
        {   PRATO_E_TITOLO, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   },
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {   PRATO_E_TITOLO, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, PRATO_E_TITOLO   }
    };

    for(short i=0; i<FROGGER_ROWS;i++)            
    {
       for(short j = 0; j<FROGGER_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printS(short x, short y)
{
    wchar_t sprite[FROGGER_ROWS][FROGGER_COLUMNS] =
    { 
        {L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'},  
        {L'█', L' ', L' ', L' ', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'█'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n'}, 
        {L'█', L' ', L' ', L' ', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'}, 
        {L'█', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L' ', L' ', L'█'}, 
        {L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█'}, 
        {L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█'}, 
        {L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L' ', L' ', L'█'}, 
        {L'█', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'█'}             
    };

    short colors[FROGGER_ROWS][FROGGER_COLUMNS] =
    {
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC,                 0,                 0,                 0,                 0,                 0,                 0,                 0,                 0},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {                0,                 0,                 0,                 0,                 0,                 0,                 0,                 0, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {                0,                 0,                 0,                 0,                 0,                 0,                 0,                 0, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC},
        {TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC, TITOLOS_E_TITOLOC}
    };

    for(short i=0; i<FROGGER_ROWS;i++)            
    {
       for(short j = 0; j<FROGGER_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printFROGGER(short x, short y)
{
    printF(x,y);
    x+=FROGGER_COLUMNS+FROGGER_SPACE;
    printR(x,y);
    x+=FROGGER_COLUMNS+FROGGER_SPACE;
    printO(x,y);
    x+=FROGGER_COLUMNS+FROGGER_SPACE;
    printBigG(x,y);
    x+=FROGGER_COLUMNS+1+FROGGER_SPACE;
    printG(x,y);
    x+=FROGGER_COLUMNS+FROGGER_SPACE;
    printE(x,y);
    x+=FROGGER_COLUMNS+FROGGER_SPACE;
    printR(x,y);   
}

// VS
void printVS(short x, short y)
{       
    wchar_t sprite[VS_ROWS][VS_COLUMNS] =
    { 
        {L'▄', L'▀', L'▀', L'▀', L'▄', L'n', L'n', L'n', L'▄', L'▀', L'▄', L'n', L'▄', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄', L'n'},
        {L'▀', L'▄', L'▀', L'▄', L'▀', L'▄', L'n', L'▄', L'▀', L'▄', L'▀', L'█', L' ', L'▄', L'▀', L'█', L'▀', L'▀', L'▄', L'▀'},
        {L'n', L'▀', L'▄', L'▀', L'▄', L'▀', L'▄', L'▀', L'▄', L'▀', L'n', L'n', L'▀', L'▄', L'▀', L'▄', L'▀', L'▄', L'n', L'n'},
        {L'n', L'n', L'▀', L'▄', L'▀', L'▄', L'▀', L'▄', L'▀', L'n', L'n', L'n', L'▄', L'n', L'▀', L'▄', L'▀', L'▄', L'▀', L'▄'},
        {L'n', L'n', L'n', L'▀', L'▄', L' ', L'▄', L'▀', L'n', L'n', L'n', L'▀', L'▄', L'▀', L'▀', L'▀', L' ', L'▀', L'▄', L'▀'},
        {L'n', L'n', L'n', L'n', L'n', L'▀', L'n', L'n', L'n', L'n', L'n', L'n', L'n', L'▀', L'▀', L'▀', L'▀', L'▀', L'n', L'n'}        
    };

    short colors[VS_ROWS][VS_COLUMNS] =
    {   
        {ROSA_E_PRATO, ROSA_E_AZZURRO,    ROSA_E_AZZURRO,    ROSA_E_AZZURRO,      ROSA_E_PRATO,                 0,              0,              0,   ROSA_E_PRATO, ROSA_E_AZZURRO, ROSA_E_PRATO,            0,   ROSA_E_PRATO,    ROSA_E_AZZURRO,    ROSA_E_AZZURRO,    ROSA_E_AZZURRO,    ROSA_E_AZZURRO,    ROSA_E_AZZURRO,   ROSA_E_PRATO, 0                },
        {ROSA_E_PRATO, ROSA_E_AZZURRO, CELESTE_E_AZZURRO, CELESTE_E_AZZURRO,    ROSA_E_AZZURRO,      ROSA_E_PRATO,              0,   ROSA_E_PRATO, ROSA_E_AZZURRO, ROSA_E_AZZURRO, ROSA_E_PRATO, ROSA_E_PRATO, ROSA_E_AZZURRO, CELESTE_E_AZZURRO,    ROSA_E_AZZURRO,    ROSA_E_AZZURRO,      ROSA_E_PRATO,      ROSA_E_PRATO, ROSA_E_AZZURRO, ROSA_E_PRATO     },
        {           0,   ROSA_E_PRATO,    ROSA_E_AZZURRO, CELESTE_E_AZZURRO, CELESTE_E_AZZURRO,    ROSA_E_AZZURRO,   ROSA_E_PRATO, ROSA_E_AZZURRO, ROSA_E_AZZURRO,   ROSA_E_PRATO,            0,            0,   ROSA_E_PRATO,    ROSA_E_AZZURRO, CELESTE_E_AZZURRO, CELESTE_E_AZZURRO,    ROSA_E_AZZURRO,      ROSA_E_PRATO,              0, 0                },
        {           0,              0,      ROSA_E_PRATO,    ROSA_E_AZZURRO, CELESTE_E_AZZURRO, CELESTE_E_AZZURRO, ROSA_E_AZZURRO, ROSA_E_AZZURRO,   ROSA_E_PRATO,              0,            0,            0,   ROSA_E_PRATO,                 0,      ROSA_E_PRATO,    ROSA_E_AZZURRO, CELESTE_E_AZZURRO, CELESTE_E_AZZURRO, ROSA_E_AZZURRO, ROSA_E_PRATO     },
        {           0,              0,                 0,      ROSA_E_PRATO,    ROSA_E_AZZURRO,    ROSA_E_AZZURRO, ROSA_E_AZZURRO,   ROSA_E_PRATO,              0,              0,            0, ROSA_E_PRATO, ROSA_E_AZZURRO,    ROSA_E_AZZURRO,    ROSA_E_AZZURRO,    ROSA_E_AZZURRO,    ROSA_E_AZZURRO, CELESTE_E_AZZURRO, ROSA_E_AZZURRO, ROSA_E_PRATO     },
        {           0,              0,                 0,                 0,                 0,      ROSA_E_PRATO,              0,              0,              0,              0,            0,            0,              0,      ROSA_E_PRATO,      ROSA_E_PRATO,      ROSA_E_PRATO,      ROSA_E_PRATO,      ROSA_E_PRATO,              0, 0                }
    };

    for(short i=0; i<VS_ROWS;i++)            
    {
       for(short j = 0; j<VS_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

// PLANTS
void printPlatsP(short x, short y)
{
    wchar_t sprite[PLANTS_ROWS][PLANTS_COLUMNS] =
    { 
        {L'▄', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄', L'n'},
        {L'█', L' ', L' ', L'▄', L'▀', L'▀', L'▄', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'▀', L'▄', L'▄', L'▀', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L' ', L'▄', L'▄', L'▄', L'▄', L'▀', L'n'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'▀', L'▄', L'▄', L'▀', L'n', L'n', L'n', L'n', L'n', L'n'}
    };

    short colors[PLANTS_ROWS][PLANTS_COLUMNS] =
    {   
        { PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_PRATO , 0              },
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,  PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO},
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,  PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO},
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO, 0              },
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,               0,               0,               0,               0,               0, 0              },
        { PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,               0,               0,               0,               0,               0, 0              }
    
    };

    for(short i=0; i<PLANTS_ROWS;i++)            
    {
       for(short j = 0; j<PLANTS_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printPlatsL(short x, short y)
{
    wchar_t sprite[PLANTS_ROWS][PLANTS_COLUMNS] =
    { 
        {L'▄', L'▀', L'▀', L'▄', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄'},
        {L'▀', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀'}        
    };

    short colors[PLANTS_ROWS][PLANTS_COLUMNS] =
    {   
        { PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,               0,               0,               0,               0,               0, 0              },
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,               0,               0,               0,               0,               0, 0              },
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,               0,               0,               0,               0,               0, 0              },
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,               0,               0,               0,               0,               0, 0              },
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_PRATO }, 
        { PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_PRATO }
    };  

    for(short i=0; i<PLANTS_ROWS;i++)            
    {
       for(short j = 0; j<PLANTS_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printPlatsA(short x, short y)
{
    wchar_t sprite[PLANTS_ROWS][PLANTS_COLUMNS] =
    { 
        {L'n', L'▄', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄', L'n'},
        {L'█', L' ', L' ', L'▄', L'▀', L'▀', L'▄', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'▀', L'▄', L'▄', L'▀', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L' ', L'▄', L'▄', L' ', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'█', L' ', L' ', L'█'},
        {L'▀', L'▄', L'▄', L'▀', L' ', L' ', L'▀', L'▄', L'▄', L'▀'}        
    };

    short colors[PLANTS_ROWS][PLANTS_COLUMNS] =
    {   
        {              0,  PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO, 0              },
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,  PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO},
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,  PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO},
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO},
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,               0,               0, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO},
        { PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,               0,               0,  PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO}
    
    };  

    for(short i=0; i<PLANTS_ROWS;i++)            
    {
       for(short j = 0; j<PLANTS_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printPlatsN(short x, short y)
{
    wchar_t sprite[PLANTS_ROWS][PLANTS_COLUMNS] =
    { 
        {L'▄', L'▀', L'▀', L'▄', L'n', L'n', L'▄', L'▀', L'▀', L'▄'},
        {L'█', L' ', L' ', L'▀', L'▄', L'n', L'█', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'▄', L' ', L'▀', L'█', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'█', L'▀', L'▄', L' ', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'█', L'n', L'n', L'█', L' ', L' ', L'█'},
        {L'▀', L'▄', L'▄', L'▀', L'n', L'n', L'▀', L'▄', L'▄', L'▀'} 
    };

    short colors[PLANTS_ROWS][PLANTS_COLUMNS] =
    {           
        { PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,               0,               0,  PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO},
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,               0, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO},
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO},
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO},
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,               0,               0, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO},
        { PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,               0,               0,  PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO}
    };  

    for(short i=0; i<PLANTS_ROWS;i++)            
    {
       for(short j = 0; j<PLANTS_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printPlatsT(short x, short y)
{
    wchar_t sprite[PLANTS_ROWS][PLANTS_COLUMNS] =
    { 
        {L'▄', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄'},
        {L'▀', L'▄', L'▄', L'▄', L' ', L' ', L'▄', L'▄', L'▄', L'▀'},
        {L'n', L'n', L'n', L'█', L' ', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'█', L' ', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'█', L' ', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'▀', L'▄', L'▄', L'▀', L'n', L'n', L'n'}
    };

    short colors[PLANTS_ROWS][PLANTS_COLUMNS] =
    {     
        { PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO},
        { PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO},
        {              0,               0,               0, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,               0,               0,  0             },
        {              0,               0,               0, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,               0,               0,  0             },
        {              0,               0,               0, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,               0,               0,  0             },
        {              0,               0,               0,  PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,               0,               0,  0             }
    
    };  

    for(short i=0; i<PLANTS_ROWS;i++)            
    {
       for(short j = 0; j<PLANTS_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printPlatsS(short x, short y)
{
    wchar_t sprite[PLANTS_ROWS][PLANTS_COLUMNS] =
    { 
        {L'n', L'▄', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄'},
        {L'█', L' ', L' ', L' ', L' ', L' ', L'▄', L'▄', L'▄', L'▀'},
        {L'n', L'▀', L'▄', L' ', L' ', L'▀', L'▄', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'▀', L'▄', L' ', L' ', L'▀', L'▄', L'n'},
        {L'▄', L'▀', L'▀', L'▀', L' ', L' ', L' ', L' ', L' ', L'█'},
        {L'▀', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀', L'n'}
    };

    short colors[PLANTS_ROWS][PLANTS_COLUMNS] =
    {
        {              0,  PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO},
        {PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO},
        {              0,  PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,               0,               0,  0             },
        {              0,               0,               0,  PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,  0             },
        { PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO},
        { PETALI_E_PRATO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO, PETALI_E_CENTRO,  PETALI_E_PRATO,  0             }
    }; 

    for(short i=0; i<PLANTS_ROWS;i++)            
    {
       for(short j = 0; j<PLANTS_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printPLANTS(short x, short y)
{
    printPlatsP(x,y);
    x+=PLANTS_COLUMNS+PLANTS_SPACE;
    printPlatsL(x,y);
    x+=PLANTS_COLUMNS+PLANTS_SPACE;
    printPlatsA(x,y);
    x+=PLANTS_COLUMNS+PLANTS_SPACE;
    printPlatsN(x,y);
    x+=PLANTS_COLUMNS+PLANTS_SPACE;
    printPlatsT(x,y);
    x+=PLANTS_COLUMNS+PLANTS_SPACE;
    printPlatsS(x,y);
}

// CURSORE
void printCursor(short x, short y)
{
    wchar_t sprite[CURSOR_ROWS][CURSOR_COLUMNS] =
    { 
        {L' ', L' ', L' ', L' ', L'n', L'n', L'n', L'n', L' ', L' ',L' ',L' ',L' ',L' ',L' ',L' ',},
        {L'n', L'n', L'n', L'n', L' ', L' ', L' ', L' ', L' ', L' ',L' ',L' ',L'n',L'n',L'n',L'n',},
        {L'n', L'n', L'n', L'n', L' ', L' ', L' ', L' ', L' ', L' ',L' ',L' ',L' ',L' ',L'█',L'█',},
        {L'n', L'n', L'n', L'n', L' ', L' ', L' ', L' ', L' ', L' ',L' ',L' ',L' ',L' ',L' ',L' ',},
        {L'n', L'n', L'n', L'n', L' ', L' ', L' ', L' ', L' ', L' ',L' ',L' ',L' ',L' ',L'▄',L'▄',},
        {L'n', L'n', L'n', L'n', L' ', L' ', L' ', L' ', L' ', L' ',L' ',L' ',L'▄',L'▄',L'▀',L'▀',},
        {L'▀', L'▀', L'▀', L'▀', L'▄', L'▄', L'▄', L'▄', L' ', L' ',L' ',L' ',L'▀',L'▀',L'▀',L'▀',},
        {L'▄', L'▄', L'▄', L'▄', L'n', L'n', L'n', L'n', L'▄', L'▄',L'▄',L'▄',L'▄',L'▄',L'▄',L'▄',}
    };

    short colors[CURSOR_ROWS][CURSOR_COLUMNS] =
    {
        {FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA,            0,            0,            0,            0, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA,   FIUME_E_RANA,   FIUME_E_RANA},
        {           0,            0,            0,            0, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA,            0,            0,              0, 0             },
        {           0,            0,            0,            0, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, EYES_E_CELESTE, EYES_E_CELESTE},
        {           0,            0,            0,            0, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA,   FIUME_E_RANA,  FIUME_E_RANA },
        {           0,            0,            0,            0, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA,           RANA,           RANA},
        {           0,            0,            0,            0, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, EYES_E_CELESTE, EYES_E_CELESTE},
        {FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA,   FIUME_E_RANA,   FIUME_E_RANA},
        {FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA,            0,            0,            0,            0, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA, FIUME_E_RANA,   FIUME_E_RANA,   FIUME_E_RANA}
    };

    for(short i=0; i<CURSOR_ROWS;i++)            
    {
       for(short j = 0; j<CURSOR_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

// INTERFACCE
void printInterfacciaP(short x, short y)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄', L'n'},
        {L'█', L' ', L'▄', L'▀', L'▀', L'▀', L'▄', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L' ', L' ', L'█'},
        {L'█', L' ', L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'n'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'▀', L'▀', L'▀', L'n', L'n', L'n', L'n', L'n', L'n'}    
    };

    short colors[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE,0             },
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, 0            },
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0, 0            },
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0, 0            },
        {NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0,              0,              0,              0, 0            }        
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaL(short x, short y)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'█', L'▀', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'},
        {L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀'}
        
    };

    short colors[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        {NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE}       
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {                   
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaA(short x, short y,short colore)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'n', L'▄', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'},
        {L'█', L' ', L'▄', L'▀', L'▀', L'▀', L'▄', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L' ', L' ', L'█'},
        {L'█', L' ', L'█', L'▀', L'▀', L'▀', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'▀', L'▀', L'▀', L'n', L'n', L'n', L'▀', L'▀', L'▀'}
        
    };

    short colors[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        {              0, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,   NERO_E_BIANCO,   NERO_E_BIANCO,  NERO_E_BIANCO},
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_BIANCO,   NERO_E_BIANCO,  NERO_E_BIANCO},
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,   NERO_E_BIANCO,   NERO_E_BIANCO,  NERO_E_BIANCO},
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,   NERO_E_BIANCO,   NERO_E_BIANCO,  NERO_E_BIANCO},
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_BIANCO,   NERO_E_BIANCO,  NERO_E_BIANCO},
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,   NERO_E_BIANCO,   NERO_E_BIANCO,  NERO_E_BIANCO},
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,   NERO_E_BIANCO,   NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0,              0,  NERO_E_CELESTE,  NERO_E_CELESTE, NERO_E_CELESTE}
    };

    short colorsV[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {
        {              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,    NERO_E_VERDE,    NERO_E_VERDE,   NERO_E_VERDE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,    NERO_E_VERDE,    NERO_E_VERDE,   NERO_E_VERDE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,              0,              0,    NERO_E_VERDE,    NERO_E_VERDE,   NERO_E_VERDE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,    NERO_E_VERDE,    NERO_E_VERDE,   NERO_E_VERDE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,    NERO_E_VERDE,    NERO_E_VERDE,   NERO_E_VERDE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,              0,              0,    NERO_E_VERDE,    NERO_E_VERDE,   NERO_E_VERDE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,              0,              0,    NERO_E_VERDE,    NERO_E_VERDE,   NERO_E_VERDE},
        { NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0,              0,  NERO_E_CELESTE,  NERO_E_CELESTE, NERO_E_CELESTE}
    };

    short colorsR[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {
        {              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,    NERO_E_ROSSO,    NERO_E_ROSSO,   NERO_E_ROSSO},
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,    NERO_E_ROSSO,    NERO_E_ROSSO,   NERO_E_ROSSO},
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,    NERO_E_ROSSO,    NERO_E_ROSSO,   NERO_E_ROSSO},
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,    NERO_E_ROSSO,    NERO_E_ROSSO,   NERO_E_ROSSO},
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,    NERO_E_ROSSO,    NERO_E_ROSSO,   NERO_E_ROSSO},
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,    NERO_E_ROSSO,    NERO_E_ROSSO,   NERO_E_ROSSO},
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,    NERO_E_ROSSO,    NERO_E_ROSSO,   NERO_E_ROSSO},
        { NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0,              0,  NERO_E_CELESTE,  NERO_E_CELESTE, NERO_E_CELESTE}
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {   
                switch (colore)
                {
                case 1:
                    CHANGE_COLOR(colors[i][j]);
                    break;
                case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                    break;
                case 4:
                    CHANGE_COLOR(colorsR[i][j]);
                    break;
                default:
                    break;
                }                
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaY(short x, short y,short colore)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'█', L'▀', L'█', L'n', L'n', L'n', L'█', L'▀', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'▀', L'▄', L' ', L'▀', L'▄', L'▀', L' ', L'▄', L'▀'},
        {L'n', L'n', L'▀', L'▄', L' ', L'▄', L'▀', L'n', L'n'},
        {L'n', L'n', L'n', L'█', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'█', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'▀', L'▀', L'▀', L'n', L'n', L'n'}        
    };

    short colors[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE},
        {              0,              0, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE,              0, 0             },
        {              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0, 0             },
        {              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0, 0             },
        {              0,              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0, 0             }
    };

    short colorsV[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,              0,              0,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,              0,              0,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,              0,              0,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        { NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE},
        {              0,              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE,              0, 0             },
        {              0,              0,              0,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,              0, 0             },
        {              0,              0,              0,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,              0, 0             },
        {              0,              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0, 0             }
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                case 1:
                    CHANGE_COLOR(colors[i][j]);
                    break;
                case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                    break;
                default:
                    break;
                }    
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaS(short x, short y,short colore)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'},
        {L'█', L' ', L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'},
        {L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█', L' ', L'█'},
        {L'n', L'n', L'n', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L' ', L'█'},
        {L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀'}
        
    };

    short colors[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE},
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0,              0},
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        {              0,              0,              0,              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE}
    };

    short colorsV[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,              0,              0,              0,              0,              0},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        { NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        {              0,              0,              0,              0,              0,              0,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        { NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE}
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                case 1:
                    CHANGE_COLOR(colors[i][j]);
                    break;
                case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                    break;
                default:
                    break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaC(short x, short y)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'},
        {L'█', L' ', L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'},
        {L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀'}        
    };

    short colors[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        {NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE}       
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaO(short x, short y)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'▄', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄'},
        {L'█', L' ', L'▄', L'▀', L'▀', L'▀', L'▄', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L' ', L' ', L'█'},
        {L'n', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'n'}
        
    };

    short colors[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        {NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        {             0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0}       
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaR(short x, short y,short colore)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▄'},
        {L'█', L' ', L'▄', L'▀', L'▀', L'▀', L'▄', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L' ', L' ', L'█'},
        {L'█', L' ', L'▄', L'▄', L' ', L'▀', L'█', L'▀', L'n'},
        {L'█', L' ', L'█', L'n', L'▀', L'▄', L' ', L'▀', L'▄'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'▀', L'▀', L'▀', L'n', L'n', L'n', L'▀', L'▀', L'▀'}    
    };

    short colors[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},      
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE,  0            },
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        {NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE}        
    };

    short colorsR[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},      
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE,  0            },
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE}        
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                case 1:
                    CHANGE_COLOR(colors[i][j]);
                    break;
                case 4:
                    CHANGE_COLOR(colorsR[i][j]);
                    break;
                default:
                    break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaE(short x, short y,short colore)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'},
        {L'█', L' ', L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'▀', L'▀', L'▀', L'▀', L'█', L'n', L'n'},
        {L'█', L' ', L'█', L'▀', L'▀', L'▀', L'▀', L'n', L'n'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'n', L'n', L'n'},
        {L'█', L' ', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'},
        {L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀'}
        
    };

    short colors[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0,              0,              0,              0,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        {NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE}       
    };

    short colorsV[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE},
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,              0,              0,              0,              0,              0},
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,              0},
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0},
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,              0,              0,              0,              0,              0},
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        {NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE}       
    };

    short colorsG[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE},
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,              0,              0,              0,              0},
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0},
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0},
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,              0,              0,              0,              0},
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE}       
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                case 1:
                    CHANGE_COLOR(colors[i][j]);
                    break;
                case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                    break;
                case 3:
                    CHANGE_COLOR(colorsG[i][j]);
                default:
                    break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaX(short x, short y)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'█', L'▀', L'▄', L'n', L'n', L'n', L'▄', L'▀', L'█'},
        {L'█', L' ', L'▀', L'▄', L'n', L'▄', L'▀', L' ', L'█'},
        {L'▀', L'▄', L' ', L'▀', L'▄', L'▀', L' ', L'▄', L'▀'},
        {L'n', L'▀', L'▄', L' ', L' ', L' ', L'▄', L'▀', L'n'},
        {L'n', L'█', L' ', L' ', L'▄', L' ', L' ', L'█', L'n'},
        {L'█', L' ', L' ', L'█', L'n', L'█', L' ', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'▀', L'▀', L'n', L'n', L'n', L'n', L'n', L'▀', L'▀'}        
    };

    short colors[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {       
        { NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE,              0,              0,              0, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE,              0, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        {NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE},
        {             0, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE,              0},
        {             0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0},
        { NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE,              0,              0,              0, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO},       
        {NERO_E_CELESTE, NERO_E_CELESTE,              0,              0,              0,              0,              0, NERO_E_CELESTE, NERO_E_CELESTE}
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaI(short x, short y,short colore)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'n', L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'█', L'n'},
        {L'n', L'▀', L'▀', L'█', L' ', L'█', L'▀', L'▀', L'n'},
        {L'n', L'n', L'n', L'█', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'█', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'█', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'█', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'█', L'▀', L'▀', L' ', L'▀', L'▀', L'█', L'n'},
        {L'n', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'n'}        
    };

    short colors[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        { 0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, 0},
        { 0, NERO_E_CELESTE, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE, NERO_E_CELESTE, 0},
        { 0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0, 0},
        { 0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0, 0},
        { 0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0, 0},
        { 0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0, 0},
        { 0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, 0},      
        { 0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, 0}
    };

    short colorsG[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        { 0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, 0},
        { 0, NERO_E_CELESTE, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, NERO_E_CELESTE, 0},
        { 0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0, 0},
        { 0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0, 0},
        { 0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0, 0},
        { 0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0, 0},
        { 0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, 0},      
        { 0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, 0}
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                 switch (colore)
                {
                case 1:
                    CHANGE_COLOR(colors[i][j]);
                    break;
                case 3:
                    CHANGE_COLOR(colorsG[i][j]);
                default:
                    break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaT(short x, short y)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█'},
        {L'▀', L'▀', L'▀', L'█', L' ', L'█', L'▀', L'▀', L'▀'},
        {L'n', L'n', L'n', L'█', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'█', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'█', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'█', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'█', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'n', L'▀', L'▀', L'▀', L'n', L'n', L'n'}        
    };
    
    short colors[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        {  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO},
        { NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE},
        {              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0, 0             },
        {              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0, 0             },
        {              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0, 0             },
        {              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0, 0             },
        {              0,              0,              0,  NERO_E_BIANCO,  NERO_E_BIANCO,  NERO_E_BIANCO,              0,              0, 0             },      
        {              0,              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0, 0             }
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colors[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaM(short x, short y)
{
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'█', L'▀', L'▄', L'n', L'n', L'n', L'▄', L'▀', L'█'},
        {L'█', L' ', L' ', L'▀', L'▄', L'▀', L' ', L' ', L'█'},
        {L'█', L' ', L'█', L'▄', L' ', L'▄', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'▀', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'▀', L'▀', L'▀', L'n', L'n', L'n', L'▀', L'▀', L'▀'}     
    };

    short colorsG[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {
        {  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE,              0,              0,             0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0, NERO_E_CELESTE,             0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,             0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,             0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,             0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        { NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0,             0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE}    
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colorsG[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaD(short x, short y,short colore)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'█', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'█', L'▄'},
        {L'█', L' ', L'█', L'▀', L'▀', L'▀', L'▄', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'▀', L'▀', L'▀', L'▀', L' ', L'▄', L'█'},
        {L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'▀', L'n'}       
    };

    short colorsG[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        { NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0}
    };

    short colorsR[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0}
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                 switch (colore)
                {
                case 3:
                    CHANGE_COLOR(colorsG[i][j]);
                    break;
                case 4:
                    CHANGE_COLOR(colorsR[i][j]);
                default:
                    break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaU(short x, short y)
{   
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'█', L'▀', L'█', L'n', L'n', L'n', L'█', L'▀', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'▀', L'▄', L' ', L'▀', L'▀', L'▀', L' ', L'▄', L'▀'}, 
        {L'n', L'n', L'▀', L'▀', L'▀', L'▀', L'▀', L'n', L'n'}
        
    };

    short colorsG[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {        
        {NERO_E_CELESTE, NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        { NERO_E_GIALLO, NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        { NERO_E_GIALLO, NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        { NERO_E_GIALLO, NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        { NERO_E_GIALLO, NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        { NERO_E_GIALLO, NERO_E_GIALLO,  NERO_E_GIALLO,              0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {NERO_E_CELESTE, NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_CELESTE},
        {             0,             0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0}       
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colorsG[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printInterfacciaH(short x, short y)
{
    wchar_t sprite[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    { 
        {L'█', L'▀', L'█', L'n', L'n', L'n', L'█', L'▀', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'▀', L'▀', L'▀', L'▀', L'▀', L' ', L'█'},
        {L'█', L' ', L'█', L'▀', L'▀', L'▀', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'█', L' ', L'█', L'n', L'n', L'n', L'█', L' ', L'█'},
        {L'▀', L'▀', L'▀', L'n', L'n', L'n', L'▀', L'▀', L'▀'}     
    };

    short colorsR[INTERFACE_ROWS][INTERFACE_COLUMNS] =
    {
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0,              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE}  
    };

    for(short i=0; i<INTERFACE_ROWS;i++)            
    {
       for(short j = 0; j<INTERFACE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                CHANGE_COLOR(colorsR[i][j]);
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

// NUMERI SCORE.
void printScore0(short x, short y,short colore)
{   
    wchar_t sprite[SCORE_ROWS][SCORE_COLUMNS] =
    { 
        {L'n', L'n', L'▄', L'▄', L'▄', L'▄', L'▄', L'n', L'n'},
        {L'▄', L'▀', L' ', L' ', L' ', L' ', L' ', L'▀', L'▄'},
        {L'█', L' ', L' ', L'█', L'▀', L'█', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'█', L'n', L'█', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'█', L'n', L'█', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L' ', L' ', L'█'},
        {L'n', L'▀', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀', L'n'}
    };

    short colorsV[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {             0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        {NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE},
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,              0,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE},
        {             0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, 0             }
    
    };  

    short colorsG[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {             0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        {NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO},
        {             0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, 0             }
    };

    short colorsR[SCORE_ROWS][SCORE_COLUMNS] =
    {
        
        {             0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        {NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO},
        {             0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, 0             }
    
    };

    for(short i=0; i<SCORE_ROWS;i++)            
    {
       for(short j = 0; j<SCORE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                    case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                      break;                
                    case 3:
                    CHANGE_COLOR(colorsG[i][j]);
                      break;
                    case 4:
                    CHANGE_COLOR(colorsR[i][j]);
                    default:
                      break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printScore1(short x, short y,short colore)
{   
    wchar_t sprite[SCORE_ROWS][SCORE_COLUMNS] =
    { 
        {L'n', L'n', L'n', L'n', L'n', L'▄', L'▄', L'n', L'n'},
        {L'n', L'n', L'n', L'▄', L'▀', L' ', L' ', L'█', L'n'},
        {L'n', L'▄', L'▀', L' ', L'▄', L' ', L' ', L'█', L'n'},
        {L'n', L'n', L'▀', L'▀', L'█', L' ', L' ', L'█', L'n'},
        {L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█', L'n'},
        {L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█', L'n'},
        {L'n', L'n', L'n', L'n', L'▀', L'▄', L'▄', L'▀', L'n'}
    };

    short colorsV[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {0,              0,              0,              0,              0, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0},
        {0,              0,              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, 0},
        {0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, 0},
        {0,              0, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, 0},
        {0,              0,              0,              0,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, 0},
        {0,              0,              0,              0,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, 0},
        {0,              0,              0,              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, 0}
    };

    short colorsG[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {0,              0,              0,              0,               0, NERO_E_CELESTE, NERO_E_CELESTE,             0, 0},
        {0,              0,              0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, 0},
        {0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, 0},
        {0,              0, NERO_E_CELESTE, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, 0},
        {0,              0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, 0},
        {0,              0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, 0},
        {0,              0,              0,              0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, 0}

    };

    short colorsR[SCORE_ROWS][SCORE_COLUMNS] =
    {
        
        {0,              0,              0,              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, 0},
        {0,              0,              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, 0},
        {0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, 0},
        {0,              0, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, 0},
        {0,              0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, 0},
        {0,              0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, 0},
        {0,              0,              0,              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, 0}
    };

    for(short i=0; i<SCORE_ROWS;i++)            
    {
       for(short j = 0; j<SCORE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                    case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                      break;                
                    case 3:
                    CHANGE_COLOR(colorsG[i][j]);
                      break;
                    case 4:
                    CHANGE_COLOR(colorsR[i][j]);
                    default:
                      break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printScore2(short x, short y,short colore)
{   
    wchar_t sprite[SCORE_ROWS][SCORE_COLUMNS] =
    { 
        {L'n', L'n', L'▄', L'▄', L'▄', L'▄', L'▄', L'n', L'n'},
        {L'▄', L'▀', L' ', L' ', L' ', L' ', L' ', L'▀', L'▄'},
        {L'▀', L'▄', L'▄', L'▀', L'▀', L'█', L' ', L' ', L'█'},
        {L'n', L'n', L'n', L'▄', L'▀', L' ', L' ', L'▄', L'▀'},
        {L'n', L'▄', L'▀', L' ', L' ', L'▄', L'▀', L'n', L'n'},
        {L'█', L' ', L' ', L' ', L'▀', L'▀', L'▀', L'▀', L'▄'},
        {L'▀', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀'}
    };

    short colorsV[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {             0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,            0, 0             },
        {NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE, NERO_E_CELESTE},
        {NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE, NERO_E_VERDE  },
        {             0,              0,              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE, NERO_E_CELESTE},
        {             0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE,            0, 0             },
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE, NERO_E_CELESTE},
        {NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE, NERO_E_CELESTE}

    };

    short colorsG[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {             0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,            0, 0             },
        {NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,NERO_E_GIALLO, NERO_E_CELESTE},
        {NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,NERO_E_GIALLO,NERO_E_GIALLO  },
        {             0,              0,              0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,NERO_E_GIALLO, NERO_E_CELESTE},
        {             0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE,            0, 0             },
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,NERO_E_GIALLO, NERO_E_CELESTE},
        {NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,NERO_E_GIALLO, NERO_E_CELESTE}

    };

    short colorsR[SCORE_ROWS][SCORE_COLUMNS] =
    {
        
        {             0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,            0, 0             },
        {NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO, NERO_E_CELESTE},
        {NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO, NERO_E_ROSSO  },
        {             0,              0,              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO, NERO_E_CELESTE},
        {             0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE,            0, 0             },
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO, NERO_E_CELESTE},
        {NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO, NERO_E_CELESTE}
    };

    for(short i=0; i<SCORE_ROWS;i++)            
    {
       for(short j = 0; j<SCORE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                    case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                      break;                
                    case 3:
                    CHANGE_COLOR(colorsG[i][j]);
                      break;
                    case 4:
                    CHANGE_COLOR(colorsR[i][j]);
                    default:
                      break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printScore3(short x, short y,short colore)
{   
    wchar_t sprite[SCORE_ROWS][SCORE_COLUMNS] =
    { 
        {L'n', L'n', L'▄', L'▄', L'▄', L'▄', L'▄', L'n', L'n'},
        {L'▄', L'▀', L' ', L' ', L' ', L' ', L' ', L'▀', L'▄'},
        {L'▀', L'▄', L'▄', L'▀', L'█', L'█', L' ', L' ', L'█'},
        {L'n', L'n', L'n', L'█', L' ', L' ', L' ', L'█', L'n'},
        {L'n', L'▄', L'▄', L'n', L'▀', L'█', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L' ', L' ', L'█'},
        {L'n', L'▀', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀', L'n'}
    };

    short colorsV[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {             0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        {NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE},
        {NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {             0,              0,              0,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, 0             },
        {             0, NERO_E_CELESTE, NERO_E_CELESTE,              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE},
        {             0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, 0             }

    };

    short colorsG[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {             0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        {NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        {NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO  },
        {             0,              0,              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, 0             },
        {             0, NERO_E_CELESTE, NERO_E_CELESTE,              0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO  },
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        {             0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, 0             }

    };

    short colorsR[SCORE_ROWS][SCORE_COLUMNS] =
    {
        
        {             0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        {NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {             0,              0,              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, 0             },
        {             0, NERO_E_CELESTE, NERO_E_CELESTE,              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {             0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, 0             }
    };

    for(short i=0; i<SCORE_ROWS;i++)            
    {
       for(short j = 0; j<SCORE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                    case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                      break;                
                    case 3:
                    CHANGE_COLOR(colorsG[i][j]);
                      break;
                    case 4:
                    CHANGE_COLOR(colorsR[i][j]);
                    default:
                      break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printScore4(short x, short y,short colore)
{   
    wchar_t sprite[SCORE_ROWS][SCORE_COLUMNS] =
    { 
        {L'n', L'n', L'n', L'n', L'n', L'▄', L'▄', L'n', L'n'},
        {L'n', L'n', L'n', L'▄', L'▀', L' ', L' ', L'█', L'n'},
        {L'n', L'▄', L'▀', L' ', L' ', L' ', L' ', L'█', L'n'},
        {L'█', L' ', L' ', L'▄', L'█', L' ', L' ', L'█', L'n'},
        {L'█', L' ', L' ', L'▀', L'▀', L' ', L' ', L'▀', L'▄'},
        {L'▀', L'▄', L'▄', L'▄', L'▄', L' ', L' ', L'▄', L'▀'},
        {L' ', L' ', L' ', L' ', L'▀', L'▄', L'▄', L'▀', L'n'}
    };

    short colorsV[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {             0,              0,             0,              0,              0, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        {             0,              0,             0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, 0             },
        {             0, NERO_E_CELESTE,  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, 0             },
        {  NERO_E_VERDE,   NERO_E_VERDE,  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, 0             },
        {  NERO_E_VERDE,   NERO_E_VERDE,  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE},
        {NERO_E_CELESTE,   NERO_E_VERDE,  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE},
        {             0,              0,             0,              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, 0             }

    };

    short colorsG[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {             0,              0,             0,              0,              0, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        {             0,              0,             0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, 0             },
        {             0, NERO_E_CELESTE, NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, 0             },
        { NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, 0             },
        { NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        {NERO_E_CELESTE,  NERO_E_GIALLO, NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        {             0,              0,             0,              0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, 0             }
    };

    short colorsR[SCORE_ROWS][SCORE_COLUMNS] =
    {        
        {             0,              0,             0,              0,              0, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        {             0,              0,             0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, 0             },
        {             0, NERO_E_CELESTE,  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, 0             },
        {  NERO_E_ROSSO,   NERO_E_ROSSO,  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, 0             },
        {  NERO_E_ROSSO,   NERO_E_ROSSO,  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {NERO_E_CELESTE,   NERO_E_ROSSO,  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {             0,              0,             0,              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, 0             }
    };

    for(short i=0; i<SCORE_ROWS;i++)            
    {
       for(short j = 0; j<SCORE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                    case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                      break;                
                    case 3:
                    CHANGE_COLOR(colorsG[i][j]);
                      break;
                    case 4:
                    CHANGE_COLOR(colorsR[i][j]);
                    default:
                      break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printScore5(short x, short y,short colore)
{   
    wchar_t sprite[SCORE_ROWS][SCORE_COLUMNS] =
    { 
        {L'n', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'n'},
        {L'█', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'█', L'█', L'█', L'█', L'▀', L'n'},
        {L'█', L' ', L' ', L' ', L' ', L' ', L' ', L'▀', L'▄'},
        {L'n', L'█', L'█', L'▀', L'▀', L'█', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L' ', L' ', L'█'},
        {L'n', L'▀', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀', L'n'}
    };

    short colorsV[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {             0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, 0             },
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, 0             },
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE},
        {             0,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {  NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {             0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, 0             }

    };

    short colorsG[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {             0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, 0             },
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO },
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, 0             },
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        {             0,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO },
        { NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO },
        {             0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, 0             }
    };

    short colorsR[SCORE_ROWS][SCORE_COLUMNS] =
    {        
        {             0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, 0             },
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, 0             },
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {             0,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {  NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {             0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, 0             }
    };

    for(short i=0; i<SCORE_ROWS;i++)            
    {
       for(short j = 0; j<SCORE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                    case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                      break;                
                    case 3:
                    CHANGE_COLOR(colorsG[i][j]);
                      break;
                    case 4:
                    CHANGE_COLOR(colorsR[i][j]);
                    default:
                      break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printScore6(short x, short y,short colore)
{   
    wchar_t sprite[SCORE_ROWS][SCORE_COLUMNS] =
    { 
        {L'n', L'n', L'▄', L'▄', L'▄', L'▄', L'▄', L'n', L'n'},
        {L'▄', L'▀', L' ', L' ', L' ', L' ', L' ', L'▀', L'▄'},
        {L'█', L' ', L' ', L'█', L'█', L'█', L'▄', L'▄', L'▀'},
        {L'█', L' ', L' ', L' ', L' ', L' ', L' ', L'▀', L'▄'},
        {L'█', L' ', L' ', L'█', L'▀', L'█', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L' ', L' ', L'█'},
        {L'n', L'▀', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀', L'n'}
    };

    short colorsV[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        { NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, 0             }

    };

    short colorsG[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        { NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO },
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO },
        {              0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, 0             }
    };

    short colorsR[SCORE_ROWS][SCORE_COLUMNS] =
    {        
        {              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        { NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, 0             }
    };

    for(short i=0; i<SCORE_ROWS;i++)            
    {
       for(short j = 0; j<SCORE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                    case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                      break;                
                    case 3:
                    CHANGE_COLOR(colorsG[i][j]);
                      break;
                    case 4:
                    CHANGE_COLOR(colorsR[i][j]);
                    default:
                      break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printScore7(short x, short y,short colore)
{   
    wchar_t sprite[SCORE_ROWS][SCORE_COLUMNS] =
    { 
        {L'n', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'▄', L'n'},
        {L'█', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'█'},
        {L'n', L'▀', L'▀', L'▀', L'▀', L'█', L' ', L' ', L'█'},
        {L'n', L'n', L'n', L'n', L'█', L' ', L' ', L'█', L'n'},
        {L'n', L'n', L'n', L'█', L' ', L' ', L'█', L'n', L'n'},
        {L'n', L'n', L'█', L' ', L' ', L'█', L'n', L'n', L'n'},
        {L'n', L'n', L'▀', L'▄', L'▄', L'▀', L'n', L' ', L'n'}
    };

    short colorsV[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {            0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, 0             },
        { NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {            0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {            0,              0,              0,              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, 0             },
        {            0,              0,              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE,              0, 0             },
        {            0,              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE,              0,              0, 0             },
        {            0,              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE,              0,              0, 0             }

    };

    short colorsG[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {            0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, 0             },
        {NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO  },
        {            0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO  },
        {            0,              0,              0,              0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, 0             },
        {            0,              0,              0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE,              0, 0             },
        {            0,              0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE,              0,              0, 0             },
        {            0,              0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE,              0,              0, 0             }
    };

    short colorsR[SCORE_ROWS][SCORE_COLUMNS] =
    {        
        {            0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, 0             },
        { NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {            0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {            0,              0,              0,              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, 0             },
        {            0,              0,              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE,              0, 0             },
        {            0,              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE,              0,              0, 0             },
        {            0,              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE,              0,              0, 0             }
    };

    for(short i=0; i<SCORE_ROWS;i++)            
    {
       for(short j = 0; j<SCORE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                    case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                      break;                
                    case 3:
                    CHANGE_COLOR(colorsG[i][j]);
                      break;
                    case 4:
                    CHANGE_COLOR(colorsR[i][j]);
                    default:
                      break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printScore8(short x, short y,short colore)
{   
    wchar_t sprite[SCORE_ROWS][SCORE_COLUMNS] =
    { 
        {L'n', L'n', L'▄', L'▄', L'▄', L'▄', L'▄', L'n', L'n'},
        {L'▄', L'▀', L' ', L' ', L' ', L' ', L' ', L'▀', L'▄'},
        {L'█', L' ', L' ', L'█', L'▀', L'█', L' ', L' ', L'█'},
        {L'▀', L'▄', L' ', L'▀', L'▀', L'▀', L' ', L'▄', L'▀'},
        {L'█', L' ', L' ', L'█', L'▀', L'█', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L' ', L' ', L'█'},
        {L'n', L'▀', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀', L'n'}
    };

    short colorsV[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        { NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        { NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, 0             }

    };

    short colorsG[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        { NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO },
        { NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO },
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO },
        {              0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, 0             }
    };

    short colorsR[SCORE_ROWS][SCORE_COLUMNS] =
    {        
        {              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        { NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        { NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, 0             }
    };

    for(short i=0; i<SCORE_ROWS;i++)            
    {
       for(short j = 0; j<SCORE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                    case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                      break;                
                    case 3:
                    CHANGE_COLOR(colorsG[i][j]);
                      break;
                    case 4:
                    CHANGE_COLOR(colorsR[i][j]);
                    default:
                      break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

void printScore9(short x, short y,short colore)
{   
    wchar_t sprite[SCORE_ROWS][SCORE_COLUMNS] =
    { 
        {L'n', L'n', L'▄', L'▄', L'▄', L'▄', L'▄', L'n', L'n'},
        {L'▄', L'▀', L' ', L' ', L' ', L' ', L' ', L'▀', L'▄'},
        {L'█', L' ', L' ', L'█', L'▀', L'█', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L' ', L' ', L'█'},
        {L'n', L'█', L'▄', L'▄', L'▄', L'▄', L' ', L' ', L'█'},
        {L'█', L' ', L' ', L'▀', L'▀', L'▀', L' ', L' ', L'█'},
        {L'n', L'▀', L'▄', L'▄', L'▄', L'▄', L'▄', L'▀', L'n'}
    };

    short colorsV[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        { NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE},
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {              0,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_VERDE  },
        {              0, NERO_E_CELESTE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE,   NERO_E_VERDE, NERO_E_CELESTE, 0             }

    };

    short colorsG[SCORE_ROWS][SCORE_COLUMNS] =
    {
        {              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        { NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE},
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO  },
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO  },
        {              0,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO  },
        {  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_GIALLO  },
        {              0, NERO_E_CELESTE,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO,  NERO_E_GIALLO, NERO_E_CELESTE, 0             }
    };

    short colorsR[SCORE_ROWS][SCORE_COLUMNS] =
    {        
        {              0,              0, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE, NERO_E_CELESTE,              0, 0             },
        { NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE},
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {              0,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_ROSSO  },
        {              0, NERO_E_CELESTE,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO,   NERO_E_ROSSO, NERO_E_CELESTE, 0             }
    };

    for(short i=0; i<SCORE_ROWS;i++)            
    {
       for(short j = 0; j<SCORE_COLUMNS;j++)
        {   
            if(sprite[i][j]!= 'n')
            {    
                switch (colore)
                {
                    case 2:
                    CHANGE_COLOR(colorsV[i][j]);
                      break;                
                    case 3:
                    CHANGE_COLOR(colorsG[i][j]);
                      break;
                    case 4:
                    CHANGE_COLOR(colorsR[i][j]);
                    default:
                      break;
                }  
                mvprintw(y+i,x+j,"%lc",sprite[i][j]);
            }
        }
    } 
}

//FILE
void GetScore(char easy[],char medium[], char hard[])
{
    FILE *file=fopen("SCORE.txt", "r");
    if(file!=NULL)
    {
        fscanf(file,"%c%c%c%c-%c%c%c%c-%c%c%c%c",&easy[0],&easy[1],&easy[2],&easy[3],&medium[0],&medium[1],&medium[2],&medium[3],&hard[0],&hard[1],&hard[2],&hard[3]);
    }
    else
    {   
        file=fopen("SCORE.txt", "w");
        fprintf(file,"0000-0000-0000");
        for(short i=0;i<4;i++)
        {
            easy[i]='0';
        }
        for(short i=0;i<4;i++)
        {
            medium[i]='0';
        }
        for(short i=0;i<4;i++)
        {
            hard[i]='0';
        }
    }    
    fclose(file);   
}

void SetScore(char easy[],char medium[], char hard[])
{
    FILE *file=fopen("SCORE.txt", "w");    
    if (file != NULL) 
    {       
        fprintf(file,"%c%c%c%c-",easy[0],easy[1],easy[2],easy[3]);    
        fprintf(file,"%c%c%c%c-",medium[0],medium[1],medium[2],medium[3]);   
        fprintf(file,"%c%c%c%c",hard[0],hard[1],hard[2],hard[3]);
    }
    fclose(file);  
}

short charToShort(char array[])
{
    short n = 0;
    
    n += (array[0] - 48) * 1000;
    n += (array[1] - 48) * 100;
    n += (array[2] - 48) * 10;
    n += (array[3] - 48);
    
    return n;
}

//SFONDO
void printBackMenu(short x, short y, short topGrass, short river,short bottomGrass)
{
    short startX = x; 
    CHANGE_COLOR(ERBA);
    for(short i = 0; i < topGrass; i++)
    {
        for(short j = 0; j < COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS; j++)
        {
            mvprintw(y, x, " ");
            x++;
        }
        y++;
        x = startX;
    } 
    CHANGE_COLOR(FIUME);
    for(short i = 0; i < river; i++)
    {
        for(short j = 0; j < COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS; j++)
        {
            mvprintw(y, x, " ");
            x++;
        }
        y++;
        x = startX;
    }
    CHANGE_COLOR(ERBA);
    for(short i = 0; i < bottomGrass; i++)
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
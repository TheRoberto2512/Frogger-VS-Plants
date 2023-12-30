#include <curses.h>
#include "colors.h"


/******************\
*  COSTANTI MAPPA  *
\******************/

// Numero di righe occupate dalle tane.
#define LILY_PADS_ROWS 1

// Numero di righe oltre il fiume (tane escluse).
#define RIVERSIDE_ROWS 2

// Numero di corsie del fiume.
#define RIVER_ROWS 8

// Numero di righe del marciapiede (spawn della rana).
#define SIDEWALK_ROWS 1

// Numero di righe per ogni blocco.
#define ROWS_PER_BLOCK 4

// Numero di colonne per ogni blocco.
#define COLUMNS_PER_BLOCK 7

// Numero di colonne per il separatore tra righe oltre il fiume e tane.
#define ROWS_PER_SEPARATOR 1

// Numero di "blocchi" (colonne) per ogni riga.
#define BLOCK_PER_MAP_ROWS 15

// Numero totale di righe (in caratteri) della mappa di gioco.
#define ROWS_PER_MAP (LILY_PADS_ROWS + RIVERSIDE_ROWS + RIVER_ROWS + SIDEWALK_ROWS) * ROWS_PER_BLOCK + ROWS_PER_SEPARATOR


/********************\
*  COSTANTI ENTITA'  *
\********************/

// Numero di colonne per la stampa delle tane.
#define LILY_PADS_COLUMNS 9

// Numero di colonne per la stampa dei coccodrilli.
#define CROCODILE_COLUMNS 21

// Numero di colonne per la stampa dei nemici.
#define ENEMY_COLUMNS 7

// Numero di colonne per la stampa della rana.
#define FROG_COLUMNS 7

// Numero di colonne per la stampa del coccodrillo in background.
#define BC_CROC_COLUMNS 19


/*************************\
*  PROTOTIPI DI FUNZIONE  *
\*************************/

// Stampa tutta la scoreboard nella finestra indicata.
void printScoreBoard(WINDOW *win, short lives, short score, short time, short fullTime);

// Stampa tutta la mappa nella finestra indicata. Gli ultimi due booleani servono rispettivamente per decidere se aggiornare o no anche la parte superiore della mappa (le tane) e la parte inferiore (il marciapiede).
void printMap(WINDOW *win, bool top, bool bottom, bool lilyPads[], bool newBG);

// Stampa la riga contenene le tane della rana (implementata dentro printMap).
void printLilyPads(WINDOW *win, short x, short y, bool lilyPads[], short crocX,short direction);

// Stampa la una singola tana (implementata dentro printLilyPads).
void printSingleLilyPad(WINDOW *win, short x, short y, bool empty);

// Stampa una riga di terreno, usata per marciapiede e sponda (implementata dentro printMap).
void printGroundLine(WINDOW *win, short x, short y);

// Stampa una riga di fiume.
void printRiverLine(WINDOW *win, short x, short y);

// Stampa la rana.
void printFrog(WINDOW *win, short x, short y);

// Stampa un coccodrillo (direction == 0 -> sinistra, direction == 1 -> destra).
void printCrocodile(WINDOW *win, short x, short y, short direction);

// Stampa un coccodrillo cattivo (direction == 0 -> sinistra, direction == 1 -> destra).
void printBadCrocodile(WINDOW *win, short x, short y, short direction);

// Stampa un fiore nemico.
void printEnemy(WINDOW *win, short x, short y);

// Stampa un coccodrillo di background sotto le tane.
void printBGCrocodile(WINDOW *win, short x, short y, short direction);
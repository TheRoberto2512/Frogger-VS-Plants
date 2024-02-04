#include <curses.h>
#include "colors.h"


/******************\
*  COSTANTI MAPPA  *
\******************/

// Numero di righe occupate dalle tane.
#define SCOREBOARD_ROWS ((short)4)

// Numero di righe occupate dalle tane.
#define LILY_PADS_ROWS ((short)1)

// Numero di righe oltre il fiume (tane escluse).
#define RIVERSIDE_ROWS ((short)2)

// Numero di corsie del fiume.
#define RIVER_ROWS ((short)8)

// Numero di righe del marciapiede (spawn della rana).
#define SIDEWALK_ROWS ((short)1)

// Numero di righe per ogni blocco.
#define ROWS_PER_BLOCK ((short)4)

// Numero di colonne per ogni blocco.
#define COLUMNS_PER_BLOCK ((short)7)

// Numero di colonne per il separatore tra righe oltre il fiume e tane.
#define ROWS_PER_SEPARATOR ((short)1)

// Numero di "blocchi" (colonne) per ogni riga.
#define BLOCK_PER_MAP_ROWS ((short)15)

// Numero di caratteri per ogni riga.
#define COLUMNS_PER_MAP ((short)(COLUMNS_PER_BLOCK * BLOCK_PER_MAP_ROWS))

// Numero totale di righe (in caratteri) della mappa di gioco.
#define ROWS_PER_MAP (LILY_PADS_ROWS + RIVERSIDE_ROWS + RIVER_ROWS + SIDEWALK_ROWS) * ROWS_PER_BLOCK + ROWS_PER_SEPARATOR


/********************\
*  COSTANTI ENTITA'  *
\********************/

// Numero di colonne per la stampa delle tane.
#define LILY_PADS_COLUMNS ((short)9)

// Numero di colonne per la stampa dei coccodrilli.
#define CROCODILE_COLUMNS (short)(21)

// Numero di colonne per la stampa dei nemici.
#define ENEMY_COLUMNS (short)(7)

// Numero di colonne per la stampa della rana.
#define FROG_COLUMNS (short)(7)

// Numero di colonne per la stampa del coccodrillo in background.
#define BC_CROC_COLUMNS (short)(19)

// Numero di colonne per la stampa dei nemici.
#define DANGER_SIGN_COLUMNS (short)(7)


/*********************************\
*  PROTOTIPI DI FUNZIONE (MAPPA)  *
\*********************************/

// Stampa un bordo custom.
void customBorder(short x, short y, short top, short side, bool separator);

// Stampa tutta la scoreboard nella finestra indicata.
void printScoreBoard(short lives, short score, short time, short fullTime);

// Stampa tutta la mappa nella finestra indicata. Gli ultimi due booleani servono rispettivamente per decidere se aggiornare o no anche la parte superiore della mappa (le tane) e la parte inferiore (il marciapiede).
void printMap(bool bottom, bool lilyPads[], bool newBG);

// Stampa una riga di terreno, usata per marciapiede e sponda (implementata dentro printMap).
void printGroundLine(short x, short y);

// Stampa una riga di fiume.
void printRiverLine(short x, short y);

// Stampa la riga contenene le tane della rana (implementata dentro printMap).
void printLilyPads(short x, short y, bool lilyPads[], short crocX, short direction);

// Stampa la una singola tana (implementata dentro printLilyPads).
void printSingleLilyPad(short x, short y, bool empty);


/***********************************\
*  PROTOTIPI DI FUNZIONE (ENTITA')  *
\***********************************/

// Stampa la rana.
void printFrog(short x, short y);

// Stampa un coccodrillo (direction == 0 -> sinistra, direction == 1 -> destra).
void printCrocodile(short x, short y, short direction);

// Stampa un coccodrillo cattivo (direction == 0 -> sinistra, direction == 1 -> destra).
void printBadCrocodile(short x, short y, short direction);

// Stampa un fiore nemico.
void printEnemy(short x, short y, short genTime);

// Stampa un coccodrillo di background sotto le tane.
void printBGCrocodile(short x, short y, short direction);

// Stampa un proiettile.
void printProjectile(short x, short y, bool fromFrog);

// Stampa un segnale di allarme.
void printDangerSign(short direction, short y);
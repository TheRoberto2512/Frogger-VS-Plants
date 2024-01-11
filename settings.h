#include <curses.h>

// Struttura per gestire le regole di gioco.
#ifndef RULES_GUARD
#define RULES_GUARD
typedef struct gameRules
{
    short time;
    bool enemies;
    float BadCrocodile;
    short speed;
} GameRules;
#endif

#ifndef GAMEUPDATES_GUARD
#define GAMEUPDATES_GUARD
typedef struct gameUpdates
{
    short lives;
    short score;
} GameUpdates;
#endif

// Costante per riferirisi alla difficolta' facile.
#define LIVES 3

// Costante per riferirisi alla difficolta' facile.
#define EASY 1

// Costante per riferirisi alla difficolta' intermedia.
#define MEDIUM 2

// Costante per riferirisi alla difficolta' difficile.
#define HARD 3

// Costante per definire il numero di secondi della barra.
#define FULL_TIME 30

// Funzione che restituisce le impostazioni in base alla difficolta'.
GameRules getRules(short difficult);


/************\
*  PUNTEGGI  *
\************/

// Punteggio da assegnare quando la rana avanza di una riga.
#define ROW_UP 10

// Punteggio da assegnare quando viene riempita una tana.
#define FILLED_LILYPAD 50

// Punteggio da assegnare per ogni secondo avanzato alla fine della manche.
#define POINTS_PER_SECOND 20

// Punteggio da assegnare quando tutte le tane sono riempite.
#define VICTORY 1000


/**********\
*  REGOLE  *
\**********/

// Tempo (in secondi) per la difficolta' facile.
#define EASY_TIME 30

// Regola lo spawn dei nemici per la difficolta' facile.
#define EASY_ENEMIES false

// Regola lo spawn-rate dei coccodrilli cattivi.
#define EASY_BAD_CROCODILE 0 // (non ci saranno coccodrilli cattivi)

// Velocita' massima di movimento dei coccodrilli.
#define EASY_SPEED 2


// Tempo (in secondi) per la difficolta' intermedia.
#define MEDIUM_TIME 30

// Regola lo spawn dei nemici per la difficolta' intermedia.
#define MEDIUM_ENEMIES true

// Regola lo spawn-rate dei coccodrilli cattivi.
#define MEDIUM_BAD_CROCODILE 4 // (1/4 saranno coccodrilli cattivi)

// Velocita' massima di movimento dei coccodrilli.
#define MEDIUM_SPEED 2


// Tempo (in secondi) per la difficolta' difficile.
#define HARD_TIME 25

// Regola lo spawn dei nemici per la difficolta' difficile. 
#define HARD_ENEMIES true

// Regola lo spawn-rate dei coccodrilli cattivi.
#define HARD_BAD_CROCODILE 3 // (1/3 saranno coccodrilli cattivi)

// Velocita' massima di movimento dei coccodrilli.
#define HARD_SPEED 1

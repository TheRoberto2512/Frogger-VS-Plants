#include <curses.h>

#include "sprite.h"


/*********\
*  MACRO  *
\*********/

// Macro per rilevare collisioni tra la rana e un nemico.
#define frogEnemyCD(frogX, frogY, enemyX, enemyY) entityCollisionDetector(frogX, frogY, FROG_COLUMNS, enemyX, enemyY, ENEMY_COLUMNS)

// Macro per rilevare collisioni tra due nemici (serve per la loro generazione).
#define enemyEnemyCD(enemyX, enemyY, enemy2X, enemy2Y) entityCollisionDetector(enemyX, enemyY, ENEMY_COLUMNS, enemy2X, enemy2Y, ENEMY_COLUMNS)

// Macro per rilevare collisioni tra la rana e un coccodrillo.
#define frogCrocodileCD(frogX, frogY, crocX, crocY) entityCollisionDetector(frogX, frogY, FROG_COLUMNS, crocX, crocY, CROCODILE_COLUMNS)

// Macro per rilevare collisioni tra la rana e un proiettile nemico.
#define frogEnemyProjCD(frogX, frogY, projX, projY) entityAndProjectileCollisionDetector(frogX, frogY, FROG_COLUMNS, ROWS_PER_BLOCK, projX, projY)

// Macro per rilevare collisioni tra un nemico e un proiettile rana.
#define enemyFrogProjCD(enemyX, enemyY, projX, projY) entityAndProjectileCollisionDetector(enemyX, enemyY, ENEMY_COLUMNS, ROWS_PER_BLOCK, projX, projY)

// Macro per rilevare collisioni tra un coccodrillo e un proiettile rana.
#define crocFrogProjCD(crocX, crocY, fProjX, fProjY) entityAndProjectileCollisionDetector(crocX, crocY, CROCODILE_COLUMNS, ROWS_PER_BLOCK, fProjX, fProjY)


/*************************\
*  PROTOTIPI DI FUNZIONE  *
\*************************/

// Funzione generale di rilevazione delle collisioni tra due entita' (1) e (2).
bool entityCollisionDetector(short x1, short y1, short top1, short x2, short y2, short top2);

// Funzione generale per la rilevazione delle collisioni tra un'entita' (1) e un proiettile (2).
bool entityAndProjectileCollisionDetector(short x1, short y1, short top1, short side1, short x2, short y2);

// Funzione generale per la rilevazione delle collisioni tra due proiettili.
bool frogProjectileEnemyProjectileCollisionDetector(short frogProjX, short frogProjY, short enemyProjX, short enemyProjY);

// Funzione per la rilevazione dell'entrata della rana all'interno di una tana vuota.
bool isFrogEnteredInside(short frogX, short frogTop, short lilyPadX, short lilyPadTop);
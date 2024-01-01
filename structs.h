#ifndef FROG_GUARD
#define FROG_GUARD
typedef struct frog
{
    short x;            // coordinata x
    short y;            // coordinata y
} Frog;
#endif

#ifndef CROC_GUARD
#define CROC_GUARD
typedef struct crocodile
{
    short x;            // coordinata x
    short y;            // coordinata y
    short direction;    // direzione
    short splash;       // conto alla rovescia prima di affondare (-10 se coccodrillo buono)
} Crocodile;
#endif

#ifndef ENEMY_GUARD
#define ENEMY_GUARD
typedef struct enemy
{
    short x;            // coordinata x
    short y;            // coordinata y
    short shot;         // conto alla rovescia prima di sparare un proiettile
} Enemy;
#endif

#ifndef PROJ_GUARD
#define PROJ_GUARD
typedef struct projectile
{
    short x;            // coordinata x
    short y;            // coordinata y
    short speed;        // velocita' del proiettile
} Projectile;
#endif

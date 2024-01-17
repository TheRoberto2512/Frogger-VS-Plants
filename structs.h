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
    pid_t PID;          // per identificare il coccodrillo
    short x;            // coordinata x
    short y;            // coordinata y
    short direction;    // direzione
    short speed;        // velocita' di aggiornamento
    short splash;       // conto alla rovescia prima di affondare (-10 se coccodrillo buono)
} Crocodile;
#endif

#ifndef ENEMY_GUARD
#define ENEMY_GUARD
typedef struct enemy
{
    pid_t PID;          // pid del processo nemico
    short ID;           // numero del nemico
    short x;            // coordinata x
    short y;            // coordinata y
    short shot;         // conto alla rovescia prima di sparare un proiettile
    short genTime;      // conto alla rovescia prima di generarsi completamente
} Enemy;
#endif

#ifndef PROJ_GUARD
#define PROJ_GUARD
typedef struct projectile
{
    pid_t PID;          // sorgente (0 per la rana, wxyz per i nemici)
    short FID;          // identificatore del padre
    short ID;           // identificatore del proiettile
    short x;            // coordinata x
    short y;            // coordinata y
    short speed;        // velocita' del proiettile
} Projectile;
#endif

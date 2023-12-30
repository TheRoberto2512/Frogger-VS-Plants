
typedef struct frog
{
    short x;            // coordinata x
    short y;            // coordinata y
} Frog;

typedef struct crocodile
{
    short x;            // coordinata x
    short y;            // coordinata y
    short direction;    // direzione
    short splash;       // conto alla rovescia prima di affondare (-10 se coccodrillo buono)
} Crocodile;

typedef struct enemy
{
    short x;            // coordinata x
    short y;            // coordinata y
    short shot;         // conto alla rovescia prima di sparare un proiettile
} Enemy;

typedef struct projectile
{
    short x;            // coordinata x
    short y;            // coordinata y
    short speed;        // velocita' del proiettile
} Projectile;


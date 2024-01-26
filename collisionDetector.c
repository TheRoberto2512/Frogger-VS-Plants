#include "collisionDetector.h"


/************\
*  FUNZIONI  *
\************/

bool entityCollisionDetector(short x1, short y1, short top1, short x2, short y2, short top2)
{
    bool collisionDetected = false;

    if(y1 == y2) // in un gioco strutturato a blocchi, l'altezza deve essere la stessa per collidere 
    {
        if ( ( ( x1 + (top1-1) ) >= x2 ) && ( x1 < ( x2 + (top2-1) ) ) )
        {
            collisionDetected = true;

            // Se l'estremo destro del primo viene dopo l'estremo sinistro del secondo ...
            // e l'estremo sinistro del primo viene prima dell'estremo destro del secondo.
        }
        else if ( ( ( x2 + (top2-1) ) >= x1 ) && ( x2 < ( x1 + (top1-1) ) ) )
        {
            collisionDetected = true;

            // Se l'estremo destro del secondo viene dopo l'estremo sinistro del primo ...
            // e l'estremo destro del primo viene prima dell'estremo sinistro del secondo.
        } 
    }

    return collisionDetected;
}

bool entityAndProjectileCollisionDetector(short x1, short y1, short top1, short side1, short x2, short y2)
{
    if( ( x2 >= x1 && x2 <= (x1+top1-1) ) && ( y2 >= y1 && y2 <= (y1+side1-1) ) )
    {
        // Se x2 e' inclusa tra x1 (estremo sinistro) e x1+top-1 (estremo destro) e
        // y2 e' inclusa tra y1 (estremo superiore) e y1+side-1 (estremo inferiore).

        return true;
    }
    return false;
}

bool frogProjectileEnemyProjectileCollisionDetector(short frogProjX, short frogProjY, short enemyProjX, short enemyProjY)
{
    if( ( frogProjX == enemyProjX ) && ( frogProjY <= enemyProjY ) )
    {
        // Dato che i proiettili solo entita' 1x1, affinche' si verifichi una collisione
        // entrambe le coordinate (x e y) di ogni proiettile devono avere gli stessi valori.

        // Verifichiamo (frogProjY <= enemyProjY) invece che (frogProjY == enemyProjY) per
        // garantire una rilevazione piu' precisa. Proiettili a velocita' diverse potrebbero
        // collidere logicamente (se l'ascissa e' la stessa collidono per forza) ma mai
        // matematicamente (potrebbe non esserci il momento esatto in cui le due Y si eguagliano).

        return true;
    }
    return false;
}

bool isFrogEnteredInside(short frogX, short frogTop, short lilyPadX, short lilyPadTop)
{
    // diamo per scontato che le Y corrispondano (la verifica sara' fatta dalla funzione chiamante)
    short extra_columns = 2; // colonne "extra" per l'approssimazione  
    short r = (lilyPadX + (lilyPadTop-1) + 2) - (frogTop-1);
    if( ( frogX >= ( lilyPadX - extra_columns ) ) && ( frogX < r ) )
    {
        return true;
    }
    return false;
}
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "crocList.h"
#include "sprite.h"
#include "collisionDetector.h"

// La si vuole "nascosta" e utilizzabile solo dalle funzioni di questo file.
void KillR(CrocElement* elem, pid_t crocPid);

void Update(CrocList* list, short lane, Crocodile croc, short updateTime)
{
    void UpdateR(CrocElement* list, Crocodile croc, short updateTime);
    if(list->lanes[lane] == NULL) // se non esiste lo crea
    {
        list->lanes[lane] = (CrocElement*) malloc(sizeof(CrocElement));
        list->lanes[lane]->croc = croc;
        list->lanes[lane]->next = NULL;
        list->lanes[lane]->lastUpdate = updateTime;
    }
    else if(list->lanes[lane]->croc.PID == croc.PID) // aggiorna il suo stato
    {
        list->lanes[lane]->lastUpdate = updateTime;
        list->lanes[lane]->croc = croc;
    }
    else // fa una ricerca piu' approfondita
    {
        UpdateR(list->lanes[lane], croc, updateTime);  
    }

    CrocElement* c = list->lanes[lane]; short cont = 0;
    while(c != NULL)
    {
        cont++;
        c = c->next;
    }
    list->counts[lane] = cont;
}

void UpdateR(CrocElement* elem, Crocodile croc, short updateTime)
{
    if(elem->next == NULL) // se non esiste lo crea
    {
        elem->next = (CrocElement*) malloc(sizeof(CrocElement));
        elem->next->croc = croc; 
        elem->next->next = NULL; 
        elem->next->lastUpdate = updateTime; 
    }
    else if(elem->next->croc.PID == croc.PID) // aggiorna il suo stato
    {
        elem->next->croc = croc;
        elem->next->lastUpdate = updateTime;
    }
    else // continua la ricerca
    {
        UpdateR(elem->next, croc, updateTime);
    }
}

void Kill(CrocList* list, short lane, pid_t crocPid)
{
    if(list->lanes[lane] != NULL) // esiste almeno un elemento
    {
        if(list->lanes[lane]->croc.PID == crocPid) // controlliamo se e' lui che cerchiamo
        {
            CrocElement* x = list->lanes[lane]; // salviamo l'elemento da eliminare

            if(list->lanes[lane]->next != NULL) // se ha un successore
                list->lanes[lane] = list->lanes[lane]->next; // avanza
            else
                list->lanes[lane] = NULL; // altrimenti nulla

            kill(crocPid, SIGTERM);           // uccidiamo il croc selezionato
            int status;
            waitpid(crocPid, &status, 0);     // eseguiamo la wait per liberare le risorse

            free(x); // de-allochiamo la memoria allocata
        }
        else if(list->lanes[lane]->next->croc.PID == crocPid) // se il croc e' il successore
        {
            CrocElement* x = list->lanes[lane]->next;

            list->lanes[lane]->next = list->lanes[lane]->next->next;

            kill(crocPid, SIGTERM);           // uccidiamo il croc selezionato
            int status;
            waitpid(crocPid, &status, 0);     // eseguiamo la wait per liberare le risorse

            free(x); // de-allochiamo la memoria allocata
        }
        else if(list->lanes[lane]->next->next != NULL) // se esiste un terzo successore
        {
            KillR(list->lanes[lane]->next, crocPid);
        }
    }
}

void KillR(CrocElement* elem, pid_t crocPid)
{
    if(elem->next != NULL)
    {
        if(elem->next->croc.PID == crocPid)
        {
            CrocElement* x = elem->next;

            elem->next = elem->next->next;

            kill(crocPid, SIGTERM);           // uccidiamo il croc selezionato
            int status;
            waitpid(crocPid, &status, 0);     // eseguiamo la wait per liberare le risorse

            free(x); // de-allochiamo la memoria allocata
        }
        else if(elem->next->next != NULL)
        {
            KillR(elem->next, crocPid);
        }
    }
}

void killAll(CrocList* list)
{
    for(short i = 0; i < RIVER_ROWS; i++)
    {
        while(list->lanes[i] != NULL)
        {
            Kill(list, i, list->lanes[i]->croc.PID);
        }
    }
}

void FixLane(CrocList* list, short lane)
{
    /*
    FILE *fp = fopen("file.txt", "w");
    CrocElement *c = list->lanes[lane];
    while(c != NULL)
    {
        //Kill(list, lane, list->lanes[lane]->croc.PID);
        fprintf(fp, "-------------------------------\n");
        fprintf(fp, "[%d] %d . %d . %d\n", lane, c->croc.PID, c->croc.x, c->croc.y);
        c = c->next;
    }
    fclose(fp);
    */
    
    int cont = 0;

    CrocElement* c = list->lanes[lane];                         // pesca il primo
    while (c->croc.x >= RIGHT_LIMIT || c->croc.x <= LEFT_LIMIT) // se sgarra
    {
        pid_t toKill = c->croc.PID;                             // salva il pid
        c = c->next;                                            // va avanti al secondo
        Kill(list, lane, toKill);                               // uccide il primo
        cont++;
    }
    // una volta uscito da qui, c rispetta i limiti ma potrebbe avere successori che sgarrano
    do
    {
        if(c->next != NULL) // se ha un successore
        {
            if(c->next->croc.x >= RIGHT_LIMIT || c->next->croc.x <= LEFT_LIMIT) // controlliamo il successore
            {
                pid_t toKill = c->next->croc.PID; 
                KillR(c, toKill); cont++;
            }
            else
            {
                c = c->next;
            }
        }
    } while (c->next != NULL);

    /*
    if(cont > 0)
    {
        FILE *f = fopen("report.txt", "a");
        fprintf(f, "-------------------------------\n");
        fprintf(f, "[%d] Ho ucciso %d coccodrilli\n", lane, cont);
        fclose(f);
    }
    */   

}

void printList(CrocList *list)
{
    void printListR(CrocElement *Elem);
    for(short i = 0; i < RIVER_ROWS; i++)
    {
        if(list->lanes[i] != NULL)
        {
            printListR(list->lanes[i]);
        }
    }
}

void printListR(CrocElement *Elem)
{
    if(Elem->croc.splash == -10)
    {
        printCrocodile(Elem->croc.x, Elem->croc.y, Elem->croc.direction);
    }
    else
    {
        printBadCrocodile(Elem->croc.x, Elem->croc.y, Elem->croc.direction);
    }

    if(Elem->next != NULL)
    {
        printListR(Elem->next);
    }
} 

void DeleteUnnecessary(CrocList *list, short updateTime)
{
    void DeleteUnnecessaryR(CrocElement *Elem, short updateTime);
    for(short i = 0; i < RIVER_ROWS; i++)
    {
        if(list->lanes[i] != NULL)
        {
            DeleteUnnecessaryR(list->lanes[i], updateTime);
            if(updateTime - list->lanes[i]->lastUpdate >= SKIPPED_UPDATES)
            {
                Kill(list, i, list->lanes[i]->croc.PID);
            }
        }
    }
}

void DeleteUnnecessaryR(CrocElement *Elem, short updateTime)
{
    if(Elem->next != NULL)
    {
        DeleteUnnecessaryR(Elem->next , updateTime);
        if(updateTime - Elem->next->lastUpdate >= SKIPPED_UPDATES)
        {
            KillR(Elem, Elem->next->croc.PID);
        }
    }
}
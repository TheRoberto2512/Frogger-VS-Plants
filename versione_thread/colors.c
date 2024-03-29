#include <curses.h>
#include "colors.h"

void inizializzaColore(int N, int r, int g, int b)
{
    init_color(N, r * CONVERSION_CONSTANT, g * CONVERSION_CONSTANT, b * CONVERSION_CONSTANT);
}

void istanziaColori()
{
    // per il fiume
    inizializzaColore(CELESTE, 153, 205, 255);
    inizializzaColore(BLU, 42, 73, 130);
    init_pair(FIUME, BLU, CELESTE);

    // per il prato
    inizializzaColore(ERBA, 68, 248, 99);
    inizializzaColore(VERDE_SCURO, 0, 128, 0);
    init_pair(PRATO, VERDE_SCURO, ERBA);

    // per le tane
    inizializzaColore(VERDE_CHIARO, 0, 204, 0);
    init_pair(PRATO_E_ACQUA, ERBA, CELESTE);
    init_pair(ACQUA_E_BORDO, VERDE_SCURO, CELESTE);
    init_pair(NINFEA, VERDE_SCURO, VERDE_CHIARO);

    // per il coccodrillo buono
    inizializzaColore(NERO, 0, 0, 0);
    inizializzaColore(BIANCO, 255, 255, 255);
    inizializzaColore(GIALLINO, 255, 255, 163);

    inizializzaColore(VERDE_CROCC, 11, 139, 42);

    init_pair(ACQUA_E_CROCC, VERDE_CROCC, CELESTE);
    init_pair(DENTI_CROCC, BIANCO, VERDE_CROCC);
    init_pair(PUPILLA_CROCC, NERO, VERDE_CROCC);
    init_pair(IRIDE_CROCC, GIALLINO, VERDE_CROCC);

    // per il coccodrillo cattivo
    inizializzaColore(DARK_CROCC, 140, 98, 65);

    init_pair(ACQUA_E_DARK_CROCC, DARK_CROCC, CELESTE);
    init_pair(DENTI_DARK_CROCC, BIANCO, DARK_CROCC);
    init_pair(PUPILLA_DARK_CROCC, NERO, DARK_CROCC);
    init_pair(IRIDE_DARK_CROCC, GIALLINO, DARK_CROCC);

    // per i nemici (fiori)
    inizializzaColore(GAMBO, 0, 128, 0);
    inizializzaColore(CENTRO, 255, 192, 0);
    inizializzaColore(PETALI, 255, 0, 0);

    init_pair(PETALI_E_PRATO, PETALI, ERBA);
    init_pair(PETALI_E_CENTRO, PETALI, CENTRO);
    init_pair(FACCIA_CENTRO, NERO, CENTRO);
    init_pair(PETALI_E_GAMBO, PETALI, GAMBO);
    init_pair(GAMBO_E_PRATO, GAMBO, ERBA);

    // per la rana (giocatore)
    inizializzaColore(EYES, 255, 255, 0);
    inizializzaColore(COLORE_RANA, 112, 48, 160);

    init_pair(RANA, EYES, COLORE_RANA);

    // per il tempo della scoreboard
    inizializzaColore(LOW, 207, 2, 2);
    inizializzaColore(MID, 250, 188, 2);
    inizializzaColore(HIGH, 2, 250, 81);

    init_pair(SC_LOW, LOW, LOW);
    init_pair(SC_MID, MID, MID);
    init_pair(SC_HIGH, HIGH, HIGH);

    // per i bordi
    inizializzaColore(DEFAULT_BG, 23, 20, 33);
    inizializzaColore(DEFAULT_FG, 208, 207, 204);
    init_pair(DEFAULT, DEFAULT_FG, DEFAULT_BG);

    // per i proiettili
    inizializzaColore(PURPLE_PROJ, 181, 29, 252);
    init_pair(PROIETTILI, PETALI, PURPLE_PROJ); // per il proiettile della rana si stampa ' ', per quello dei nemici '█'

    // per il titolo
    inizializzaColore(VIOLA_TIT, 94, 57, 121);
    inizializzaColore(ROSA, 174, 70, 170);
    inizializzaColore(AZZURRO, 62, 109, 228);
    init_pair(TITOLOS_E_TITOLOC, VIOLA_TIT, COLORE_RANA);
    init_pair(PRATO_E_TITOLO, VIOLA_TIT, ERBA);    
    init_pair(ROSA_E_PRATO, ROSA, ERBA);
    init_pair(ROSA_E_AZZURRO, ROSA, AZZURRO);
    init_pair(CELESTE_E_AZZURRO, CELESTE, AZZURRO);

    // per il cursore a froma di rana
    init_pair(EYES_E_CELESTE, EYES, CELESTE);
    init_pair(FIUME_E_RANA, CELESTE,COLORE_RANA);

    // per l'interfaccia della home
    init_pair(NERO_E_BIANCO,NERO,BIANCO);
    init_pair(NERO_E_CELESTE,NERO,CELESTE);

    // per le difficolta'
    init_pair(NERO_E_VERDE, NERO, HIGH);
    init_pair(NERO_E_GIALLO, NERO, MID);
    init_pair(NERO_E_ROSSO, NERO, LOW);
    
    // per le scritte colorate in debug
    init_pair(RED_DEBUG, LOW, DEFAULT_BG);
    init_pair(GREEN_DEBUG, HIGH, DEFAULT_BG);   
    
    // per il segnale di pericolo
    init_pair(ROSSO_E_BIANCO, LOW, BIANCO);
    init_pair(ROSSO_E_CELESTE, LOW, CELESTE); 
}
// initpair ( NOME , FOREGROUND, BACKGROUND )
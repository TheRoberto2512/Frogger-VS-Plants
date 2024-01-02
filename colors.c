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
    init_pair(PETALI_E_CENTRO, CENTRO, PETALI);
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
}
// initpair( NOME , FOREGROUND, BACKGROUND )
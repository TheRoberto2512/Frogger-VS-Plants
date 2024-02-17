#include "settings.h"

GameRules getRules(short difficult)
{
    GameRules rules; // qui verranno caricate le regole in base alla difficolta'

    switch (difficult)
    {
        case EASY:
            rules.time = (short) EASY_TIME;
            rules.enemies = (bool) EASY_ENEMIES;
            rules.BadCrocodile = (short) EASY_BAD_CROCODILE;
            rules.speed = (short) EASY_SPEED;
          break;

        case MEDIUM:
            rules.time = (short) MEDIUM_TIME;
            rules.enemies = (bool) MEDIUM_ENEMIES;
            rules.BadCrocodile = (short) MEDIUM_BAD_CROCODILE;
            rules.speed = (short) MEDIUM_SPEED;
          break;

        case HARD:
            rules.time = (short) HARD_TIME;
            rules.enemies = (bool) HARD_ENEMIES;
            rules.BadCrocodile = (short) HARD_BAD_CROCODILE;
            rules.speed = (short) HARD_SPEED;  
          break;
    }

    return rules;
}
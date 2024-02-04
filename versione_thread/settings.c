#include "settings.h"

GameRules getRules(short difficult)
{
    GameRules rules;

    switch (difficult)
    {
        case EASY:
            rules.time = (short) EASY_TIME;
            rules.enemies = (short) EASY_ENEMIES;
            rules.BadCrocodile = (short) EASY_BAD_CROCODILE;
            rules.speed = (short) EASY_SPEED;
          break;

        case MEDIUM:
            rules.time = (short) MEDIUM_TIME;
            rules.enemies = (short) MEDIUM_ENEMIES;
            rules.BadCrocodile = (short) MEDIUM_BAD_CROCODILE;
            rules.speed = (short) MEDIUM_SPEED;
          break;

        case HARD:
            rules.time = (short) HARD_TIME;
            rules.enemies = (short) HARD_ENEMIES;
            rules.BadCrocodile = (short) HARD_BAD_CROCODILE;
            rules.speed = (short) HARD_SPEED;  
          break;
    }

    return rules;
}
#include "settings.h"

GameRules getRules(short difficult)
{
    GameRules rules;

    switch (difficult)
    {
        case EASY:
            rules.time = EASY_TIME;
            rules.enemies = EASY_ENEMIES;
            rules.BadCrocodile = EASY_BAD_CROCODILE;
            rules.speed = EASY_SPEED;
          break;

        case MEDIUM:
            rules.time = MEDIUM_TIME;
            rules.enemies = MEDIUM_ENEMIES;
            rules.BadCrocodile = MEDIUM_BAD_CROCODILE;
            rules.speed = MEDIUM_SPEED;
          break;

        case HARD:
            rules.time = HARD_TIME;
            rules.enemies = HARD_ENEMIES;
            rules.BadCrocodile = HARD_BAD_CROCODILE;
            rules.speed = HARD_SPEED;  
          break;
    }

    return rules;
}
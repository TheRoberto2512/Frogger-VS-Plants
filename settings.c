#include "settings.h"

GameRules getRules(short difficult)
{
    GameRules rules;

    switch (difficult)
    {
        case 1:
            rules.time = EASY_TIME;
            rules.enemies = EASY_ENEMIES;
            rules.BadCrocodile = EASY_BAD_CROCODILE;
            rules.speed = EASY_SPEED;
          break;

        case 2:
            rules.time = MEDIUM_TIME;
            rules.enemies = MEDIUM_ENEMIES;
            rules.BadCrocodile = MEDIUM_BAD_CROCODILE;
            rules.speed = MEDIUM_SPEED;
          break;

        case 3:
            rules.time = HARD_TIME;
            rules.enemies = HARD_ENEMIES;
            rules.BadCrocodile = HARD_BAD_CROCODILE;
            rules.speed = HARD_SPEED;  
          break;
    }

    return rules;
}
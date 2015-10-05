#include "arcade.h"
//#include "nonsimple.h"
#include "field.h"
#include "options.h"


void arcade_restart()
{
    if (single_player)
    {
        init_snake(0, 60,90, UP, starting_size);
        snake[1].alive = 0;
    }
    else
    {
        init_snake(0, 60,90, UP, starting_size);
        init_snake(1, 60,70, DOWN, starting_size);
    }
}

void show_arcade_options()
{
    show_duel_options();
    show_torus_options();
    show_speed_options();
    show_food_options();
    show_size_options();
    show_gun_options();
}

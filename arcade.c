#include "arcade.h"
//#include "nonsimple.h"
#include "field.h"
#include "options.h"

inline int options_quick_modifiable()
{
    return ((snake[0].alive || snake[1].alive) && (restart_after_timer || timer < 255));
}

void start_arcade_play()
{
    // black everything:
    dynamics = 1;
    clear();

    // setup the snakes:
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

    // put the snake on the map
    for (int p=0; p<2-single_player; ++p)
        superpixel[snake[p].head.y][snake[p].head.x] = snake[p].color;

    if (!torus)
        make_walls();
    make_food(food_count);
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

void update_arcade_options()
{
    if (gamepad_press[0] & gamepad_down)
    {
        if (speed < 9)
            ++speed;
        message("speed = %d\n", (int)speed);
        if (options_quick_modifiable())
            show_speed_options();
    }
    else if (gamepad_press[0] & gamepad_up)
    {
        if (speed > 1)
            --speed;
        message("speed = %d\n", (int)speed);
        if (options_quick_modifiable())
            show_speed_options();
    }
    if (gamepad_press[0] & gamepad_Y)
    {
        torus = 1 - torus;
        message("torus = %d\n", (int)torus);
        if (torus)
        {
            // can safely make it into a torus, without resetting game
            if (options_quick_modifiable())
                show_torus_options();
            remove_walls();
        }
        else
        {
            // but cannot safely add walls without possibly breaking the game
            if (options_quick_modifiable())
                show_torus_options();
            else
            {
                restart_after_timer = 1; // you must reset the game
                show_options();
            }

            make_walls();
        }
    }
    if (gamepad_press[0] & gamepad_B)
    {   // add/remove bullets
        bullet_length = (bullet_length + 1)%4;
        if (bullet_length == 0)
        {   // we switched from guns to no guns, this requires a reset of the game...
            // kill all alive bullets:
            for (int p=0; p<2; ++p)
            for (int b=0; b<BULLETS; ++b)
                if (bullet[p][b].alive)
                {
                    superpixel[bullet[p][b].y][bullet[p][b].x] = bg_color;
                    bullet[p][b].alive = 0;
                }
                
        }
        if (options_quick_modifiable())
            show_gun_options();
    }
    if (gamepad_press[0] & gamepad_X)
    {
        single_player = 1 - single_player;
        message("single_player = %d\n", (int)single_player);
        if (options_quick_modifiable())
            show_duel_options();
        else
        { // create or destroy second player now
            if (single_player)
            {   // zip up tail til it reaches the head
                // now can remove the head (and tail):
                zip_snake(1, snake[1].head.y, snake[1].head.x,  bg_color);
                // kill off head, too:
                superpixel[snake[1].head.y][snake[1].head.x] = bg_color;
            }
            else
            {
                // find a spot to put second player
                uint8_t y = rand()%SCREEN_H, x = rand()%SCREEN_W;
                while (superpixel[y][x] != bg_color)
                {
                    y = rand()%SCREEN_H;
                    x = rand()%SCREEN_W;
                }
                init_snake(1, y,x, rand()%4, starting_size);
            }
        }
    }
    if (gamepad_press[0] & gamepad_R)
    {
        if (starting_size < 13000)
        {
            int32_t increment = option_increment(starting_size);

            starting_size += increment;
            for (int p=0; p<2-single_player; ++p)
                snake[p].tail_wait += increment;
            message("size = %d\n", (int)starting_size);
            if (options_quick_modifiable())
                show_size_options();
        }
    }
    else if (gamepad_press[0] & gamepad_L)
    {
        if (starting_size > 1)
        {
            uint32_t decrement = option_decrement(starting_size);

            starting_size -= decrement;
            for (int p=0; p<2-single_player; ++p)
                snake[p].tail_wait -= decrement;
            message("size = %d\n", (int)starting_size);
            if (options_quick_modifiable())
                show_size_options();
        }
    }
    if (gamepad_press[0] & gamepad_right)
    {
        if (food_count < FOOD)
        {
            int32_t increment = option_increment(food_count);

            food_count += increment;
            make_food(increment);
            
            if (options_quick_modifiable())
                show_food_options();
        }
    }
    else if (gamepad_press[0] & gamepad_left)
    {
        if (food_count)
        { 
            int32_t decrement = option_decrement(food_count);

            food_count -= decrement;
            if (options_quick_modifiable())
                show_food_options();
            else
            {
                restart_after_timer = 1;
                show_options();
            }
        }
    }
}

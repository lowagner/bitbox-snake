//#include <math.h>
#include "nonsimple.h"
#include "field.h"
#include "arcade.h"
#include "options.h"
#include "abc.h"
#include "io.h"


typedef int (int_fn)(void);

void_fn *start_mode_play;
void_fn *show_mode_options;
void_fn *update_mode_options;
int_fn *handle_mode_meta;


void show_options()
{
    if (show_mode_options)
        show_mode_options();
    
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    
    uint8_t y = SCREEN_H*3/4;
    uint8_t x0=SCREEN_W/2 - 9;
    uint8_t x=x0-6;
    // LOWAGNER
    draw_l(y,x,c1,c2);x+=4;
    draw_o(y,x,c2,c1);x+=4;
    draw_w(y,x,c1,c2);x+=4;
    draw_a(y,x,c2,c1);x+=4;
    draw_g(y,x,c1,c2);x+=4;
    draw_n(y,x,c2,c1);x+=4;
    draw_e(y,x,c1,c2);x+=4;
    draw_r(y,x,c2,c1);

    y += 4;
    x = x0-2;
    // 2015
    x+=4;
    draw_z(y,x,c2,c1);x+=4;
    draw_o(y,x,c1,c2);x+=4;
    draw_1(y,x,c2,c1);x+=4;
    draw_s(y,x,c1,c2);
}

void start_game_play()
{
    // get rid of bullets:
    for (int p=0; p<2; ++p)
    for (int b=0; b<BULLETS; ++b)
            bullet[p][b].alive = 0;

    // reset screen and what not.
    if (!start_mode_play)
    {
        start_mode_play = &start_arcade_play;
        show_mode_options = &show_arcade_options;
        update_mode_options = &update_arcade_options;
        handle_mode_meta = &handle_arcade_meta;
    }
    start_mode_play();
}

void start_play_countdown()
{
    bg_color = 0; // this MUST BE ZERO.  DO NOT MODIFY.
    message("restarting game\n");
    srand(vga_frame); // reseed the random # generator

    graph_line_callback = NULL;

    // give a preview of what the screen will look like:
    start_game_play();

    timer = 4; // after 4 seconds...,
    // do not restart after timer, hit play:
    restart_after_timer = 0;

    // show the options during the countdown:
    show_options();
}

void game_init()
{ 
    io_init();

    start_mode_play = &start_arcade_play;
    show_mode_options = &show_arcade_options;
    update_mode_options = &update_arcade_options;
    handle_mode_meta = &handle_arcade_meta;

    torus = 1; 
    speed = INIT_SPEED; // smaller values is faster snakes
    // speed should not be any larger than 9.  that's already super slow!
    food_count = INIT_FOOD;
    starting_size = INIT_SIZE;

    start_play_countdown();

    timer = 255; // go to pause
    restart_after_timer = 1; // go to menu on pause
    show_controls();
}

void do_snake_dynamics()
{
    for (int p=0; p<2-single_player; ++p)
    if (snake[p].alive)
    {
        if (vga_frame % speed == 0)
        {
            if (GAMEPAD_PRESSED(p, up) && (snake[p].heading == LEFT || snake[p].heading == RIGHT))
                snake[p].heading = UP;
            else if (GAMEPAD_PRESSED(p, down) && (snake[p].heading == LEFT || snake[p].heading == RIGHT))
                snake[p].heading = DOWN;
            else if (GAMEPAD_PRESSED(p, left) && (snake[p].heading == UP || snake[p].heading == DOWN))
                snake[p].heading = LEFT;
            else if (GAMEPAD_PRESSED(p, right) && (snake[p].heading == UP || snake[p].heading == DOWN))
                snake[p].heading = RIGHT;
        
            // encode direction you're going onto the current pixel, so tail can follow:
            superpixel[snake[p].head.y][snake[p].head.x] = encode(snake[p].color, snake[p].heading);

            switch (snake[p].heading)
            {
            case UP:
                if (snake[p].head.y)
                    --snake[p].head.y;
                else
                    snake[p].head.y = SCREEN_H-1; // should only happen on torus
                break;
            case DOWN:
                if (snake[p].head.y < SCREEN_H-1)
                    ++snake[p].head.y;
                else
                    snake[p].head.y = 0; // should only happen on torus
                break;
            case LEFT: 
                if (snake[p].head.x)
                    --snake[p].head.x;
                else
                    snake[p].head.x = SCREEN_W-1; // should only happen on torus
                break;
            case RIGHT: 
                if (snake[p].head.x < SCREEN_W-1)
                    ++snake[p].head.x;
                else
                    snake[p].head.x = 0; // should only happen on torus
                break;
            }
            
            // check collisions on new spot first:
            if (superpixel[snake[p].head.y][snake[p].head.x] != bg_color)
            {
                if (superpixel[snake[p].head.y][snake[p].head.x] == food_color)
                {
                    ++snake[p].tail_wait;
                    make_food(1);
                }
                else
                {
                    if (p == 1 && snake[1].head.x == snake[0].head.x && snake[1].head.y == snake[0].head.y )
                    {   // second player, check if player 1 ran into player 0's head
                        // they should both die if that's the case.
                        kill_snake(0);
                        message("snake battle!  both die.\n");
                        superpixel[snake[1].head.y][snake[1].head.x] = (player_color[0] + player_color[1])/2;
                    }
                    kill_snake(p);
                    continue;
                }
            }
            
            // then mark the next spot as where you're going!
            // (encode heading for that next time)
            superpixel[snake[p].head.y][snake[p].head.x] = snake[p].color;

            // finished with the snake's head, now go onto the snake's tail!
            if (snake[p].tail_wait > 0)
                --snake[p].tail_wait;
            else
            {
                while (snake[p].tail_wait <= 0)
                {
                    // decode the direction the tail is heading from the color it's on:
                    uint8_t tail_heading = decode(superpixel[snake[p].tail.y][snake[p].tail.x]);
                    // blank the tail
                    superpixel[snake[p].tail.y][snake[p].tail.x] = bg_color;
                    switch (tail_heading)
                    {
                    case UP:
                        if (snake[p].tail.y)
                            --snake[p].tail.y;
                        else
                            snake[p].tail.y = SCREEN_H-1; // should only happen on torus
                        break;
                    case DOWN:
                        if (snake[p].tail.y < SCREEN_H-1)
                            ++snake[p].tail.y;
                        else
                            snake[p].tail.y = 0; // should only happen on torus
                        break;
                    case LEFT: 
                        if (snake[p].tail.x)
                            --snake[p].tail.x;
                        else
                            snake[p].tail.x = SCREEN_W-1; // should only happen on torus
                        break;
                    case RIGHT: 
                        if (snake[p].tail.x < SCREEN_W-1)
                            ++snake[p].tail.x;
                        else
                            snake[p].tail.x = 0; // should only happen on torus
                        break;
                    }
                    ++snake[p].tail_wait;
                }
                snake[p].tail_wait = 0;
            }
        } 

        // fire bullets if you want and are able
        if (gamepad_press[p] & gamepad_B && bullet_length)
        {   
            message("FIRE!\n");
            // scan bullets for anyone not alive
            int b=0;
            for (; b<BULLETS; ++b)
                if (!bullet[p][b].alive)
                    break;
            if (b < BULLETS)
            {   // found a dead bullet to use
                bullet[p][b].alive = BULLET_LIFE;
                bullet[p][b].heading = snake[p].heading;
                bullet[p][b].y = snake[p].head.y;
                bullet[p][b].x = snake[p].head.x;
                // push the bullet in front of player
                switch (bullet[p][b].heading)
                {
                case UP:
                    if (bullet[p][b].y)
                        --bullet[p][b].y;
                    else
                        bullet[p][b].y = SCREEN_H-1; // should only happen on torus
                    break;
                case DOWN:
                    if (bullet[p][b].y < SCREEN_H-1)
                        ++bullet[p][b].y;
                    else
                        bullet[p][b].y = 0; // should only happen on torus
                    break;
                case LEFT: 
                    if (bullet[p][b].x)
                        --bullet[p][b].x;
                    else
                        bullet[p][b].x = SCREEN_W-1; // should only happen on torus
                    break;
                case RIGHT: 
                    if (bullet[p][b].x < SCREEN_W-1)
                        ++bullet[p][b].x;
                    else
                        bullet[p][b].x = 0; // should only happen on torus
                    break;
                }
            }
        }
    }
}

void do_bullet_dynamics()
{
    if (vga_frame % speed == 0)
    for (int step=0; step < bullet_length; ++step)
    for (int b=0; b<BULLETS; ++b)
    for (int p=0; p<2; ++p)
    if (bullet[p][b].alive)
    {
        // remove bullet from its current spot:
        superpixel[bullet[p][b].y][bullet[p][b].x] = bg_color;
    
        // move it forward:
        switch (bullet[p][b].heading)
        {
        case UP:
            if (bullet[p][b].y)
                --bullet[p][b].y;
            else
                bullet[p][b].y = SCREEN_H-1; // should only happen on torus
            break;
        case DOWN:
            if (bullet[p][b].y < SCREEN_H-1)
                ++bullet[p][b].y;
            else
                bullet[p][b].y = 0; // should only happen on torus
            break;
        case LEFT: 
            if (bullet[p][b].x)
                --bullet[p][b].x;
            else
                bullet[p][b].x = SCREEN_W-1; // should only happen on torus
            break;
        case RIGHT: 
            if (bullet[p][b].x < SCREEN_W-1)
                ++bullet[p][b].x;
            else
                bullet[p][b].x = 0; // should only happen on torus
            break;
        }

        // check collisions
        if (superpixel[bullet[p][b].y][bullet[p][b].x] != bg_color)
        {
            if (superpixel[bullet[p][b].y][bullet[p][b].x] & (1<<15))
            {   // indestructible, ignore!  can't shoot through, either.
            }
            else if (superpixel[bullet[p][b].y][bullet[p][b].x] == bullet_color)
            {
                // find the other bullet and kill it
                for (int pb=0; pb<2*BULLETS; ++pb)
                if (bullet[p][b].y == bullet[pb/BULLETS][pb%BULLETS].y && 
                    bullet[p][b].x == bullet[pb/BULLETS][pb%BULLETS].x)
                {
                    bullet[pb/BULLETS][pb%BULLETS].alive = 0; 
                    break;
                }
                superpixel[bullet[p][b].y][bullet[p][b].x] = bg_color;
            }
            else if ((superpixel[bullet[p][b].y][bullet[p][b].x] & CODE_MASK) == 
                     (snake[1-p].color & CODE_MASK))
            {
                // kill off enemy snake (1-p) if it's his head, otherwise zip him up
                if (snake[1-p].alive)
                {
                    if (bullet[p][b].y == snake[1-p].head.y && bullet[p][b].x == snake[1-p].head.x)
                    {
                        kill_snake(1-p); 
                        message("killed enemy by a bullet\n");
                    }
                    else // was not the head, zip up tail to where bullet hit
                    {
                        zip_snake(1-p, bullet[p][b].y, bullet[p][b].x, dead_player_color[1-p]);
                        snake[1-p].tail_wait = -1; // tail will jump forward one
                        message("hurt enemy by a bullet\n");
                    }
                    // don't blank the spot, since the tail needs to zip past its encoding
                }
                else
                    // dead snake, put a hole in it
                    superpixel[bullet[p][b].y][bullet[p][b].x] = bg_color;
            }
            else if ((superpixel[bullet[p][b].y][bullet[p][b].x] & CODE_MASK) == 
                     (snake[p].color & CODE_MASK))
            {
                // you just hurt or killed yourself!
                if (snake[p].alive)
                {
                    if (bullet[p][b].y == snake[p].head.y && bullet[p][b].x == snake[p].head.x)
                    {
                        kill_snake(p); 
                        message("killed by your own bullet\n");
                    }
                    else // was not the head, zip up tail to where bullet hit
                    {
                        zip_snake(p, bullet[p][b].y, bullet[p][b].x, dead_player_color[p]);
                        snake[p].tail_wait = -1; // tail will jump forward one
                        message("hurt by your own bullet\n");
                    }
                    // don't blank the spot, since the tail needs to zip past its encoding
                }
                else
                    // dead snake, put a hole in it
                    superpixel[bullet[p][b].y][bullet[p][b].x] = bg_color;
            }
            else
                // put hole in whatever it is.  if it was food, your bad!  less food overall.
                superpixel[bullet[p][b].y][bullet[p][b].x] = bg_color;

            bullet[p][b].alive = 1; // will get killed here next...
        }
       
        --bullet[p][b].alive;
        // check whether the bullet should continue:
        if (bullet[p][b].alive)
            superpixel[bullet[p][b].y][bullet[p][b].x] = bullet_color;
    }

}

void game_frame()
{
    // update the gamepad and notice any new button presses:
    static uint16_t old_gamepad[2];

    kbd_emulate_gamepad(); // update
   
    // get new presses:
    gamepad_press[0] = gamepad_buttons[0] & ~old_gamepad[0];
    gamepad_press[1] = gamepad_buttons[1] & ~old_gamepad[1];

    old_gamepad[0] = gamepad_buttons[0];
    old_gamepad[1] = gamepad_buttons[1];

    // meta game controls here:
    if (handle_mode_meta)
      if (handle_mode_meta()) // execute fancy pause/modify-game instructions
        return;
    
    if (dynamics)
    {
        // do bullet dynamics
        do_bullet_dynamics();
        // do snake dynamics
        do_snake_dynamics();
    }
}



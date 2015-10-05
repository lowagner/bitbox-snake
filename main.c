#include <math.h>
#include "nonsimple.h"
#include "field.h"
#include "io.h"
#include "abc.h"

// variables for heading / direction:

inline int32_t option_increment(int v)
{
    if (v < 5)
        return 1;
    else if (v < 115)
        return 5;
    else if (v < 1000)
        return 59;
    else
        return 1000;
}

inline int32_t option_decrement(int v)
{
    if (v <= 5)
        return 1;
    else if (v <= 115)
        return 5;
    else if (v <= 1000)
        return 59;
    else
        return 1000;
}

void show_duel_options()
{
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    uint8_t y=20;
    uint8_t x = SCREEN_W/2 - 9 +2;
    if (!single_player)
    {
        draw_d(y,x,c1,c2);x+=4;
        draw_u(y,x,c2,c1);x+=4;
        draw_e(y,x,c1,c2);x+=4;
        draw_l(y,x,c2,c1);
    }
    else
    {
        draw_s(y,x,c1,c2);x+=4;
        draw_o(y,x,c2,c1);x+=4;
        draw_l(y,x,c1,c2);x+=4;
        draw_o(y,x,c2,c1);
    }
}

void show_torus_options()
{
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    uint8_t y=24, x=SCREEN_W/2 - 9;
    if (torus)
    {
        draw_t(y,x,c1,c2);x+=4;
        draw_o(y,x,c2,c1);x+=4;
        draw_r(y,x,c1,c2);x+=4;
        draw_u(y,x,c2,c1);x+=4;
        draw_s(y,x,c1,c2);
    }
    else
    {
        draw_w(y,x,c1,c2);x+=4;
        draw_a(y,x,c2,c1);x+=4;
        draw_l(y,x,c1,c2);x+=4;
        draw_l(y,x,c2,c1);x+=4;
        draw_s(y,x,c1,c2);
    }

}

void show_speed_options()
{
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    uint8_t y=28, x0=SCREEN_W/2 - 9;
    uint8_t x = x0;
    // SPEED speed!
    x=x0-3;
    draw_s(y,x,c1,c2);x+=4;
    draw_p(y,x,c2,c1);x+=4;
    draw_e(y,x,c1,c2);x+=4;
    draw_e(y,x,c2,c1);x+=4;
    draw_d(y,x,c1,c2);

    uint8_t felt_speed = (uint8_t) 10 - speed;
    x+=6;
    numbers[felt_speed](y,x,c2,c1);
}

void show_big_number(int v, uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    if (v < 1000)
    {
        numbers[v/100](y,x,c1,c2);x+=4;
        numbers[(v/10)%10](y,x,c2,c1);x+=4;
        numbers[v%10](y,x,c1,c2);
    }
    else
    {
        numbers[v/10000](y,x,c1,c2);x+=4;
        numbers[(v/1000)%10](y,x,c2,c1);x+=4;
        draw_k(y,x,c1,c2);
    }
}

void show_food_options()
{
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    uint8_t y=32, x=SCREEN_W/2 - 9-5;
    // FOOD
    draw_f(y,x,c1,c2);x+=4;
    draw_o(y,x,c2,c1);x+=4;
    draw_o(y,x,c1,c2);x+=4;
    draw_d(y,x,c2,c1);
    x+=6;
    show_big_number(food_count, y, x, c1, c2);
}

void show_size_options()
{
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    uint8_t y=36, x=SCREEN_W/2 - 9-5;
    // SIZE
    draw_s(y,x,c1,c2);x+=4;
    draw_i(y,x,c2,c1);x+=4;
    draw_z(y,x,c1,c2);x+=4;
    draw_e(y,x,c2,c1);
 
    // put in the number
    x+=6;
    show_big_number(starting_size, y, x, c1, c2);
}

void show_gun_options()
{
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    uint8_t y=40, x=SCREEN_W/2 - 9-1;
    // bullets
    draw_g(y,x,c1,c2);x+=4;
    draw_u(y,x,c2,c1);x+=4;
    draw_n(y,x,c1,c2);x+=4;
    draw_s(y,x,c2,c1);
 
    // put in the number
    x+=6;
    numbers[bullet_length%10](y,x,c1,c2);

}

void show_options()
{
    show_duel_options();
    show_torus_options();
    show_speed_options();
    show_food_options();
    show_size_options();
    show_gun_options();
    
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

void show_controls()
{
    uint16_t c2 = RGB(255,255,0);
    uint16_t c1 = RGB(0,255,255);
    uint8_t y=20, x=SCREEN_W/2 - 26;

    draw_x(y,x,c1,c2);
    y += 4;
    draw_y(y,x,c1,c2);
    y += 4;
    x -= 4;
    draw_up(y,x,c1,c2); x += 4;
    draw_slash(y,x,c1,c2); x += 4;
    draw_down(y,x,c1,c2);
    x -= 8;
    y += 4;
    draw_left(y,x,c1,c2); x += 4;
    draw_slash(y,x,c1,c2); x += 4;
    draw_right(y,x,c1,c2);
    x -= 8;
    y += 4;
    draw_l(y,x,c1,c2); x += 4;
    draw_slash(y,x,c1,c2); x += 4;
    draw_r(y,x,c1,c2);
    x -= 4;
    y += 4;
    draw_b(y,x,c1,c2); x += 4;
}

void game_restart()
{
    bg_color = 0; // this MUST BE ZERO.  DO NOT MODIFY.

    message("restarting game\n");

    srand(vga_frame); // reseed the random # generator

    graph_line_callback = NULL;

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
 
    screen_reset();

    restart_after_timer = 0;
    show_options();
    timer = 4;
}

void game_init()
{ 
    io_init();

    torus = 1; 
    speed = INIT_SPEED; // smaller values is faster snakes
    // speed should not be any larger than 9.  that's already super slow!
    food_count = INIT_FOOD;
    starting_size = INIT_SIZE;

    game_restart();

    timer = 255; // go to pause
    restart_after_timer = 1; // go to menu on pause
    show_controls();
}

void game_frame()
{
    static uint16_t old_gamepad[2];

    kbd_emulate_gamepad();
    
    uint16_t gamepad_press[2] = { gamepad_buttons[0] & ~old_gamepad[0],
    gamepad_buttons[1] & ~old_gamepad[1] };

    old_gamepad[0] = gamepad_buttons[0];
    old_gamepad[1] = gamepad_buttons[1];

    // meta game controls here:

    if (gamepad_press[0] & gamepad_start)
    {
        if (timer == 255) // if we were paused...
        {
            if (restart_after_timer)
            {
                message("unpausing from menu\n");
                game_restart();
                return;
            }
            message("unpausing\n");
            timer = 0;
        }
        else if (timer == 0) // we weren't paused
        {
            if (GAMEPAD_PRESSED(0, R))
            {
                restart_after_timer = 1;
                show_options();
                show_controls();
            }
            timer = 255;
        }
    }

    // pause game if it's on a timer...
    if (timer)
    {
        if (gamepad_press[0] & gamepad_select)
        {   // save screen shot
            message("taking picture\n");
            take_screenshot();
            return;
        }
        if (timer < 255) // not paused, probably the game preparing for something...
        {
            if (gamepad_press[0] & gamepad_start)
            {
                // if we just pressed start, either go to special options (if R is pressed):
                if (GAMEPAD_PRESSED(0, R))
                {
                    restart_after_timer = 1;
                    timer = 255;
                    show_controls();
                    show_options();
                }
                else // or speed up whatever is happening:
                {
                    timer = 0; // shortcut
                    message("speeding up whatever is happening (by start)\n");
                    if (restart_after_timer)
                        return game_restart();
                    else
                        return screen_reset();
                }
            }
            if (vga_frame % 60 == 0)
            {
                --timer;
                if (restart_after_timer)
                {   
                    // showing the player their mistake,
                    // until timer runs out:
                    if (!timer)
                        game_restart();
                }
                else // no restart, count down!
                {
                    switch (timer)
                    {
                    case 3:
                        superpixel[56][80] = RGB(255,0,0);
                        superpixel[58][80] = RGB(255,255,0);
                        superpixel[60][80] = RGB(0,255,0);
                        break;
                    case 2:
                        superpixel[56][80] = bg_color;
                        break;
                    case 1:
                        superpixel[58][80] = bg_color;
                        break;
                    case 0:
                        screen_reset();
                        break;
                    }

                }
            }
        }
        else // we're paused!  show the options and/or allow them to change
        {
            if (gamepad_press[0] & gamepad_down)
            {
                if (speed < 9)
                    ++speed;
                message("speed = %d\n", (int)speed);
                if (restart_after_timer)
                    show_speed_options();
            }
            else if (gamepad_press[0] & gamepad_up)
            {
                if (speed > 1)
                    --speed;
                message("speed = %d\n", (int)speed);
                if (restart_after_timer)
                    show_speed_options();
            }
            if (gamepad_press[0] & gamepad_Y)
            {
                torus = 1 - torus;
                message("torus = %d\n", (int)torus);
                if (!restart_after_timer)
                {
                    restart_after_timer = 1; // you must reset the game
                    show_options();
                }
                else
                    show_torus_options();

                if (torus)
                    remove_walls();
                else
                    make_walls();
            }
            if (gamepad_press[0] & gamepad_B)
            {   // add/remove bullets
                bullet_length = (bullet_length + 1)%4;
                if (!restart_after_timer)
                {
                    restart_after_timer = 1; // you must reset the game
                    show_options();
                }
                else
                    show_gun_options();
            }
            if (gamepad_press[0] & gamepad_X)
            {
                single_player = 1 - single_player;
                message("single_player = %d\n", (int)single_player);
                if (!restart_after_timer)
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
                if (restart_after_timer)
                    show_duel_options();
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
                    if (restart_after_timer)
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
                    if (restart_after_timer)
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
                    
                    if (restart_after_timer)
                        show_food_options();
                }
            }
            else if (gamepad_press[0] & gamepad_left)
            {
                if (food_count)
                { 
                    int32_t decrement = option_decrement(food_count);

                    food_count -= decrement;
                    if (!restart_after_timer)
                    {
                        restart_after_timer = 1;
                        show_options();
                    }
                    else
                        show_food_options();

                }
            }
        }
        return;
    }
    
    // do bullet dynamics
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
    
    // do snake dynamics
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
        if (gamepad_press[p] & gamepad_B)
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

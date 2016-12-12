//#include <math.h>
#include "common.h"
#include "chiptune.h"
#include "field.h"
#include "arcade.h"
#include "options.h"
#include "abc.h"
#include "font.h"
#include "name.h"
#include "io.h"
#include "anthem.h"
#include "verse.h"
#include "instrument.h"

#include <string.h> // memset

uint16_t superpixel[SCREEN_H/2][SCREEN_W/2] CCM_MEMORY;
uint16_t bg_color CCM_MEMORY;

const uint16_t palette[16] = {
    RGB(0, 0, 0),
    RGB(157, 157, 157),
    65535,
    RGB(224, 111, 139),
    RGB(190, 38, 51),
    RGB(235, 137, 49),
    RGB(164, 100, 34),
    RGB(73, 60, 43),
    RGB(247, 226, 107),
    RGB(163, 206, 39),
    RGB(68, 137, 26),
    RGB(47, 72, 78),
    RGB(178, 220, 239),
    RGB(49, 162, 242),
    RGB(0, 87, 132),
    RGB(28, 20, 40),
};

typedef int (int_fn)(void);

VisualMode visual_mode CCM_MEMORY; 
VisualMode previous_visual_mode CCM_MEMORY;
VisualMode old_visual_mode CCM_MEMORY;
uint16_t old_gamepad[2] CCM_MEMORY;
uint16_t gamepad_press[2] CCM_MEMORY;
uint8_t gamepad_press_wait CCM_MEMORY;
uint8_t game_message[32] CCM_MEMORY;


void game_init()
{ 
    font_init();
    instrument_init();
    anthem_init();
    verse_init();
    chip_init();
    io_init();

    if (io_get_recent_filename())
    {
        message("resetting everything\n");
        // had troubles loading a filename
        base_filename[0] = 'T';
        base_filename[1] = 'M';
        base_filename[2] = 'P';
        base_filename[3] = 0;

        // need to reset everything
        anthem_reset();
        verse_reset();
        instrument_reset();
    }
    else // there was a filename to look into
    {
        if (io_load_anthem())
        {
            anthem_reset();
        }
        if (io_load_verse(16))
        {
            verse_reset();
        }
        if (io_load_instrument(16))
        {
            instrument_reset();
        }
    }

    torus = 1; 
    speed = INIT_SPEED; // smaller values is faster snakes
    // speed should not be any larger than 9.  that's already super slow!
    food_count = INIT_FOOD;
    starting_size = INIT_SIZE;

    game_switch(GameOn);
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
                    snake[p].head.y = SCREEN_H/2-1; // should only happen on torus
                break;
            case DOWN:
                if (snake[p].head.y < SCREEN_H/2-1)
                    ++snake[p].head.y;
                else
                    snake[p].head.y = 0; // should only happen on torus
                break;
            case LEFT: 
                if (snake[p].head.x)
                    --snake[p].head.x;
                else
                    snake[p].head.x = SCREEN_W/2-1; // should only happen on torus
                break;
            case RIGHT: 
                if (snake[p].head.x < SCREEN_W/2-1)
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
                            snake[p].tail.y = SCREEN_H/2-1; // should only happen on torus
                        break;
                    case DOWN:
                        if (snake[p].tail.y < SCREEN_H/2-1)
                            ++snake[p].tail.y;
                        else
                            snake[p].tail.y = 0; // should only happen on torus
                        break;
                    case LEFT: 
                        if (snake[p].tail.x)
                            --snake[p].tail.x;
                        else
                            snake[p].tail.x = SCREEN_W/2-1; // should only happen on torus
                        break;
                    case RIGHT: 
                        if (snake[p].tail.x < SCREEN_W/2-1)
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
                        bullet[p][b].y = SCREEN_H/2-1; // should only happen on torus
                    break;
                case DOWN:
                    if (bullet[p][b].y < SCREEN_H/2-1)
                        ++bullet[p][b].y;
                    else
                        bullet[p][b].y = 0; // should only happen on torus
                    break;
                case LEFT: 
                    if (bullet[p][b].x)
                        --bullet[p][b].x;
                    else
                        bullet[p][b].x = SCREEN_W/2-1; // should only happen on torus
                    break;
                case RIGHT: 
                    if (bullet[p][b].x < SCREEN_W/2-1)
                        ++bullet[p][b].x;
                    else
                        bullet[p][b].x = 0; // should only happen on torus
                    break;
                }
            }
        }
    }
}

int do_bullet_dynamics()
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
                bullet[p][b].y = SCREEN_H/2-1; // should only happen on torus
            break;
        case DOWN:
            if (bullet[p][b].y < SCREEN_H/2-1)
                ++bullet[p][b].y;
            else
                bullet[p][b].y = 0; // should only happen on torus
            break;
        case LEFT: 
            if (bullet[p][b].x)
                --bullet[p][b].x;
            else
                bullet[p][b].x = SCREEN_W/2-1; // should only happen on torus
            break;
        case RIGHT: 
            if (bullet[p][b].x < SCREEN_W/2-1)
                ++bullet[p][b].x;
            else
                bullet[p][b].x = 0; // should only happen on torus
            break;
        }

        // check collisions
        if (superpixel[bullet[p][b].y][bullet[p][b].x] != bg_color)
        {
            if ((superpixel[bullet[p][b].y][bullet[p][b].x] & CODE_MASK) == CODE_MASK)
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
                        if (zip_snake(1-p, bullet[p][b].y, bullet[p][b].x, dead_player_color[1-p]))
                            return 1;
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
                        if (zip_snake(p, bullet[p][b].y, bullet[p][b].x, dead_player_color[p]))
                            return 1;
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

    return 0;
}

void game_frame()
{
    // get new presses:
    gamepad_press[0] = gamepad_buttons[0] & ~old_gamepad[0];
    gamepad_press[1] = gamepad_buttons[1] & ~old_gamepad[1];

    old_gamepad[0] = gamepad_buttons[0];
    old_gamepad[1] = gamepad_buttons[1];
    
    if (gamepad_press_wait)
        --gamepad_press_wait;

    // meta game controls here:
    switch (visual_mode)
    {
        case GameOn:
            arcade_controls();
            break;
        case EditAnthem:
            anthem_controls();
            break;
        case EditInstrument:
            instrument_controls();
            break;
        case EditVerse:
            verse_controls();
            break;
        case ChooseFilename:
            name_controls();
            break;
        default:
            game_switch(GameOn);
            break;
    }
}

void graph_frame() {}

void graph_line() 
{
    if (vga_odd)
        return;
    switch (visual_mode)
    {
        case GameOn:
        {
            uint32_t *dst = (uint32_t*)draw_buffer;
            int j = vga_line/2;
            for (int i=0; i<SCREEN_W/2; ++i)
            {
                uint16_t current_color = superpixel[j][i];
                *dst++ = current_color | (current_color<<16);
            }
            break;
        }
        case ChooseFilename:
            name_line();
            break;
        case EditAnthem:
            anthem_line();
            break;
        case EditVerse:
            verse_line();
            break;
        case EditInstrument:
            instrument_line();
            break;
        default:
            break;
    }
}

void clear() 
{
   memset(superpixel, 0, sizeof(superpixel));
}

void game_switch(VisualMode new_visual_mode)
{
    chip_kill();
    if (new_visual_mode == GameOn)
    {
        chip_play_init(0);
        start_arcade_countdown();

        timer = 255; // go to pause
        restart_after_timer = 1; // go to menu on pause
        show_controls();
    }
    visual_mode = new_visual_mode;
}

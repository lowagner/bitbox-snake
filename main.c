#include <stdlib.h> // rand
#include <math.h>
#include "nonsimple.h"
#include "fatfs/ff.h"
#include "abc.h"

// variables for heading / direction:

#define UP 0
#define LEFT 1
#define RIGHT 2
#define DOWN 3

// other random variables:

#define FOOD 16
#define INIT_FOOD 2
#define INIT_SIZE 15
#define INIT_SPEED 5 // higher is slower.  1 is fastest
#define CODE_MASK 31710 // color equivalence should be & this mask.

uint16_t player_color[2] = {RGB(255,0,0), RGB(0,50,255)};

struct snake {
    struct {
        uint8_t y, x;
    } head;
    struct {
        uint8_t y, x;
    } tail;
    uint16_t color;
    uint8_t heading; // direction that the snake is going.
    uint8_t tail_wait; // set tail_wait > 0 to grow snake.
} snake[2];

// game variables
uint8_t torus; // is the topography a torus?
uint8_t single_player; // 
uint8_t speed;
uint8_t starting_size;

uint8_t timer;
uint8_t restart_after_timer;

uint16_t food_color;
uint8_t food_count;
struct location {
    uint8_t y, x;
} food[FOOD];

FATFS fat_fs;
FIL fat_file;
FRESULT fat_result;

uint16_t encode(uint16_t color, uint8_t heading)
{   
    // returns encoded color
    // encode up to 16 values into the color (4 bits):
    return (color & CODE_MASK) | (heading & 1) | ((heading & 2) << 4) | ((heading & 4) << 8) | ((heading & 8) << 12);
    
}

uint8_t decode(uint16_t color)
{
    return (uint8_t)((color & 1) | ((color >> 4) & 2) | ((color >> 8) & 4) | ((color >> 12) & 8));
}

void snake_init(int p, uint8_t y, uint8_t x, uint8_t heading, uint8_t size)
{
    snake[p].head.y = snake[p].tail.y = y;
    snake[p].head.x = snake[p].tail.x = x;
    snake[p].color = player_color[p];
    snake[p].heading = heading;
    snake[p].tail_wait = size;
}

void kill_snake(int p)
{
    timer = 3;
    restart_after_timer = 1;
}

void screen_reset()
{
    clear();
    for (int i=0; i<food_count; ++i)
        superpixel[food[i].y][food[i].x] = food_color;
    for (int p=0; p<2-single_player; ++p)
        superpixel[snake[p].head.y][snake[p].head.x] = snake[p].color;
}

void show_duel()
{
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    uint8_t y=20, x0=SCREEN_W/2 - 9;
    uint8_t x = x0+2;
    if (!single_player)
    {
        draw_d(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_u(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_e(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_l(y,x,c1,c2);
    }
    else
    {
        draw_s(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_o(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_l(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_o(y,x,c1,c2);
    }
}

void show_torus()
{
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    uint8_t y=24, x0=SCREEN_W/2 - 9;
    uint8_t x = x0;
    if (torus)
    {
        draw_t(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_o(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_r(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_u(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_s(y,x,c1,c2);
    }
    else
    {
        draw_w(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_a(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_l(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_l(y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_s(y,x,c1,c2);
    }

}

void show_speed()
{
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    uint8_t y=28, x0=SCREEN_W/2 - 9;
    uint8_t x = x0;
    // SPEED speed!
    x=x0-3;
    draw_s(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_p(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_e(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_e(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_d(y,x,c1,c2);

    uint8_t felt_speed = (uint8_t) 10 - speed;
    
    swap_colors(&c1,&c2);x+=6;
    numbers[felt_speed](y,x,c1,c2);
}

void show_food()
{
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    uint8_t y=32, x0=SCREEN_W/2 - 9;
    uint8_t x = x0;
    x=x0-3;
    // FOOD
    draw_f(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_o(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_o(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_d(y,x,c1,c2);
    
    swap_colors(&c1,&c2);x+=6;
    numbers[food_count/10](y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    numbers[food_count%10](y,x,c1,c2);
}

void show_size()
{
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    uint8_t y=36, x0=SCREEN_W/2 - 9;
    uint8_t x = x0;
    x=x0-5;
    // SIZE
    draw_s(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_i(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_z(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_e(y,x,c1,c2);
   
    swap_colors(&c1,&c2);x+=6;
    numbers[starting_size/100](y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    numbers[(starting_size/10)%10](y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    numbers[starting_size%10](y,x,c1,c2);
}

void show_options()
{
    show_duel();
    show_torus();
    show_speed();
    show_food();
    show_size();
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    
    uint8_t y = SCREEN_H*3/4;
    uint8_t x0=SCREEN_W/2 - 9;
    uint8_t x=x0-6;
    // LOWAGNER
    draw_l(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_o(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_w(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_a(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_g(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_n(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_e(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_r(y,x,c1,c2);

    y += 4;
    x = x0-2;
    // 2015
    swap_colors(&c1,&c2);x+=4;
    draw_z(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_o(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_1(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_s(y,x,c1,c2);
}

void game_restart()
{
    bg_color = RGB(0,0,0); // probably will break if this is not 0.
    
    if (!speed)
        speed = INIT_SPEED; // smaller values is faster snakes
    // speed should not be any larger than 9.  that's already super slow!

    if (!starting_size)
        starting_size = INIT_SIZE;

    if (!food_count)
        food_count = INIT_FOOD;

    uint16_t test = encode(bg_color, 0);
    message("encoding test:  %d -> %d\n", (int)bg_color, (int)test);
    message("     decoding:  %d\n", (int)decode(test));

    graph_line_callback = NULL;

    if (single_player)
    {
        snake_init(0, 60,90, UP, starting_size);
    }
    else
    {
        snake_init(0, 60,90, UP, starting_size);
        snake_init(1, 60,70, DOWN, starting_size);
    }

    food_color = RGB(0,255,0);
    if (food_count == 2)
    {
        food[0].y = 60;
        food[0].x = 100;
        food[1].y = 60;
        food[1].x = 60;
    }
    else if (food_count == 1)
    {
        food[0].y = 60;
        food[0].x = 80;
    }
    
    screen_reset();
    
    for (int i=2; i<food_count; ++i)
    {
        food[i].y = rand()%SCREEN_H;
        food[i].x = rand()%SCREEN_W;
        while (superpixel[food[i].y][food[i].x] != bg_color)
        {
            food[i].y = rand()%SCREEN_H;
            food[i].x = rand()%SCREEN_W;
        }
    }

    timer = 5;
    restart_after_timer = 0;

    show_options();
}

void game_init()
{ 
    torus = 1;
    game_restart();
}


void game_frame()
{
    static uint16_t old_gamepad;

    kbd_emulate_gamepad();
    
    uint16_t gamepad_press = gamepad_buttons[0] & ~old_gamepad;
    old_gamepad = gamepad_buttons[0];

    // meta game controls here:

    if (gamepad_press & gamepad_start)
    {
        if (timer == 255) // if we were paused...
        {
            if (restart_after_timer)
                return game_restart();
            timer = 0;
        }
        else if (timer == 0) // we weren't paused
        {
            if (GAMEPAD_PRESSED(0, R))
            {
                restart_after_timer = 1;
                show_options();
            }
            timer = 255;
        }
    }

    // pause game if it's on a timer...
    if (timer)
    {
        if (gamepad_press & gamepad_select)
        {   // save screen shot
            srand(vga_frame); // seed random # generator
            message("taking picture\n");
            char filename[13];
            filename[0] = 's';
            filename[1] = 'n';
            filename[2] = 'a';
            filename[3] = 'k';
            filename[4] = 'e';
            int num = rand()%1000;
            filename[5] = '0' + num%10; // do reverse base 10
            num /= 10;
            filename[6] = '0' + num%10;
            num /= 10;
            filename[7] = '0' + num%10;
            filename[8] = '.';
            filename[9] = 'p';
            filename[10] = 'p';
            filename[11] = 'm';
            filename[12] = 0;
            fat_result = f_open(&fat_file,filename, FA_WRITE | FA_OPEN_ALWAYS);
            f_lseek(&fat_file, 0);
            if (fat_result == FR_OK)
            {
                UINT bytes_get;
                f_write(&fat_file, "P6\n160 120 31\n", 14, &bytes_get);
                //uint32_t *src = (uint32_t*) superpixel;
                for (int j=0; j<SCREEN_H; j++)
                for (int i=0; i<SCREEN_W; i++)
                {
                    uint16_t C = superpixel[j][i];
                    char msg[3];
                    msg[0] = (C >> 10)&31; // red
                    msg[1] = (C >> 5)&31; // green
                    msg[2] = (C)&31; // blue
                    f_write(&fat_file, msg, 3, &bytes_get);
                }
                f_close(&fat_file);
            }

        }
        if (timer < 255) // not paused, probably the game preparing for something...
        {
            if (GAMEPAD_PRESSED(0, start))
            {
                timer = 0; // shortcut
                if (restart_after_timer)
                    return game_restart();
                else
                    return screen_reset();
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
        else // we're paused!  show the options and allow them to change
        {
            if (gamepad_press & gamepad_down)
            {
                if (speed < 9)
                    ++speed;
                message("speed = %d\n", (int)speed);
                if (restart_after_timer)
                    show_speed();
            }
            else if (gamepad_press & gamepad_up)
            {
                if (speed > 1)
                    --speed;
                message("speed = %d\n", (int)speed);
                if (restart_after_timer)
                    show_speed();
            }
            if (gamepad_press & gamepad_Y)
            {
                torus = 1 - torus;
                message("torus = %d\n", (int)torus);
                if (restart_after_timer)
                    show_torus();
            }
            if (gamepad_press & gamepad_X)
            {
                single_player = 1 - single_player;
                message("single_player = %d\n", (int)single_player);
                if (!restart_after_timer)
                { // create or destroy second player now
                    if (single_player)
                    {   // zip up tail til it reaches the head
                        while (!(snake[1].tail.y == snake[1].head.y && snake[1].tail.x == snake[1].head.x))
                        {
                            // decode the direction the tail is heading from the color it's on:
                            uint8_t tail_heading = decode(superpixel[snake[1].tail.y][snake[1].tail.x]);
                            // blank the tail
                            superpixel[snake[1].tail.y][snake[1].tail.x] = bg_color;
                            switch (tail_heading)
                            {
                            case UP:
                                if (snake[1].tail.y)
                                    --snake[1].tail.y;
                                else
                                    snake[1].tail.y = SCREEN_H-1; // should only happen on torus
                                break;
                            case DOWN:
                                if (snake[1].tail.y < SCREEN_H-1)
                                    ++snake[1].tail.y;
                                else
                                    snake[1].tail.y = 0; // should only happen on torus
                                break;
                            case LEFT: 
                                if (snake[1].tail.x)
                                    --snake[1].tail.x;
                                else
                                    snake[1].tail.x = SCREEN_W-1; // should only happen on torus
                                break;
                            case RIGHT: 
                                if (snake[1].tail.x < SCREEN_W-1)
                                    ++snake[1].tail.x;
                                else
                                    snake[1].tail.x = 0; // should only happen on torus
                                break;
                            }

                        }
                        // now can remove the head (and tail):
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
                        snake_init(1, y,x, rand()%4, starting_size);
                    }
                }
                if (restart_after_timer)
                    show_duel();
            }
            if (gamepad_press & gamepad_R)
            {
                if (starting_size <= 250)
                {
                    starting_size += 5;
                    for (int p=0; p<2-single_player; ++p)
                        snake[p].tail_wait += 5;
                    message("size = %d\n", (int)starting_size);
                    if (restart_after_timer)
                        show_size();
                }
            }
            else if (gamepad_press & gamepad_L)
            {
                if (starting_size > 6)
                {
                    starting_size -= 5;
                    if (restart_after_timer)
                        show_size();
                    message("size = %d\n", (int)starting_size);
                }
            }
            if (gamepad_press & gamepad_right)
            {
                if (food_count < FOOD)
                {
                    food[food_count].y = rand()%SCREEN_H;
                    food[food_count].x = rand()%SCREEN_W;
                    while (superpixel[food[food_count].y][food[food_count].x] != bg_color)
                    {
                        food[food_count].y = rand()%SCREEN_H;
                        food[food_count].x = rand()%SCREEN_W;
                    }
                    superpixel[food[food_count].y][food[food_count].x] = food_color;
                    ++food_count;
                    if (restart_after_timer)
                        show_size();
                }
            }
            else if (gamepad_press & gamepad_left)
            {
                if (food_count)
                { 
                    --food_count;
                    superpixel[food[food_count].y][food[food_count].x] = bg_color;
                    if (restart_after_timer)
                        show_size();
                }
            }
        }
        return;
    }

    if (vga_frame % speed == 0)
    for (int p=0; p<2-single_player; ++p)
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
            {
                if (torus)
                    snake[p].head.y = SCREEN_H-1; // should only happen on torus
                else
                    return kill_snake(p);
            }
            break;
        case DOWN:
            if (snake[p].head.y < SCREEN_H-1)
                ++snake[p].head.y;
            else
            {
                if (torus)
                    snake[p].head.y = 0; // should only happen on torus
                else
                    return kill_snake(p);
            }
            break;
        case LEFT: 
            if (snake[p].head.x)
                --snake[p].head.x;
            else
            {
                if (torus)
                    snake[p].head.x = SCREEN_W-1; // should only happen on torus
                else
                    return kill_snake(p);
            }
            break;
        case RIGHT: 
            if (snake[p].head.x < SCREEN_W-1)
                ++snake[p].head.x;
            else
            {
                if (torus)
                    snake[p].head.x = 0; // should only happen on torus
                else
                    return kill_snake(p);
            }
            break;
        }
        
        // check collisions on new spot first:
        if (superpixel[snake[p].head.y][snake[p].head.x] != bg_color)
        {
            if (superpixel[snake[p].head.y][snake[p].head.x] == food_color)
            {
                for (int i=0; i<food_count; ++i)
                {
                    if (food[i].y == snake[p].head.y && food[i].x == snake[p].head.x)
                    {
                        while (superpixel[food[i].y][food[i].x] != bg_color)
                        {
                            food[i].y = rand()%SCREEN_H;
                            food[i].x = rand()%SCREEN_W;
                        }
                        superpixel[food[i].y][food[i].x] = food_color;
                        break;
                    }
                }
                ++snake[p].tail_wait;
            }
            else
                return kill_snake(p);
        }
        
        // then mark the next spot as where you're going!
        // (encode heading for that next time)
        superpixel[snake[p].head.y][snake[p].head.x] = snake[p].color;

        // finished with the snake's head, now go onto the snake's tail!
        if (snake[p].tail_wait)
            --snake[p].tail_wait;
        else
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
        }
    }
}

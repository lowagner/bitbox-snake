#include <stdlib.h> // rand
#include <math.h>
#include "nonsimple.h"
#include "fatfs/ff.h"
#include "string.h" // memset
#include "abc.h"

// variables for heading / direction:

#define UP 0
#define LEFT 1
#define RIGHT 2
#define DOWN 3

// other random variables:

#define FOOD 16
#define BULLETS 3
#define BULLET_LIFE 80
#define INIT_FOOD 2
#define INIT_SIZE 15
#define INIT_SPEED 5 // higher is slower.  1 is fastest
#define CODE_MASK 31710 // color equivalence should be & this mask.

uint16_t player_color[2] = {RGB(255,0,0), RGB(0,50,255)};
uint16_t dead_player_color[2] = {RGB(200,0,0), RGB(0,50,200)};
const uint16_t bullet_color = RGB(200,200,200);

struct snake {
    struct {
        uint8_t y, x;
    } head;
    struct {
        uint8_t y, x;
    } tail;
    uint16_t color;
    uint8_t heading; // direction that the snake is going.
    uint8_t alive;
    uint32_t tail_wait; // set tail_wait > 0 to grow snake.
} snake[2];

struct bullet {
    uint8_t y, x;
    uint8_t heading;
    uint8_t alive;
} bullet[2][BULLETS];

// game variables
uint8_t torus; // is the topography a torus?
uint8_t single_player; // 
uint8_t speed;
uint8_t bullet_length;
uint32_t starting_size;

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

void game_restart();

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

void snake_init(int p, uint8_t y, uint8_t x, uint8_t heading, uint32_t size)
{
    snake[p].head.y = snake[p].tail.y = y;
    snake[p].head.x = snake[p].tail.x = x;
    snake[p].color = player_color[p];
    snake[p].heading = heading;
    snake[p].tail_wait = size;
    snake[p].alive = 1;
}

void kill_snake(int p)
{
    snake[p].alive = 0;
    if (snake[0].alive == 0 && snake[1].alive == 0)
    {   // reset game, both are dead!
        timer = 3;
        restart_after_timer = 1;
    }
}

void zip_snake(int p, uint8_t y, uint8_t x, uint16_t color)
{
    
    // zip up snake p until the tail reaches point y,x.
    // leave a trail of "color" in the wake.
    int i = 0; // counter, once it exceeds the largest possible snake length, it returns...
    while (!(snake[p].tail.y == y && snake[p].tail.x == x))
    {
        // decode the direction the tail is heading from the color it's on:
        uint8_t tail_heading = decode(superpixel[snake[p].tail.y][snake[p].tail.x]);
        // blank the tail
        superpixel[snake[p].tail.y][snake[p].tail.x] = color;
        switch (tail_heading)
        {
        case UP:
            if (snake[p].tail.y)
                --snake[p].tail.y;
            else
                snake[p].tail.y = SCREEN_H-1; 
            break;
        case DOWN:
            if (snake[p].tail.y < SCREEN_H-1)
                ++snake[p].tail.y;
            else
                snake[p].tail.y = 0; 
            break;
        case LEFT: 
            if (snake[p].tail.x)
                --snake[p].tail.x;
            else
                snake[p].tail.x = SCREEN_W-1; 
            break;
        case RIGHT: 
            if (snake[p].tail.x < SCREEN_W-1)
                ++snake[p].tail.x;
            else
                snake[p].tail.x = 0; 
            break;
        }

        if (++i > 19200)
            // something got funny...
            return game_restart();
    }
}

void make_walls()
{
    // setup the walls on the torus
    memset(superpixel[0], 255, 2*SCREEN_W);
    memset(superpixel[SCREEN_H-1], 255, 2*SCREEN_W);
    for (int j=0; j<SCREEN_H; ++j)
        superpixel[j][0] = superpixel[j][SCREEN_W-1] = 65535;
}

void remove_walls()
{
    memset(superpixel[0], 0, 2*SCREEN_W);
    memset(superpixel[SCREEN_H-1], 0, 2*SCREEN_W);
    for (int j=0; j<SCREEN_H; ++j)
        superpixel[j][0] = superpixel[j][SCREEN_W-1] = 0;
}

void screen_reset()
{
    clear();
    for (int i=0; i<food_count; ++i)
        superpixel[food[i].y][food[i].x] = food_color;
    for (int p=0; p<2-single_player; ++p)
        superpixel[snake[p].head.y][snake[p].head.x] = snake[p].color;
    if (!torus)
    {
        make_walls();
    }
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
 
    // put in the number
    swap_colors(&c1,&c2);x+=6;
    if (starting_size < 1000)
    {
        numbers[starting_size/100](y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        numbers[(starting_size/10)%10](y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        numbers[starting_size%10](y,x,c1,c2);
    }
    else
    {
        numbers[starting_size/10000](y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        numbers[(starting_size/1000)%10](y,x,c1,c2);
        swap_colors(&c1,&c2);x+=4;
        draw_k(y,x,c1,c2);

    }
}

void show_bullets()
{
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    uint8_t y=40, x0=SCREEN_W/2 - 9;
    uint8_t x = x0;
    x=x0-1;
    // bullets
    draw_g(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_u(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_n(y,x,c1,c2);
    swap_colors(&c1,&c2);x+=4;
    draw_s(y,x,c1,c2);
 
    // put in the number
    swap_colors(&c1,&c2);x+=6;
    numbers[bullet_length%10](y,x,c1,c2);

}

void show_options()
{
    show_duel();
    show_torus();
    show_speed();
    show_food();
    show_size();
    show_bullets();
    
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
    bg_color = 0; // this MUST BE ZERO.  DO NOT MODIFY.

    srand(vga_frame);

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
    
    for (int i=2; i<FOOD; ++i)
    {
        // pre-randomize the food positions
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

    if (single_player)
        snake[1].alive = 0;

    show_options();
}

void game_init()
{ 
    torus = 1; 
    speed = INIT_SPEED; // smaller values is faster snakes
    // speed should not be any larger than 9.  that's already super slow!
    starting_size = INIT_SIZE;
    food_count = INIT_FOOD;


    game_restart();

    timer = 255; // go to pause
    restart_after_timer = 1; // go to menu on pause
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
                message("restarting game\n");
                game_restart();
                timer = 4;
                return;
            }
            message("set timer to zero\n");
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
        if (gamepad_press[0]  & gamepad_select)
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
            if (gamepad_press[0]  & gamepad_start)
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
            if (gamepad_press[0] & gamepad_down)
            {
                if (speed < 9)
                    ++speed;
                message("speed = %d\n", (int)speed);
                if (restart_after_timer)
                    show_speed();
            }
            else if (gamepad_press[0] & gamepad_up)
            {
                if (speed > 1)
                    --speed;
                message("speed = %d\n", (int)speed);
                if (restart_after_timer)
                    show_speed();
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
                    show_torus();

                if (torus)
                    remove_walls();
                else
                    make_walls();
            }
            if (gamepad_press[0] & gamepad_B)
            {   // add/remove bullets
                bullet_length = (bullet_length + 1)%4;
                if (restart_after_timer)
                    show_bullets();
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
                        snake_init(1, y,x, rand()%4, starting_size);
                    }
                }
                if (restart_after_timer)
                    show_duel();
            }
            if (gamepad_press[0] & gamepad_R)
            {
                if (starting_size < 19000)
                {
                    uint32_t increment;
                    if (starting_size < 111)
                        increment= 5;
                    else if (starting_size < 1000)
                        increment = 59;
                    else
                        increment = 1000;

                    starting_size += increment;
                    for (int p=0; p<2-single_player; ++p)
                        snake[p].tail_wait += increment;
                    message("size = %d\n", (int)starting_size);
                    if (restart_after_timer)
                        show_size();
                }
            }
            else if (gamepad_press[0] & gamepad_L)
            {
                if (starting_size > 6)
                {
                    uint32_t decrement;
                    if (starting_size < 116)
                        decrement= 5;
                    else if (starting_size < 1001)
                        decrement = 59;
                    else
                        decrement = 1000;

                    starting_size -= decrement;
                    for (int p=0; p<2-single_player; ++p)
                        snake[p].tail_wait -= decrement;
                    message("size = %d\n", (int)starting_size);
                    if (restart_after_timer)
                        show_size();
                }
            }
            if (gamepad_press[0] & gamepad_right)
            {
                if (food_count < FOOD)
                {
                    while (superpixel[food[food_count].y][food[food_count].x] != bg_color)
                    {
                        food[food_count].y = rand()%SCREEN_H;
                        food[food_count].x = rand()%SCREEN_W;
                    }
                    superpixel[food[food_count].y][food[food_count].x] = food_color;
                    ++food_count;
                    if (restart_after_timer)
                        show_food();
                }
            }
            else if (gamepad_press[0] & gamepad_left)
            {
                if (food_count)
                { 
                    --food_count;
                    superpixel[food[food_count].y][food[food_count].x] = bg_color;
                    if (restart_after_timer)
                        show_food();
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
            if (superpixel[bullet[p][b].y][bullet[p][b].x] == 65535)
            {   // wall color, ignore!
            }
            else if (superpixel[bullet[p][b].y][bullet[p][b].x] == bullet_color)
            {
                // find the other bullet and kill it
                for (int pb=0; pb<2*BULLETS; ++pb)
                if (bullet[p][b].y == bullet[pb/BULLETS][pb%BULLETS].y && 
                    bullet[p][b].x == bullet[pb/BULLETS][pb%BULLETS].x)
                {
                    bullet[pb/BULLETS][pb%BULLETS].alive = 0; 
                    superpixel[bullet[p][b].y][bullet[p][b].x] = bg_color;
                    break;
                }
            }
            else if ((superpixel[bullet[p][b].y][bullet[p][b].x] & CODE_MASK) == 
                     (snake[1-p].color & CODE_MASK))
            {
                // kill off enemy snake (1-p) if it's his head, otherwise zip him up
                if (bullet[p][b].y == snake[1-p].head.y && bullet[p][b].x == snake[1-p].head.x)
                {
                    kill_snake(1-p); 
                    message("killed enemy by a bullet\n");
                }
                else // was not the head, zip up tail to where bullet hit
                {
                    zip_snake(1-p, bullet[p][b].y, bullet[p][b].x, dead_player_color[1-p]);
                    message("hurt enemy by a bullet\n");
                }
            }
            else if ((superpixel[bullet[p][b].y][bullet[p][b].x] & CODE_MASK) == 
                     (snake[p].color & CODE_MASK))
            {
                // you just hurt or killed yourself!
                if (bullet[p][b].y == snake[p].head.y && bullet[p][b].x == snake[p].head.x)
                {
                    kill_snake(p); 
                    message("killed by your own bullet\n");
                }
                else // was not the head, zip up tail to where bullet hit
                {
                    zip_snake(p, bullet[p][b].y, bullet[p][b].x, dead_player_color[p]);
                    message("hurt by your own bullet\n");
                }
            }
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
                    for (int i=0; i<food_count; ++i)
                    {
                        if (food[i].y == snake[p].head.y && food[i].x == snake[p].head.x)
                        {
                            if (torus)
                            {
                                while (superpixel[food[i].y][food[i].x] != bg_color)
                                {
                                    food[i].y = rand()%SCREEN_H;
                                    food[i].x = rand()%SCREEN_W;
                                }
                            }
                            else
                            {
                                while (superpixel[food[i].y][food[i].x] != bg_color)
                                {
                                    food[i].y = 1 + (rand()%(SCREEN_H-2));
                                    food[i].x = 1 + (rand()%(SCREEN_W-2));
                                }
                            }
                            superpixel[food[i].y][food[i].x] = food_color;
                            break;
                        }
                    }
                    ++snake[p].tail_wait;
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

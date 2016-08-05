#include "field.h"
#include "string.h" // memset

const uint16_t player_color[2] = {RGB(255,0,0), RGB(0,50,255)};
const uint16_t dead_player_color[2] = {RGB(200,0,0), RGB(0,50,200)};
const uint16_t bullet_color = RGB(200,200,200);
const uint16_t food_color = RGB(0,255,0);

// game variables
uint8_t dynamics; // on/off for bullet/snake physics
uint8_t torus; // is the topography a torus?
uint8_t speed;
uint8_t bullet_length;
int32_t starting_size;
int32_t food_count;

// various gameplay variables shared by normal.c and level.c
uint8_t timer;
uint8_t restart_after_timer;
uint8_t single_player; // 
uint16_t gamepad_press[2]; // new button presses only!

struct snake snake[2];
struct bullet bullet[2][BULLETS];

void make_food(int how_much)
{
    if (!torus)
    {
        for (int i=0; i<how_much; ++i)
        {
            uint8_t y = 1+rand()%((SCREEN_H/2)-2);
            uint8_t x = 1+rand()%((SCREEN_W/2)-2);
            int k=0;
            while (superpixel[y][x] != bg_color && k < 50)
            {
                y = 1+rand()%((SCREEN_H/2)-2);
                x = 1+rand()%((SCREEN_W/2)-2);
                ++k;
            }
            superpixel[y][x] = food_color;
        }
    }
    else
    {
        for (int i=0; i<how_much; ++i)
        {
            uint8_t y = rand()%(SCREEN_H/2);
            uint8_t x = rand()%(SCREEN_W/2);
            int k=0;
            while (superpixel[y][x] != bg_color && k < 50)
            {
                y = rand()%(SCREEN_H/2);
                x = rand()%(SCREEN_W/2);
                ++k;
            }
            superpixel[y][x] = food_color;
        }
    }
}

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

void init_snake(int p, uint8_t y, uint8_t x, uint8_t heading, int32_t size)
{
    snake[p].head.y = snake[p].tail.y = y;
    snake[p].head.x = snake[p].tail.x = x;
    snake[p].color = player_color[p];
    snake[p].heading = heading;
    if (size <= 0)
        size = 1;
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

int zip_snake(int p, uint8_t y, uint8_t x, uint16_t color)
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
                snake[p].tail.y = (SCREEN_H/2)-1; 
            break;
        case DOWN:
            if (snake[p].tail.y < (SCREEN_H/2)-1)
                ++snake[p].tail.y;
            else
                snake[p].tail.y = 0; 
            break;
        case LEFT: 
            if (snake[p].tail.x)
                --snake[p].tail.x;
            else
                snake[p].tail.x = (SCREEN_W/2)-1; 
            break;
        case RIGHT: 
            if (snake[p].tail.x < (SCREEN_W/2)-1)
                ++snake[p].tail.x;
            else
                snake[p].tail.x = 0; 
            break;
        }

        if (++i > 19200)
            // something got funny...
            return 1;
    }
    // remove any wait from the tail
    snake[p].tail_wait = 0;
    return 0;
}

void make_walls()
{
    // setup the walls on the torus
    memset(superpixel[0], 255, 2*(SCREEN_W/2));
    memset(superpixel[SCREEN_H/2-1], 255, 2*(SCREEN_W/2));
    for (int j=0; j<SCREEN_H/2; ++j)
        superpixel[j][0] = superpixel[j][SCREEN_W/2-1] = 65535;
}

void remove_walls()
{
    memset(superpixel[0], 0, 2*SCREEN_W/2);
    memset(superpixel[SCREEN_H/2-1], 0, 2*SCREEN_W/2);
    for (int j=0; j<SCREEN_H/2; ++j)
        superpixel[j][0] = superpixel[j][SCREEN_W/2-1] = 0;
}


#ifndef FIELD_H
#define FIELD_H

#include "common.h"
#include <stdlib.h> // rand

// variables for heading / direction:

#define UP 0
#define LEFT 1
#define RIGHT 2
#define DOWN 3

// other random variables:

#define FOOD 5000 // should be some small-ish fraction of 19200 (max things on screen)
#define BULLETS 3 // max bullets
#define BULLET_LIFE 86
#define INIT_FOOD 3
#define INIT_SIZE 20
#define INIT_SPEED 3 // higher is slower.  1 is fastest
#define CODE_MASK 31710 // color equivalence should be & this mask.

extern const uint16_t player_color[2];
extern const uint16_t dead_player_color[2];
extern const uint16_t bullet_color;
extern const uint16_t food_color;

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
    int32_t tail_wait; // set tail_wait > 0 to grow snake.  set it < 0 to make it shrink.
};

extern struct snake snake[2];

struct bullet {
    uint8_t y, x;
    uint8_t heading;
    uint8_t alive;
};
extern struct bullet bullet[2][BULLETS];

// game variables
extern uint8_t dynamics; // on/off for bullet/snake physics
extern uint8_t torus; // is the topography a torus?
extern uint8_t speed;
extern uint8_t bullet_length;
extern int32_t starting_size;
extern int32_t food_count;

// various gameplay variables shared by normal.c and level.c
extern uint8_t timer;
extern uint8_t restart_after_timer;
extern uint8_t single_player; // 
extern uint16_t gamepad_press[2]; // new button presses only!


// field-specific functions

void make_food(int how_much);

uint16_t encode(uint16_t color, uint8_t heading);
uint8_t decode(uint16_t color);

void init_snake(int p, uint8_t y, uint8_t x, uint8_t heading, int32_t size);
void kill_snake(int p);
int zip_snake(int p, uint8_t y, uint8_t x, uint16_t color);

void make_walls();
void remove_walls();


// some things defined in main that other people need to see
void start_play_countdown();
void show_options();
void start_game_play();

inline int options_already_visible()
{
    return ((snake[0].alive || snake[1].alive) && (restart_after_timer || timer < 255));
}


#endif

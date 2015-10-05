#ifndef OPTIONS_H
#define OPTIONS_H
#include <stdint.h>

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

void show_duel_options();

void show_torus_options();

void show_speed_options();

void show_big_number(int v, uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);

void show_food_options();

void show_size_options();

void show_gun_options();

void show_controls();

#endif

#ifndef ABC_H
#define ABC_H

#include "nonsimple.h"

inline void swap_colors(uint16_t *c1, uint16_t *c2)
{
    uint16_t c = *c1;
    *c1 = *c2;
    *c2 = c;
}

typedef void (letter_fn)(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);

void draw_space(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_t(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_i(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_f(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_l(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_o(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_w(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_a(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_g(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_n(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_e(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_r(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_u(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_s(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_p(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_d(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);

void draw_z(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);

void draw_1(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_3(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_4(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_6(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_7(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_8(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);
void draw_9(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2);


letter_fn *numbers[10];


#endif

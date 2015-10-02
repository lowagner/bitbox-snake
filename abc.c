#include "abc.h"

letter_fn *numbers[10] = { &draw_o, &draw_1, &draw_z, &draw_3, &draw_4, &draw_s, &draw_6, &draw_7, &draw_b, &draw_9 };

void draw_space(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = bg_color;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = bg_color;
}

void draw_t(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;
}

void draw_i(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c2;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c2;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c2;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c2;
}

void draw_f(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = bg_color;
}

void draw_k(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;
}
void draw_w(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = c1;
}

void draw_a(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = bg_color;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;
}

void draw_g(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;
}

void draw_n(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;
}

void draw_o(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;
}

void draw_r(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c2;
}

void draw_u(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;
}

void draw_l(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;
}

void draw_s(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = bg_color;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;
}

void draw_p(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = c2;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = bg_color;
}

void draw_e(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;
}

void draw_d(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c2;
}

void draw_1(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = bg_color;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;
}

void draw_z(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;
}

void draw_3(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = bg_color;
}

void draw_4(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;
}

void draw_6(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = bg_color;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = c2;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;
}

void draw_7(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;
}

void draw_b(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = c1;
}

void draw_9(uint8_t y, uint8_t x, uint16_t c1, uint16_t c2)
{
    superpixel[y][x] = c1;
    superpixel[y][x+1] = c1;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = c1;
    superpixel[y][x+1] = c2;
    superpixel[y][x+2] = c1;

    superpixel[++y][x] = bg_color;
    superpixel[y][x+1] = bg_color;
    superpixel[y][x+2] = c1;
}

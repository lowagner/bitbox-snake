#include "options.h"
#include "field.h"
#include "abc.h"

void show_duel_options()
{
    uint16_t c1 = RGB(255,255,0);
    uint16_t c2 = RGB(0,255,255);
    uint8_t y=22;
    uint8_t x = SCREEN_W/4 - 9 +2;
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
    uint8_t y=26, x=SCREEN_W/4 - 9;
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
    uint8_t y=34, x0=SCREEN_W/4 - 9;
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
    uint8_t y=38, x=SCREEN_W/4 - 9-5;
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
    uint8_t y=42, x=SCREEN_W/4 - 9-5;
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
    uint8_t y=18, x=SCREEN_W/4 - 9-1;
    // bullets
    draw_g(y,x,c1,c2);x+=4;
    draw_u(y,x,c2,c1);x+=4;
    draw_n(y,x,c1,c2);x+=4;
    draw_s(y,x,c2,c1);
 
    // put in the number
    x+=6;
    numbers[bullet_length%10](y,x,c1,c2);

}

void show_controls()
{
    uint16_t c2 = RGB(255,255,0);
    uint16_t c1 = RGB(0,255,255);
    uint8_t y=10, x=SCREEN_W/4 - 26;
    //draw_a(y,x,c1,c2);
    y += 8;
    draw_b(y,x,c1,c2);
    y += 4;
    draw_x(y,x,c1,c2);
    y += 4;
    draw_y(y,x,c1,c2);
    y += 8;
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
}


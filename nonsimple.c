#include "nonsimple.h"

#include <stdint.h>
#include <bitbox.h>
#include <stdlib.h> // abs
#include <string.h> // memset

void_fn *graph_line_callback;
uint16_t superpixel[SCREEN_H][SCREEN_W] __attribute__ ((section (".ccm")));
uint16_t bg_color __attribute__ ((section (".ccm")));


void graph_frame() {}
// --------------------------------------------------------------

void graph_line() 
{
    if (vga_odd)
    {
        if (graph_line_callback)
            graph_line_callback();
        return;
    }
    uint32_t *dst = (uint32_t*)draw_buffer;
    int j = vga_line/2;
    for (int i=0; i<SCREEN_W; ++i)
    {
        uint16_t current_color = superpixel[j][i];
        *dst++ = current_color | (current_color<<16);
    }
}

// --------------------------------------------------------------
// utilities 

void clear() 
{
   memset(superpixel, 0, sizeof(superpixel));
}


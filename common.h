#ifndef COMMON_H
#define COMMON_H
#include "bitbox.h"

#define SCREEN_W 320 // number of regular pixels
#define SCREEN_H 240
//#define SCREEN_W 160      // number of super pixels (2x2 pixels)
//#define SCREEN_H 120
    
const uint16_t palette[16];

typedef void (void_fn)(void);

extern uint16_t superpixel[SCREEN_H/2][SCREEN_W/2];
extern uint16_t bg_color;
extern void_fn* graph_line_callback;

void clear();

typedef enum {
    None=0,
    GameOn,
    EditAnthem,
    EditVerse,
    EditInstrument,
    ChooseFilename
} VisualMode;

extern VisualMode visual_mode;
extern VisualMode previous_visual_mode;

#define GAMEPAD_PRESS(id, key) (gamepad_press[id] & (gamepad_##key))
#define GAMEPAD_PRESSING(id, key) ((gamepad_press[id] & (gamepad_##key)) || ((gamepad_press_wait == 0) && (gamepad_buttons[id] & (gamepad_##key))))
extern uint16_t old_gamepad[2];
extern uint16_t gamepad_press[2];
extern uint8_t gamepad_press_wait;

#define GAMEPAD_PRESS_WAIT 8

extern uint8_t game_message[32];
extern const uint8_t hex[64]; // not exactly hex but ok!

void game_switch(VisualMode new_visual_mode);

void do_snake_dynamics();
int do_bullet_dynamics();

#endif

# DO NOT FORGET to define BITBOX environment variable 

USE_SDCARD = 1      # allow use of SD card for io

NAME = snake
GAME_C_FILES = nonsimple.c arcade.c field.c options.c io.c abc.c main.c
GAME_H_FILES = nonsimple.h arcade.h field.h options.h io.h abc.h
GAME_C_OPTS += -DVGAMODE_320

# see this file for options
include $(BITBOX)/lib/bitbox.mk


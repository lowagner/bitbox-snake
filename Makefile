# DO NOT FORGET to define BITBOX environment variable 

USE_SDCARD = 1      # allow use of SD card for io
DEFINES += DEBUG_CHIPTUNE

NAME = snake
GAME_C_FILES = font.c chiptune.c song.c track.c instrument.c arcade.c \
    field.c options.c io.c abc.c name.c main.c
GAME_H_FILES = font.h chiptune.h song.h track.h instrument.h arcade.h \
    field.h options.h io.h abc.h name.h common.h
GAME_C_OPTS += -DVGAMODE_320

# see this file for options
include $(BITBOX)/lib/bitbox.mk


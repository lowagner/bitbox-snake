# DO NOT FORGET to define BITBOX environment variable 

USE_SDCARD = 1      # allow use of SD card for io

NAME = snakey
GAME_C_FILES = nonsimple.c abc.c main.c
GAME_H_FILES = nonsimple.h abc.h
GAME_C_OPTS += -DVGAMODE_640

# see this file for options
include $(BITBOX)/lib/bitbox.mk


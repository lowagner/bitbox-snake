#ifndef IO_H
#define IO_H

#include <stdint.h>

typedef enum {
    NoError = 0,
    MountError,
    ConstraintError,
    OpenError,
    ReadError,
    WriteError,
    NoDataError,
    MissingDataError,
    BotchedIt
} FileError;

extern char base_filename[9];
extern uint8_t mount_ok;

FileError io_init();
void take_screenshot();

FileError io_get_recent_filename();
void io_message_from_error(char *msg, FileError error, int save_not_load);
FileError io_save_instrument(unsigned int i);
FileError io_load_instrument(unsigned int i);
FileError io_save_verse(unsigned int i);
FileError io_load_verse(unsigned int i);
FileError io_save_anthem();
FileError io_load_anthem();

#endif

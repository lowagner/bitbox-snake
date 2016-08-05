#include "io.h"
#include "chiptune.h"
#include "fatfs/ff.h"

#include "field.h"
#include <string.h> // strlen

FATFS fat_fs;
FIL fat_file;
FRESULT fat_result;
uint8_t mount_ok CCM_MEMORY;

char base_filename[9] CCM_MEMORY;
char old_base_filename[9] CCM_MEMORY;
uint8_t old_chip_volume CCM_MEMORY;
    
FileError io_init()
{
    base_filename[0] = 0;
    fat_result = f_mount(&fat_fs, "", 1); // mount now...
    if (fat_result != FR_OK)
    {
        mount_ok = 0;
        return MountError;
    }
    mount_ok = 1;
    return NoError;
}

FileError io_set_recent_filename()
{
    int filename_len = strlen(base_filename);
    if (filename_len == 0)
        return ConstraintError;

    if (mount_ok == 0)
    {
        if (io_init())
            return MountError;
    }
    
    if (strcmp(old_base_filename, base_filename) == 0 && chip_volume == old_chip_volume)
        return NoError; // don't rewrite  

    fat_result = f_open(&fat_file, "RECENT16.TXT", FA_WRITE | FA_CREATE_ALWAYS); 
    if (fat_result != FR_OK) 
        return OpenError;
    UINT bytes_get;
    fat_result = f_write(&fat_file, base_filename, filename_len, &bytes_get);
    if (fat_result != FR_OK)
    {
        f_close(&fat_file);
        return WriteError;
    }
    if (bytes_get != filename_len)
    {
        f_close(&fat_file);
        return MissingDataError;
    }
    strcpy(old_base_filename, base_filename);
    uint8_t msg[2] = { '\n', chip_volume };
    fat_result = f_write(&fat_file, msg, 2, &bytes_get);
    f_close(&fat_file);
    if (fat_result != FR_OK)
        return WriteError;
    if (bytes_get != 2)
        return MissingDataError;
    old_chip_volume = chip_volume;
    return NoError;
}


void io_message_from_error(char *msg, FileError error, int save_not_load)
{
    switch (error)
    {
    case NoError:
        if (save_not_load == 1)
            strcpy(msg, "saved!");
        else
            strcpy(msg, "loaded!");
        break;
    case MountError:
        strcpy(msg, "fs unmounted!");
        break;
    case ConstraintError:
        strcpy(msg, "unconstrained!");
        break;
    case OpenError:
        strcpy(msg, "no open!");
        break;
    case ReadError:
        strcpy(msg, "no read!");
        break;
    case WriteError:
        strcpy(msg, "no write!");
        break;
    case NoDataError:
        strcpy(msg, "no data!");
        break;
    case MissingDataError:
        strcpy(msg, "miss data!");
        break;
    case BotchedIt:
        strcpy(msg, "fully bungled.");
        break;
    }
}

void take_screenshot()
{
    if (!mount_ok)
    {
        if (io_init())
            return;
    }
    char count[3] = { '0', '0', '0' };
    fat_result = f_open(&fat_file,"SNAKESS.TXT",FA_READ | FA_WRITE | FA_OPEN_ALWAYS); 
    UINT bytes_read;
    if (fat_result==FR_OK) 
    {
        if (f_read(&fat_file, &count, 3, &bytes_read) == FR_OK && bytes_read == 3) 
        {
            ++count[2];
            if (count[2] > '9')
            {
                count[2] = '0';
                ++count[1];
                if (count[1] > '9')
                {
                    count[1] = '0';
                    ++count[0];
                    if (count[0] > '9')
                    {
                        count[0] = '0';
                    }
                }
            }
        } 
        
        f_lseek(&fat_file,0);
        f_write(&fat_file,  &count, 3, &bytes_read); // dont check result
        f_close(&fat_file);
    }

    char filename[13] = { 'S', 'N', 'A', 'K', 'E', count[0], count[1], count[2], '.', 'P', 'P', 'M', 0 };

    fat_result = f_open(&fat_file,filename, FA_WRITE | FA_OPEN_ALWAYS);
    if (fat_result == FR_OK)
    {
        UINT bytes_get;
        f_write(&fat_file, "P6\n160 120 31\n", 14, &bytes_get);
        for (int j=0; j<SCREEN_H/2; j++)
        {
            char msg[SCREEN_W*3/2]; // number of rgb chars to write per line
            char *src = msg;
            for (int i=0; i<SCREEN_W/2; i++)
            {
                uint16_t C = superpixel[j][i];
                *src++ = (C >> 10)&31; // red
                *src++ = (C >> 5)&31; // green
                *src++ = (C)&31; // blue
            }
            f_write(&fat_file, msg, SCREEN_W*3/2, &bytes_get);
        }
        f_close(&fat_file);
    }
}

FileError io_set_extension(char *filename, const char *ext)
{
    if (mount_ok == 0)
    {
        if (io_init())
            return MountError;
    }
    io_set_recent_filename();

    int k = 0;
    for (; k<8 && base_filename[k]; ++k)
    { 
        filename[k] = base_filename[k];
    }
    --k;
    filename[++k] = '.';
    filename[++k] = *ext++;
    filename[++k] = *ext++;
    filename[++k] = *ext;
    filename[++k] = 0;
    return NoError;
}

FileError io_open_or_zero_file(const char *fname, unsigned int size)
{
    if (size == 0)
        return NoDataError;

    fat_result = f_open(&fat_file, fname, FA_WRITE | FA_OPEN_EXISTING);
    if (fat_result != FR_OK)
    {
        fat_result = f_open(&fat_file, fname, FA_WRITE | FA_OPEN_ALWAYS);
        if (fat_result != FR_OK)
            return OpenError;
        uint8_t zero[128] = {0};
        while (size) 
        {
            int write_size;
            if (size <= 128)
            {
                write_size = size;
                size = 0;
            }
            else
            {
                write_size = 128;
                size -= 128;
            }
            UINT bytes_get;
            f_write(&fat_file, zero, write_size, &bytes_get);
            if (bytes_get != write_size)
            {
                f_close(&fat_file);
                return MissingDataError;
            }
        }
    }
    return NoError;
}

FileError io_get_recent_filename()
{
    if (mount_ok == 0)
    {
        if (io_init())
            return MountError;
    }

    fat_result = f_open(&fat_file, "RECENT16.TXT", FA_READ | FA_OPEN_EXISTING); 
    if (fat_result != FR_OK) 
        return OpenError;

    UINT bytes_get;
    uint8_t buffer[10];
    fat_result = f_read(&fat_file, &buffer[0], 10, &bytes_get); 
    f_close(&fat_file);
    if (fat_result != FR_OK)
        return ReadError;
    if (bytes_get == 0)
        return NoDataError;
    int i=0;
    while (i < bytes_get)
    {
        if (buffer[i] == '\n')
        {
            base_filename[i] = 0;
            if (++i < bytes_get)
                chip_volume = buffer[i];
            return NoError;
        }
        if (i < 8)
            base_filename[i] = buffer[i];
        else
        {
            base_filename[8] = 0;
            return ConstraintError;
        }
        ++i;
    }
    base_filename[i] = 0;
    return NoError;
}

FileError io_load_instrument(unsigned int i)
{
    char filename[13];
    if (io_set_extension(filename, "I16"))
        return MountError;
    
    fat_result = f_open(&fat_file, filename, FA_READ | FA_OPEN_EXISTING);
    if (fat_result != FR_OK)
        return OpenError;

    UINT bytes_get; 
    if (i >= 16)
    {
        for (i=0; i<16; ++i)
        {
            uint8_t read;
            fat_result = f_read(&fat_file, &read, 1, &bytes_get);
            if (fat_result != FR_OK)
            {
                f_close(&fat_file);
                return ReadError;
            }
            if (bytes_get != 1)
            {
                f_close(&fat_file);
                return MissingDataError;
            }
            instrument[i].is_drum = read&15;
            instrument[i].octave = read >> 4;
            fat_result = f_read(&fat_file, &instrument[i].cmd[0], MAX_INSTRUMENT_LENGTH, &bytes_get);
            if (fat_result != FR_OK)
            {
                f_close(&fat_file);
                return ReadError;
            }
            if (bytes_get != MAX_INSTRUMENT_LENGTH)
            {
                f_close(&fat_file);
                return MissingDataError;
            }
        }
        f_close(&fat_file);
        return NoError;
    }

    f_lseek(&fat_file, i*(MAX_INSTRUMENT_LENGTH+1)); 
    uint8_t read;
    fat_result = f_read(&fat_file, &read, 1, &bytes_get);
    if (fat_result != FR_OK)
    {
        f_close(&fat_file);
        return ReadError;
    }
    if (bytes_get != 1)
    {
        f_close(&fat_file);
        return MissingDataError;
    }
    instrument[i].is_drum = read&15;
    instrument[i].octave = read >> 4;
    fat_result = f_read(&fat_file, &instrument[i].cmd[0], MAX_INSTRUMENT_LENGTH, &bytes_get);
    if (fat_result != FR_OK)
    {
        f_close(&fat_file);
        return ReadError;
    }
    if (bytes_get != MAX_INSTRUMENT_LENGTH)
    {
        f_close(&fat_file);
        return MissingDataError;
    }
    
    f_close(&fat_file);
    return NoError;
}

FileError io_save_instrument(unsigned int i)
{
    char filename[13];
    if (io_set_extension(filename, "I16"))
        return MountError; 

    if (i >= 16)
    {
        fat_result = f_open(&fat_file, filename, FA_WRITE | FA_OPEN_ALWAYS);
        if (fat_result != FR_OK)
            return OpenError;

        for (i=0; i<16; ++i)
        {
            UINT bytes_get; 
            uint8_t write = (instrument[i].is_drum ? 1 : 0) | (instrument[i].octave << 4);
            fat_result = f_write(&fat_file, &write, 1, &bytes_get);
            if (fat_result != FR_OK)
            {
                f_close(&fat_file);
                return WriteError;
            }
            if (bytes_get != 1)
            {
                f_close(&fat_file);
                return MissingDataError;
            }
            fat_result = f_write(&fat_file, &instrument[i].cmd[0], MAX_INSTRUMENT_LENGTH, &bytes_get);
            if (fat_result != FR_OK)
            {
                f_close(&fat_file);
                return WriteError;
            }
            if (bytes_get != MAX_INSTRUMENT_LENGTH)
            {
                f_close(&fat_file);
                return MissingDataError;
            }
        }
        f_close(&fat_file);
        return NoError;
    }

    FileError ferr = io_open_or_zero_file(filename, 16*(MAX_INSTRUMENT_LENGTH+1));
    if (ferr)
        return ferr;

    f_lseek(&fat_file, i*(MAX_INSTRUMENT_LENGTH+1)); 
    UINT bytes_get; 
    uint8_t write = (instrument[i].is_drum ? 1 : 0) | (instrument[i].octave << 4);
    fat_result = f_write(&fat_file, &write, 1, &bytes_get);
    if (fat_result != FR_OK)
    {
        f_close(&fat_file);
        return WriteError;
    }
    if (bytes_get != 1)
    {
        f_close(&fat_file);
        return MissingDataError;
    }
    fat_result = f_write(&fat_file, &instrument[i].cmd[0], MAX_INSTRUMENT_LENGTH, &bytes_get);
    if (fat_result != FR_OK)
    {
        f_close(&fat_file);
        return WriteError;
    }
    if (bytes_get != MAX_INSTRUMENT_LENGTH)
    {
        f_close(&fat_file);
        return MissingDataError;
    }
    
    f_close(&fat_file);
    return NoError;
}

FileError io_load_verse(unsigned int i)
{
    char filename[13];
    if (io_set_extension(filename, "V16"))
        return MountError; 

    fat_result = f_open(&fat_file, filename, FA_READ | FA_OPEN_EXISTING);
    if (fat_result != FR_OK)
        return OpenError;

    if (i >= 16)
    {
        for (i=0; i<16; ++i)
        {
            UINT bytes_get; 
            fat_result = f_read(&fat_file, &chip_track[i], sizeof(chip_track[0]), &bytes_get);
            if (fat_result != FR_OK)
            {
                f_close(&fat_file);
                return ReadError;
            }
            if (bytes_get != sizeof(chip_track[0]))
            {
                f_close(&fat_file);
                return MissingDataError;
            }
        }
        f_close(&fat_file);
        return NoError;
    }

    f_lseek(&fat_file, i*sizeof(chip_track[0])); 
    UINT bytes_get; 
    fat_result = f_read(&fat_file, &chip_track[i], sizeof(chip_track[0]), &bytes_get);
    if (fat_result != FR_OK)
    {
        f_close(&fat_file);
        return ReadError;
    }
    if (bytes_get != sizeof(chip_track[0]))
    {
        f_close(&fat_file);
        return MissingDataError;
    }
    f_close(&fat_file);
    return NoError;
}

FileError io_save_verse(unsigned int i)
{
    char filename[13];
    if (io_set_extension(filename, "V16"))
        return MountError; 

    if (i >= 16)
    {
        fat_result = f_open(&fat_file, filename, FA_WRITE | FA_OPEN_ALWAYS);
        if (fat_result != FR_OK)
            return OpenError;

        for (i=0; i<16; ++i)
        {
            UINT bytes_get; 
            fat_result = f_write(&fat_file, &chip_track[i], sizeof(chip_track[0]), &bytes_get);
            if (fat_result != FR_OK)
            {
                f_close(&fat_file);
                return WriteError;
            }
            if (bytes_get != sizeof(chip_track[0]))
            {
                f_close(&fat_file);
                return MissingDataError;
            }
        }
        f_close(&fat_file);
        return NoError;
    }

    FileError ferr = io_open_or_zero_file(filename, 16*sizeof(chip_track[0]));
    if (ferr)
        return ferr;

    f_lseek(&fat_file, i*sizeof(chip_track[0])); 
    UINT bytes_get; 
    fat_result = f_write(&fat_file, &chip_track[i], sizeof(chip_track[0]), &bytes_get);
    if (fat_result != FR_OK)
    {
        f_close(&fat_file);
        return WriteError;
    }
    if (bytes_get != sizeof(chip_track[0]))
    {
        f_close(&fat_file);
        return MissingDataError;
    }
    f_close(&fat_file);
    return NoError;
}

FileError io_load_anthem()
{
    char filename[13];
    if (io_set_extension(filename, "A16"))
        return MountError; 

    fat_result = f_open(&fat_file, filename, FA_READ | FA_OPEN_EXISTING); 
    if (fat_result)
        return OpenError;

    // set some defaults
    track_length = 16;
    song_speed = 4;

    UINT bytes_get; 
    fat_result = f_read(&fat_file, &song_length, 1, &bytes_get);
    if (fat_result != FR_OK)
    {
        f_close(&fat_file);
        return ReadError;
    }
    if (bytes_get != 1)
    {
        f_close(&fat_file);
        return MissingDataError;
    }

    if (song_length < 16 || song_length > MAX_SONG_LENGTH)
    {
        message("got song length %d\n", song_length);
        song_length = 16;
        f_close(&fat_file);
        return ConstraintError;
    }
    
    fat_result = f_read(&fat_file, &chip_song[0], 2*song_length, &bytes_get);
    if (fat_result != FR_OK)
    {
        f_close(&fat_file);
        return ReadError;
    }
    if (bytes_get != 2*song_length)
    {
        f_close(&fat_file);
        return MissingDataError;
    }

    f_close(&fat_file);
    return NoError;
}

FileError io_save_anthem()
{
    char filename[13];
    if (io_set_extension(filename, "A16"))
        return MountError; 

    fat_result = f_open(&fat_file, filename, FA_WRITE | FA_CREATE_ALWAYS); 
    if (fat_result)
        return OpenError;

    UINT bytes_get; 
    fat_result = f_write(&fat_file, &song_length, 1, &bytes_get);
    if (fat_result != FR_OK)
    {
        f_close(&fat_file);
        return WriteError;
    }
    if (bytes_get != 1)
    {
        f_close(&fat_file);
        return MissingDataError;
    }
    
    fat_result = f_write(&fat_file, &chip_song[0], 2*song_length, &bytes_get);
    if (fat_result != FR_OK)
    {
        f_close(&fat_file);
        return WriteError;
    }
    if (bytes_get != 2*song_length)
    {
        f_close(&fat_file);
        return MissingDataError;
    }

    f_close(&fat_file);
    return NoError;
}

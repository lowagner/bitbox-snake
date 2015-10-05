#include "io.h"
#include "field.h"

FATFS fat_fs;
FIL fat_file;
FRESULT fat_result;
uint8_t mount_ok;
    
void io_init()
{
    fat_result = f_mount(&fat_fs, "", 1); // mount now...
    mount_ok = (fat_result == FR_OK);
}

void take_screenshot()
{
    if (!mount_ok)
    {
        io_init();
        if (!mount_ok)
            return;
    }
    char count[3] = { '0', '0', '0' };
    fat_result = f_open(&fat_file,"snakess.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS); 
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

    char filename[13] = { 's', 'n', 'a', 'k', 'e', count[0], count[1], count[2], '.', 'p', 'p', 'm', 0 };

    fat_result = f_open(&fat_file,filename, FA_WRITE | FA_OPEN_ALWAYS);
    if (fat_result == FR_OK)
    {
        UINT bytes_get;
        f_write(&fat_file, "P6\n160 120 31\n", 14, &bytes_get);
        for (int j=0; j<SCREEN_H; j++)
        {
            char msg[SCREEN_W*3]; // number of rgb chars to write per line
            char *src = msg;
            for (int i=0; i<SCREEN_W; i++)
            {
                uint16_t C = superpixel[j][i];
                *src++ = (C >> 10)&31; // red
                *src++ = (C >> 5)&31; // green
                *src++ = (C)&31; // blue
            }
            f_write(&fat_file, msg, SCREEN_W*3, &bytes_get);
        }
        f_close(&fat_file);
    }
}

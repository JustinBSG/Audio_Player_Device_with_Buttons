#ifndef __SD_CARD_FS_H__
#define __SD_CARD_FS_H__

#include <stdint.h>
#include <string.h>

#include "fatfs.h"

#define MAX_FILENAME_LENGTH 128
#define MAX_FILES 100

extern UART_HandleTypeDef huart1;

void sd_card_init(void);
FRESULT sd_card_mount(void);
FRESULT sd_card_unmount(void);
FRESULT sd_card_format(void);
FRESULT sd_card_get_free_space(int* free_byte);

FRESULT sd_card_create_directory(const char* dir_name);
FRESULT sd_card_delete_directory(const char* dir_name);
FRESULT sd_card_cd(const char* dir_name);
FRESULT sd_card_ls(char filename[][MAX_FILENAME_LENGTH], int max_files, int* file_count);
FRESULT sd_card_pwd(char* path, int max_len);

FRESULT sd_card_create_file(const char* filename);
FRESULT sd_card_delete_file(const char* filename);
FRESULT sd_card_read_file(const char* filename, uint8_t* buffer, UINT bytes_to_read, UINT* bytes_read);
FRESULT sd_card_write_file(const char* filename, const uint8_t* data, UINT data_size, UINT* bytes_written);

void sd_card_test(void);

extern FATFS fatfs;  // Fatfs handle
extern FIL file;     // File handle
extern FRESULT err;  // Result
extern UINT write_count, read_count;
extern DWORD free_clusters;
extern uint32_t total_size, free_space;

#endif /* __SD_CARD_FS_H__ */
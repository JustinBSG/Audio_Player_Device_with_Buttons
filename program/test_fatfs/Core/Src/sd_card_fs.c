#include "sd_card_fs.h"

FATFS fatfs;          // Fatfs handle
FIL file;             // File handle
FRESULT err = FR_OK;  // Result
UINT write_count, read_count;
DWORD free_clusters;
uint32_t total_size, free_space;

void sd_card_init(void) {
  MX_FATFS_Init();
  // TODO: need mounting before any file operations
}

// TODO: remove all while (1) loops in error handling after testing

FRESULT sd_card_mount(void) {
  char uart_buffer[256];
  err = f_mount(&fatfs, "", 1);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_mount error: %d\n", err);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
    while (1);
  }
  sprintf(uart_buffer, "SD card mounted successfully.\n");
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
  return err;
}

FRESULT sd_card_unmount(void) {
  char uart_buffer[256];
  err = f_mount(NULL, "", 0);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_mount unmount error: %d\n", err);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
    while (1);
  }
  sprintf(uart_buffer, "SD card unmounted successfully.\n");
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
  return err;
}

FRESULT sd_card_format(void) {
  char uart_buffer[256];
  err = f_mkfs("", 0, 0);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_mkfs error: %d\n", err);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
    while (1);
  }
  sprintf(uart_buffer, "SD card formatted successfully.\n");
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
  return err;
}

FRESULT sd_card_get_free_space(int *free_byte) {
  char uart_buffer[256];
  err = f_getfree("", &free_clusters, &fatfs);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_getfree error: %d\n", err);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
    while (1);
  }
  total_size = (fatfs.n_fatent - 2) * fatfs.csize / 2 * 1024;  // in bytes
  free_space = free_clusters * fatfs.csize / 2 * 1024;         // in bytes
  sprintf(uart_buffer, "SD card total size: %lu B, free space: %lu B\n", total_size, free_space);
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
  *free_byte = free_space;
  return err;
}

FRESULT sd_card_create_directory(const char *dir_name) {
  char uart_buffer[256];
  err = f_mkdir(dir_name);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_mkdir error: %d\n", err);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
    while (1);
  }
  sprintf(uart_buffer, "Directory %s created successfully.\n", dir_name);
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
  return err;
}

FRESULT sd_card_delete_directory(const char *dir_name) {
  char uart_buffer[256];
  // dir need to be empty to be deleted
  err = f_unlink(dir_name);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_unlink error: %d\n", err);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
    while (1);
  }
  sprintf(uart_buffer, "Directory %s deleted successfully.\n", dir_name);
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
  return err;
}

FRESULT sd_card_cd(const char *dir_name) {
  char uart_buffer[256];
  err = f_chdir(dir_name);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_chdir error: %d\n", err);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
    while (1);
  }
  sprintf(uart_buffer, "Changed directory to %s\n", dir_name);
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
  return err;
}

FRESULT sd_card_ls(char filename[][MAX_FILENAME_LENGTH], int max_files, int *file_count) {
  DIR dir;
  FILINFO fno;
  char uart_buffer[256];
  FRESULT res;

  *file_count = 0;

  res = f_opendir(&dir, "");
  if (res != FR_OK) {
    sprintf(uart_buffer, "f_opendir error: %d\n", res);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
    while (1);
  }

  sprintf(uart_buffer, "Directory listing:\n");
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);

  while (1) {
    res = f_readdir(&dir, &fno);
    if (res != FR_OK || fno.fname[0] == 0)
      break;

    if (*file_count < max_files) {
      strcpy(filename[*file_count], fno.fname);
      // char temp[128];
      // strcpy(temp, fno.fname);
      // HAL_UART_Transmit(&huart1, "test1\n", strlen("test1\n"), HAL_MAX_DELAY);
      // strcpy(filename[*file_count], temp);
      // HAL_UART_Transmit(&huart1, "test2\n", strlen("test2\n"), HAL_MAX_DELAY);
      (*file_count)++;
    }

    sprintf(uart_buffer, "%s\n", fno.fname);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
  }

  f_closedir(&dir);
  return res;
}

FRESULT sd_card_pwd(char *path, int max_len) {}

// static void print_tree(const char* path, int level) {
//   DIR dir;
//   FILINFO fno;
//   char uart_buffer[256];
//   FRESULT res = f_opendir(&dir, path);
//   if (res != FR_OK) {
//     sprintf(uart_buffer, "f_opendir error in tree: %d\n", res);
//     HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
//     return;
//   }
  
//   while (1) {
//     res = f_readdir(&dir, &fno);
//     if (res != FR_OK || fno.fname[0] == 0) break;
    
//     // Indentation
//     for (int i = 0; i < level; i++) {
//       HAL_UART_Transmit(&huart1, (uint8_t *)"  ", 2, HAL_MAX_DELAY);
//     }
    
//     sprintf(uart_buffer, "%s", fno.fname);
//     HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
    
//     if (fno.fattrib & AM_DIR) {
//       HAL_UART_Transmit(&huart1, (uint8_t *)"/\n", 2, HAL_MAX_DELAY);
//       // Build new path
//       char newpath[256];
//       if (strcmp(path, "") == 0) {
//         sprintf(newpath, "%s", fno.fname);
//       } else {
//         sprintf(newpath, "%s/%s", path, fno.fname);
//       }
//       print_tree(newpath, level + 1);
//     } else {
//       HAL_UART_Transmit(&huart1, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
//     }
//   }
  
//   f_closedir(&dir);
// }

// FRESULT sd_card_tree(void) {
//   char uart_buffer[256];
//   sprintf(uart_buffer, "Directory tree:\n");
//   HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
//   print_tree("", 0);
//   return FR_OK;
// }

FRESULT sd_card_create_file(const char *filename) {
  char uart_buffer[256];
  err = f_open(&file, filename, FA_CREATE_ALWAYS);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_open error: %d\n", err);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
    while (1);
  }
  f_close(&file);
  sprintf(uart_buffer, "File %s created successfully.\n", filename);
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
  return err;
}

FRESULT sd_card_delete_file(const char *filename) {
  char uart_buffer[256];
  err = f_unlink(filename);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_unlink error: %d\n", err);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
    while (1);
  }
  sprintf(uart_buffer, "File %s deleted successfully.\n", filename);
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
  return err;
}

FRESULT sd_card_rename_file(const char* old_filename, const char* new_filename) {
  char uart_buffer[256];
  err = f_rename(old_filename, new_filename);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_rename error: %d\n", err);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
    while (1);
  }
  sprintf(uart_buffer, "File %s renamed to %s successfully.\n", old_filename, new_filename);
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
  return err;
}

FRESULT sd_card_read_file(const char *filename, uint8_t *buffer, UINT bytes_to_read, UINT *bytes_read) {}

FRESULT sd_card_write_file(const char *filename, const uint8_t *data, UINT data_size, UINT *bytes_written) {}

void sd_card_test(void) {
  char data[256];
  char uart_buffer[256];

  // // open a txt file and write data
  // // observe append or overwrite behavior, should be overwrite
  err = f_open(&file, "test.txt", FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_open error: %d\n", err);
    HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
    while (1);
  }
  sprintf(uart_buffer, "File created/opened successfully.\n");
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
  f_puts("Hello, World!1\n", &file);
  memset(data, 0, sizeof(data));
  strcpy(data, "Hello, World!2\n");
  f_write(&file, data, strlen(data), &write_count);
  f_close(&file);

  // // open txt file and read data
  // err = f_open(&file, "test.txt", FA_READ);
  // if (err != FR_OK) {
  //   sprintf(uart_buffer, "f_open error: %d\n", err);
  //   HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, sizeof(uart_buffer) - 1, HAL_MAX_DELAY);
  //   while (1);
  // }
  // sprintf(uart_buffer, "File opened successfully for reading.\n");
  // HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, sizeof(uart_buffer) - 1, HAL_MAX_DELAY);
  // memset(data, 0, sizeof(data));
  // f_gets(data, sizeof(data), &file);
  // sprintf(uart_buffer, "f_gets read: %s", data);
  // HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, sizeof(uart_buffer) - 1, HAL_MAX_DELAY);
  // memset(data, 0, sizeof(data));
  // f_read(&file, data, f_size(&file), &read_count);
  // sprintf(uart_buffer, "f_read read (%d bytes): %s", read_count, data);
  // HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, sizeof(uart_buffer) - 1, HAL_MAX_DELAY);
  // f_close(&file);

  // // open an existing file and modify it and read its content back
  // err = f_open(&file, "test.txt", FA_WRITE | FA_OPEN_EXISTING);
  // if (err != FR_OK) {
  //   sprintf(uart_buffer, "f_open error: %d\n", err);
  //   HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, sizeof(uart_buffer) - 1, HAL_MAX_DELAY);
  //   while (1);
  // }
  // sprintf(uart_buffer, "File opened successfully for modification.\n");
  // HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, sizeof(uart_buffer) - 1, HAL_MAX_DELAY);
  // err = f_lseek(&file, f_size(&file)); // move to the end of the file
  // if (err != FR_OK) {
  //   sprintf(uart_buffer, "f_lseek error: %d\n", err);
  //   HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, sizeof(uart_buffer) - 1, HAL_MAX_DELAY);
  //   while (1);
  // }
  // f_puts("Modified Line 1\n", &file);
  // f_close(&file);
  // memset(data, 0, sizeof(data));
  // err = f_open(&file, "test.txt", FA_READ);
  // if (err != FR_OK) {
  //   sprintf(uart_buffer, "f_open error: %d\n", err);
  //   HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, sizeof(uart_buffer) - 1, HAL_MAX_DELAY);
  //   while (1);
  // }
  // sprintf(uart_buffer, "File opened successfully for reading modified content.\n");
  // HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, sizeof(uart_buffer) - 1, HAL_MAX_DELAY);
  // f_read(&file, data, f_size(&file), &read_count);
  // sprintf(uart_buffer, "Modified file content (%d bytes): %s", read_count, data);
  // HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, sizeof(uart_buffer) - 1, HAL_MAX_DELAY);
  // f_close(&file);

  // // delete the file
  // err = f_unlink("test.txt");
  // if (err != FR_OK) {
  //   sprintf(uart_buffer, "f_unlink error: %d\n", err);
  //   HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, sizeof(uart_buffer) - 1, HAL_MAX_DELAY);
  //   while (1);
  // }
  // sprintf(uart_buffer, "File deleted successfully.\n");
  // HAL_UART_Transmit(&huart1, (uint8_t *)uart_buffer, sizeof(uart_buffer) - 1, HAL_MAX_DELAY);
}
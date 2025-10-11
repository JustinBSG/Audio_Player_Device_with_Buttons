#include "sd_card_fs.h"

FATFS fatfs;          // Fatfs handle
FIL file;              // File handle
FRESULT err = FR_OK;  // Result
UINT write_count, read_count;
DWORD free_clusters;
uint32_t total_size, free_space;

void sd_card_init(void) {
  MX_FATFS_Init();
  // TODO: need mounting before any file operations
}

FRESULT sd_card_mount(void) {}

FRESULT sd_card_unmount(void) {}

FRESULT sd_card_format(void) {}

FRESULT sd_card_get_free_space(int* free_kb) {}

FRESULT sd_card_create_file(const char* filename) {}

FRESULT sd_card_read_file(const char* filename, uint8_t* buffer, UINT bytes_to_read, UINT* bytes_read) {}

FRESULT sd_card_write_file(const char* filename, const uint8_t* data, UINT data_size, UINT* bytes_written) {}

void sd_card_test(void) {
  char data[256];

  // mount the sd card
  err = f_mount(&fatfs, "", 1);
  char uart_buffer[256];
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_mount error: %d\n", err);
    // uart transmit uart_buffer
    while (1);
  }
  sprintf(uart_buffer, "SD card mounted successfully.\n");
  // uart transmit uart_buffer

  // get and print the sd card size and free space
  err = f_getfree("", &free_clusters, &fatfs);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_getfree error: %d\n", err);
    // uart transmit uart_buffer
    while (1);
  }
  total_size = (fatfs.n_fatent - 2) * fatfs.csize / 2; // in KB
  free_space = free_clusters * fatfs.csize / 2;        // in KB
  sprintf(uart_buffer, "SD card total size: %lu KB, free space: %lu KB\n", total_size, free_space);

  // open a txt file and write data
  // observe append or overwrite behavior, should be overwrite
  err = f_open(&file, "test.txt", FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_open error: %d\n", err);
    // uart transmit uart_buffer
    while (1);
  }
  sprintf(uart_buffer, "File created/opened successfully.\n");
  // uart transmit uart_buffer
  f_puts("Hello, World!1\n", &file);
  memset(data, 0, sizeof(data));
  strcpy(data, "Hello, World!2\n");
  f_write(&file, data, strlen(data), &write_count);
  f_close(&file);

  // open txt file and read data
  err = f_open(&file, "test.txt", FA_READ);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_open error: %d\n", err);
    // uart transmit uart_buffer
    while (1);
  }
  sprintf(uart_buffer, "File opened successfully for reading.\n");
  // uart transmit uart_buffer
  memset(data, 0, sizeof(data));
  f_gets(data, sizeof(data), &file);
  sprintf(uart_buffer, "f_gets read: %s", data);
  // uart transmit uart_buffer
  memset(data, 0, sizeof(data));
  f_read(&file, data, f_size(&file), &read_count);
  sprintf(uart_buffer, "f_read read (%d bytes): %s", read_count, data);
  // uart transmit uart_buffer
  f_close(&file);

  // open an existing file and modify it and read its content back
  err = f_open(&file, "test.txt", FA_WRITE | FA_OPEN_EXISTING);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_open error: %d\n", err);
    // uart transmit uart_buffer
    while (1);
  }
  sprintf(uart_buffer, "File opened successfully for modification.\n");
  // uart transmit uart_buffer
  err = f_lseek(&file, f_size(&file)); // move to the end of the file
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_lseek error: %d\n", err);
    // uart transmit uart_buffer
    while (1);
  }
  f_puts("Modified Line 1\n", &file);
  f_close(&file);
  memset(data, 0, sizeof(data));
  err = f_open(&file, "test.txt", FA_READ);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_open error: %d\n", err);
    // uart transmit uart_buffer
    while (1);
  }
  sprintf(uart_buffer, "File opened successfully for reading modified content.\n");
  // uart transmit uart_buffer
  f_read(&file, data, f_size(&file), &read_count);
  sprintf(uart_buffer, "Modified file content (%d bytes): %s", read_count, data);
  // uart transmit uart_buffer
  f_close(&file);

  // delete the file
  err = f_unlink("test.txt");
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_unlink error: %d\n", err);
    // uart transmit uart_buffer
    while (1);
  }
  sprintf(uart_buffer, "File deleted successfully.\n");
  // uart transmit uart_buffer

  // unmount the sd card
  err = f_mount(NULL, "", 0);
  if (err != FR_OK) {
    sprintf(uart_buffer, "f_mount unmount error: %d\n", err);
    // uart transmit uart_buffer
    while (1);
  }
  sprintf(uart_buffer, "SD card unmounted successfully.\n");
  // uart transmit uart_buffer
}
#ifndef GEMDOS_FILE_H
#define GEMDOS_FILE_H

#include <stdint.h>

/* Initializes the GEMDOS file-handle table, binding handles 0/1/2 to the
 * host's standard input/output/error streams. Call once at startup. */
void gemdos_file_init(void);

int32_t gemdos_file_open(const char *path, int16_t mode);

int32_t gemdos_file_create(const char *path, int16_t attrib);

int32_t gemdos_file_close(int16_t handle);

int32_t gemdos_file_read(int16_t handle, unsigned char *buf, uint32_t count);

int32_t gemdos_file_write(int16_t handle, const unsigned char *buf, uint32_t count);

int32_t gemdos_file_seek(int16_t handle, int32_t offset, int16_t mode);

// GEMDOS trap handlers (Fcreate/Fopen/Fclose/Fread/Fwrite/Fseek, TRAP #1)
unsigned int gemdos_fcreate(unsigned int args_addr);
unsigned int gemdos_fopen(unsigned int args_addr);
unsigned int gemdos_fclose(unsigned int args_addr);
unsigned int gemdos_fread(unsigned int args_addr);
unsigned int gemdos_fwrite(unsigned int args_addr);
unsigned int gemdos_fseek(unsigned int args_addr);

#endif
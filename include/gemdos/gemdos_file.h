#ifndef GEMDOS_FILE_H
#define GEMDOS_FILE_H

#include <stdint.h>

// Initializes the GEMDOS file-handle table, binding handles 0/1/2 to the
// host's standard input/output/error streams. Call once at startup.
void gemdos_file_init(void);

// Fopen(fname, mode): mode 0=read-only, 1=write-only, 2=read/write.
// Returns a GEMDOS handle (>=0) or a negative GEMDOS error code.
int32_t gemdos_file_open(const char *path, int16_t mode);

// Fcreate(fname, attrib): creates or truncates a file for read/write.
// File attribute bits aren't modeled on the host and are ignored.
int32_t gemdos_file_create(const char *path, int16_t attrib);

// Fclose(handle). Closing 0/1/2 releases the slot without closing the
// underlying std stream.
int32_t gemdos_file_close(int16_t handle);

// Fread(handle, buf, count) into a host buffer already sized for count bytes.
int32_t gemdos_file_read(int16_t handle, unsigned char *buf, uint32_t count);

// Fwrite(handle, buf, count). Handles 1/2 go through the console API so
// output stays consistent regardless of which GEMDOS call produced it.
int32_t gemdos_file_write(int16_t handle, const unsigned char *buf, uint32_t count);

// Fseek(offset, handle, mode): mode 0=SEEK_SET, 1=SEEK_CUR, 2=SEEK_END.
int32_t gemdos_file_seek(int16_t handle, int32_t offset, int16_t mode);

// GEMDOS trap handlers (Fcreate/Fopen/Fclose/Fread/Fwrite/Fseek, TRAP #1)
unsigned int gemdos_fcreate(unsigned int args_addr);
unsigned int gemdos_fopen(unsigned int args_addr);
unsigned int gemdos_fclose(unsigned int args_addr);
unsigned int gemdos_fread(unsigned int args_addr);
unsigned int gemdos_fwrite(unsigned int args_addr);
unsigned int gemdos_fseek(unsigned int args_addr);

#endif
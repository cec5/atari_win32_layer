#ifndef GEMDOS_FILE_H
#define GEMDOS_FILE_H

#include <stdint.h>

/* Initializes the GEMDOS file-handle table, binding handles 0/1/2 to the
 * host's standard input/output/error streams. Call once at startup. */
void gemdos_file_init(void);

/* Fopen(fname, mode): mode 0=read-only, 1=write-only, 2=read/write.
 * Returns a GEMDOS handle (>=0) or a negative GEMDOS error code. */
int32_t gemdos_file_open(const char *path, int16_t mode);

/* Fcreate(fname, attrib): creates (or truncates) a file for read/write.
 * File attribute bits aren't modeled on the host filesystem and are
 * ignored. Returns a GEMDOS handle (>=0) or a negative GEMDOS error code. */
int32_t gemdos_file_create(const char *path, int16_t attrib);

/* Fclose(handle). Returns 0 on success, a negative GEMDOS error code on
 * failure. Closing 0/1/2 releases the handle slot without closing the
 * underlying std stream. */
int32_t gemdos_file_close(int16_t handle);

/* Fread(handle, buf, count) into a host buffer already sized for count
 * bytes. Returns bytes read (>=0) or a negative GEMDOS error code. */
int32_t gemdos_file_read(int16_t handle, unsigned char *buf, uint32_t count);

/* Fwrite(handle, buf, count) from a host buffer holding count bytes.
 * Handles 1/2 go through the console API (matching Cconws) so console
 * output stays consistent regardless of which GEMDOS call produced it;
 * any other handle writes to its underlying file. Returns bytes written
 * (>=0) or a negative GEMDOS error code. */
int32_t gemdos_file_write(int16_t handle, const unsigned char *buf, uint32_t count);

/* Fseek(offset, handle, mode): mode 0=SEEK_SET, 1=SEEK_CUR, 2=SEEK_END.
 * Returns the new position (>=0) or a negative GEMDOS error code. */
int32_t gemdos_file_seek(int16_t handle, int32_t offset, int16_t mode);

/* GEMDOS trap handlers (Fcreate/Fopen/Fclose/Fread/Fwrite/Fseek,
 * TRAP #1): read args from the guest stack and call the API above. */
unsigned int gemdos_fcreate(unsigned int args_addr);
unsigned int gemdos_fopen(unsigned int args_addr);
unsigned int gemdos_fclose(unsigned int args_addr);
unsigned int gemdos_fread(unsigned int args_addr);
unsigned int gemdos_fwrite(unsigned int args_addr);
unsigned int gemdos_fseek(unsigned int args_addr);

#endif

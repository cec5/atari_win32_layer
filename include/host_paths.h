#ifndef HOST_PATHS_H
#define HOST_PATHS_H

#include <stddef.h>

void resolve_project_path(const char *relative_path, char *out, size_t out_size);

#endif
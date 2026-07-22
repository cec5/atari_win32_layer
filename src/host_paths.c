#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "host_paths.h"

void resolve_project_path(const char *relative_path, char *out, size_t out_size) {
    char exe_path[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, exe_path, MAX_PATH);
    if (len == 0 || len == MAX_PATH) {
        snprintf(out, out_size, "%s", relative_path);
        return;
    }

    char *last_sep = strrchr(exe_path, '\\');
    if (last_sep) {
        *last_sep = '\0';
    }

    snprintf(out, out_size, "%s\\..\\..\\%s", exe_path, relative_path);
}

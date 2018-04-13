/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once

namespace vlg {

#if defined WIN32 && defined _MSC_VER
void *dynamic_lib_open(const wchar_t *path);
#else
void *dynamic_lib_open(const char *path);
#endif

int dynamic_lib_close(void *handle);
void *dynamic_lib_load_symbol(void *handle, const char *sym);
const char *dynamic_lib_error();

}


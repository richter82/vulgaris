/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@live.com
 *
 */

#include "dl.h"

namespace vlg {

#if defined WIN32 && defined _MSC_VER
#include <windows.h>

void *dynamic_lib_open(const wchar_t *path)
{
    return LoadLibrary(path);
}

int dynamic_lib_close(void *handle)
{
    return FreeLibrary((HMODULE)handle);
}

void *dynamic_lib_load_symbol(void *handle, const char *sym)
{
    return GetProcAddress((HMODULE)handle, sym);
}

const char *dynamic_lib_error()
{
    return "";
}

#else
#include <dlfcn.h>
#ifndef RTLD_NOW
#define RTLD_NOW        1
#endif

void *dynamic_lib_open(const char *path)
{
    return dlopen(path, RTLD_NOW);
}

int dynamic_lib_close(void *handle)
{
    return dlclose(handle);
}

void *dynamic_lib_load_symbol(void *handle, const char *sym)
{
    return dlsym(handle, sym);
}

const char *dynamic_lib_error()
{
    return dlerror();
}

#endif

}

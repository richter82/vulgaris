/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#ifndef VLG_DRV_SQLITE_H_
#define VLG_DRV_SQLITE_H_
#include "vlg.h"

#if defined WIN32 && defined _MSC_VER
#ifdef VLG_PERS_DRIV_EXPORTS
#define EXP_SYM __declspec(dllexport)
#else
#ifdef VLG_PERS_DRIV_IMPORTS
#define EXP_SYM __declspec(dllimport)
#else
#define EXP_SYM
#endif
#endif
#else
#ifdef __linux
#define EXP_SYM
#endif
#if defined (__MACH__) || defined (__APPLE__)
#define EXP_SYM __attribute__((visibility("default")))
#endif
#endif

#define VLG_PERS_DRIV_SQLITE_ID     101
#define VLG_PERS_DRIV_SQLITE_NAME   "sqlite"

#if defined(__cplusplus)
namespace vlg {
#endif

#if defined(__cplusplus)
extern "C" {
#endif
EXP_SYM const char *get_pers_driv_ver_sqlite();
#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
extern "C" {
#endif
EXP_SYM RetCode set_db_data_dir_sqlite(const char *dir);
#if defined(__cplusplus)
}
#endif

/*******************************
VLG_PERS_DRIV_SQLITE ENTRY POINT
*******************************/
#if defined(__cplusplus)
extern "C" {
#endif
EXP_SYM persistence_driver *get_pers_driv_sqlite(logger *log);
#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
}
#endif
#endif


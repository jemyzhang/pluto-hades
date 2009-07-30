#ifndef PLATFORM_GLOBAL_H
#define PLATFORM_GLOBAL_H

#include <Qt/qglobal.h>

#ifdef PLATFORM_LIB
# define PLATFORM_EXPORT Q_DECL_EXPORT
#else
# define PLATFORM_EXPORT Q_DECL_IMPORT
#endif

#endif // PLATFORM_GLOBAL_H

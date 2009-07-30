#ifndef FOUNDATION_GLOBAL_H
#define FOUNDATION_GLOBAL_H

#include <Qt/qglobal.h>

#ifdef FOUNDATION_LIB
# define FOUNDATION_EXPORT Q_DECL_EXPORT
#else
# define FOUNDATION_EXPORT Q_DECL_IMPORT
#endif

#endif // FOUNDATION_GLOBAL_H

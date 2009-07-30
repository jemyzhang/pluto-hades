#ifndef PDF_GLOBAL_H
#define PDF_GLOBAL_H

#include <Qt/qglobal.h>

#include "../../../include/foundation_include.h"

#ifdef PDF_LIB
# define PDF_EXPORT Q_DECL_EXPORT
#else
# define PDF_EXPORT Q_DECL_IMPORT
#endif

#endif // PDF_GLOBAL_H

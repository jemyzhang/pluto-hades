#ifndef COMIC_GLOBAL_H
#define COMIC_GLOBAL_H

#include <Qt/qglobal.h>

#ifdef COMIC_LIB
# define COMIC_EXPORT Q_DECL_EXPORT
#else
# define COMIC_EXPORT Q_DECL_IMPORT
#endif

#endif // COMIC_GLOBAL_H

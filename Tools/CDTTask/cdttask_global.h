#ifndef CDTTASK_GLOBAL_H
#define CDTTASK_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CDTTASK_LIBRARY)
#  define CDTTASKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CDTTASKSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CDTTASK_GLOBAL_H

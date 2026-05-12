#pragma once

#include <QtCore/QtGlobal>

#if defined(LIBY_GLOBAL)
#  define LIBY_GLOBAL Q_DECL_EXPORT
#else
#  define LIBY_GLOBAL Q_DECL_IMPORT
#endif


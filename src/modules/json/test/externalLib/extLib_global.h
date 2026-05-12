#pragma once

#include <QtCore/QtGlobal>

#if defined(EXTERNAL_LIBRARY)
#  define EXTERNAL_EXPORT Q_DECL_EXPORT
#else
#  define EXTERNAL_EXPORT Q_DECL_IMPORT
#endif

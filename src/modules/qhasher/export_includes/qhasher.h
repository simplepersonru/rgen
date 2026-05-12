#pragma once

#include <QHash>

/*!
 * \brief Redkit-gen модуль Hasher
 * \warning Не для вызова, а для поддержки переходов по символу в Doxygen
 */
void QHasher();

#define RGEN_QHasher(Prefix, ClassName) \
    Prefix uint qHash(const ClassName& name, uint iSeed);

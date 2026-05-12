#pragma once

#include <QVariantMap>
// Хедер <rgen/private/qvariantmap_pre.h> отсуствует намеренно.

/*!
 * \brief Redkit-gen модуль QVariantMapSerializer
 * \warning Не для вызова, а для поддержки переходов по символу в Doxygen
 */
void QVariantMapSerializer();

namespace rgen
{

template <typename S>
class QVariantMapSerializer
{
public:
    static S fromVariant(const QVariant& map)
    {
        QVariant variant = QVariant::fromValue(map);
        S val;
        fromVariantToType(variant, val);
        return val;
    }
    static QVariantMap toVariant(const S& val)
    {
        QVariant variant;
        fromTypeToVariant(variant, val);
        return variant.toMap();
    }
};

} // namespace rgen

/// For DERIVE support
#define RGEN_QVariantMap(prefix, name)

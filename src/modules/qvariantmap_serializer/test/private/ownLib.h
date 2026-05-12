// Объявляем собственные операторы стрелок для библиотечной структуры,
// которую мы не можем пометить тегом генерации.
#pragma once

#include <rgen/private/qvariantmap_pre.h>

#include <externalLib/extLib.h>

template <>
inline void fromTypeToVariant(QVariant& variant, const ExtPoint& val)
{
    QVariantMap map;
    fromTypeToVariant(map["x"], val.x);
    fromTypeToVariant(map["y"], val.y);
    variant = map;
}

template <>
inline void fromVariantToType(const QVariant& variant, ExtPoint& val)
{
    QVariantMap map = variant.toMap();
    fromVariantToType(map["x"], val.x);
    fromVariantToType(map["y"], val.y);
}

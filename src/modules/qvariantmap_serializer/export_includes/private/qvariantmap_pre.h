#pragma once

#include <QVariant>

template <typename T>
void fromVariantToType(const QVariant& variant, T& value);

template <typename T>
void fromTypeToVariant(QVariant& variant, const T& value);

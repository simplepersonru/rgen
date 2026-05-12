#ifndef RGEN_COMPARISON_H
#define RGEN_COMPARISON_H

#include <QVariant>
#include <cmath>

namespace rgen
{
/*!
 * Функция проверки идентичности двух однотипных объектов.
 * \param lhs 1-ый объект
 * \param rhs 2-ой объект
 * true, если объекты идентичны, иначе - false
 */
template <typename T>
bool equals(const T& lhs, const T& rhs)
{
    return lhs == rhs;
}

// специализации

template <>
inline bool equals<double>(const double& lhs, const double& rhs)
{
    return ((std::isnan(lhs) && std::isnan(rhs)) || (fabs(lhs - rhs) < std::numeric_limits<double>::epsilon()));
}

template<>
inline bool equals<float>(const float& lhs, const float& rhs)
{
    return ((std::isnan(lhs) && std::isnan(rhs)) || (fabs(lhs - rhs) < std::numeric_limits<float>::epsilon()));
}

template <>
inline bool equals<QVariant>(const QVariant& lhs, const QVariant& rhs)
{
    if ((lhs.type() == QVariant::Double) && (rhs.type() == QVariant::Double))
        return equals(lhs.toDouble(), rhs.toDouble());
    return lhs == rhs;
}

} // namespace rgen

#endif // RGEN_COMPARISON_H

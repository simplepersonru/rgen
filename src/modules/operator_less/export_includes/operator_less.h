#pragma once

/*!
 * \brief Redkit-gen модуль OperatorLess
 * \warning Не для вызова, а для поддержки переходов по символу в Doxygen
 */
void OperatorLess();

#define RGEN_Less(Prefix, ClassName) \
    Prefix bool operator<(const ClassName& lhs, const ClassName& rhs);

#define DECLARE_LESS_OPERATOR(ClassName) RGEN_Less(friend, ClassName)

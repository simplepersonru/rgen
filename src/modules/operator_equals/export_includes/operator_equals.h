#pragma once

/*!
 * \brief Redkit-gen модуль OperatorEquals
 * \warning Не для вызова, а для поддержки переходов по символу в Doxygen
 */
void OperatorEquals();

#define RGEN_Equals(Prefix, ClassName)                                  \
    Prefix bool operator==(const ClassName& lhs, const ClassName& rhs); \
    Prefix bool operator!=(const ClassName& lhs, const ClassName& rhs);

#define DECLARE_EQUALS_OPERATOR(name) RGEN_Equals(friend, name)

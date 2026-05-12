#pragma once

/*!
 * \brief Redkit-gen модуль OperatorQStream
 * \warning Не для вызова, а для поддержки переходов по символу в Doxygen
 */
void OperatorQStream();

#define RGEN_QStream(Prefix, ClassName)                                      \
    Prefix QDataStream& operator<<(QDataStream& out, const ClassName& info); \
    Prefix QDataStream& operator>>(QDataStream& in, ClassName& info);

#define DECLARE_QSTREAM_OPERATOR(ClassName) RGEN_QStream(friend, ClassName)

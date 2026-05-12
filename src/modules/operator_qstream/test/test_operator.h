#pragma once

#include <QString>
#include <rgen/operator_qstream.h>

/// @OperatorQStream
struct Day
{
    DECLARE_QSTREAM_OPERATOR(Day);

    int value;
};

/// Значение по умолчанию для полей в TestIgnore
constexpr int defaultKeyValueForTestIgnore = 5;

/// @OperatorQStream
struct TestIgnore
{
    DECLARE_QSTREAM_OPERATOR(TestIgnore);

    int a = defaultKeyValueForTestIgnore; ///< @Ignore
    int b = defaultKeyValueForTestIgnore;
};


template<typename T>
struct TestTemplate
{
    T a;
};

/// @OperatorQStream
using TestUsing = TestTemplate<int>;
RGEN_QStream(, TestUsing)
// Необходимо для инициализации TestUsing. Иначе получаем ошибку
// error: Template with alias "TestTemplate" is not initialized
template struct TestTemplate<int>;

/// @OperatorQStream
struct TestClass
{
    DECLARE_QSTREAM_OPERATOR(TestClass);

    Day a;
    QString s;
};

namespace The::Longest::Namespace
{

/// @OperatorQStream
struct ClassInNamespace
{
    DECLARE_QSTREAM_OPERATOR(ClassInNamespace);

    int a;
};

template<typename T>
struct TestTemplateNamespace
{
    T a;
};

/// @OperatorQStream
using TestUsingNamespace = TestTemplateNamespace<int>;
RGEN_QStream(, TestUsingNamespace)
// Необходимо для инициализации TestUsingNamespace. Иначе получаем ошибку
// error: Template with alias "TestTemplateNamespace" is not initialized
template struct TestTemplateNamespace<int>;

} // namespace The::Longest::Namespace

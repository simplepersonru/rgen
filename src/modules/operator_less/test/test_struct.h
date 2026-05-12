#pragma once

#include <QString>
#include <rgen/operator_less.h>
#include <string>

/// @OperatorLess
struct Day
{
    DECLARE_LESS_OPERATOR(Day);

    int value = 0;
};

/// @OperatorLess
struct TestIgnore
{
    DECLARE_LESS_OPERATOR(TestIgnore);

    int a; ///< @Ignore
    int b;
};

template<typename T>
struct TestTemplate
{
    T a;
};

/// @OperatorLess
using TestUsing = TestTemplate<int>;
RGEN_Less(, TestUsing)
// Необходимо для инициализации TestUsing. Иначе получаем ошибку
// error: Template with alias "TestTemplate" is not initialized
template struct TestTemplate<int>;

/// @OperatorLess
struct ComplexStruct
{
    DECLARE_LESS_OPERATOR(ComplexStruct);

    Day day;
    std::string name;
};

namespace The::Longest::Namespace
{

/// @OperatorLess
struct ClassInNamespace
{
    DECLARE_LESS_OPERATOR(ClassInNamespace);

    int a;
};

template<typename T>
struct TestTemplateNamespace
{
    T a;
};

/// @OperatorLess
using TestUsingNamespace = TestTemplateNamespace<int>;
RGEN_Less(, TestUsingNamespace)
// Необходимо для инициализации TestUsingNamespace. Иначе получаем ошибку
// error: Template with alias "TestTemplateNamespace" is not initialized
template struct TestTemplateNamespace<int>;

} // namespace The::Longest::Namespace

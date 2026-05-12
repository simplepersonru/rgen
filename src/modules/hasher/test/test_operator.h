#pragma once

#include <QString>

#include <rgen/operator_equals.h>

/// @Hasher
/// @OperatorEquals
struct TestClass
{
    DECLARE_EQUALS_OPERATOR(TestClass)

    int a;
};

/// @Hasher
/// @OperatorEquals
struct ComplexStruct
{
    DECLARE_EQUALS_OPERATOR(ComplexStruct)

    TestClass day;
    std::string name;
};

template <typename T>
struct StructTemplate
{
    T a;
};

/// @OperatorEquals
/// @Hasher
using StructUsing = StructTemplate<int>;
RGEN_Equals(, StructUsing)
// Необходимо для инициализации StructUsing. Иначе получаем ошибку
// error: Template with alias "StructTemplate" is not initialized
template struct StructTemplate<int>;

namespace The::Longest::Namespace
{

/// @OperatorEquals
/// @Hasher
struct ClassInNamespace
{
    DECLARE_EQUALS_OPERATOR(ClassInNamespace)

    int a;
};

template <typename T>
struct StructTemplateNamespace
{
    T a;
};

/// @OperatorEquals
/// @Hasher
using StructUsingNamespace = StructTemplateNamespace<int>;
RGEN_Equals(, StructUsingNamespace)
// Необходимо для инициализации StructUsingNamespace. Иначе получаем ошибку
// error: Template with alias "StructTemplateNamespace" is not initialized
template struct StructTemplateNamespace<int>;

} // namespace The::Longest::Namespace

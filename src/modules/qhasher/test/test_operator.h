#pragma once

#include <QString>
#include <rgen/operator_equals.h>
#include <rgen/qhasher.h>

/// @QHasher
/// @OperatorEquals
struct TestClass
{
    RGEN_QHasher(friend, TestClass);
    DECLARE_EQUALS_OPERATOR(TestClass)

    int a;
};

/// @QHasher
/// @OperatorEquals
struct ComplexStruct
{
    RGEN_QHasher(friend, ComplexStruct);
    DECLARE_EQUALS_OPERATOR(ComplexStruct)

    TestClass day;
    QString name;
};

template <typename T>
struct StructTemplate
{
    T a;
};

/// @OperatorEquals
/// @QHasher
using StructUsing = StructTemplate<int>;
RGEN_Equals(, StructUsing)
RGEN_QHasher(, StructUsing)
// Необходимо для инициализации StructUsing. Иначе получаем ошибку
// error: Template with alias "StructTemplate" is not initialized
template struct StructTemplate<int>;

namespace The::Longest::Namespace
{

/// @OperatorEquals
/// @QHasher
struct ClassInNamespace
{
    RGEN_QHasher(friend, ClassInNamespace)
    DECLARE_EQUALS_OPERATOR(ClassInNamespace)

    int a;
};

template <typename T>
struct StructTemplateNamespace
{
    T a;
};

/// @OperatorEquals
/// @QHasher
using StructUsingNamespace = StructTemplateNamespace<int>;
RGEN_Equals(, StructUsingNamespace)
RGEN_QHasher(, StructUsingNamespace)
// Необходимо для инициализации StructUsingNamespace. Иначе получаем ошибку
// error: Template with alias "StructTemplateNamespace" is not initialized
template struct StructTemplateNamespace<int>;

} // namespace The::Longest::Namespace

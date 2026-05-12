#pragma once

#include <QString>
#include <rgen/derive.h>

/// @Derive
struct TestClassEquals
{
    RGEN_DERIVE(friend, TestClassEquals, RGEN_Equals)

    int a;
};

/// @Derive
struct TestClass
{
    RGEN_DERIVE(friend, TestClass, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap)

    int a;
};

template <typename T>
struct TestTemplate
{
    T a;
};

/// @Derive
using TestUsing = TestTemplate<int>;
RGEN_DERIVE(, TestUsing, RGEN_Equals);
// Необходимо для инициализации TestUsing. Иначе получаем ошибку
// error: Template with alias "TestTemplate" is not initialized
template struct TestTemplate<int>;

namespace The::Longest::Namespace
{
/// @Derive
struct TestNamespace
{
    RGEN_DERIVE(friend, The::Longest::Namespace::TestNamespace, RGEN_Equals);

    int a;
};

template <typename T>
struct TestTemplateNamespace
{
    T a;
};

/// @Derive
using TestUsingNamespace = TestTemplateNamespace<int>;
RGEN_DERIVE(, The::Longest::Namespace::TestUsingNamespace, RGEN_Equals);
template struct TestTemplateNamespace<int>;

} // namespace The::Longest::Namespace

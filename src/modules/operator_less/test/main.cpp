#include "test_struct.h"
#include <gtest/gtest.h>

TEST(OperatorLess, simpleUsage)
{
    Day d1 { .value = 1 };
    Day d2 { .value = 2 };

    EXPECT_TRUE(operator<(d1, d2));
}

TEST(OperatorLess, Using)
{
    TestUsing elem { 1 };

    EXPECT_FALSE(operator<(elem, elem));
}

TEST(OperatorLess, IgnoreField)
{
    /// Игнорируется первое поле в структуре.
    /// Ожидалось бы, что т.к. d1.a < d2.a (3 < 4), то должно
    /// вернуться true, но так как первое пело игнорируется,
    /// а для второго поля d1.b == d2.b, то должно вернуться
    /// false
    TestIgnore d1 { 3, 5 };
    TestIgnore d2 { 4, 5 };

    EXPECT_FALSE(operator<(d1, d2));
}

TEST(OperatorLess, UsingNamespace)
{
    using namespace The::Longest::Namespace;
    TestUsingNamespace elem { 1 };

    EXPECT_FALSE(operator<(elem, elem));
}

TEST(OperatorLess, ComplexStruct)
{
    ComplexStruct c1 { .day = { .value = 1 }, .name = "a" };
    ComplexStruct c2 { .day = { .value = 2 }, .name = "a" };
    ComplexStruct c3 { .day = { .value = 2 }, .name = "b" };

    EXPECT_TRUE(operator<(c1, c2));
    EXPECT_TRUE(operator<(c1, c3));
    EXPECT_TRUE(operator<(c2, c3));
}

TEST(OperatorLess, ClassInNamespace)
{
    using namespace The::Longest::Namespace;
    ClassInNamespace elem { 1 };

    EXPECT_FALSE(operator<(elem, elem));
}

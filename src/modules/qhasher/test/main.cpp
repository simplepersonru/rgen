#include "test_operator.h"
#include <gtest/gtest.h>

#include <QHash>

TEST(QHasher, EqualElements)
{
    TestClass st { 1 };

    QHash<TestClass, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

TEST(QHasher, ComplexStruct)
{
    ComplexStruct st { { 1 }, "name" };

    QHash<ComplexStruct, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

TEST(QHasher, Using)
{
    StructUsing st { 1 };

    QHash<StructUsing, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

TEST(QHasher, InNamespace)
{
    using namespace The::Longest::Namespace;
    ClassInNamespace st { 1 };

    QHash<ClassInNamespace, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

TEST(QHasher, UsingInNamespace)
{
    using namespace The::Longest::Namespace;

    StructUsingNamespace st { 1 };

    QHash<StructUsingNamespace, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

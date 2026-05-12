#include "test_operator.h"

#include <unordered_map>

#include <gtest/gtest.h>

#include <Hasher_test_gen/test_operator.rgen.hxx>

TEST(Hasher, EqualElements)
{
    TestClass st { 1 };

    std::unordered_map<TestClass, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

TEST(Hasher, ComplexStruct)
{
    ComplexStruct st { { 1 }, "name" };

    std::unordered_map<ComplexStruct, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

TEST(Hasher, Using)
{
    StructUsing st { 1 };

    std::unordered_map<StructUsing, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

TEST(Hasher, InNamespace)
{
    using namespace The::Longest::Namespace;
    ClassInNamespace st { 1 };

    std::unordered_map<ClassInNamespace, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

TEST(Hasher, UsingInNamespace)
{
    using namespace The::Longest::Namespace;

    StructUsingNamespace st { 1 };

    std::unordered_map<StructUsingNamespace, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

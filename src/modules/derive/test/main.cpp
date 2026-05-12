#include "test_operator.h"
#include <gtest/gtest.h>

#include <QDataStream>

#include <Derive_test_gen/test_operator.rgen.hxx>

TEST(Derive, EqualElementsSimple)
{
    TestClassEquals elem { 1 };

    EXPECT_TRUE(operator==(elem, elem));
}

TEST(Derive, MultipleDerivesEquals)
{
    TestClass elem { 1 };

    EXPECT_TRUE(operator==(elem, elem));
}

TEST(Derive, MultipleDerivesLess)
{
    TestClass elem { 1 };

    EXPECT_FALSE(operator<(elem, elem));
}

TEST(Derive, MultipleDerivesJson)
{
    TestClass expected = { 1 };
    std::string serialized = rgen::Json<TestClass>::toJson(expected);
    TestClass actual = rgen::Json<TestClass>::fromJson(serialized);

    EXPECT_EQ(expected, actual);
}

TEST(Derive, MultipleDerivesQStream)
{
    TestClass original { 1 };
    TestClass restored;

    QByteArray data;
    QDataStream outStream(&data, QIODevice::WriteOnly);
    outStream << original;
    EXPECT_TRUE(!data.isEmpty());
}

TEST(Derive, MultipleDerivesHasher)
{
    TestClass st { 1 };

    std::unordered_map<TestClass, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

TEST(Derive, MultipleDerivesQHasher)
{
    TestClass st { 1 };

    QHash<TestClass, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

TEST(Derive, MultipleDerivesQVariantMap)
{
    TestClass expected = { 1 };
    QVariantMap serialized = rgen::QVariantMapSerializer<TestClass>::toVariant(expected);
    TestClass actual = rgen::QVariantMapSerializer<TestClass>::fromVariant(serialized);

    EXPECT_EQ(expected, actual);
}

TEST(Derive, EqualElementsNamespace)
{
    using namespace The::Longest::Namespace;
    TestNamespace elem { 1 };

    EXPECT_TRUE(operator==(elem, elem));
}

TEST(Derive, UsingNamespace)
{
    using namespace The::Longest::Namespace;
    TestUsingNamespace elem { 1 };

    EXPECT_TRUE(operator==(elem, elem));
}

#include <QDataStream>
#include <gtest/gtest.h>

#include "test_operator.h"

TEST(OperatorQStream, SimpleUsage)
{
    Day original { 1 };
    Day restored;

    QByteArray data;
    QDataStream outStream(&data, QIODevice::WriteOnly);
    outStream << original;
    EXPECT_GT(data.length(), 0);

    QDataStream inStream(&data, QIODevice::ReadOnly);
    inStream >> restored;

    EXPECT_EQ(original.value, restored.value);
}

TEST(OperatorQStream, IgnoreField)
{
    /// Ожидаем, что после сериализации мы получим
    /// значение по умолчанию
    TestIgnore original { defaultKeyValueForTestIgnore + 1, 0 };
    TestIgnore restored;

    QByteArray data;
    QDataStream outStream(&data, QIODevice::WriteOnly);
    outStream << original;
    EXPECT_GT(data.length(), 0);

    QDataStream inStream(&data, QIODevice::ReadOnly);
    inStream >> restored;

    EXPECT_EQ(restored.a, defaultKeyValueForTestIgnore);
}


TEST(OperatorQStream, ComplexStruct)
{
    TestClass original { 1, "a" };
    TestClass restored;

    QByteArray data;
    QDataStream outStream(&data, QIODevice::WriteOnly);
    outStream << original;
    EXPECT_TRUE(!data.isEmpty());

    QDataStream inStream(&data, QIODevice::ReadOnly);
    inStream >> restored;

    EXPECT_EQ(original.a.value, restored.a.value);
    EXPECT_STREQ(original.s.toStdString().c_str(), restored.s.toStdString().c_str());
}

TEST(OperatorQStream, ClassInNamespace)
{
    using namespace The::Longest::Namespace;
    ClassInNamespace original { 1 };

    QByteArray data;
    QDataStream outStream(&data, QIODevice::WriteOnly);
    outStream << original;
    EXPECT_TRUE(!data.isEmpty());
}

TEST(OperatorQStream, Using)
{
    TestUsing original { 1 };

    QByteArray data;
    QDataStream outStream(&data, QIODevice::WriteOnly);
    outStream << original;
    EXPECT_TRUE(!data.isEmpty());
}

TEST(OperatorQStream, UsingNamespace)
{
    using namespace The::Longest::Namespace;
    TestUsingNamespace original { 1 };

    QByteArray data;
    QDataStream outStream(&data, QIODevice::WriteOnly);
    outStream << original;
    EXPECT_TRUE(!data.isEmpty());
}

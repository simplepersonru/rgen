#include <gtest/gtest.h>

#include "someLib/liby.h"
#include <tests/general_functionality/library_depends/someLib/someLib_gen/liby.rgen.hxx>

#include <rgen/enum.h>
#include <rgen/json_qt.h>
#include <rgen/meta_types.h>

TEST(GenerationForLib, Enum)
{
    EXPECT_EQ(rgen::Enum<EnumGeneration>::toString(EnumGeneration::Here), "Here");
    EXPECT_EQ(rgen::Enum<EnumGeneration>::fromString("Here"), EnumGeneration::Here);
}

TEST(GenerationForLib, Json)
{
    StructGeneration st = { 1 };
    EXPECT_FALSE(rgen::Json<StructGeneration>::toJson(st).empty());
}

TEST(GenerationForLib, OperatorEquals)
{
    StructGeneration st1 = { 1 };
    StructGeneration st2 = { 2 };
    EXPECT_TRUE(st1 == st1);
    EXPECT_TRUE(st1 != st2);
}

TEST(GenerationForLib, OperatorLess)
{
    StructGeneration st1 = { 1 };
    StructGeneration st2 = { 2 };
    EXPECT_TRUE(st1 < st2);
}

TEST(GenerationForLib, OperatorQStream)
{
    StructGeneration original { 1 };
    StructGeneration restored;

    QByteArray data;
    QDataStream outStream(&data, QIODevice::WriteOnly);
    outStream << original;
    EXPECT_GT(data.length(), 0);

    QDataStream inStream(&data, QIODevice::ReadOnly);
    inStream >> restored;

    EXPECT_EQ(original.a, restored.a);
}

TEST(GenerationForLib, QMetaTypesRegistrator)
{
    rgen::QMetaTypesRegistrator<StructGeneration>();
}

TEST(GenerationForLib, Hasher)
{
    StructGeneration st { 1 };

    std::unordered_map<StructGeneration, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

TEST(GenerationForLib, QHasher)
{
    StructGeneration st { 1 };

    QHash<StructGeneration, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

/// Using

TEST(GenerationForLib, UsingJson)
{
    TestUsing st = { 1 };
    EXPECT_FALSE(rgen::Json<TestUsing>::toJson(st).empty());
}

TEST(GenerationForLib, UsingOperatorEquals)
{
    TestUsing st1 = { 1 };
    TestUsing st2 = { 2 };
    EXPECT_TRUE(st1 == st1);
    EXPECT_TRUE(st1 != st2);
}

TEST(GenerationForLib, UsingOperatorLess)
{
    TestUsing st1 = { 1 };
    TestUsing st2 = { 2 };
    EXPECT_TRUE(st1 < st2);
}

TEST(GenerationForLib, UsingOperatorQStream)
{
    TestUsing original { 1 };
    TestUsing restored;

    QByteArray data;
    QDataStream outStream(&data, QIODevice::WriteOnly);
    outStream << original;
    EXPECT_GT(data.length(), 0);

    QDataStream inStream(&data, QIODevice::ReadOnly);
    inStream >> restored;

    EXPECT_EQ(original.a, restored.a);
}

TEST(GenerationForLib, UsingQMetaTypesRegistrator)
{
    rgen::QMetaTypesRegistrator<TestUsing>();
}

TEST(GenerationForLib, UsingHasher)
{
    TestUsing st { 1 };

    std::unordered_map<TestUsing, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

TEST(GenerationForLib, UsingQHasher)
{
    TestUsing st { 1 };

    QHash<TestUsing, int> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

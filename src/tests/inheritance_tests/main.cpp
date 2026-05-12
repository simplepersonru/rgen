#include "inheritance_tests.h"

#include <QString>
#include <gtest/gtest.h>

//  OperatorEquals Tests

TEST(Derive, InheritanceEquals)
{
    OverMultiDerived o1;
    o1.id = 1;
    o1.name = "text";

    OverMultiDerived o2 = o1;

    EXPECT_EQ(o1, o2);

    o1.id = 2;
    EXPECT_NE(o1, o2);

    o2.id = 2;
    o2.name = "TEXT";
    EXPECT_NE(o1, o2);
}

//  OperatorLess Tests

TEST(Derive, InheritanceLess)
{
    OverMultiDerived o1;
    o1.baseField = 0;
    o1.value = 0.0;
    o1.flag = false;
    o1.id = 1;
    o1.name = "A";
    o1.flag1 = false;

    OverMultiDerived o2 = o1;
    o2.id = 1;
    o2.name = "B";

    OverMultiDerived o3 = o1;
    o3.id = 10;
    o3.name = "A";

    EXPECT_TRUE(o1 < o2);
    EXPECT_TRUE(o1 < o3);
    EXPECT_TRUE(o2 < o3);
}

//  OperatorQStream Tests

TEST(Derive, InheritanceQStream)
{
    OverMultiDerived o1;
    o1.id = 1;
    o1.name = "test";
    o1.baseField = 100;
    o1.value = 3.14;
    o1.flag = true;
    o1.flag1 = false;
    OverMultiDerived o2;

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << o1;

    QDataStream in(&data, QIODevice::ReadOnly);
    in >> o2;

    EXPECT_EQ(o1, o2);
}

//  QVariantMapSerializer Tests

TEST(Derive, InheritanceQVMSerializeer)
{
    OverMultiDerived expected;
    expected.id = 1;
    expected.name = "test";
    expected.baseField = 100;
    expected.value = 3.14;
    expected.flag = true;
    expected.flag1 = false;

    QVariantMap serialized = rgen::QVariantMapSerializer<OverMultiDerived>::toVariant(expected);
    OverMultiDerived actual = rgen::QVariantMapSerializer<OverMultiDerived>::fromVariant(serialized);

    EXPECT_EQ(expected, actual);
}

//  Json Tests

TEST(Derive, InheritanceJson)
{
    OverMultiDerived expected;
    expected.baseField = 10;
    expected.value = 3.14;
    expected.flag = true;
    expected.id = 1;
    expected.name = "text";
    expected.flag1 = false;

    std::string serialized = rgen::Json<OverMultiDerived>::toJson(expected);
    OverMultiDerived actual = rgen::Json<OverMultiDerived>::fromJson(serialized);

    EXPECT_EQ(expected, actual);
}

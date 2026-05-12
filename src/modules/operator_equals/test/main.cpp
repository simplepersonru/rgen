#include "test_operator.h"
#include <gtest/gtest.h>

TEST(OperatorEquals, EqualElements)
{
    TestClass lhs { 1 };
    TestClass rhs { 1 };

    EXPECT_TRUE(operator==(lhs, rhs));
    EXPECT_FALSE(operator!=(lhs, rhs));
}

TEST(OperatorEquals, IgnoreField)
{
    /// Игнорируем первое поле TestIgnore. Ожидаем, что раз вторые элементы
    /// равны, то и сами TestIgnore будут равны
    TestIgnore lhs { 5, 1 };
    TestIgnore rhs { 4, 1 };

    EXPECT_TRUE(operator==(lhs, rhs));
    EXPECT_FALSE(operator!=(lhs, rhs));
}

TEST(OperatorEquals, NotEqualElements)
{
    TestClass lhs { 1 };
    TestClass rhs { 2 };

    EXPECT_FALSE(operator==(lhs, rhs));
    EXPECT_TRUE(operator!=(lhs, rhs));
}

TEST(OperatorEquals, Using)
{
    StructUsing elem{ 1 };

    EXPECT_TRUE(operator==(elem, elem));
}

TEST(OperatorEquals, UsingNamespace)
{
    using namespace The::Longest::Namespace;

    StructUsingNamespace elem { 1 };

    EXPECT_TRUE(operator==(elem, elem));
}

TEST(OperatorEquals, ComplexStruct)
{
    ComplexStruct elem { { 1 }, { "a" } };

    EXPECT_TRUE(operator==(elem, elem));
}

TEST(OperatorEquals, InUsing)
{
    using namespace The::Longest::Namespace;
    ClassInNamespace elem { 1 };

    EXPECT_TRUE(operator==(elem, elem));
}

TEST(OperatorEquals, LinearsStruct)
{
    LinearsStruct lhs
    {
        .vec = {1, 2},
        .deq = {3, 4},
        .list = {5, 6},
        .flist = {7, 8}
    };

    LinearsStruct rhs = lhs;
    LinearsStruct rhs1
    {
        .vec = {7, 8},
        .deq = {5, 6},
        .list = {3, 4},
        .flist = {1, 2}
    };

    EXPECT_TRUE(operator==(lhs, rhs));
    EXPECT_TRUE(operator!=(lhs, rhs1));
}

TEST(OperatorEquals, AssociatedStruct)
{
    AssociatedStruct lhs
    {
        .s = {1, 2, 3},
        .unord_s = {4, 5, 6}
    };

    AssociatedStruct rhs =  lhs;

    AssociatedStruct rhs1
    {
        .s = {4, 5, 6},
        .unord_s = {1, 2, 3}
    };

    EXPECT_TRUE(operator==(lhs, lhs));
    EXPECT_TRUE(operator!=(lhs, rhs1));
}

TEST(OperatorEquals, StdStruct)
{
    StdStruct lhs {
        .pair = {1, 2},
        .opt = 3
    };

    StdStruct rhs = lhs;
    StdStruct rhs1
    {
        .pair = {3, 4},
        .opt = 5
    };


    EXPECT_TRUE(operator==(lhs, rhs));
    EXPECT_TRUE(operator!=(lhs, rhs1));
}

TEST(OperatorEquals, QStructs)
{
    QStructs lhs
    {
        .vec = {1, 2, 3},
        .map = {{1, 2}, {3, 4}},
        .name = "Test",
        .uuid = QUuid::createUuid(),
        .pair = {1, 2},
        .strList = {"a", "b", "c"},
        .hash = {{1, 2}, {3, 4}},
        .byteArray = QByteArray("test")
    };

    QStructs rhs = lhs;

    QStructs rhs1
    {
        .vec = {3, 2, 1},
        .map = {{10, 1}, {20, 2}},
        .name = "Text",
        .uuid = QUuid::createUuid(),
        .pair = {2, 1},
        .strList = {"c", "b", "a"},
        .hash = {{2, 1}, {3, 4}},
        .byteArray = QByteArray("text")
    };

    EXPECT_TRUE(operator==(lhs, rhs));
    EXPECT_TRUE(operator!=(lhs, rhs1));
}

TEST(OperatorEquals, Pointers)
{
    auto sharedInt = std::make_shared<int>(1);
    auto qsharedInt = QSharedPointer<int>::create(2);

    Pointers lhs
    {
        .uPtr = nullptr,
        .sPtr = sharedInt,
        .qsPtr = qsharedInt
    };

    Pointers rhs
    {
        .uPtr = nullptr,
        .sPtr = sharedInt,
        .qsPtr = qsharedInt
    };

    Pointers rhs1
    {
        .uPtr = std::make_unique<int>(3),
        .sPtr = std::make_shared<int>(2),
        .qsPtr = QSharedPointer<int>::create(1)
    };


    EXPECT_TRUE(operator==(lhs, rhs));
    EXPECT_TRUE(operator!=(lhs, rhs1));
}

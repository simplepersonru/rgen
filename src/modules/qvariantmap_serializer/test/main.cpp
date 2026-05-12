#include "structs.h"
#include <gtest/gtest.h>

#include <QString>

TEST(QVariantMapSerializer, SimpleUsage)
{
    SimpleStruct expected = { 1 };
    QVariantMap serialized = rgen::QVariantMapSerializer<SimpleStruct>::toVariant(expected);
    SimpleStruct actual = rgen::QVariantMapSerializer<SimpleStruct>::fromVariant(serialized);

    EXPECT_EQ(expected, actual);
}

TEST(QVariantMapSerializer, Enum)
{
    StructWithEnum expected = { TestEnum::A };
    QVariantMap serialized = rgen::QVariantMapSerializer<StructWithEnum>::toVariant(expected);
    StructWithEnum actual = rgen::QVariantMapSerializer<StructWithEnum>::fromVariant(serialized);

    EXPECT_EQ(expected.value, actual.value);
}

TEST(QVariantMapSerializer, IgnoreField)
{
    /// Ожидаем, что после десериализации проигнорированное поле
    /// будет инициализировано значение по умолчанию
    TestIgnore expected = { defaultKeyValueForTestIgnore + 1, 0 };
    QVariantMap serialized = rgen::QVariantMapSerializer<TestIgnore>::toVariant(expected);
    TestIgnore actual = rgen::QVariantMapSerializer<TestIgnore>::fromVariant(serialized);

    EXPECT_EQ(actual.a, defaultKeyValueForTestIgnore);
}

/*!
 * \brief Наша структура, помеченная тегом генерации, включающая в себя внешнюю структуру
 */
TEST(QVariantMapSerializer, OwnStructWithExternalStruct)
{
    Point expected = { { 1, 2 } };
    QVariantMap serialized = rgen::QVariantMapSerializer<Point>::toVariant(expected);
    Point actual = rgen::QVariantMapSerializer<Point>::fromVariant(serialized);

    EXPECT_EQ(expected, actual);
}

/*!
 * \brief Using, помеченный тегом генерации
 */
TEST(QVariantMapSerializer, UsingForTemplate)
{
    MyDotPointInt expected = { 1, 2 };
    QVariantMap serialized = rgen::QVariantMapSerializer<MyDotPointInt>::toVariant(expected);
    MyDotPointInt actual = rgen::QVariantMapSerializer<MyDotPointInt>::fromVariant(serialized);

    EXPECT_EQ(expected, actual);
}

/*!
 * \brief Класс в namespace
 */
TEST(QVariantMapSerializer, ClassInNamespace)
{
    using namespace The;
    Book expected;
    QVariantMap serialized = rgen::QVariantMapSerializer<Book>::toVariant(expected);
    Book actual = rgen::QVariantMapSerializer<Book>::fromVariant(serialized);

    EXPECT_EQ(expected, actual);
}

/*!
 * \brief Тест для линейных контейнеров из std::
 */
TEST(QVariantMapSerializer, LinearStdContainer)
{
    LinearsStruct expected {
        .vec = { 1, 2, 3 },
        .deq = { 1, 2, 3 },
        .list = { 1, 2, 3 },
        .flist = { 1, 2, 3 }
    };
    QVariantMap serialized = rgen::QVariantMapSerializer<LinearsStruct>::toVariant(expected);
    auto actual = rgen::QVariantMapSerializer<LinearsStruct>::fromVariant(serialized);

    EXPECT_EQ(expected, actual);
}

TEST(QVariantMapSerializer, VariablesUsage)
{
    Variables expected = {
        .c = '\0',
        .d = std::numeric_limits<double>::max(),
        .f = std::numeric_limits<float>::max(),
        .b = false,
        .i8 = std::numeric_limits<int8_t>::max(),
        .ui8 = std::numeric_limits<uint8_t>::max(),
        .i16 = std::numeric_limits<int16_t>::max(),
        .ui16 = std::numeric_limits<uint16_t>::max(),
        .i32 = std::numeric_limits<int32_t>::max(),
        .ui32 = std::numeric_limits<uint32_t>::max(),
        .i64 = std::numeric_limits<int64_t>::max(),
        .ui64 = std::numeric_limits<uint64_t>::max()
    };
    QVariantMap serialized = rgen::QVariantMapSerializer<Variables>::toVariant(expected);
    auto actual = rgen::QVariantMapSerializer<Variables>::fromVariant(serialized);

    EXPECT_EQ(expected, actual);
}

/*!
 * \brief Тест для ассоциативных контейнеров из std::
 */
TEST(QVariantMapSerializer, AssociativeStdContainer)
{
    // Важно проверить с использованием std:: структуры и самописной структуры
    AssociatedStruct expected = {
        .m = { { "1", { 1 } } },
        .unord_m = { { "1", { 2 } } },
        .s = { 1 },
        .unord_s = { 1 }
    };
    QVariantMap serialized = rgen::QVariantMapSerializer<AssociatedStruct>::toVariant(expected);
    auto actual = rgen::QVariantMapSerializer<AssociatedStruct>::fromVariant(serialized);

    EXPECT_EQ(expected, actual);
}

TEST(QVariantMapSerializer, StdStructs)
{
    StdStruct expected = {
        .pair = std::make_pair(1, 2),
        .opt = 1
    };
    QVariantMap serialized = rgen::QVariantMapSerializer<StdStruct>::toVariant(expected);
    auto actual = rgen::QVariantMapSerializer<StdStruct>::fromVariant(serialized);

    EXPECT_EQ(expected, actual);
}

/*!
 * \brief Тест для различных структур Qt
 */
TEST(QVariantMapSerializer, QtStructs)
{
    QStructs expected = {
        .vec = { 1, 2, 3 },
        .map = { { 1, 10 }, { 2, 20 } },
        .name = "str",
        .uuid = QUuid::createUuid(),
        .pair = { 1, 2 },
        .strList = { "1", "2" },
        .hash = { { 1, 10 }, { 2, 20 } },
        .byteArray = { "1" },
    };

    QVariantMap serialized = rgen::QVariantMapSerializer<QStructs>::toVariant(expected);
    auto actual = rgen::QVariantMapSerializer<QStructs>::fromVariant(serialized);

    EXPECT_EQ(expected, actual);
}

TEST(QVariantMapSerializer, PointersFilled)
{
    Pointers expected = {
        .uPtr = std::make_unique<int>(1),
        .sPtr = std::make_shared<int>(1),
        .qsPtr = QSharedPointer<int>(new int { 1 }),
    };
    QVariantMap serialized = rgen::QVariantMapSerializer<Pointers>::toVariant(expected);
    auto actual = rgen::QVariantMapSerializer<Pointers>::fromVariant(serialized);

    EXPECT_EQ(*expected.uPtr, *actual.uPtr);
    EXPECT_EQ(*expected.sPtr, *actual.sPtr);
    EXPECT_EQ(*expected.qsPtr, *actual.qsPtr);
}

TEST(QVariantMapSerializer, PointersNullptr)
{
    Pointers expected = {
        .uPtr = nullptr,
        .sPtr = nullptr,
        .qsPtr = nullptr,
    };
    QVariantMap serialized = rgen::QVariantMapSerializer<Pointers>::toVariant(expected);
    auto actual = rgen::QVariantMapSerializer<Pointers>::fromVariant(serialized);

    EXPECT_EQ(expected, actual);
}

#include "structs.h"
#include <gtest/gtest.h>

#include <Json_test_gen/structs.rgen.hxx>
#include <QString>

TEST(Json, SimpleUsage)
{
    SimpleStruct expected = { 1 };
    std::string serialized = rgen::Json<SimpleStruct>::toJson(expected);
    SimpleStruct actual = rgen::Json<SimpleStruct>::fromJson(serialized);

    EXPECT_TRUE(rgen::Json<SimpleStruct>::assertJsonEquals(std::cerr, actual, expected));
}

TEST(Json, JsonKeysSimple)
{
    rgen::JsonKeys<SimpleStruct> fields;
    EXPECT_EQ(fields.value, "value");
}

TEST(Json, JsonEnum)
{
    JsonEnum expected = { TestEnum::A };
    std::string serialized = rgen::Json<JsonEnum>::toJson(expected);
    JsonEnum actual = rgen::Json<JsonEnum>::fromJson(serialized);

    EXPECT_TRUE(rgen::Json<JsonEnum>::assertJsonEquals(std::cerr, actual, expected));
}

TEST(Json, JsonKeysCustomKey)
{
    /// JsonKeys check
    rgen::JsonKeys<JsonKeysUsage> fields;
    EXPECT_EQ(fields.originalKey, "myCustomKey");

    /// JsonSerializer check
    JsonKeysUsage expected { 1, 2 };
    std::string serialized = rgen::Json<JsonKeysUsage>::toJson(expected);
    JsonKeysUsage actual = rgen::Json<JsonKeysUsage>::fromJson(serialized);

    EXPECT_EQ(expected.originalKey, actual.originalKey);
}

TEST(Json, JsonKeysIgnoredKey)
{
    /// JsonSerializer check
    JsonKeysUsage expected { 1, 2 };
    std::string serialized = rgen::Json<JsonKeysUsage>::toJson(expected);
    JsonKeysUsage actual = rgen::Json<JsonKeysUsage>::fromJson(serialized);

    /// Ожидаем, что ignoredKey будет проинициализирован значением по умолчанию
    EXPECT_EQ(actual.ignoredKey, defaultKeyValueForJsonKeysTest);
}

TEST(Json, IgnoreField)
{
    /// Игнорируется первое поле. Ожидаем, что оно после сериализации будет инициализировано
    /// по умолчанию.
    JsonIgnore expected { defaultKeyValueForJsonKeysTest + 1, 0 };
    std::string serialized = rgen::Json<JsonIgnore>::toJson(expected);
    JsonIgnore actual = rgen::Json<JsonIgnore>::fromJson(serialized);

    /// Ожидаем, что ignoredKey будет проинициализирован значением по умолчанию
    EXPECT_EQ(actual.ignoredKey, defaultKeyValueForJsonKeysTest);
}

/*!
 * \brief Нигде в коде мы не помечали ExtPoint тегом генерации, но прописали для него
 * методы из api
 */
TEST(Json, ExternalStruct)
{
    ExtPoint expected { 1, 2 };
    std::string serialized = rgen::Json<ExtPoint>::toJson(expected);
    ExtPoint actual = rgen::Json<ExtPoint>::fromJson(serialized);

    EXPECT_TRUE(rgen::Json<ExtPoint>::assertJsonEquals(std::cerr, actual, expected));
}

/*!
 * \brief Наша структура, помеченная тегом генерации, включающая в себя внешнюю структуру
 */
TEST(Json, OwnStructWithExternalStruct)
{
    Point expected { { 1, 2 } };
    std::string serialized = rgen::Json<Point>::toJson(expected);
    Point actual = rgen::Json<Point>::fromJson(serialized);

    EXPECT_TRUE(rgen::Json<Point>::assertJsonEquals(std::cerr, actual, expected));
}

/*!
 * \brief Using, помеченный тегом генерации
 */
TEST(Json, UsingForTemplate)
{
    MyDotPointInt expected = { 1, 2 };
    std::string serialized = rgen::Json<MyDotPointInt>::toJson(expected);
    MyDotPointInt actual = rgen::Json<MyDotPointInt>::fromJson(serialized);

    EXPECT_TRUE(rgen::Json<MyDotPointInt>::assertJsonEquals(std::cerr, actual, expected));
}

/*!
 * \brief Класс в namespace
 */
TEST(Json, ClassInNamespace)
{
    using namespace The;
    Book expected;
    std::string serialized = rgen::Json<Book>::toJson(expected);
    Book actual = rgen::Json<Book>::fromJson(serialized);

    EXPECT_TRUE(rgen::Json<Book>::assertJsonEquals(std::cerr, actual, expected));
}

/*!
 * \brief Тест для линейных контейнеров из std::
 */
TEST(Json, LinearStdContainer)
{
    LinearsStruct expected {
        .vec = { 1, 2 },
        .deq = { 1, 2 },
        .list = { 1, 2 },
        .flist = { 1, 2 }
    };
    std::string serialized = rgen::Json<LinearsStruct>::toJson(expected);
    auto actual = rgen::Json<LinearsStruct>::fromJson(serialized);

    EXPECT_TRUE(rgen::Json<LinearsStruct>::assertJsonEquals(std::cerr, actual, expected));
}

/*!
 * \brief Тест для ассоциативных контейнеров из std::
 */
TEST(Json, AssociativeStdContainer)
{
    // Важно проверить с использованием std:: структуры и самописной структуры
    AssociatedStruct expected {
        .m = { { "a", { 1, 2 } } },
        .unord_m = { { "b", { 1, 2 } } },
        .s = { 1 },
        .unord_s = { 1 }
    };
    std::string serialized = rgen::Json<AssociatedStruct>::toJson(expected);
    auto actual = rgen::Json<AssociatedStruct>::fromJson(serialized);

    EXPECT_TRUE(rgen::Json<AssociatedStruct>::assertJsonEquals(std::cerr, actual, expected));
}

/*!
 * \brief Тест для некоторых структур из std::
 */
TEST(Json, StdStructs)
{
    StdStruct expected {
        .pair = std::make_pair(1, 2),
        .opt = 1
    };
    std::string serialized = rgen::Json<StdStruct>::toJson(expected);
    auto actual = rgen::Json<StdStruct>::fromJson(serialized);

    EXPECT_TRUE(rgen::Json<StdStruct>::assertJsonEquals(std::cerr, actual, expected));
}

/*!
 * \brief Тест для стандартных переменных
 */
TEST(Json, VariablesUsage)
{
    Variables expected {
        .c = 'a',
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
    std::string serialized = rgen::Json<Variables>::toJson(expected);
    auto actual = rgen::Json<Variables>::fromJson(serialized);

    EXPECT_TRUE(rgen::Json<Variables>::assertJsonEquals(std::cerr, actual, expected));
}

/*!
 * \brief Тест для различных форматов json
 */
TEST(Json, JsonFormats)
{
    SimpleStruct expected = { 1 };
    std::string serializedCompact = rgen::Json<SimpleStruct>::toJson(expected, rgen::JsonFormat::Compact);
    SimpleStruct actualCompact = rgen::Json<SimpleStruct>::fromJson(serializedCompact);

    std::string expectedCompactString = R"COMPACT({"value":1})COMPACT";

    EXPECT_EQ(expected, actualCompact);
    EXPECT_EQ(expectedCompactString, serializedCompact);

    std::string serializedPretty = rgen::Json<SimpleStruct>::toJson(expected, rgen::JsonFormat::Pretty);
    SimpleStruct actualPretty = rgen::Json<SimpleStruct>::fromJson(serializedPretty);

    std::string expectedPrettyString = "{\n    \"value\": 1\n}";

    EXPECT_EQ(expected, actualPretty);
    EXPECT_EQ(serializedPretty, expectedPrettyString);
}

/*!
 * \brief Тест для различных структур Qt
 */
TEST(Json, QtStructs)
{
    QStructs expected {
        .vec = { 1, 2 },
        .map = { { 1, 2 }, { 3, 4 } },
        .name = "a",
        .uuid = QUuid::createUuid(),
        .pair = { 1, 2 },
        .strList = { "a", "b" },
        .hash = { { 1, 2 }, { 3, 4 } },
        .byteArray = { "a" },
    };

    std::string serialized = rgen::Json<QStructs>::toJson(expected);
    auto actual = rgen::Json<QStructs>::fromJson(serialized);

    EXPECT_TRUE(rgen::Json<QStructs>::assertJsonEquals(std::cerr, actual, expected));
}

TEST(Json, PointersUsage)
{
    Pointers expected {
        .uPtr = std::make_unique<int>(1),
        .sPtr = std::make_shared<int>(1),
        .qsPtr = QSharedPointer<int>(new int { 1 })
    };
    std::string serialized = rgen::Json<Pointers>::toJson(expected);
    auto actual = rgen::Json<Pointers>::fromJson(serialized);

    EXPECT_EQ(*expected.uPtr, *actual.uPtr);
    EXPECT_EQ(*expected.sPtr, *actual.sPtr);
    EXPECT_EQ(*expected.qsPtr, *actual.qsPtr);
}

TEST(Json, PointersNullptr)
{
    Pointers expected {
        .uPtr = nullptr,
        .sPtr = nullptr,
        .qsPtr = nullptr
    };
    std::string serialized = rgen::Json<Pointers>::toJson(expected);
    auto actual = rgen::Json<Pointers>::fromJson(serialized);

    EXPECT_EQ(expected, actual);
}

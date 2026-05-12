
#include <QDataStream>

#include <gtest/gtest.h>

#include "enum_test.h"
#include "derive_test.h"
#include "jsonschema_test.h"

#include <doc_tests_gen/derive_test.rgen.hxx>
#include <rgen/enum.h>
#include <rgen/json_schema.h>
#include <rgen/json_qt.h>
#include <jsoncons/json.hpp>

#ifndef Q_MOC_RUN // Отключаем moc для файла
#include <jsoncons_ext/jsonschema/jsonschema.hpp>
#endif

using namespace rgen;

// TEST(Enum, DocTest)
// {
//     // Тестирование парсинга строк в enum
//     EXPECT_EQ(rgen::Enum<TestColor>::fromString("Красный"), TestColor::Red);
//     EXPECT_EQ(rgen::Enum<TestColor>::fromString("Зелёный"), TestColor::Green);
//     EXPECT_EQ(rgen::Enum<TestColor>::fromString("Синий"), TestColor::Blue);
//     EXPECT_EQ(rgen::Enum<TestColor>::fromString("Pink"), TestColor::Pink);

//     // Тестирование преобразования enum в строки
//     EXPECT_EQ(rgen::Enum<TestColor>::toString(TestColor::Red), "Красный");
//     EXPECT_EQ(rgen::Enum<TestColor>::toString(TestColor::Green), "Зелёный");
//     EXPECT_EQ(rgen::Enum<TestColor>::toString(TestColor::Blue), "Синий");
//     EXPECT_EQ(rgen::Enum<TestColor>::toString(TestColor::Pink), "Pink");

//     auto allValues = rgen::Enum<TestColor>::values();

//     // Проверка количество элементов
//     EXPECT_EQ(allValues.size(), 4);

//     // Проверка конкретного элемента
//     EXPECT_EQ(allValues[1], TestColor::Green);
// }

TEST(Json, DocTest)
{
    TestStruct expected = { .name = "text", .originalKey = 5 };

    std::string serialized = rgen::Json<TestStruct>::toJson(expected);
    TestStruct actual = rgen::Json<TestStruct>::fromJson(serialized);

    EXPECT_EQ(expected, actual);

    /// JsonKeys check
    rgen::JsonKeys<TestStruct> fields;
    EXPECT_EQ(fields.name, "name");

    rgen::JsonKeys<TestStruct> fields1;
    EXPECT_EQ(fields1.originalKey, "myCustomKey");

    TestStruct expected1 = {
        .ignoredKey = 1,
        .originalKey = 2
    };

    /// JsonSerializer check
    std::string serialized1 = rgen::Json<TestStruct>::toJson(expected1);
    TestStruct actual1 = rgen::Json<TestStruct>::fromJson(serialized1);

    EXPECT_EQ(actual1.originalKey, actual1.originalKey);
    EXPECT_EQ(actual1.ignoredKey, 5);
}

TEST(OperatorEquals, DocTest)
{
    TestStruct lhs { .name = "text" };
    TestStruct rhs { .name = "text" };

    EXPECT_TRUE(operator==(lhs, rhs));

    /// Игнорируем первое поле TestStruct,
    /// значит сравнивается только второй элемент
    TestStruct lhs1 { .id = 1, .name = "text" };
    TestStruct rhs1 { .id = 2, .name = "text" };

    EXPECT_TRUE(operator==(lhs1, rhs1));
}

TEST(OperatorLess, DocTest)
{
    TestStruct lhs { .name = "a" };
    TestStruct rhs { .name = "b" };

    EXPECT_TRUE(operator<(lhs, rhs));

    /// Игнорируем первое поле TestStruct,
    /// значит сравнивается только второй элемент
    TestStruct lhs1 { .id = 100, .name = "a" };
    TestStruct rhs1 { .id = 2, .name = "b" };

    EXPECT_TRUE(operator<(lhs1, rhs1));
}

TEST(OperatorQStream, DocTest)
{
    TestStruct original { .name = "text" };
    TestStruct restored;

    QByteArray data;
    QDataStream outStream(&data, QIODevice::WriteOnly);
    outStream << original;
    EXPECT_GT(data.length(), 0);

    QDataStream inStream(&data, QIODevice::ReadOnly);
    inStream >> restored;

    EXPECT_EQ(original.name, restored.name);
}

TEST(Hasher, DocTest)
{
    TestStructHasher st { .name = "text"  };

    std::unordered_map<TestStructHasher, std::string> h;
    h[st] = "text";

    EXPECT_EQ(h[st], "text");
}

TEST(QHasher, DocTest)
{
    TestStruct st { .name = "text" };

    QHash<TestStruct, QString> h;
    h[st] = 1;

    EXPECT_EQ(h[st], 1);
}

TEST(JsonSchema, DocTest)
{
    TestStructJS testObj = { .username = "Иван", .age = 30, .isActive = true};

    // Конвертируем объект в JSON
    auto jsonString = rgen::Json<TestStructJS>::toJson(testObj);

    // Получаем JSON Schema для структуры
    std::string schemaStr = rgen::JsonSchema<TestStructJS>::getJsonSchema();

    jsoncons::json jsonObj = jsoncons::json::parse(jsonString);
    jsoncons::json schema = jsoncons::json::parse(schemaStr);

    auto validator = jsoncons::jsonschema::make_json_schema(schema);

    try
    {
        validator.validate(jsonObj);
        SUCCEED();
    }
    catch (std::exception& ex)
    {
        FAIL() << ex.what() << "Схема не прошла валидацию";
    }
}

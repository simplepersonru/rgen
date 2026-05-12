#include <iostream>

#include <gtest/gtest.h>

#include <jsoncons/json.hpp>
#ifndef Q_MOC_RUN // Отключаем moc для файла
#include <jsoncons_ext/jsonschema/jsonschema.hpp>
#endif

#include <rgen/json_schema.h>
#include <rgen/json_qt.h>

#include "S_1.h"

namespace
{

struct JsonObject
{
    jsoncons::json jsonObject; // Объект с данными от Json
    jsoncons::json jsonSchema; // Объект JsonSchema

    // Вернуть рзультат проверки валидности загруженных объектов
    bool isValid() { return getValidator().is_valid(jsonObject); }

    // Получить объект валидатора с загруженной JsonSchema
    jsoncons::jsonschema::json_schema<jsoncons::json> getValidator()
    {
        return jsoncons::jsonschema::make_json_schema(jsonSchema);
    }
};

template <class T>
JsonObject prepareData(T obj)
{
    const jsoncons::json jc_json_s = jsoncons::json::parse(rgen::Json<T>::toJson(obj));
    const jsoncons::json jc_schema = jsoncons::json::parse(rgen::JsonSchema<T>::getJsonSchema());
    return { jc_json_s, jc_schema };
}

void checkWrongValid(JsonObject& jObj, QString exceptionMsg = {})
{
    try
    {
        jObj.getValidator().validate(jObj.jsonObject);
        QString errorMsg = "Схема валидна, а ожидалось исключение " + exceptionMsg;
        FAIL() << errorMsg.toStdString().c_str();
    }
    catch (std::exception& ex)
    {
        // Если мы передали сообщение, то хотим его проверить на правильность выброса исключения
        if (exceptionMsg.isEmpty())
            GTEST_LOG_(WARNING) << ex.what();
        else
            EXPECT_STREQ(exceptionMsg.toStdString().c_str(), ex.what());
    }
}

} // namespace
namespace tests
{

TEST(JsonSchema, isValidGeneral)
{
    Hierarchy_lvl1 obj_s;
    auto jObj = prepareData(obj_s);

    try
    {
        jObj.getValidator().validate(jObj.jsonObject);
        SUCCEED();
    }
    catch (std::exception& ex)
    {
        FAIL() << ex.what() << "Схема не прошла валидацию";
    }
}

TEST(JsonSchema, isValidNamespace)
{
    using namespace The::Longest::Namespace;

    StructNamespaceSchema obj_s;
    auto jObj = prepareData(obj_s);

    try
    {
        jObj.getValidator().validate(jObj.jsonObject);
        SUCCEED();
    }
    catch (std::exception& ex)
    {
        FAIL() << ex.what() << "Схема не прошла валидацию";
    }
}

TEST(JsonSchema, isNotValid_MinValue)
{
    Hierarchy_lvl1 obj_s;
    auto jObj = prepareData(obj_s);
    auto& selfStructObjRef = jObj.jsonObject["hierarchy_lvl1_lvl2"];

    selfStructObjRef["uint8__field_Hierarchy_lvl1_lvl2"] = -1;
    checkWrongValid(jObj, "/hierarchy_lvl1_lvl2/uint8__field_Hierarchy_lvl1_lvl2: Minimum value is 0 but found -1");
}

TEST(JsonSchema, isNotValid_ExpectedInteger)
{
    Hierarchy_lvl1 obj_s;
    auto jObj = prepareData(obj_s);
    auto& selfStructObjRef = jObj.jsonObject["hierarchy_lvl1_lvl2"];

    selfStructObjRef["uint8__field_Hierarchy_lvl1_lvl2"] = "-1";
    checkWrongValid(jObj, "/hierarchy_lvl1_lvl2/uint8__field_Hierarchy_lvl1_lvl2: Expected integer, found string");
}

TEST(JsonSchema, isNotValid_AddAdditionalProperty)
{
    Hierarchy_lvl1 obj_s;
    auto jObj = prepareData(obj_s);
    auto& selfStructObjRef = jObj.jsonObject["hierarchy_lvl1_lvl2"];

    selfStructObjRef["wrongSomefield"] = "wrongValue";
    checkWrongValid(jObj, "/hierarchy_lvl1_lvl2/wrongSomefield: Additional property 'wrongSomefield' not allowed by schema.");
}

TEST(JsonSchema, isNotValid_Required)
{
    Hierarchy_lvl1 obj_s;
    auto jObj = prepareData(obj_s);
    auto& selfStructObjRef = jObj.jsonObject["hierarchy_lvl1_lvl2"];

    selfStructObjRef.erase("uint8__field_Hierarchy_lvl1_lvl2");
    checkWrongValid(jObj, "/hierarchy_lvl1_lvl2: Required property 'uint8__field_Hierarchy_lvl1_lvl2' not found.");
}

TEST(JsonSchema, isNotValid_Contains)
{
    Hierarchy_lvl1 obj_s;
    auto jObj = prepareData(obj_s);

    // Заменяем items на contains
    auto& schemaQByteArray_field = jObj.jsonSchema["properties"]["hierarchy_lvl1_lvl1"]["properties"]["qByteArray__field_Hierarchy_lvl1_lvl1"];
    schemaQByteArray_field["contains"]["type"] = schemaQByteArray_field["items"]["type"];
    schemaQByteArray_field.erase("items");

    checkWrongValid(jObj, "/hierarchy_lvl1_lvl1/qByteArray__field_Hierarchy_lvl1_lvl1: A schema must match a contains constraint at least 1 times but it matched 0 times.");
}

} // namespace tests

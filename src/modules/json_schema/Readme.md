
## Модуль JsonSchema

**JsonSchema** — модуль rgen для автоматической генерации JSON Schema из C++ структур

### Параметры и атрибуты:
| Аннотация | Описание |
| --- | --- |
| `@JsonSchema` |  Помечает для генерации сериализатора |
| `@Display` |  Задаёт описание для поля |

### Display-имена:
Как добавить @Display разметку к элементу перечисления:
1. Справа от элемента перечисления ///< text <br>
2. Над элементом перечисления /// text
3. Также в обоих случаях можно использовать аннотацию @Display, если требуется использовать другие команды Doxygen в комментарии, как будет показано дальше <br>

Подробную информацию про синтаксис Doxygen и правила для комментариев [`тут`](https://www.doxygen.nl/manual/docblocks.html)
```cpp
/*!
 * @JsonSchema
 * @Json
 */
struct TestStructJS
{
    /// @Display Имя пользователя
    QString username;
    /// Возраст
    int age;
    bool isActive;///< Активен
};

```

### Использование:
```cpp
/*!
 * @JsonSchema
 * @Json
 */
struct TestStructJS
{
    /// @Display Имя пользователя
    QString username;
    /// Возраст
    int age;
    bool isActive;///< Активен
};

TEST(JsonSchema, DocTest)
{
    TestStructJS testObj = { .username = "Иван", .age = 30, .isActive = true};

    // Конвертируем объект в JSON
    auto jsonString = rgen::Json<TestStructJS>::toJson(testObj);

    // Получаем JSON Schema для структуры
    QString schemaStr = rgen::JsonSchema<TestStructJS>::getJsonSchema();

    jsoncons::json jsonObj = jsoncons::json::parse(jsonString);
    jsoncons::json schema = jsoncons::json::parse(schemaStr.toStdString());

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
```

Сгенерируемый код для данного случая:
- [`jsonschema_test.rgen.cxx`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/tests/doc_tests/jsonschema_test.rgen.cxx) <br>

Тест можно запустить через:
- [`Тесты для документации`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/tests/doc_tests/CMakeLists.txt)
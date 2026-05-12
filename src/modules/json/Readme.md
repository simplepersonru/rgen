## Модуль Json
Json — модуль rgen для автоматической генерации JSON сериализации структур и классов C++.

| Аннотация | Описание |
| --- | --- |
| `@Json` |  Помечает структуры для генерации JSON сериализации |
| `@Ignore` |  Исключает отдельные поля из обработки всех модулей rgen |
| `@Json { ignore: true }` | Атрибут для исключения поля 
| `@Json { key: "customName" }` | Атрибут для задания кастомного имени JSON ключа |


### Атрибуты:

#### Игнорирование полей:
```cpp
/// @Json
struct TestStruct
{
	/// @Json { ignore: true }
    int id; // Не будет сериализовано
    QString name;
};
```

#### Кастомные ключи JSON:
```cpp
/// @Json
struct TestStruct
{
	/// @Json { key: myCustomValue }
    int id; // В JSON будет ключ "myCustomId"
    QString name;
};
```

#### @Ignore:
Про @Ignore можно узнать [`тут`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/modules/derive/Readme.md#ignore)

### Какие C++ конструкции поддерживаются:
1.  C++ STL:
```cpp
/// @Derive
struct STLStruct
{
    RGEN_DERIVE(friend, STLStruct, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap)
    
    std::vector<int> vec;
    std::deque<int> deq;
    std::list<int> list;
    std::forward_list<int> flist;
   
    std::map<std::string, Point> m;
    std::unordered_map<std::string, Point> unord_m;
    std::set<int> s;
    std::unordered_set<int> unord_s;
    
    std::pair<int, int> pair;
    std::optional<int> opt;
};
```
2.   Структуры с Qt типами:
```cpp
/// @Derive
struct QStructs 
{
    RGEN_DERIVE(friend, QStructs, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap)
    
    QVector<int> vec;
    QMap<int, int> map;
    QString name;
    QUuid uuid;
    QPair<int, int> pair;
    QStringList strList;
    QHash<int, int> hash;
    QByteArray byteArray;
};
```
Остальные С++ конструкции можно посмотреть [`тут`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/modules/derive/Readme.md#%D0%BA%D0%B0%D0%BA%D0%B8%D0%B5-c-%D0%BA%D0%BE%D0%BD%D1%81%D1%82%D1%80%D1%83%D0%BA%D1%86%D0%B8%D0%B8-%D0%BF%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B8%D0%B2%D0%B0%D1%8E%D1%82%D1%81%D1%8F)

### Использование:
```cpp
/// @Derive
struct TestStruct
{
	RGEN_DERIVE(friend, TestStruct, RGEN_Json)
	
    int id; ///< @Ignore
    float value;

    /// @Json { ignore: true }
    int ignoredKey = 5;

    /// @Json { key: myCustomKey }
    int originalKey = 5;
};

TEST(Json, DocTest)
{
    TestStruct expected = { .value = 3.14, .originalKey = 5 };

    std::string serialized = rgen::Json<TestStruct>::toJson(expected);
    TestStruct actual = rgen::Json<TestStruct>::fromJson(serialized);

    EXPECT_EQ(expected, actual);

    /// JsonKeys check
    rgen::JsonKeys<TestStruct> fields;
    EXPECT_EQ(fields.value, "value");

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
    EXPECT_EQ(actual1.ignoredKey, defaultValue);
}

```

Сгенерируемый код для данного случая:
- [`derive_test.rgen.hxx`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/tests/doc_tests/derive_test.rgen.hxx)
- [`derive_test.rgen.cxx`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/tests/doc_tests/derive_test.rgen.cxx) <br>

Тест можно запустить через:
- [`Тесты для документации`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/tests/doc_tests/CMakeLists.txt)
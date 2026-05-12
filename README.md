# Проект кодогенератора rgen

## Введение

**rgen** (Redkit Generator) — это генератор кода для C++, построенный на базе Clang. Он анализирует исходный код, находит специальные аннотации в Doxygen-комментариях и автоматически генерирует различный вспомогательный код: сериализацию в JSON, операторы сравнения, хэш-функции и многое другое.

Проект [WafflePlusPlus](https://github.com/Izaron/WafflePlusPlus) послужил основой и источником вдохновения

### Зачем нужен rgen?

- **Уменьшение шаблонного кода** — генерация повторяющихся конструкций
- **Согласованность** — гарантирует единообразие сгенерированного кода
- **Расширяемость** — легко добавлять новые генераторы
- **Интеграция с Qt** — полная поддержка Qt-типов
### Как это работает

1. **Анализ кода** — rgen использует Clang для парсинга C++ кода и поиска Doxygen-аннотаций
2. **Сбор данных** — извлечение помеченных структур, enum'ов, полей и их атрибутов
3. **Генерация** — на основе шаблонов [`inja`](https://pantor.github.io/inja/) создаются выходные файлы
4. **Интеграция** — сгенерированные файлы автоматически добавляются в сборку через систему сборки (CMake, QBS)

## Быстрый старт

### 1. Настройка проекта с CMAKE
Возьмем за основу [Пример](examples/cmake/simple-example/CMakeLists.txt)

### 2. Первая аннотация
Создайте файл my_struct.h:
```cpp
#pragma once

#include <QString>

/// @Derive
struct MyStruct
{
    RGEN_DERIVE(friend, MyStruct, RGEN_Json, RGEN_Equals)
    
    QString name;
    int age;
};  
```

### 3. Сборка и результат
При сборке проекта rgen автоматически сгенерирует:

- my_struct.rgen.hxx - JSON ключи и декларации
```cpp
#ifndef MYSTRUCT_JSON_HXX
#define MYSTRUCT_JSON_HXX

#include <rgen/private/json_pre.h>

#include <string>

#include "/home/user/project/src/my_struct.h" // Полный путь к заголовочнику

template <>
struct rgen::JsonKeys<MyStruct>
{
    std::string name = "name";
    std::string age = "age";
};
#endif // MYSTRUCT_JSON_HXX  
```

- my_struct.rgen.cxx - реализации операторов и JSON сериализации
```cpp
#include <rgen/common/export_macro.h>
#include <rgen/json_cpp.h>

#include "/home/user/project/build/src/my_struct.rgen.hxx"// Полный путь к сгенерированному заголовочнику
#include "/home/user/project/src/my_struct.h"                                       // Полный путь к заголовочнику

template <>
DECL_EXPORT void fromTypeToJson(jsoncons::json& json, const MyStruct& val)
{
    fromTypeToJson(json["name"], val.name);
    fromTypeToJson(json["age"], val.age);
}

template <>
DECL_EXPORT void fromJsonToType(const jsoncons::json& json, MyStruct& val)
{
    fromJsonToType(json["name"], val.name);
    fromJsonToType(json["age"], val.age);
}


#include <rgen/common/comparison.h> // для сравнений

#include "/home/user/project/src/my_struct.h"                                       // Полный путь к заголовочнику

bool operator==(const MyStruct& lhs, const MyStruct& rhs)
{
    bool equals_bases = true;
    bool equals_fields = true;
    equals_fields &= rgen::equals(lhs.name, rhs.name);
    equals_fields &= rgen::equals(lhs.age, rhs.age);
    return equals_bases && equals_fields;
}

bool operator!=(const MyStruct& lhs, const MyStruct& rhs)
{
    return !(lhs == rhs);
}
```

Использование в коде:

```cpp
#include "/home/user/project/src/my_struct.h"                                      
#include "/home/user/project/build/src/my_struct.json.hxx" // Сгенерированный файл

TEST(MyStruct, MyStructTest)
{
    MyStruct expected{"Ivan", 30};
    
    // JSON сериализация
    std::string json = rgen::Json<MyStruct>::toJson(expected);
    MyStruct actual = rgen::Json<MyStruct>::fromJson(json);
    
    // Операторы сравнения (генерируются модулем OperatorEquals)
    EXPECT_TRUE(operator==(expected, actual));
}
```


### Основные концепции

### Аннотации
rgen использует Doxygen-комментарии для маркировки типов и полей:
```cpp
/// @Json                     // Аннотация для структуры
struct MyStruct 
{
    /// @Json { key: "id" }  // Аннотация для поля с параметрами
    int field;
    
    /// @Ignore               // Поле будет проигнорировано
    QString temp;
};
```
#### Макрос RGEN_DERIVE
Основной макрос для модуля Derive, указывающий какие генераторы применить:
```cpp
RGEN_DERIVE(<access>, <type>, <modules...>)
```
- access - спецификатор доступа (friend или пустая строка, или макрос экспорта)

- type - имя структуры/класса

- modules - список модулей для генерации

#### Поддерживаемые модули

| Модуль | Аннотация | Назначение | Документация |
| --- | --- | --- | --- |
| `Derive` |  @Derive | Базовый модуль для групп генераторов через макрос RGEN_DERIVE | [`Подробнее`](https://github.com/simplepersonru/rgen/src/modules/derive/Readme.md)<br> |
| `Enum` |  @Enum | Двусторонняя конвертация enum <-> string с поддержкой display-имен | [`Подробнее`](https://github.com/simplepersonru/rgen/src/modules/enum/Readme.md)<br> |
| `Json` |  @Json | JSON сериализация с поддержкой кастомных ключей и игнорирования полей | [`Подробнее`](https://github.com/simplepersonru/rgen/src/modules/json/Readme.md)<br> |
| `JsonSchema` |  @JsonSchema | Генерация JSON Schema для валидации структур | [`Подробнее`](https://github.com/simplepersonru/rgen/src/modules/json_schema/Readme.md)<br> |
| `OperatorEquals` |  @OperatorEquals | Генерация операторов == и != | [`Подробнее`](https://github.com/simplepersonru/rgen/src/modules/operator_equals/Readme.md)<br> |
| `OperatorLess` |  @OperatorLess | Генерация оператора < для сортировки | [`Подробнее`](https://github.com/simplepersonru/rgen/src/modules/operator_less/Readme.md)<br> |
| `OperatorQStream` |  @OperatorQStream | Сериализация в QDataStream | [`Подробнее`](https://github.com/simplepersonru/rgen/src/modules/operator_qstream/Readme.md)<br> |
| `Hasher` |  @Hasher | Специализация std::hash для использования в unordered-контейнерах | [`Подробнее`](https://github.com/simplepersonru/rgen/src/modules/hasher/Readme.md)<br> |
| `QHasher` |  @QHasher | Генерация функции qHash() для Qt-контейнеров | [`Подробнее`](https://github.com/simplepersonru/rgen/src/modules/qhasher/Readme.md)<br> |
| `CustomModule` |  --- | Пользовательская команда, указанная в конфигурации | [`Подробнее`](https://github.com/simplepersonru/rgen/src/modules/custom_module/Readme.md)<br> |

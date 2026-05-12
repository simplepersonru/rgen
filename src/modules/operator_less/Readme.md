## Модуль OperatorLess

**OperatorLess** — модуль rgen для автоматической генерации оператора сравнения `<` для структур и классов C++.

### Параметры и атрибуты:
| Аннотация | Описание |
| --- | --- |
| `@OperatorLess` | Помечает структуру для генерации оператора `<` |
| `@Ignore` | Исключает поле из генерации |

#### @Ignore:
Про @Ignore можно узнать [`тут`](https://github.com/simplepersonru/rgen/src/modules/derive/Readme.md#ignore)

### Какие C++ конструкции поддерживаются:
Это можно посмотреть [`тут`](https://github.com/simplepersonru/rgen/src/modules/derive/Readme.md#%D0%BA%D0%B0%D0%BA%D0%B8%D0%B5-c-%D0%BA%D0%BE%D0%BD%D1%81%D1%82%D1%80%D1%83%D0%BA%D1%86%D0%B8%D0%B8-%D0%BF%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B8%D0%B2%D0%B0%D1%8E%D1%82%D1%81%D1%8F)

### Использование:
```cpp
/// @Derive
struct TestStruct
{
    RGEN_DERIVE(friend, TestStruct, RGEN_Less)

    int id; ///< @Ignore
    QString name;
};

TEST(OperatorLess, DocTest)
{
    TestStruct lhs { .name = "a" };
    TestStruct rhs { .name = "b" };

    EXPECT_TRUE(operator<(lhs, rhs));

    TestStruct lhs1 { .id = 100, .name = "a" };
    TestStruct rhs1 { .id = 2, .name = "b" };

    EXPECT_TRUE(operator<(lhs1, rhs1));
}
```

Сгенерируемый код для данного случая:
- [`derive_test.rgen.cxx`](https://github.com/simplepersonru/rgen/src/tests/doc_tests/derive_test.rgen.cxx#L210) <br>

Тест можно запустить через:
- [`Тесты для документации`](https://github.com/simplepersonru/rgen/src/tests/doc_tests/CMakeLists.txt)
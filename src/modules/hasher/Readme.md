## Модуль Hasher

**Hasher** — модуль Hasher предназначен для автоматической генерации специализации std::hash для структур и классов C++

### Параметры и атрибуты:
| Аннотация | Описание |
| --- | --- |
| `@Hasher` | Помечает структуру/класс для генерации |
| `@Ignore` | Исключает поле из генерации |

#### @Ignore:
Про @Ignore можно узнать [`тут`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/modules/derive/Readme.md#ignore)

### Какие C++ конструкции поддерживаются:
Это можно посмотреть [`тут`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/modules/derive/Readme.md#%D0%BA%D0%B0%D0%BA%D0%B8%D0%B5-c-%D0%BA%D0%BE%D0%BD%D1%81%D1%82%D1%80%D1%83%D0%BA%D1%86%D0%B8%D0%B8-%D0%BF%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B8%D0%B2%D0%B0%D1%8E%D1%82%D1%81%D1%8F)

### Использование:
```cpp
/// @Derive
struct TestStructHasher
{
    RGEN_DERIVE(friend, TestStruct, RGEN_Hasher)

    int id; ///< @Ignore
    std::string name;
};

TEST(Hasher, TestStructHasher)
{
    TestStruct st { .name = "text"  };

    std::unordered_map<TestStructHasher, std::string> h;
    h[st] = "text";

    EXPECT_EQ(h[st], "text");
}

```

Сгенерируемый код для данного случая:
- [`derive_test.rgen.hxx`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/tests/doc_tests/test/derive_test.rgen.hxx) <br>

Тест можно запустить через:
- [`Тесты для документации`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/tests/doc_tests/CMakeLists.txt)
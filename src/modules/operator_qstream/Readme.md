## Модуль OperatorQStream

**OperatorQStream** — модуль rgen для автоматической генерации операторов сериализации/десериализации `QDataStream` для структур и классов C++.

### Параметры и атрибуты:
| Аннотация | Описание |
| --- | --- |
| `@OperatorQStream` | Помечает структуру для генерации оператора `<` |
| `@Ignore` | Исключает поле из генерации |

#### @Ignore:
Про @Ignore можно узнать [`тут`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/modules/derive/Readme.md#ignore)

### Какие C++ конструкции поддерживаются:
Это можно посмотреть [`тут`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/modules/derive/Readme.md#%D0%BA%D0%B0%D0%BA%D0%B8%D0%B5-c-%D0%BA%D0%BE%D0%BD%D1%81%D1%82%D1%80%D1%83%D0%BA%D1%86%D0%B8%D0%B8-%D0%BF%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B8%D0%B2%D0%B0%D1%8E%D1%82%D1%81%D1%8F)

### Использование:
```cpp
/// @Derive
struct TestStruct
{
    RGEN_DERIVE(friend, TestStruct, RGEN_QStream)

    int id; ///< @Ignore
    QString name;
};

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

```

Сгенерируемый код для данного случая:
- [`derive_test.rgen.cxx`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/commit/917806bbed0d497fe1c53d379cbe0b90506fcd93/src/tests/doc_tests/derive_test.rgen.cxx#L162) <br>

Тест можно запустить через:
- [`Тесты для документации`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/tests/doc_tests/CMakeLists.txt)
## Модуль Derive

**Derive** — модуль rgen для автоматической генерации различных операторов и сериализаторов для структур и классов на основе макроса RGEN_DERIVE.

### Параметры и атрибуты:
| Аннотация | Описание |
| --- | --- |
| `@Derive` |  Помечает структуру/класс для генерации операторов и сериализаторов |
| `RGEN_DERIVE` |  Макрос, определяющий какие именно модули генерировать для структуры |
| `@Ignore` |  Исключает поле из генерации |

### RGEN_DERIVE
#### Синтаксис:
```cpp
RGEN_DERIVE(<access>, <type>, <modules...>)
```
| Параметр | Описание |
| --- | --- |
| `<access>` |  Спецификатор доступа: "friend" для генерации friend-функций внутри класса, пустая строка для using-алиасов, а можно экспортный макрос, чтобы пометить символы для экспорта |
| `<type>` |  Имя генерируемого типа (структуры/класса) |
| `<modules...>` |  Список модулей для генерации |  
  
### Поддерживаемые модули 

[`RGEN_Equals`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/modules/operator_equals/Readme.md)<br>
[`RGEN_Less`](https://gitea.redkit-lab.work/redkit-lab/rgen/src/branch/master/src/modules/operator_less/Readme.md)<br>
[`RGEN_QStream `](https://gitea.redkit-lab.work/redkit-lab/rgen/src/branch/master/src/modules/operator_qstream/Readme.md)<br>
[`RGEN_Json`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/modules/json/Readme.md)<br>
[`RGEN_Hasher`](https://gitea.redkit-lab.work/redkit-lab/rgen/src/branch/master/src/modules/hasher/Readme.md)<br>
[`RGEN_QHasher`](https://gitea.redkit-lab.work/redkit-lab/rgen/src/branch/master/src/modules/qhasher/Readme.md)<br>

### @Ignore
@Ignore позволяет исключить конкретные поля структуры из сгенерированного кода:

```cpp
/// @Derive
struct TestStruct
{
    RGEN_DERIVE(friend, TestStruct, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap)
	
    int id; ///< @Ignore
    QString name;
};
```

### Какие C++ конструкции поддерживаются:
1. Простые структуры:
```cpp
/// @Derive
struct TestStruct
{
    RGEN_DERIVE(friend, TestStruct, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap)
	
    int id;
    QString name;
};
```
2. Сложные вложенные структуры:
  ```cpp
/// @Derive
struct ComplexStruct
{
    RGEN_DERIVE(friend, ComplexStruct, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap)
    
    TestStruct day;
    QString name;
};
```
3.  Структуры в namespace:
```cpp
namespace The_TestStruct 
{
/// @Derive
struct TestStruct
{
	RGEN_DERIVE(friend, TestStruct, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap)
	
    int id;
    QString name;
};
}
```
4. Наследование:
```cpp
/// @Derive
struct Base 
{ 
	RGEN_DERIVE(friend, Base, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap)
	         
    int id;
};

/// @Derive
struct Derived : Base 
{ 
	RGEN_DERIVE(friend, Derived, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap)
	
    double name;
};
```
5.  Using-алиасы для шаблонных и нешаблонных классов:
```cpp
template <typename T>
struct MyCustomPoint 
{
    T x;
};

/// @Derive
using MyDotPointInt = MyCustomPoint<int>;
RGEN_DERIVE(, MyDotPointInt, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap);

// Для инстанциации шаблона
template struct MyCustomPoint<int>;
```
6. Using-алиасы для шаблонных классов в namespace:
```cpp
namespace The::Longest::Namespace
{
template<typename T>
struct StructTemplateNamespace
{
    T a;
};

/// @OperatorEquals
using StructUsingNamespace = StructTemplateNamespace<int>;
RGEN_DERIVE(, StructTemplateNamespace, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap);

template struct StructTemplateNamespace<int>;
}
```

 Примеры взяты из реальных тестов для данного модуля - [`Тесты Derive`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/modules/derive/test/test_operator.h)

### Использование:
```cpp
/// @Derive
struct TestStruct
{
    RGEN_DERIVE(friend, TestStruct, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap)

    int id; ///< @Ignore
    QString name;
};
```

Сгенерируемый код для данного случая:
- [`derive_test.rgen.hxx`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/tests/doc_tests/derive_test.rgen.hxx)
- [`derive_test.rgen.cxx`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/tests/doc_tests/derive_test.rgen.cxx) <br>

Тест можно запустить через:
- [`Тесты для документации`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/tests/doc_tests/CMakeLists.txt)
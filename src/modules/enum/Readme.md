## Модуль Enum

**Enum** — модуль rgen для автоматической генерации сериализации enum в строки и обратно. Создаёт специализации шаблона rgen::Enum<T> с методами toString(), fromString() и values()

### Параметры и атрибуты:
| Аннотация | Описание |
| --- | --- |
| `@Enum` |  Помечает для генерации сериализатора |
| `@Ignore` |  Исключает отдельные элементы enum из генерации |
| `@Display` |  Определяет отображаемое имя для элемента enum |

### @Ignore
@Ignore позволяет исключить конкретные элементы перечисления из сгенерированного кода

```cpp
/// @Enum
enum TestColor
{
    Red,
    Green ///< @Ignore
};
```


### Display-имена:
Как добавить @Display разметку к элементу перечисления:
1. Справа от элемента перечисления ///< text <br>
2. Над элементом перечисления /// text
3. Также в обоих случаях можно использовать аннотацию @Display, если требуется использовать другие команды Doxygen в комментарии, как будет показано дальше <br>

Подробную информацию про синтаксис Doxygen и правила для комментариев тут - https://www.doxygen.nl/manual/docblocks.html
```cpp
/// @Enum
enum TestColor
{
    Red,   ///< Красный
    /// Зелёный
    Green,
    /// @Display Синий
    Blue
};
```

#### Ключевой момент
При использовании ///< text, /// text или /// @Display text — модуль работает с display-именами ("Красный", "Зелёный", "Синий"), а не с именами из кода ("Red", "Green", "Blue").


#### Комментарии разработчика:
Для добавления служебных комментариев, которые не будут использоваться как display-имена, используйте другие Doxygen-теги:
```cpp
/// @Enum
enum TestEnum 
{
    /// @Display тест
    /// \note служебный комментарий
    Test
 }
```

Подробная информация про теги https://www.doxygen.nl/manual/commands.html

### К каким C++ символам применяется:

1. Enum class
```cpp
/// @Enum
enum class BookColor
{
    Red,
    Blue
};
```
2. Обычные enum
```cpp
/// @Enum
enum Number
{
    ONE ///< one
};
```
3. Using-алиасы для enum
```cpp
enum NumberUsingTest
{
    Two
};

/// @Enum
using NumberUsing = NumberUsingTest;
```
 Примеры взяты из реальных тестов для данного модуля - [`Базовые примеры enum`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/modules/enum/test/custom_names.h)
 
4. Enum в структуре
```cpp
struct Card
{
    /// @Enum
    enum Form
    {
        Rect
    };
};
```
5. Enum в namespace	
```cpp
namespace The::Longest::Namespace
{

/// @Enum
enum class Color
{
    Red
};  

}
```
6. Enum в структуре в namespace
```cpp  
namespace The::Longest::Namespace  
{

struct Book
{
    /// @Enum
    enum Format
    {
        A5
    };
};  

}
```

Примеры взяты из реальных тестов для данного модуля - [`Сложные случаи вложенности`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/modules/enum/test/misc_enum_places.h)
### Использование:
```cpp
/// @Enum
enum TestColor
{
    Red, ///< Красный
    /// Зелёный
    Green,
    /// @Display Синий
    Blue,
    Yellow ///< @Ignore
};

TEST(Enum, DisplayName)
{
    // Тестирование парсинга строк в enum
    EXPECT_EQ(rgen::Enum<TestColor>::fromString("Красный"), TestColor::Red);
    EXPECT_EQ(rgen::Enum<TestColor>::fromString("Зелёный"), TestColor::Green);
    EXPECT_EQ(rgen::Enum<TestColor>::fromString("Синий"), TestColor::Blue);

    // Тестирование преобразования enum в строки
    EXPECT_EQ(rgen::Enum<TestColor>::toString(TestColor::Red), "Красный");
    EXPECT_EQ(rgen::Enum<TestColor>::toString(TestColor::Green), "Зелёный");
    EXPECT_EQ(rgen::Enum<TestColor>::toString(TestColor::Blue), "Синий");

    auto allValues = rgen::Enum<TestColor>::values();

    // Проверка количество элементов
    EXPECT_EQ(allValues.size(), 3);

    // Проверка конкретные элементы
    EXPECT_EQ(allValues[1], TestColor::Green);
}
```

Сгенерируемый код для данного случая:
- [`enum_test.rgen.cxx`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/master/src/tests/doc_tests/enum_test.rgen.cxx) <br>

Тест можно запустить через:
- [`Тесты для документации`](https://gitea.redkit-lab.work/redkit-lab/redkit-gen/src/branch/kb/93623/src/tests/doc_tests/CMakeLists.txt)
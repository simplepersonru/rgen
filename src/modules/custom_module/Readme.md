## Модуль CustomModule

**CustomModule** — модуль rgen для создания пользовательских генераторов кода без необходимости писать C++ код. Позволяет определять собственные аннотации и шаблоны генерации для структур и enum'ов.

### Параметры и атрибуты:
| Аннотация | Описание |
| --- | --- |
| `@<command>` |  Пользовательская команда, указанная в конфигурации customModules |
| `@Meta` |  Дополнительная JSON-информация для структуры, enum'а или поля |

### Конфигурация в QBS:
```qbs
rgen.customModules: [
    {
        command: "myCommand",            // Имя команды 
        templatePath: "templates/my.j2", // Путь к шаблону Jinja2
        extension: "test.hxx",           // Расширение выходного файла
        fileTag: "hpp"                   // Тег для QBS
    }
]
```

### Использование
1. Создайте заголовочный файл custom_module.h:
```cpp
#pragma once

template <typename T>
int countFields();

/// @myCommand
struct TestStruct
{
    int a;
    float b;
    double c;
};
```
2. Создайте шаблон для генерации структуры custom_module_struct.j2:
```jinja
#include "{{ header_filename_full }}"

{% for class in classes %}
struct Gen_{{ class.name }}
{
{% for field in class.fields %}
    {{ field.type }} Gen_{{ field.name }};
{% endfor %}
};
{% endfor %}
```
3. Создайте шаблон для генерации реализации custom_module_countfields.j2:
```jinja
#include "{{ header_filename_full }}"

{% for class in classes %}
template<>
int countFields<{{ class.name }}>() {
    return {{ class.fields | length }};
}
{% endfor %}  
```
4. Настройте mylib.qbs:
```qbs
Product {
    name: "mylib"
    
    Depends { name: "rgen" }
    
    rgen.customModules: [
        {
            command: "myCommand",
            templatePath: sourceDirectory + "/custom_module_struct.j2",
            extension: "test.hxx",
            fileTag: "hpp"
        },
        {
            command: "myCommand",
            templatePath: sourceDirectory + "/custom_module_countfields.j2",
            extension: "test.cxx",
            fileTag: "cpp"
        }
    ]
    
    Group {
        name: "rgen sources"
        files: ["custom_module.h"]
        fileTags: ["rgen"]
    }
}
```

Использование в коде (main.cpp):
Сгенерируемый код для данного случая:
- [`custom_module.test.hxx`](https://github.com/simplepersonru/rgen/examples/qbs/simple-example/mylib/custom_module.test.hxx)
- [`custom_module.test.cxx`](https://github.com/simplepersonru/rgen/examples/qbs/simple-example/mylib/custom_module.test.cxx) <br>

Тест можно запустить через:
- [`Тесты для документации`](https://github.com/simplepersonru/rgen/examples/qbs/simple-example/simple-example.qbs)
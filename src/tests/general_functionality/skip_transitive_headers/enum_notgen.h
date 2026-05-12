#pragma once
// Суть этого файла в том, чтобы не появиться в генерации enum_gen.h
// Если появится - возникнет ошибка multiple definition (множественного определения)
// .../src/modules/enum/test/something_enums/enum_notgen.enum.cxx:14: ошибка: multiple definition of `NOTGEN rgen::FromString<NOTGEN>(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >)'; CMakeFiles/enum_test.dir/something_enums/enum_gen.enum.cxx.o:.../src/modules/enum/test/something_enums/enum_gen.enum.cxx:14: first defined here
// Что будет означать, что тест провален

/// @Enum
enum class NOTGEN
{
    Bad
};

#pragma once

#include <clang/AST/Decl.h>
#include <string>
#include <vector>

namespace rgen::Enum
{

struct ValueData
{
    std::string name;
    std::string display;
};

struct EnumData
{
    std::string name;
    std::vector<ValueData> values;
};

struct PrintData
{
    std::string header_filename_full; // Полный путь к файлу, в котором был обнаружен тег
    std::vector<EnumData> enums;
};

} // namespace rgen::Enum

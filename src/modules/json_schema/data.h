#pragma once

#include <clang/AST/Decl.h>
#include <string>
#include <vector>

namespace rgen::JsonSchema
{

/// @JsonSerializerV2
struct ClassData
{
    std::string nameWithNamespace;
    std::string jsonSchema;
};

/// @JsonSerializerV2
struct PrintData
{
    std::string header_filename_full; // Полный путь к файлу, в котором был обнаружен тег
    std::vector<ClassData> classes;
};

} // namespace rgen::JsonSchema

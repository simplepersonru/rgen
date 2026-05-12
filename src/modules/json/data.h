#pragma once

#include <clang/AST/Decl.h>
#include <core/derive_data.h>
#include <core/inheritance.h>
#include <string>
#include <vector>

namespace rgen::JsonNS
{

/// @JsonSerializerV2
struct FieldData : public rgen::DeriveFieldData
{
    std::string key;
};

/// @JsonSerializerV2
struct ClassData : public rgen::DeriveClassData
{
    std::vector<FieldData> fields;
};

/// @JsonSerializerV2
struct PrintData
{
    std::string header_filename_full; ///< Полный путь к файлу, в котором был обнаружен тег
    std::vector<ClassData> classes;
};

} // namespace rgen::JsonNS

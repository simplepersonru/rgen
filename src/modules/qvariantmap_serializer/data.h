#pragma once

#include <clang/AST/Decl.h>
#include <core/inheritance.h>
#include <string>
#include <vector>

namespace rgen::QVariantMapSerializer
{

/// @JsonSerializerV2
struct FieldData
{
    std::string name;
};

/// @JsonSerializerV2
struct ClassData
{
    std::string nameWithNamespace;
    std::string name;
    std::string namespace_;
    bool hasNamespace = false;
    std::vector<rgen::utils::BaseClassInfo> bases;
    std::vector<FieldData> fields;
};

/// @JsonSerializerV2
struct PrintData
{
    std::string header_filename_full;
    std::vector<ClassData> classes;
};

} // namespace rgen::QVariantMapSerializer

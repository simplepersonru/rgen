#pragma once

#include <core/inheritance.h>
#include <string>
#include <vector>

namespace rgen
{

/// @JsonSerializerV2
struct DeriveFieldData
{
    std::string name;
};

/// @JsonSerializerV2
struct DeriveClassData
{
    std::string nameWithNamespace;
    std::string name;
    std::string namespace_;
    std::vector<utils::BaseClassInfo> bases;
};

/// @JsonSerializerV2
struct DeriveClassDataWithFields : public DeriveClassData
{
    std::vector<DeriveFieldData> fields;
};

/// @JsonSerializerV2
struct DerivePrintData
{
    std::string header_filename_full; ///< Полный путь к файлу, в котором был обнаружен тег
    std::vector<DeriveClassDataWithFields> classes;
};

} // namespace rgen

#pragma once

#include <core/logger.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace rgen::CustomModule
{

struct MetaInfo
{
    std::string name;
    std::string fullName;
    std::string namespace_;
    std::string command;
    nlohmann::json meta;
};

struct FieldData : MetaInfo
{
    std::string type;
};

struct ClassData : MetaInfo
{
    std::vector<FieldData> fields;
};

struct EnumeratorData : MetaInfo
{
    std::string value;
};

struct EnumData : MetaInfo
{
    std::vector<EnumeratorData> enumerators;
};

struct PrintData
{
    std::string header_filename_full;
    std::vector<ClassData> classes;
    std::vector<EnumData> enums;
};

struct CustomModuleConfig
{
    std::string command;
    std::string templatePath;
    std::string extension;
    std::string fileTag;
};

std::vector<CustomModuleConfig> parseCustomCommands(const std::string& metaJson);

} // namespace rgen::CustomModule

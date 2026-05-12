#pragma once

#include "data.h"

#include <core/common_constants.h>
#include <core/extracteddata.h>
#include <core/registry.h>
#include <string>

namespace rgen::JsonNS
{

inline const std::string COMMAND_JSON_SERIALIZER_V2 = "JsonSerializerV2";
inline const std::string COMMAND_JSON = "Json";
inline const std::string TEMPLATE_FILE_NAME_HXX = "json.hxx.j2";
inline const std::string TEMPLATE_FILE_NAME_CXX = "json.cxx.j2";

struct Module
{
    static ModuleInfo getModuleInfo();
    static std::optional<PrintData> convert(const ExtractedData& extractedData, clang::ASTContext& astContext);
    static void print(PrintData&& printData, ContentManager& contentManager);
    static void process(Context& ctx);
};

} // namespace rgen::JsonNS

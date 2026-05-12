#pragma once

#include "data.h"
#include <core/common_constants.h>
#include <core/extracteddata.h>
#include <core/registry.h>

namespace rgen::Enum
{

inline const std::string COMMAND_ENUM = "Enum";
inline const std::string TEMPLATE_FILE_NAME_CXX = "enum.cxx.j2";

struct Module
{
    static ModuleInfo getModuleInfo();
    static std::optional<PrintData> convert(const ExtractedData& extractedData, clang::ASTContext& astContext);
    static void print(PrintData&& printData, ContentManager& contentManager);
    static void process(Context& ctx);
};

} // namespace rgen::Enum

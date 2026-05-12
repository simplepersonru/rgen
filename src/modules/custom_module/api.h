#pragma once

#include "data.h"
#include <core/registry.h>

namespace rgen::CustomModule
{
struct Module
{
    static ModuleInfo getModuleInfo();
    static std::optional<PrintData> convert(const ExtractedData& extractedData, clang::ASTContext& astContext);
    static void print(PrintData&& printData, ContentManager& contentManager);
    static void process(Context& ctx);
};
} // namespace rgen::CustomModule

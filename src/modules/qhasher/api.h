#pragma once

#include <core/derive_data.h>

#include <string>

#include <core/extracteddata.h>
#include <core/registry.h>

namespace rgen::QHasher
{

inline const std::string COMMAND_QHASHER = "QHasher";
inline const std::string TEMPLATE_FILE_NAME_CXX = "qhasher.cxx.j2";

struct Module
{
    static ModuleInfo getModuleInfo();
    static std::optional<rgen::DerivePrintData> convert(const ExtractedData& extractedData, clang::ASTContext& astContext);
    static void print(rgen::DerivePrintData&& printData, ContentManager& contentManager);
    static void process(Context& ctx);
};

} // namespace rgen::QHasher

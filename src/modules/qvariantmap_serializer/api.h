#pragma once

#include <core/common_constants.h>
#include <core/derive_data.h>
#include <core/extracteddata.h>
#include <core/registry.h>
#include <string>

namespace rgen::QVariantMapSerializer
{

inline const std::string COMMAND_QVARIANTMAP_SERIALIZER = "QVariantMapSerializer";
inline const std::string TEMPLATE_FILE_NAME_CXX = "qvariantmap_serializer.cxx.j2";

struct Module
{
    static ModuleInfo getModuleInfo();
    static std::optional<rgen::DerivePrintData> convert(const ExtractedData& extractedData, clang::ASTContext& astContext);
    static void print(rgen::DerivePrintData&& printData, ContentManager& contentManager);
    static void process(Context& ctx);
};

} // namespace rgen::QVariantMapSerializer

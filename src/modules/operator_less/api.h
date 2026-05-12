#pragma once

#include <core/derive_data.h>
#include <core/extracteddata.h>
#include <core/registry.h>
#include <string>

namespace rgen::OperatorLess
{

inline const std::string COMMAND_OPERATOR_LESS = "OperatorLess";
inline const std::string TEMPLATE_FILE_NAME_CXX = "operator_less.cxx.j2";

struct Module
{
    static ModuleInfo getModuleInfo();
    static std::optional<rgen::DerivePrintData> convert(const ExtractedData& extractedData, clang::ASTContext& astContext);
    static void print(rgen::DerivePrintData&& printData, ContentManager& contentManager);
    static void process(Context& ctx);
};

} // namespace rgen::OperatorLess

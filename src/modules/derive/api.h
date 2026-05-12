#pragma once

#include <core/derive_data.h>
#include <core/extracteddata.h>
#include <core/registry.h>

namespace rgen::Derive
{

inline const std::string COMMAND_DERIVE = "Derive";
inline const std::string COMMAND_DERIVE_MACROS = "RGEN_DERIVE";

struct DeriveModule
{
    std::string deriveName; /// Название модуля в макросе DERIVE
    std::string comment;    /// Комментарий, которому этот модуль соответствует (например, Json)
};

struct Module
{
    static ModuleInfo getModuleInfo();
    static std::vector<DeriveModule> getDeriveModules();
    static ExtractedData convert(const Context& ctx);
    static void process(Context& ctx);
};

} // namespace rgen::Derive

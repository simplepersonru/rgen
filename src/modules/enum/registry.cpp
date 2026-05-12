#include "api.h"

#include <core/logger.h>
#include <core/registry.h>

using namespace rgen;

namespace rgen::Enum
{

ModuleInfo Module::getModuleInfo()
{
    ModuleInfo info;
    info.Commands = {
        COMMAND_ENUM,
        COMMAND_IGNORE,
        COMMAND_DISPLAY,
    };
    info.FileExtensions = {
        { .Extension = FILE_EXTENSION_RGEN_CXX, .FileTag = FILE_EXTENSION_CXX_TAG }
    };
    info.Includes = {
        "/rgen/enum.h",
    };
    info.Templates = {
        TEMPLATE_FILE_NAME_CXX,
    };

    return info;
}

void Module::process(Context& ctx)
{
    auto printDataOpt = Module::convert(ctx.extractedData, ctx.astContext);

    if (!printDataOpt.has_value())
    {
        rlog::debug("No data to process generation for Enum");
        return;
    }

    Module::print(std::move(printDataOpt.value()), ctx.contentManager);
}

} // namespace rgen::Enum

REGISTER_MODULE(Enum, Enum::Module::process, Enum::Module::getModuleInfo());

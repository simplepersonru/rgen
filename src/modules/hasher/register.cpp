#include "api.h"
#include "core/common_constants.h"
#include <core/logger.h>
#include <core/registry.h>

using namespace rgen;

namespace rgen::Hasher
{

ModuleInfo Module::getModuleInfo()
{
    ModuleInfo info;
    info.Commands = {
        COMMAND_HASHER,
    };
    info.FileExtensions = {
        { .Extension = FILE_EXTENSION_RGEN_HXX, .FileTag = FILE_EXTENSION_HXX_TAG }
    };
    info.Includes = {
        "/rgen/hasher.h"
    };
    info.Templates = {
        TEMPLATE_FILE_NAME_HXX,
    };
    return info;
}

void Module::process(Context& ctx)
{
    auto printDataOpt = Module::convert(ctx.extractedData, ctx.astContext);

    if (!printDataOpt.has_value())
    {
        rlog::debug("No data to process generation for Hasher");
        return;
    }

    Module::print(std::move(printDataOpt.value()), ctx.contentManager);
}

} // namespace rgen::Hasher

REGISTER_MODULE(Hasher, Hasher::Module::process, Hasher::Module::getModuleInfo());

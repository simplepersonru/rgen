#include "api.h"
#include "core/common_constants.h"
#include <core/logger.h>
#include <core/registry.h>

using namespace rgen;

namespace rgen::OperatorLess
{

ModuleInfo Module::getModuleInfo()
{
    ModuleInfo info;
    info.Commands = {
        COMMAND_OPERATOR_LESS,
        COMMAND_IGNORE
    };
    info.FileExtensions = {
        { .Extension = FILE_EXTENSION_RGEN_CXX, .FileTag = FILE_EXTENSION_CXX_TAG }
    };
    info.Includes = {
        "/rgen/operator_less.h"
    };
    info.Templates = {
        TEMPLATE_FILE_NAME_CXX
    };
    return info;
}

void Module::process(Context& ctx)
{
    auto printDataOpt = Module::convert(ctx.extractedData, ctx.astContext);

    if (!printDataOpt.has_value())
    {
        rlog::debug("No data to process generation for OperatorLess");
        return;
    }

    Module::print(std::move(printDataOpt.value()), ctx.contentManager);
}

} // namespace rgen::OperatorLess

REGISTER_MODULE(OperatorLess, OperatorLess::Module::process, OperatorLess::Module::getModuleInfo());

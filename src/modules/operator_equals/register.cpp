#include "api.h"
#include "core/common_constants.h"
#include <core/logger.h>
#include <core/registry.h>

using namespace rgen;

namespace rgen::OperatorEquals
{

ModuleInfo Module::getModuleInfo()
{
    ModuleInfo info;
    info.Commands = {
        COMMAND_OPERATOR_EQUALS,
        COMMAND_IGNORE
    };
    info.FileExtensions = {
        { .Extension = FILE_EXTENSION_RGEN_CXX, .FileTag = FILE_EXTENSION_CXX_TAG }
    };
    info.Includes = {
        "/rgen/operator_equals.h"
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
        rlog::debug("No data to process generation for OperatorEquals");
        return;
    }

    Module::print(std::move(printDataOpt.value()), ctx.contentManager);
}

} // namespace rgen::OperatorEquals

REGISTER_MODULE(OperatorEquals, OperatorEquals::Module::process, OperatorEquals::Module::getModuleInfo());

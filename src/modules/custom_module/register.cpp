#include "api.h"
#include <core/command_line_args.h>
#include <core/logger.h>
#include <core/registry.h>

using namespace rgen;

namespace rgen::CustomModule
{

ModuleInfo Module::getModuleInfo()
{
    ModuleInfo info;
    info.Commands = { "Meta" };

    return info;
}

void Module::process(Context& ctx)
{
    auto printDataOpt = Module::convert(ctx.extractedData, ctx.astContext);

    PrintData printData;
    if (printDataOpt.has_value())
    {
        printData = std::move(printDataOpt.value());
        rlog::debug("No data to process generation for CustomModule");
    }

    Module::print(std::move(printData), ctx.contentManager);
}

} // namespace rgen::CustomModule

REGISTER_MODULE(CustomModule, CustomModule::Module::process, CustomModule::Module::getModuleInfo());

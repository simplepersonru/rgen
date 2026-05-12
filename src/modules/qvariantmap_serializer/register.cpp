#include "api.h"
#include <core/logger.h>
#include <core/registry.h>

using namespace rgen;

namespace rgen::QVariantMapSerializer
{

ModuleInfo Module::getModuleInfo()
{
    ModuleInfo info;
    info.Commands = {
        COMMAND_QVARIANTMAP_SERIALIZER,
        COMMAND_IGNORE
    };
    info.FileExtensions = {
        { .Extension = FILE_EXTENSION_RGEN_CXX, .FileTag = FILE_EXTENSION_CXX_TAG }
    };
    info.Includes = {
        "/rgen/private/qvariantmap_post.h",
        "/rgen/private/qvariantmap_pre.h",
        "/rgen/private/qvariantmap_lib.h",
        "/rgen/qvariantmap_serializer.h",
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
        rlog::debug("No data to process generation for QVariantMapSerializer");
        return;
    }

    Module::print(std::move(printDataOpt.value()), ctx.contentManager);
}

} // namespace rgen::QVariantMapSerializer

REGISTER_MODULE(QVariantMapSerializer, rgen::QVariantMapSerializer::Module::process, rgen::QVariantMapSerializer::Module::getModuleInfo());

#include "api.h"
#include <core/logger.h>
#include <core/registry.h>

using namespace rgen;

namespace rgen::JsonNS
{

ModuleInfo Module::getModuleInfo()
{
    ModuleInfo info;
    info.Commands = {
        COMMAND_JSON_SERIALIZER_V2,
        COMMAND_JSON,
        COMMAND_IGNORE
    };
    info.FileExtensions = {
        { .Extension = FILE_EXTENSION_RGEN_CXX, .FileTag = FILE_EXTENSION_CXX_TAG },
        { .Extension = FILE_EXTENSION_RGEN_HXX, .FileTag = FILE_EXTENSION_HXX_TAG },
    };
    info.Includes = {
        "/rgen/private/json_post.h",
        "/rgen/private/json_serializer_v2.h",
        "/rgen/json_cpp.h",
        "/rgen/json_qt.h",
        "/rgen/jsonserializer_cpp.h",
        "/rgen/jsonserializer_qt.h",
        "/rgen/private/json_pre.h",
        "/rgen/private/jsonlib_declare.h",
        "/rgen/private/jsonlib_qt.h",
        "/rgen/private/jsonlib_std.h"
    };
    info.Templates = {
        TEMPLATE_FILE_NAME_HXX,
        TEMPLATE_FILE_NAME_CXX
    };
    return info;
}

void Module::process(Context& ctx)
{
    auto printDataOpt = Module::convert(ctx.extractedData, ctx.astContext);

    if (!printDataOpt.has_value())
    {
        rlog::debug("No data to process generation for Json");
        return;
    }

    Module::print(std::move(printDataOpt.value()), ctx.contentManager);
}

} // namespace rgen::JsonNS

REGISTER_MODULE(Json, rgen::JsonNS::Module::process, rgen::JsonNS::Module::getModuleInfo());

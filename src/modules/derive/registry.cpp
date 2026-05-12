#include "api.h"

#include <core/logger.h>
#include <core/registry.h>

#include <modules/hasher/api.h>
#include <modules/json/api.h>
#include <modules/operator_equals/api.h>
#include <modules/operator_less/api.h>
#include <modules/operator_qstream/api.h>
#include <modules/qhasher/api.h>
#include <modules/qvariantmap_serializer/api.h>

using namespace rgen;

namespace rgen::Derive
{

void addIncludes(std::vector<std::string_view>& out, const std::vector<std::string_view>& in)
{
    std::copy(in.begin(), in.end(), std::back_inserter(out));
}

std::vector<std::string_view> Includes()
{
    std::vector<std::string_view> includes;

    includes.push_back("/rgen/derive.h");

    addIncludes(includes, JsonNS::Module::getModuleInfo().Includes);
    addIncludes(includes, OperatorQStream::Module::getModuleInfo().Includes);
    addIncludes(includes, OperatorLess::Module::getModuleInfo().Includes);
    addIncludes(includes, OperatorEquals::Module::getModuleInfo().Includes);
    addIncludes(includes, Hasher::Module::getModuleInfo().Includes);
    addIncludes(includes, QHasher::Module::getModuleInfo().Includes);
    addIncludes(includes, QVariantMapSerializer::Module::getModuleInfo().Includes);

    return includes;
}

ModuleInfo Module::getModuleInfo()
{
    ModuleInfo info;
    info.Commands = {
        COMMAND_DERIVE,
    };
    info.Includes = Includes();

    return info;
}

std::vector<DeriveModule> Module::getDeriveModules()
{
    static std::vector<DeriveModule> modules = {
        { "RGEN_Equals", OperatorEquals::COMMAND_OPERATOR_EQUALS },
        { "RGEN_Less", OperatorLess::COMMAND_OPERATOR_LESS },
        { "RGEN_QStream", OperatorQStream::COMMAND_OPERATOR_QSTREAM },
        { "RGEN_Json", JsonNS::COMMAND_JSON },
        { "RGEN_Hasher", Hasher::COMMAND_HASHER },
        { "RGEN_QHasher", QHasher::COMMAND_QHASHER },
        { "RGEN_QVariantMap", QVariantMapSerializer::COMMAND_QVARIANTMAP_SERIALIZER },
    };
    return modules;
}

void Module::process(Context& ctx)
{
    ExtractedData exData = convert(ctx);
    Context newCtx {
        /* .ContentManager = */ ctx.contentManager,
        /* .AstContext = */ ctx.astContext,
        /* .extractedData = */ exData,
    };
    const auto deriveModules = Module::getDeriveModules();
    for (const auto& module : ModuleRegistry::GetModules())
    {
        if (std::none_of(deriveModules.begin(), deriveModules.end(),
                         [module](const DeriveModule& dm) { return dm.comment == module.Name; }))
            continue;
        rlog::debug("Begin module processing from derive {}", module.Name);
        try
        {
            module.Func(newCtx);
        }
        catch (std::exception& ex)
        {
            rlog::error("{}", ex.what());
        }
    }
}

} // namespace rgen::Derive

REGISTER_MODULE(Derive, Derive::Module::process, Derive::Module::getModuleInfo());

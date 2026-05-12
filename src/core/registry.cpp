#include "registry.h"

#include <sstream>
#include <string>

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <thirdparty/include/nlohmann/json.hpp>

namespace rgen
{

namespace
{
using namespace fmt::literals;
}

static std::vector<Module> MODULES;

void ModuleRegistry::AddModule(Module module)
{
    MODULES.emplace_back(std::move(module));
}

const std::vector<Module>& ModuleRegistry::GetModules()
{
    return MODULES;
}

void ModuleRegistry::ClearModules()
{
    MODULES.clear();
}

void ModuleRegistry::FilterModules(const std::string& additionalModulesString)
{
    std::unordered_set<std::string> additionalModules;

    std::stringstream streamModule(additionalModulesString);
    std::string token;

    while (std::getline(streamModule, token, ','))
        if (!token.empty()) // Все же стоит проверять, не пустая ли строка, а то мало ли....
            additionalModules.insert(token);

    /// Обрабатываем все модули, указанные как "по умолчанию" и модули, которые пользователь перечислил
    /// в additionalModulesString через запятую (берётся из переменной запуска arg_modules)
    auto removeIt = std::remove_if(MODULES.begin(), MODULES.end(), [&](const Module& module) {
        return additionalModules.find(std::string(module.Name)) == additionalModules.end()
        && !module.Info.IsDefault;
    });

    MODULES.erase(removeIt, MODULES.end());
}

std::unordered_set<std::string> ModuleRegistry::getArtifactsFilenames(const std::filesystem::path& headerPath, const std::filesystem::path& outputDir)
{
    std::unordered_set<std::string> paths;

    // clang-format off
    for (const auto& module : GetModules())                                          // Для каждого подключеннного модуля
        for (const auto& ext : module.Info.FileExtensions)                                 // получить список артефактов
            paths.insert((outputDir / fmt::format("{filename}.{extension}", // и создать из этого путь
                                                 "filename"_a = headerPath.filename().stem().string(),
                                                 "extension"_a = ext.Extension)).string());

    return paths;
    // clang-format on
}

std::string ModuleRegistry::getJsonModuleExtensions()
{
    using json = nlohmann::json;
    json jModuleCollector;

    for (const auto& module : ModuleRegistry::GetModules())
    {
        if (module.Info.FileExtensions.empty())
            continue;

        json moduleInfo;
        moduleInfo["name"] = module.Name;
        moduleInfo["default"] = module.Info.IsDefault;

        for (const auto& ext : module.Info.FileExtensions)
        {
            json forExt;

            if (!ext.Extension.empty())
                forExt["name"] = fmt::format(".{}", ext.Extension);
            forExt["fileTag"] = ext.FileTag;
            moduleInfo["extension"].push_back(forExt);
        }

        for (const auto& include : module.Info.Includes)
            moduleInfo["include"].push_back(include);

        for (const auto& include : module.Info.Templates)
            moduleInfo["template"].push_back(include);

        jModuleCollector.push_back(moduleInfo);
    }

    return jModuleCollector.dump(3);
}

std::string ModuleRegistry::getModuleNames()
{
    std::string result;
    for (const auto& m : ModuleRegistry::GetModules())
    {
        if (!result.empty())
            result += ",\n";
        result += fmt::format(R"(   {name}: "{name}")",
                              "name"_a = m.Name);
    }
    return fmt::format("\n{}\n", result);
}

} // namespace rgen

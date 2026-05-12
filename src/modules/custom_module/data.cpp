#include "data.h"

namespace rgen::CustomModule
{

namespace
{
void logMissingKey(const std::string& key, const nlohmann::json& item)
{
    rlog::error("in cli argument --custom-commands, inside array -> object expected key '{}', but got json {}", key, item.dump());
}

bool checkMissing(const nlohmann::json& item, const std::string& metaJson, const std::string& key)
{
    if (item.contains(key))
        return false;
    logMissingKey(key, metaJson);
    return true;
}
} // namespace

std::vector<CustomModuleConfig> parseCustomCommands(const std::string& metaJson)
{
    const std::string command = "command";
    const std::string templatePath = "templatePath";
    const std::string extension = "extension";
    const std::string fileTag = "fileTag";

    if (metaJson.empty())
        return {};

    try
    {
        auto json = nlohmann::json::parse(metaJson);
        if (!json.is_array())
            return {};

        std::vector<CustomModuleConfig> result;
        for (const auto& item : json)
        {
            if (checkMissing(item, metaJson, command))
                return {};
            if (checkMissing(item, metaJson, templatePath))
                return {};
            if (checkMissing(item, metaJson, extension))
                return {};
            if (checkMissing(item, metaJson, fileTag))
                return {};

            CustomModuleConfig cfg;
            cfg.command = item[command].get<std::string>();
            cfg.templatePath = item[templatePath].get<std::string>();
            cfg.extension = item[extension].get<std::string>();
            cfg.fileTag = item[fileTag].get<std::string>();
            result.push_back(cfg);
        }
        return result;
    }
    catch (const std::exception& e)
    {
        rlog::error("Failed to parse custom commands: {}", e.what());
        return {};
    }
}

} // namespace rgen::CustomModule

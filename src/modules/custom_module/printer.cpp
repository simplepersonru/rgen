#include "api.h"
#include "data.h"
#include <core/command_line_args.h>
#include <core/file.h>
#include <core/logger.h>
#include <core/utils.h>
#include <inja/inja.hpp>

namespace rgen::CustomModule
{

namespace
{

inja::json metaInfoToInja(const MetaInfo& info)
{
    inja::json result;
    result["name"] = info.name;
    result["fullName"] = info.fullName;
    result["namespace"] = info.namespace_;
    result["meta"] = info.meta;
    return result;
}

inja::json toInjaJson(const PrintData& printData)
{
    inja::json result;
    result["header_filename_full"] = printData.header_filename_full;

    auto& classes = result["classes"] = inja::json::array();
    for (const auto& classData : printData.classes)
    {
        auto classJson = metaInfoToInja(classData);

        auto& fields = classJson["fields"] = inja::json::array();
        for (const auto& fieldData : classData.fields)
        {
            auto fieldJson = metaInfoToInja(fieldData);
            fieldJson["type"] = fieldData.type;
            fields.push_back(fieldJson);
        }

        classes.push_back(classJson);
    }

    auto& enums = result["enums"] = inja::json::array();
    for (const auto& enumData : printData.enums)
    {
        auto enumJson = metaInfoToInja(enumData);

        auto& enumerators = enumJson["enumerators"] = inja::json::array();
        for (const auto& enumeratorData : enumData.enumerators)
        {
            auto enumeratorJson = metaInfoToInja(enumeratorData);
            enumeratorJson["value"] = enumeratorData.value;
            enumerators.push_back(enumeratorJson);
        }

        enums.push_back(enumJson);
    }

    return result;
}

PrintData filterByCommand(const PrintData& printData, const std::string& command)
{
    PrintData result;
    result.header_filename_full = printData.header_filename_full;

    for (const auto& classData : printData.classes)
    {
        if (classData.command == command)
        {
            result.classes.push_back(classData);
        }
    }

    for (const auto& enumData : printData.enums)
    {
        if (enumData.command == command)
        {
            result.enums.push_back(enumData);
        }
    }

    return result;
}

} // namespace

void Module::print(PrintData&& printData, ContentManager& contentManager)
{
    auto commands = parseCustomCommands(cLine::Arg_custom_commands);
    if (commands.empty())
        return;

    for (const auto& command : commands)
    {
        auto& content = contentManager.getContentRef(command.extension);
        PrintData filteredData = filterByCommand(printData, command.command);

        if (!filteredData.classes.empty() || !filteredData.enums.empty())
        {
            std::string templateContent = readFileToString(command.templatePath);
            if (templateContent.empty())
            {
                rlog::error("Шаблон не найден: {}", command.templatePath);
                continue;
            }

            auto injaJson = toInjaJson(filteredData);
            content << inja::render(templateContent, injaJson);
        }
    }
}

} // namespace rgen::CustomModule

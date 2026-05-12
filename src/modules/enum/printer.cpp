#include "api.h"
#include "data.h"

#include <core/command_line_args.h>
#include <core/common_constants.h>
#include <core/template_loader.h>
#include <core/file.h>
#include <core/string_util.h>
#include <core/utils.h>

#include <fmt/core.h>
#include <inja/inja.hpp>

namespace rgen::Enum
{

namespace
{

inja::json getInjaJson(PrintData&& printData)
{
    inja::json injaData;
    injaData["header_filename_full"] = printData.header_filename_full;

    auto& injaEnums = injaData["enums"];
    injaEnums = inja::json::array();
    for (const auto& enumData : printData.enums)
    {
        inja::json injaEnumData;
        injaEnumData["name"] = enumData.name;

        auto& injaValues = injaEnumData["values"];
        for (const auto& valueData : enumData.values)
        {
            inja::json injaValueData;
            injaValueData["name"] = valueData.name;
            injaValueData["display"] = valueData.display;

            injaValues.push_back(injaValueData);
        }

        injaEnums.push_back(injaEnumData);
    }

    return injaData;
}

} // namespace

void Module::print(PrintData&& printData, ContentManager& contentManager)
{
    auto injaJson = getInjaJson(std::move(printData));
    // auto injaJson = inja::json::parse(rgen::toJson(printData).toStdString());

    const auto templateContent = loadTemplate(TEMPLATE_FILE_NAME_CXX, RGEN_TEMPLATE_DIR);

    utils::injaJsonDump(injaJson);

    contentManager.getContentRef(FILE_EXTENSION_RGEN_CXX) << inja::render(templateContent, injaJson);
}

} // namespace rgen::Enum

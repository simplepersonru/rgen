#include "api.h"

#include <core/command_line_args.h>
#include <core/common_constants.h>
#include <core/template_loader.h>
#include <core/file.h>
#include <core/string_util.h>
#include <core/utils.h>

#include <fmt/core.h>
#include <inja/inja.hpp>

using namespace fmt::literals;
using json = nlohmann::json;

namespace rgen::JsonSchema
{

namespace
{

inja::json getInjaJson(PrintData&& printData)
{
    inja::json injaData;
    injaData["header_filename_full"] = printData.header_filename_full;

    auto& injaClasses = injaData["classes"];
    injaClasses = inja::json::array();
    for (const auto& classData : printData.classes)
    {
        inja::json injaClassData;
        injaClassData["nameWithNamespace"] = classData.nameWithNamespace;
        injaClassData["jsonSchema"] = classData.jsonSchema;

        injaClasses.push_back(injaClassData);
    }

    return injaData;
}
} // namespace

void Module::print(PrintData&& printData, ContentManager& contentManager)
{
    auto injaJson = getInjaJson(std::move(printData));
    const auto templateContent = loadTemplate(TEMPLATE_FILE_NAME_CXX, RGEN_TEMPLATE_DIR);

    utils::injaJsonDump(injaJson);

    contentManager.getContentRef(FILE_EXTENSION_RGEN_CXX) << inja::render(templateContent, injaJson);
}

} // namespace rgen::JsonSchema

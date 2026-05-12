#include "api.h"

#include <clang/AST/ASTContext.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/PrettyPrinter.h>

#include <inja/inja.hpp>

#include <core/asthelper.h>
#include <core/command_line_args.h>
#include <core/common_constants.h>
#include <core/file.h>
#include <core/logger.h>
#include <core/string_util.h>
#include <core/template_loader.h>
#include <core/utils.h>

using json = nlohmann::json;

namespace rgen::JsonNS
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
        injaClassData["name"] = classData.name;
        injaClassData["namespace_"] = classData.namespace_;

        auto& injaFields = injaClassData["fields"];
        injaFields = inja::json::array();
        for (const auto& fieldData : classData.fields)
        {
            inja::json injaFieldData;
            injaFieldData["name"] = fieldData.name;
            injaFieldData["key"] = fieldData.key;

            injaFields.push_back(injaFieldData);
        }

        injaClasses.push_back(injaClassData);
    }

    return injaData;
}

} // namespace

void Module::print(PrintData&& printData, ContentManager& contentManager)
{
    auto& contentHeaderRef = contentManager.getContentRef(FILE_EXTENSION_RGEN_HXX);
    auto injaJson = getInjaJson(std::move(printData));
    injaJson["header_filename_full_hxx"] = contentHeaderRef.getGenerationFile().string();

    const auto templateContent_hxx = loadTemplate(TEMPLATE_FILE_NAME_HXX, RGEN_TEMPLATE_DIR);
    const auto templateContent_cxx = loadTemplate(TEMPLATE_FILE_NAME_CXX, RGEN_TEMPLATE_DIR);

    utils::injaJsonDump(injaJson);

    printInjaContent(contentHeaderRef, injaJson, templateContent_hxx);
    printInjaContent(contentManager.getContentRef(FILE_EXTENSION_RGEN_CXX), injaJson, templateContent_cxx);
}

} // namespace rgen::JsonNS

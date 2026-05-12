#include "api.h"

#include <clang/AST/ASTContext.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/PrettyPrinter.h>

#include <inja/inja.hpp>

#include <core/asthelper.h>
#include <core/command_line_args.h>
#include <core/common_constants.h>
#include <core/template_loader.h>
#include <core/file.h>
#include <core/string_util.h>
#include <core/utils.h>
#include <core/derive_convert.h>

using json = nlohmann::json;

namespace rgen::OperatorQStream
{

void Module::print(rgen::DerivePrintData&& printData, ContentManager& contentManager)
{
    auto injaJson = rgen::getInjaJson(std::move(printData));
    const auto templateContent = loadTemplate(TEMPLATE_FILE_NAME_CXX, RGEN_TEMPLATE_DIR);

    utils::injaJsonDump(injaJson);

    contentManager.getContentRef(FILE_EXTENSION_RGEN_CXX) << inja::render(templateContent, injaJson);
}

} // namespace rgen::OperatorQStream

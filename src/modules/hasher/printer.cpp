#include "api.h"
#include "core/derive_convert.h"
#include <inja/inja.hpp>
#include <core/common_constants.h>
#include <core/template_loader.h>
#include <core/utils.h>

using json = nlohmann::json;

namespace rgen::Hasher
{

void Module::print(rgen::DerivePrintData&& printData, ContentManager& contentManager)
{
    auto injaJson = rgen::getInjaJson(std::move(printData));
    const auto templateContent = loadTemplate(TEMPLATE_FILE_NAME_HXX, RGEN_TEMPLATE_DIR);

    utils::injaJsonDump(injaJson);

    contentManager.getContentRef(FILE_EXTENSION_RGEN_HXX) << inja::render(templateContent, injaJson);
}

} // namespace rgen::Hasher

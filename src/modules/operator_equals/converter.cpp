#include "api.h"
#include <core/asthelper.h>
#include <core/derive_convert.h>
#include <core/inheritance.h>
#include <core/utils.h>

namespace rgen::OperatorEquals
{

std::optional<rgen::DerivePrintData> Module::convert(const ExtractedData& extractedData, clang::ASTContext& astContext)
{
    auto commonResult = rgen::convertDerive(extractedData, astContext, COMMAND_OPERATOR_EQUALS);

    if (!commonResult)
    {
        return std::nullopt;
    }

    return commonResult;
}
} // namespace rgen::OperatorEquals

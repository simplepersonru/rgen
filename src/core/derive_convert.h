#pragma once

#include "derive_data.h"
#include <core/asthelper.h>
#include <core/extracteddata.h>
#include <core/inheritance.h>
#include <core/utils.h>
#include <optional>

namespace rgen
{

void fillDeriveClassData(DerivePrintData& printData, clang::ASTContext& astContext, const clang::CXXRecordDecl* decl, const clang::TypeAliasDecl* usingDecl = nullptr);

std::optional<DerivePrintData> convertDerive(const ExtractedData& extractedData, clang::ASTContext& astContext, const std::string& command);

inja::json getInjaJson(DerivePrintData&& printData);

} // namespace rgen

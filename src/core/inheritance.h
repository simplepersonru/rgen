#pragma once

#include <clang/AST/DeclCXX.h>
#include <core/utils.h>
#include <string>
#include <vector>

namespace rgen::utils
{

/// @JsonSerializerV2
struct BaseClassInfo
{
    std::string name;
    std::string nameWithNamespace;
};

std::vector<BaseClassInfo> extractBaseClassInfo(const clang::CXXRecordDecl* decl);

} // namespace rgen::utils

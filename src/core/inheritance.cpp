#include "inheritance.h"

namespace rgen::utils
{

std::vector<BaseClassInfo> extractBaseClassInfo(const clang::CXXRecordDecl* decl)
{
    std::vector<BaseClassInfo> bases;

    for (const auto& base : decl->bases())
    {
        const clang::Type* baseType = base.getType().getTypePtrOrNull();
        if (!baseType)
        {
            continue;
        }

        const clang::CXXRecordDecl* baseRecord = baseType->getAsCXXRecordDecl();
        if (!baseRecord || !baseRecord->isCompleteDefinition())
        {
            continue;
        }

        BaseClassInfo baseData;
        baseData.name = baseRecord->getNameAsString();

        std::string baseNs = collectNamespacesFromDeclToString(baseRecord->getDeclContext());
        if (!baseNs.empty())
        {
            baseData.nameWithNamespace = prefixNamespace(baseNs, baseData.name);
        }
        else
        {
            baseData.nameWithNamespace = baseData.name;
        }

        bases.push_back(std::move(baseData));
    }

    return bases;
}

} // namespace rgen::utils

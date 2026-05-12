#include "api.h"

#include <core/asthelper.h>
#include <core/comment.h>

namespace rgen::Enum
{

namespace
{

EnumData enumeratorDecls(clang::ASTContext& astContext, const clang::EnumDecl* enumDecl)
{
    EnumData enumData;
    helper::ASTHelper astHelper(&astContext);

    for (auto* enumeratorDecl : enumDecl->enumerators())
    {
        if (astHelper.hasIgnoreAttribute(enumeratorDecl))
            continue;
        ValueData valueData;
        valueData.display = astHelper.displayContent(enumeratorDecl);
        valueData.name = enumeratorDecl->getNameAsString();
        if (valueData.display.empty())
        {
            valueData.display = valueData.name;
        }
        enumData.values.push_back(std::move(valueData));
    }
    return enumData;
}

} // namespace

std::optional<PrintData> Module::convert(const ExtractedData& extractedData, clang::ASTContext& astContext)
{
    auto it = extractedData.commandsMap.find(COMMAND_ENUM);
    if (it == extractedData.commandsMap.end())
    {
        return std::nullopt;
    }

    PrintData printData;
    printData.header_filename_full = extractedData.fullHeaderPath.string();

    const auto& declList = it->second;

    for (auto decl : declList)
    {
        if (auto enumDecl = clang::dyn_cast<clang::EnumDecl>(decl))
        {
            EnumData enumData = enumeratorDecls(astContext, enumDecl);
            enumData.name = enumDecl->getQualifiedNameAsString();
            printData.enums.push_back(std::move(enumData));
        }
        else if (auto usingDecl = clang::dyn_cast<clang::TypeAliasDecl>(decl))
        {
            if (auto* enumType = usingDecl->getUnderlyingType()->getAs<clang::EnumType>())
            {
                EnumData enumData = enumeratorDecls(astContext, enumType->getDecl());
                enumData.name = usingDecl->getQualifiedNameAsString();
                printData.enums.push_back(std::move(enumData));
            }
        }
    }

    if (printData.enums.empty())
    {
        return std::nullopt;
    }

    return printData;
}
} // namespace rgen::Enum

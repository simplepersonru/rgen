#include "derive_convert.h"

namespace rgen
{

void fillDeriveClassData(DerivePrintData& printData, clang::ASTContext& astContext, const clang::CXXRecordDecl* decl, const clang::TypeAliasDecl* usingDecl)
{
    helper::ASTHelper astHelper(&astContext);

    // Прерываем разбор для Forward Declaration
    if (!decl->isCompleteDefinition())
        return;

    DeriveClassDataWithFields classData;

    // Получение имени структуры
    const std::string settingName = usingDecl ? usingDecl->getNameAsString()
                                              : decl->getNameAsString();

    classData.name = settingName;
    std::string nameWithNamespace = settingName;

    // Определение namespace
    const std::string nsOneString = usingDecl
    ? utils::collectNamespacesFromDeclToString(usingDecl->getDeclContext())
    : utils::collectNamespacesFromDeclToString(decl->getDeclContext());

    if (!nsOneString.empty())
    {
        classData.namespace_ = nsOneString;
        nameWithNamespace = utils::prefixNamespace(nsOneString, settingName);
    }

    classData.nameWithNamespace = nameWithNamespace;

    // Сбор базовых классов
    classData.bases = utils::extractBaseClassInfo(decl);

    // Сбор полей
    for (const auto field : decl->fields())
    {
        if (astHelper.hasIgnoreAttribute(field))
            continue;

        DeriveFieldData fieldData;
        fieldData.name = field->getNameAsString();
        classData.fields.push_back(std::move(fieldData));
    }

    // Если нет полей и базовых классов - возвращаем false
    if (classData.fields.empty() && classData.bases.empty())
    {
        return;
    }

    printData.classes.push_back(std::move(classData));
    return;
}

std::optional<DerivePrintData> convertDerive(const ExtractedData& extractedData, clang::ASTContext& astContext, const std::string& command)
{
    auto it = extractedData.commandsMap.find(command);
    if (it == extractedData.commandsMap.end())
    {
        return std::nullopt;
    }

    DerivePrintData result;
    result.header_filename_full = extractedData.fullHeaderPath.string();

    // Обработка declarations
    const auto& declList = it->second;
    for (auto decl : declList)
    {
        if (auto cxxRecordDecl = clang::dyn_cast<clang::CXXRecordDecl>(decl))
        {
            fillDeriveClassData(result, astContext, cxxRecordDecl);
        }
        else if (auto usingDecl = clang::dyn_cast<clang::TypeAliasDecl>(decl))
        {
            const clang::CXXRecordDecl* baseRecordDecl =
            helper::getBaseRecordDeclInUsingTemplate(usingDecl);
            if (baseRecordDecl)
            {
                fillDeriveClassData(result, astContext, baseRecordDecl, usingDecl);
            }
        }
    }

    if (result.classes.empty())
    {
        return std::nullopt;
    }

    return result;
}

inja::json getInjaJson(DerivePrintData &&printData)
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

            injaFields.push_back(injaFieldData);
        }

        injaClasses.push_back(injaClassData);
    }

    return injaData;
}

} // namespace rgen

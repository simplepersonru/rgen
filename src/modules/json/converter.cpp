#include "api.h"
#include <core/asthelper.h>
#include <core/comment.h>
#include <core/inheritance.h>
#include <core/logger.h>
#include <core/utils.h>

#include <vector>

namespace rgen::JsonNS
{

std::optional<PrintData> Module::convert(const ExtractedData& extractedData, clang::ASTContext& astContext)
{
    PrintData printData;

    printData.header_filename_full = extractedData.fullHeaderPath.string();

    std::vector<std::string> commands = { COMMAND_JSON_SERIALIZER_V2, COMMAND_JSON };

    for (const auto& command : commands)
    {
        auto it = extractedData.commandsMap.find(command);
        if (it == extractedData.commandsMap.end())
            continue;

        const auto& declList = it->second;

        for (auto decl : declList)
        {
            const clang::CXXRecordDecl* recordDecl = nullptr;
            const clang::TypeAliasDecl* usingDecl = nullptr;

            if (auto cxxRecordDecl = clang::dyn_cast<clang::CXXRecordDecl>(decl))
            {
                recordDecl = cxxRecordDecl;
            }
            else if (auto typeAliasDecl = clang::dyn_cast<clang::TypeAliasDecl>(decl))
            {
                usingDecl = typeAliasDecl;
                recordDecl = helper::getBaseRecordDeclInUsingTemplate(typeAliasDecl);
                if (!recordDecl)
                    continue;
            }
            else
            {
                continue;
            }

            if (!recordDecl || !recordDecl->isCompleteDefinition())
            {
                if (usingDecl)
                {
                    rlog::error(recordDecl, "Template with alias \"{}\" is not initialized",
                                recordDecl->getNameAsString());
                }
                continue;
            }

            ClassData classData;
            helper::ASTHelper astHelper(&astContext);

            // Заполняем общие поля через общую функцию
            const std::string settingName = usingDecl ? usingDecl->getNameAsString()
                                                      : recordDecl->getNameAsString();
            classData.name = settingName;

            const std::string nsOneString = usingDecl
            ? utils::collectNamespacesFromDeclToString(usingDecl->getDeclContext())
            : utils::collectNamespacesFromDeclToString(recordDecl->getDeclContext());

            if (!nsOneString.empty())
            {
                classData.namespace_ = nsOneString;
                classData.nameWithNamespace = utils::prefixNamespace(nsOneString, settingName);
            }
            else
            {
                classData.nameWithNamespace = settingName;
            }

            classData.bases = utils::extractBaseClassInfo(recordDecl);

            // Сбор полей
            for (const auto field : recordDecl->fields())
            {
                auto val = declJson(*field, COMMAND_JSON);

                if ((val.json.contains("ignore") && val.json["ignore"].get<bool>()) || astHelper.hasIgnoreAttribute(field))
                    continue;

                FieldData fieldData;
                fieldData.name = field->getNameAsString();
                fieldData.key = val.json.contains("key") ? val.json["key"].get<std::string>() : fieldData.name;

                classData.fields.push_back(std::move(fieldData));
            }

            printData.classes.push_back(std::move(classData));
        }
    }

    if (printData.classes.empty())
        return std::nullopt;

    return printData;
}

} // namespace rgen::JsonNS

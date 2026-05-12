#include "api.h"
#include "data.h"
#include <clang/AST/DeclCXX.h>
#include <core/command_line_args.h>
#include <core/comment.h>
#include <core/logger.h>
#include <core/utils.h>
#include <set>

namespace rgen::CustomModule
{

void fillBaseInfo(MetaInfo& metaInfo, const clang::NamedDecl* decl, const std::string& command, const nlohmann::json& meta = {})
{
    auto nameInfo = rgen::utils::getNameInfo(decl);
    metaInfo.name = nameInfo.name;
    metaInfo.fullName = nameInfo.id;
    metaInfo.namespace_ = nameInfo.namespace_;
    metaInfo.command = command;

    if (!meta.empty())
    {
        metaInfo.meta = meta;
        return;
    }

    auto comments = parseDeclComments(decl->getASTContext(), *decl, true);
    auto metaJson = comments.at_json("Meta");
    if (metaJson)
    {
        metaInfo.meta = metaJson.json;
    }
}

void fillClassData(PrintData& printData, const clang::CXXRecordDecl* definition, const std::string& command, const nlohmann::json& classMeta)
{
    ClassData classData;
    fillBaseInfo(classData, definition, command, classMeta);

    for (auto* field : definition->fields())
    {
        FieldData fieldData;
        fillBaseInfo(fieldData, field, command);
        fieldData.type = field->getType().getAsString();
        classData.fields.push_back(fieldData);
    }

    printData.classes.push_back(classData);
}

void fillEnumData(PrintData& printData, const clang::EnumDecl* enumDecl, const std::string& command)
{
    EnumData enumData;
    fillBaseInfo(enumData, enumDecl, command);

    for (auto* enumerator : enumDecl->enumerators())
    {
        EnumeratorData enumeratorData;
        fillBaseInfo(enumeratorData, enumerator, command);
        enumeratorData.value = std::to_string(enumerator->getInitVal().getExtValue());
        enumData.enumerators.push_back(enumeratorData);
    }

    printData.enums.push_back(enumData);
}

std::optional<PrintData> Module::convert(const ExtractedData& extractedData, clang::ASTContext& astContext)
{
    auto commands = parseCustomCommands(cLine::Arg_custom_commands);
    if (commands.empty())
        return std::nullopt;

    PrintData result;
    result.header_filename_full = extractedData.fullHeaderPath.string();

    std::set<const clang::CXXRecordDecl*> processedClasses;

    for (const auto& command : commands)
    {
        auto it = extractedData.commandsMap.find(command.command);
        if (it == extractedData.commandsMap.end())
            continue;

        for (auto* decl : it->second)
        {
            if (auto* record = llvm::dyn_cast_or_null<clang::CXXRecordDecl>(decl))
            {
                auto* canonical = record->getCanonicalDecl();
                auto* definition = canonical->getDefinition();
                if (!definition)
                    continue;

                if (processedClasses.count(canonical) > 0)
                    continue;
                processedClasses.insert(canonical);

                nlohmann::json classMeta;
                for (auto* redecl : canonical->redecls())
                {
                    auto comments = parseDeclComments(redecl->getASTContext(), *redecl, true);
                    if (auto meta = comments.at_json("Meta"))
                    {
                        classMeta = meta.json;
                        break;
                    }
                }

                fillClassData(result, definition, command.command, classMeta);
            }

            if (auto* enumDecl = llvm::dyn_cast_or_null<clang::EnumDecl>(decl))
            {
                if (enumDecl->isCompleteDefinition())
                {
                    fillEnumData(result, enumDecl, command.command);
                }
            }
        }
    }

    if (result.classes.empty() && result.enums.empty())
        return std::nullopt;

    return result;
}

} // namespace rgen::CustomModule

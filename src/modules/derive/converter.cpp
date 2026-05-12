#include "api.h"

#include <core/asthelper.h>
#include <core/comment.h>
#include <core/derive_convert.h>
#include <core/logger.h>
#include <core/ppcallbacks.h>
#include <core/utils.h>

namespace rgen::Derive
{

void addExtractedData(ExtractedData& exData, const std::vector<std::string>& args, const clang::Decl* decl)
{
    // Добавляем в declList при совпадении имён
    // Макрос DERIVE не позволит обратиться к невалидному имени модуля,
    // а значит обработка не дошла бы до этого этапа, если бы у нас были ошибки
    // в объявлении.
    exData.declList.push_back(decl);

    // Ищем, какой doxygen-комментарий соответсвует этому модулю
    for (const auto& deriveModule : Module::getDeriveModules())
    {
        auto itComm = std::find(args.begin(), args.end(), deriveModule.deriveName);
        if (itComm == args.end())
            continue;
        exData.commandsMap[deriveModule.comment].push_back(decl);
    }
}

ExtractedData Module::convert(const Context& ctx)
{
    ExtractedData exData = ctx.extractedData;
    exData.declList.clear();
    exData.commandsMap.clear();

    // Итерируемся по всем макросам, которые есть в файле
    for (const auto& macro : ctx.ppcallbacks->getMacroses())
    {
        auto [macroName, macroArgs] = macro;
        // Ищем только DERIVE
        if (macroName != COMMAND_DERIVE_MACROS)
            continue;

        // Там должно быть минимум 3 аргумента: префикс, тип и хотя бы один модуль derive
        if (macroArgs.size() < 2)
        {
            rlog::warn("No arguments at {} macro", COMMAND_DERIVE_MACROS);
            return {};
        }
        else if (macroArgs.size() == 2)
        {
            rlog::warn("No modules added at {} macro for class \"{}\"", COMMAND_DERIVE_MACROS, macroArgs.at(0));
            return {};
        }

        // Получаем decl-ы с @Derive
        auto it = ctx.extractedData.commandsMap.find(COMMAND_DERIVE);
        if (it == ctx.extractedData.commandsMap.end())
        {
            rlog::warn("Found macro {}, but no comments with @{}", COMMAND_DERIVE_MACROS, COMMAND_DERIVE);
            return {};
        }

        const auto& declList = it->second;

        // Проходимся по всем decl-ам и ищем совпадающим с типом из макроса
        for (auto decl : declList)
        {
            DeriveClassDataWithFields classData;

            if (auto cxxRecordDecl = clang::dyn_cast<clang::CXXRecordDecl>(decl))
            {
                DerivePrintData tempData;
                rgen::fillDeriveClassData(tempData, ctx.astContext, cxxRecordDecl);
                if (!tempData.classes.empty())
                {
                    classData = std::move(tempData.classes[0]);
                }
            }
            else if (auto usingDecl = clang::dyn_cast<clang::TypeAliasDecl>(decl))
            {
                const clang::CXXRecordDecl* baseRecordDecl = helper::getBaseRecordDeclInUsingTemplate(usingDecl);
                if (baseRecordDecl)
                {
                    DerivePrintData tempData;
                    rgen::fillDeriveClassData(tempData, ctx.astContext, baseRecordDecl, usingDecl);
                    if (!tempData.classes.empty())
                    {
                        classData = std::move(tempData.classes[0]);
                    }
                }
            }

            if (classData.name.empty())
                continue;

            // Берём тип из макроса
            const std::string declNameMacros = macroArgs.at(1);
            if (classData.nameWithNamespace == declNameMacros)
                addExtractedData(exData, macroArgs, decl);
        }
    }
    return exData;
}

} // namespace rgen::Derive

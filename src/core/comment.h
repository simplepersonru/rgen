#pragma once

#include "core/string_util.h"
#include <hjson.h>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace clang
{
class ASTContext;
class Decl;
} // namespace clang

namespace rgen
{

std::string prepareJsonToParse(std::string inputHjson);

struct DeclComments : public std::map<std::string, std::string>
{
    /// На целевой системе в std версия ниже чем надо
    bool Contains(const key_type& key) const
    {
        auto itr = find(key);
        return itr != end();
    }

    enum JsonExitCode
    {
        Success,
        KeyNotFound,
        EmptyStringValue,
        InvalidHjson
    };

    struct JsonResult
    {
        nlohmann::json json;
        JsonExitCode code = JsonExitCode::Success;
        explicit operator bool() const { return code == JsonExitCode::Success; }
    };

    JsonResult at_json(const key_type& key) const
    {
        const auto itr = find(key);
        if (itr == end())
            return { {}, JsonExitCode::KeyNotFound };

        auto text = itr->second;
        if (text.empty())
            return { {}, JsonExitCode::EmptyStringValue };

        text = prepareJsonToParse(text);

        try
        {
            const Hjson::Value value = Hjson::Unmarshal(text);

            // Настраиваем вывод, похожий на компактный json
            Hjson::EncoderOptions opt;
            opt.eol = "";
            opt.comments = false;
            opt.quoteAlways = true;
            opt.quoteKeys = true;
            opt.unknownAsNull = true;
            opt.separator = true;
            opt.preserveInsertionOrder = false;

            const std::string jsonString = Hjson::Marshal(value, opt);
            return { nlohmann::json::parse(jsonString), JsonExitCode::Success };
        }
        catch (std::exception& ex)
        {
            return { {}, JsonExitCode::InvalidHjson };
        }
    }

    std::optional<value_type> at_optional(const key_type& key) const
    {
        auto itr = find(key);
        if (itr != end())
            return *itr;
        else
            return std::nullopt;
    }
};

/*!
 * \brief Возвращает комменты, привязанные к декларации
 * \param decl - декларация, у которой собираются комменты
 *
 * В clang с аргументами-fparse-all-comments", "-fcomment-block-commands=serializable,mycmd, ...",
 * работает парсинг комментариев, причем все что передано аргументами в fcomment-block-commands
 * воспринимается как ключи и можно считать инфу по этим ключам
 */
DeclComments parseDeclComments(const clang::ASTContext& astContext, const clang::Decl& decl, bool useUncached = false);

DeclComments::JsonResult declJson(const clang::Decl& decl, const std::string& key);

} // namespace rgen

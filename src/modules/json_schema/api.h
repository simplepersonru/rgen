#pragma once

#include "data.h"

#include <clang/AST/Decl.h>
#include <core/common_constants.h>
#include <core/extracteddata.h>
#include <core/registry.h>

namespace rgen::JsonSchema
{

inline const std::string COMMAND_JSON_SCHEMA = "JsonSchema";
inline const std::string TEMPLATE_FILE_NAME_CXX = "json_schema.cxx.j2";

struct Module
{
    static ModuleInfo getModuleInfo();
    static std::optional<PrintData> convert(const ExtractedData& extractedData, clang::ASTContext& astContext);
    static void print(PrintData&& printData, ContentManager& contentManager);
    static void process(Context& ctx);
};

/*!
 * \brief Информация для дальнейшего разбора и печати
 */
struct JsonSchemaData
{
    const clang::RecordDecl* Decl;        // Декларация разбираемого класса/структуры
    std::filesystem::path fullHeaderPath; // Полный путь к файлу, в котором был обнаружен тег
};

using JsonSchemaDatas = std::vector<JsonSchemaData>;

} // namespace rgen::JsonSchema

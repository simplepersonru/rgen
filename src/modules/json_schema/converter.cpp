#include "api.h"

#include <deque>
#include <map>

#include <core/asthelper.h>
#include <core/command_line_args.h>
#include <core/string_util.h>
#include <core/utils.h>

#include <fmt/core.h>
#include <inja/inja.hpp>

namespace rgen::JsonSchema
{

namespace
{

const std::string_view SCHEMA_VERSION = "https://json-schema.org/draft/2020-12/schema";

const std::string_view R_SCHEMA = "$schema";
const std::string_view R_ID = "$id";
const std::string_view R_REF = "$ref";

const std::string_view J_PROPERTIES = "properties";
const std::string_view J_TYPE = "type";
const std::string_view J_TITLE = "title";
const std::string_view J_DESCRIPTION = "description";
const std::string_view J_CONTAINS = "contains";
const std::string_view J_ITEMS = "items";
const std::string_view J_ENUM = "enum";
const std::string_view J_ADD_PROP = "additionalProperties";
const std::string_view J_REQUIRED = "required";

const std::string_view TYPE_BOOLEAN = "boolean";
const std::string_view TYPE_INTEGER = "integer";
const std::string_view TYPE_NUMBER = "number";
const std::string_view TYPE_STRING = "string";
const std::string_view TYPE_ARRAY = "array";
const std::string_view TYPE_OBJECT = "object";
const std::string_view TYPE_NULL = "null";

const std::string_view LIMIT_MAX = "maximum";
const std::string_view LIMIT_MIN = "minimum";

using namespace fmt::literals;
using json = nlohmann::json;

class Converter
{
public:
    explicit Converter(clang::ASTContext& astContext, const ExtractedData& extractedData) :
        m_astHelper(&astContext), m_extractedData { extractedData }
    {}

    PrintData getPrintData();

private:
    /*!
     * \brief Сборка шаблона и печать результата
     * \details Документация по работе с INJA: https://github.com/pantor/inja
     */
    void buildAndPrint();

    /*!
     * \brief содержимое jsonSchema для декларации decl
     */
    std::string jsonSchemaContent(const clang::RecordDecl* decl);

    /*!
     * \brief Разобрать класс или структуру
     */
    void fillRecordDeclCollect(const clang::RecordDecl* decl, json& jsonObject, bool needHeader = false);

    /*!
     * \brief Заполнить ограничение типов перечисления
     */
    void fillEnumeralLimits(const clang::QualType& type, json& j);

    /*!
     * \brief Заполнить лимиты конкретных типов чисел
     */
    void fillNumericLimits(const clang::QualType& type, json& j);

    /*!
     * \brief Получить тип соответствующий Json Schema
     */
    std::string_view getTypeJsonSchema(clang::QualType type);

    /*!
     * \brief Получить и заполнить комментарий для поля
     */
    void fillComment(const clang::Decl* decl, json& jsonObject);

    /*!
     * \brief Заполнить шаблонный параметр типа
     */
    void fillTemplateContainer(clang::QualType templType, json& jsonObject);

    /*!
     * \brief Получить текущее расположение структуры в дереве json
     */
    std::string getCurrentPath();

    void fillEnumTypeAndLimits(nlohmann::json& jsRef, clang::QualType type);

private:
    const ExtractedData& m_extractedData;
    helper::ASTHelper m_astHelper;

    std::deque<std::string> m_pathStack;                   // список для получения текущего пути класса
    std::map<int64_t, std::string> m_classesRef;           // соотношение класса и где он определен в json
    std::map<int64_t, const clang::RecordDecl*> m_classes; // соотношение id скласса и его декларации
};

PrintData Converter::getPrintData()
{
    PrintData printData;
    printData.header_filename_full = m_extractedData.fullHeaderPath.string();

    const auto& declList = m_extractedData.commandsMap.find(COMMAND_JSON_SCHEMA)->second;

    for (auto* declPtr : declList)
    {
        if (auto cxxRecordDecl = clang::dyn_cast<clang::CXXRecordDecl>(declPtr))
        {
            auto& decl = *cxxRecordDecl;
            ClassData classData;

            // Получение имени структуры
            const std::string nsOneString = utils::collectNamespacesFromDeclToString(decl.getDeclContext());

            classData.nameWithNamespace = utils::prefixNamespace(nsOneString, decl.getNameAsString());
            classData.jsonSchema = jsonSchemaContent(&decl);

            m_classes[decl.getID()] = &decl;
            printData.classes.push_back(std::move(classData));
        }
    }

    return printData;
}

std::string Converter::jsonSchemaContent(const clang::RecordDecl* decl)
{
    json js;
    fillRecordDeclCollect(decl, js, true);
    return js.dump(2);
}

void Converter::fillRecordDeclCollect(const clang::RecordDecl* decl, json& jsonObject, bool needHeader)
{
    // Требуется только для шапки структуры
    if (needHeader)
    {
        // Штуки, которые нужны лишь в главной структуре3
        jsonObject[R_SCHEMA] = SCHEMA_VERSION;
        jsonObject[J_TITLE] = decl->getNameAsString();

        // Заполняем комментарий, что находится в шапке корневой структуры
        fillComment(decl, jsonObject);
    }

    // Производим дальнейший разбор только если это пользовательская структура
    if (!m_astHelper.isStruct(helper::getQualTypeFromDecl(decl)))
    {
        jsonObject[J_TYPE] = getTypeJsonSchema(helper::getQualTypeFromDecl(decl));
        return;
    }

    // Проверяем, не разбирался ли класс до этого
    const auto declId = decl->getID();

    if (m_classesRef.find(declId) != m_classesRef.end())
    {
        // Если разбирался, ставим на него ссылку и уходим
        jsonObject[R_REF] = fmt::format("#{}", m_classesRef.at(declId));
        return;
    }

    m_classesRef[declId] = getCurrentPath();

    jsonObject[J_TYPE] = getTypeJsonSchema(helper::getQualTypeFromDecl(decl));
    jsonObject[J_ADD_PROP] = false;

    m_pathStack.push_back(std::string(J_PROPERTIES));
    for (clang::FieldDecl* field : decl->fields())
    {
        // Игнорируем приватные поля
        if (field->getAccess() == clang::AS_private) // ??? ..Access() != clang::AS_public
            continue;

        jsonObject[J_REQUIRED].push_back(field->getNameAsString());

        json& jsonFieldRef = jsonObject[J_PROPERTIES][field->getNameAsString()];
        fillComment(field, jsonFieldRef);

        m_pathStack.push_back(field->getNameAsString());

        // Заполняем тип, связанный с шаблонным контейнером (например, вектор или лист)
        if (m_astHelper.isTemplatedContainer(field->getType()))
        {
            auto templatedType = helper::getTemplatedType(field->getType());

            fillTemplateContainer(templatedType, jsonFieldRef);

            jsonFieldRef[J_TYPE] = getTypeJsonSchema(field->getType());

            m_pathStack.pop_back(); // fieldName
            continue;
        }

        // Заполняем тип, связанный с массивом (т.е. без шаблонного параметра) (например, QByteArray)
        if (m_astHelper.isArray(field->getType()))
        {
            const clang::CXXRecordDecl* recordDecl = helper::getCXXRecordDeclFromQualType(field->getType());
            if (!recordDecl)
                continue;

            for (const auto* method : recordDecl->methods())
            {
                if (method->getNameAsString() != "data")
                    continue;

                clang::QualType returnType = method->getReturnType();
                fillTemplateContainer(returnType, jsonFieldRef);

                jsonFieldRef[J_TYPE] = getTypeJsonSchema(field->getType());
                break;
            }

            m_pathStack.pop_back(); // fieldName
            continue;
        }

        // Если поле является std::optional
        if (helper::isStdOptional(field->getType()))
        {
            auto templatedType = helper::getTemplatedType(field->getType());

            fillTemplateContainer(templatedType, jsonFieldRef);

            jsonFieldRef[J_TYPE].push_back(getTypeJsonSchema(templatedType));
            jsonFieldRef[J_TYPE].push_back(TYPE_NULL);

            m_pathStack.pop_back(); // fieldName
            continue;
        }

        // Заполняем тип, являющийся перечислением
        if (field->getType()->isEnumeralType())
        {
            fillEnumTypeAndLimits(jsonFieldRef, field->getType());

            m_pathStack.pop_back(); // fieldName
            continue;
        }

        // Заполняем тип, связанный с числом
        if (field->getType()->isIntegerType() || field->getType()->isEnumeralType() || field->getType()->isFloatingType())
        {
            jsonFieldRef[J_TYPE] = getTypeJsonSchema(field->getType());

            fillNumericLimits(field->getType(), jsonFieldRef);
            m_pathStack.pop_back(); // fieldName
            continue;
        }

        // Если это класс или структура, раскручиваем его дальше
        if (const auto recordDecl = helper::getRecordDeclFromQualType(field->getType()))
        {
            fillRecordDeclCollect(recordDecl, jsonFieldRef);

            m_pathStack.pop_back(); // fieldName
            continue;
        }

        // Если не была пройдена ни одна проверка выше
        m_pathStack.pop_back(); // fieldName
    }
    m_pathStack.pop_back(); // J_PROPERTIES
}

std::string_view Converter::getTypeJsonSchema(clang::QualType type)
{
    while (type->isPointerType())
        type = type->getPointeeType();

    if (type->isBooleanType())
        return TYPE_BOOLEAN;
    if (type->isIntegerType() || type->isEnumeralType())
        return TYPE_INTEGER;
    if (type->isFloatingType())
        return TYPE_NUMBER;
    if (m_astHelper.isQString(type))
        return TYPE_STRING;
    if (m_astHelper.relatedToJsonArray(type) || type->isArrayType())
        return TYPE_ARRAY;
    if (type->isStructureOrClassType() /*|| helper::isStdOptional(type)*/)
        return TYPE_OBJECT;

    return TYPE_NULL;
}

void Converter::fillEnumeralLimits(const clang::QualType& type, json& j)
{
    if (!type->isEnumeralType())
        return;

    const clang::Type* pType = type.getTypePtr();
    if (!pType)
        return;
    const clang::EnumType* enumType = pType->getAs<clang::EnumType>();
    if (!enumType)
        return;
    const clang::EnumDecl* enumDecl = enumType->getDecl();
    if (!enumDecl)
        return;

    for (const auto en : enumDecl->enumerators())
    {
        j[J_ENUM].push_back(en->getInitVal().getExtValue());
        j[J_ENUM].push_back(fmt::format("{}", en->getInitVal().getExtValue()));
    }
}

void Converter::fillNumericLimits(const clang::QualType& type, json& j)
{
    // bool так же проходит по условию isIntegerType, так что его исключаем
    if (!type->isIntegerType() || type->isBooleanType())
        return;

    if (type->isUnsignedIntegerType())
        j[LIMIT_MIN] = 0;

    const auto intWidth = m_astHelper.astContext->getIntWidth(type);
    if (intWidth == 8)
    {
        if (type->isUnsignedIntegerType())
        {
            j[LIMIT_MAX] = std::numeric_limits<uint8_t>::max();
        }
        else
        {
            j[LIMIT_MIN] = std::numeric_limits<int8_t>::min();
            j[LIMIT_MAX] = std::numeric_limits<int8_t>::max();
        }
    }
    else if (intWidth == 16)
    {
        if (type->isUnsignedIntegerType())
        {
            j[LIMIT_MAX] = std::numeric_limits<uint16_t>::max();
        }
        else
        {
            j[LIMIT_MIN] = std::numeric_limits<int16_t>::min();
            j[LIMIT_MAX] = std::numeric_limits<int16_t>::max();
        }
    }
}

void Converter::fillComment(const clang::Decl* decl, json& jsonObject)
{
    auto declComment = m_astHelper.displayContent(decl);
    if (!declComment.empty())
        jsonObject[J_DESCRIPTION] = declComment;
}

void Converter::fillTemplateContainer(clang::QualType templType, json& jsonObject)
{
    // Если шаблонный параметр является шаблонным контейнером
    if (m_astHelper.isTemplatedContainer(templType))
    {
        m_pathStack.push_back(std::string(J_ITEMS));

        auto templatedType = helper::getTemplatedType(templType);

        fillTemplateContainer(templatedType, jsonObject[J_ITEMS]);

        m_pathStack.pop_back(); // J_ITEMS
        return;
    }

    // Если шаблонный параметр является массивом
    if (m_astHelper.isArray(templType))
    {
        m_pathStack.push_back(std::string(J_ITEMS));

        const clang::CXXRecordDecl* recordDecl = helper::getCXXRecordDeclFromQualType(templType);
        if (!recordDecl)
            return;

        // Пытаемся узнать, какой тип содержит массив
        for (const auto* method : recordDecl->methods())
        {
            if (method->getNameAsString() != "data")
                continue;

            clang::QualType returnType = method->getReturnType();

            fillTemplateContainer(returnType, jsonObject[J_ITEMS]);
            break;
        }

        m_pathStack.pop_back(); // J_ITEMS
        return;
    }

    // Если шаблонный параметр является std::optional
    if (helper::isStdOptional(templType))
    {
        // m_pathStack.push_back(std::string(J_ITEMS));

        auto templatedType = helper::getTemplatedType(templType);

        fillTemplateContainer(templatedType, jsonObject);

        jsonObject[J_TYPE].push_back(getTypeJsonSchema(templatedType));
        jsonObject[J_TYPE].push_back(TYPE_NULL);

        return;
    }

    // Если шаблонный параметр является пользовательской структурой
    if (m_astHelper.isStruct(templType))
    {
        m_pathStack.push_back(std::string(J_ITEMS));

        const auto recordDecl = helper::getRecordDeclFromQualType(templType);
        if (!recordDecl)
            return;

        fillRecordDeclCollect(recordDecl, jsonObject[J_ITEMS]);
        m_pathStack.pop_back(); // J_ITEMS
        return;
    }

    // Если шаблонный параметр является перечислением
    if (templType->isEnumeralType())
    {
        fillEnumTypeAndLimits(jsonObject, templType);
    }

    // Если шаблонный параметр является числом
    if (templType->isIntegerType() || templType->isFloatingType())
    {
        jsonObject[J_ITEMS][J_TYPE] = getTypeJsonSchema(templType);

        fillNumericLimits(templType, jsonObject[J_ITEMS]);
        return;
    }

    // Если это ничего из предложенного, то указываем contains
    jsonObject[J_ITEMS][J_TYPE] = getTypeJsonSchema(templType);
}

std::string Converter::getCurrentPath()
{
    std::string path;
    for (const auto& el : m_pathStack)
    {
        path += fmt::format("/{}", el);
    }

    return path;
}

void Converter::fillEnumTypeAndLimits(nlohmann::json& jsRef, clang::QualType type)
{
    jsRef[J_TYPE].push_back(getTypeJsonSchema(type));
    jsRef[J_TYPE].push_back(TYPE_STRING);

    fillEnumeralLimits(type, jsRef);
}

} // namespace

std::optional<PrintData> Module::convert(const ExtractedData& extractedData, clang::ASTContext& astContext)
{
    auto it = extractedData.commandsMap.find(COMMAND_JSON_SCHEMA);
    if (it == extractedData.commandsMap.end())
    {
        return std::nullopt;
    }

    Converter converter(astContext, extractedData);
    return converter.getPrintData();
}

} // namespace rgen::JsonSchema

#pragma once

#include <clang/AST/ASTContext.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>

#include <fmt/core.h>

#include "string_view"

namespace rgen
{

namespace helper
{

/*!
 * \brief Проверка, есть такой method для переданного type
 */
bool hasMethod(clang::QualType type, const std::string& method);

/*!
 * \brief Является ли переданный тип итерабельным
 */
bool isIterable(clang::QualType type);

/*!
 * \brief Является ли переданный тип std::optional
 */
bool isStdOptional(clang::QualType type);

/*!
 * \brief Может ли переданный тип не иметь значения
 */
bool isNullable(clang::QualType type);

/*!
 * \brief Является ли переданный тип числом
 */
bool isNumber(clang::QualType type);

/*!
 * \brief Проверка, есть ли у переданного type в декларациях declarationName
 */
bool hasDeclaration(clang::QualType type, std::string_view declarationName);

/*!
 * \brief Получить тип, который содержится в шаблоне
 */
clang::QualType getTemplatedType(clang::QualType type, std::string_view declarationName = {});

/*!
 * \brief Получить тип integer, к которому можно привести enum
 */
clang::QualType getIntInEnumeral(clang::QualType type);

/*!
 * \brief Получить декларацию класса о базовом шаблонном классе из using шаблона
 */
clang::CXXRecordDecl* getBaseRecordDeclInUsingTemplate(const clang::TypeAliasDecl* usingDecl);

/*!
 * \brief Поулчить QualType из Decl
 * \details Пример: Есть переменная, являющаясь пользовательской струкутру `SomeStruct some;`.
 *           Когда на руках имеется лишь RecordDecl этой переменной `some`, c помощью функции
 *           мы можем получить ей тип, а то есть добраться до `SomeStruct`
 */
clang::QualType getQualTypeFromDecl(const clang::Decl* decl);

/*!
 * \brief Получить декларацию класса CPP из класса С
 */
const clang::CXXRecordDecl* getCxxRecordDecl(const clang::RecordDecl* rd);

/*!
 * \brief Попытаться получить декларацию класса из QualType
 */
clang::RecordDecl* getRecordDeclFromQualType(const clang::QualType& type);

/*!
 * \brief Попытаться получить декларацию класса cpp из QualType
 */
const clang::CXXRecordDecl* getCXXRecordDeclFromQualType(const clang::QualType& type);

/*!
 * \brief Получить деклорацию класса из деклорации поля
 */
clang::RecordDecl* getRecordDeclFromFieldDecl(const clang::FieldDecl* fd);

/*!
 * \brief Получить первый шаблонный тип
 */
std::optional<clang::QualType> getFirstTemplatedType(clang::QualType type);

/*!
 * \brief Класс для упрощения получения информации из ASTContext
 */
struct ASTHelper
{
    ASTHelper(clang::ASTContext* astContext);
    clang::ASTContext* astContext = nullptr;

    /*!
     * \brief Получить type как строку
     */
    std::string typeName(const clang::QualType& type);

    /*!
     * \brief ns::Templ<int> -> ns::Templ
     */
    std::string templateName(const clang::QualType& type);

    /*!
     * \brief ns::Templ<int> -> int
     */
    std::string firstTemplateTypeName(const clang::QualType& type);

    /*!
     * \brief Можно ли преобразовывать тип к JsonArray
     */
    bool relatedToJsonArray(clang::QualType type);

    /*!
     * \brief является ли тип шаблонным контейнером (например: QVector)
     */
    bool isTemplatedContainer(clang::QualType type);

    /*!
     * \brief  Является ли тип массиром (фактически, контейнер без шаблона)
     */
    bool isArray(clang::QualType type);

    /*!
     * \brief Можно ли преобразовать тип к JsonValue
     */
    bool relatedToJsonValue(clang::QualType type);

    /*!
     * \brief Является ли тип структурой для сериализации
     */
    bool isStruct(clang::QualType type);

    /*!
     * \brief Является ли переданный тип числом, который требуется преобразовать в строку
     */
    bool isNumberToString(clang::QualType type);

    /*!
     * \brief Является ли переданный тип QString
     */
    bool isQString(clang::QualType type);

    /*!
     * \brief Является ли переданный тип std::string
     */
    bool isStdString(clang::QualType type);

    /*!
     * \brief Является ли переданный тип строкой
     */
    bool isString(clang::QualType type);

    /*!
     * \brief Получить Display содержимое для декларации decl
     * \details Display содержимое это либо значение по ключу @Display
     *          Либо весь коммент, относящийся к декларации, если ключа нет
     */
    std::string displayContent(const clang::Decl* decl);

    /*!
     * \brief Получение ключа Ignore в поле
     * \details Ключ Ignore отключает обработку поля в структуре
     */
    bool hasIgnoreAttribute(const clang::Decl* decl);

    /*!
     * \brief Получить комментарий находящийся у decl
     */
    std::string getRawComment(const clang::Decl* decl);

    /*!
     * \brief Получить имя максроса, который находится между ключевым словом и именем структуры
     */
    std::string getMacrosNameClassVisibility(const clang::CXXRecordDecl* rDecl);

    /*!
     * \brief Получить политики вывода информации по классу
     */
    clang::PrintingPolicy getPrintingPolicy();
};

} // namespace helper
} // namespace rgen

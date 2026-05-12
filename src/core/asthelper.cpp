#include "asthelper.h"

#include "comment.h"
#include "common_constants.h"
#include "utils.h"

#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/TemplateBase.h>
#include <clang/AST/Type.h>
#include <clang/Lex/Lexer.h>

namespace rgen
{

namespace helper
{

namespace
{

/*!
 * \brief Поулчить тип, который соответствует declarationName
 */
std::optional<clang::QualType> getOptionalDeclaration(clang::QualType type, std::string_view declarationName)
{
    const auto cxxRecordDecl = type->getAsCXXRecordDecl();
    if (!cxxRecordDecl)
    {
        return std::nullopt;
    }

    for (const clang::Decl* decl : cxxRecordDecl->decls())
    {
        if (const clang::TypedefNameDecl* typedefNameDecl = clang::dyn_cast<clang::TypedefNameDecl>(decl))
        {
            if (typedefNameDecl->getNameAsString() == declarationName)
            {
                return { typedefNameDecl->getUnderlyingType() };
            }
        }
    }
    return std::nullopt;
}

/*!
 * \brief Получить числовой тип, к  которому можно привети enum
 */
std::optional<clang::QualType> getOptionalIntTypeInEnumeral(clang::QualType type)
{
    if (const auto* enumType = type->getAs<clang::EnumType>())
    {
        if (const clang::EnumDecl* enumDecl = enumType->getDecl())
        {
            return enumDecl->getIntegerType();
        }
    }
    return std::nullopt;
}

} // namespace

clang::CXXRecordDecl* getBaseRecordDeclInUsingTemplate(const clang::TypeAliasDecl* usingDecl)
{
    // тип, на который указывает using
    const clang::QualType underlyingType = usingDecl->getUnderlyingType();

    // Проверяем, чтбоы этот тип был шаблоном, иначе нам не интересно (пока)
    const auto* templateSpecType = underlyingType->getAs<clang::TemplateSpecializationType>();
    if (!templateSpecType)
        return nullptr;

    // Так же проверям, что тип является классом
    auto recordType = underlyingType->getAs<clang::RecordType>();
    if (!recordType)
        return nullptr;

    // И получаем декларацию этого класс
    auto recordDecl = recordType->getDecl();
    if (!recordDecl)
        return nullptr;

    const auto ctx = &usingDecl->getASTContext();
    helper::ASTHelper astHelper(ctx);

    // Если под using находится итерабельный контейнер, то подменяем recordDecl на шаблонный параметр в контейнере
    if (astHelper.isTemplatedContainer(underlyingType))
    {
        std::optional<clang::QualType> templatedType = helper::getFirstTemplatedType(underlyingType);

        if (!templatedType.has_value())
            return nullptr;

        recordDecl = helper::getRecordDeclFromQualType(templatedType.value());
    }

    if (!astHelper.isStruct(ctx->getTypeDeclType(recordDecl)))
        return nullptr;

    return clang::dyn_cast<clang::CXXRecordDecl>(recordDecl);
}

bool hasMethod(clang::QualType type, const std::string& method)
{
    // Пытаемся получить из поля класс
    const auto fRDecl = type->getAsCXXRecordDecl();

    // И, соответственно, проверка на то, что поле является классом
    if (!fRDecl)
        return false;

    for (auto meth : fRDecl->methods())
    {
        if (meth->getNameAsString() == method)
            return true;
    }
    return false;
}

bool isIterable(clang::QualType type)
{
    return (hasMethod(type, "begin")
            && hasMethod(type, "end")
            && hasDeclaration(type, "value_type"));
}

bool isStdOptional(clang::QualType type)
{
    return ((hasMethod(type, "has_value")
             && hasMethod(type, "value")));
}

bool isNullable(clang::QualType type)
{
    return (type->isFloatingType()
            || isStdOptional(type));
}

bool hasDeclaration(clang::QualType type, std::string_view declarationName)
{
    return getOptionalDeclaration(type, declarationName).has_value();
}

bool isNumber(clang::QualType type)
{
    return (type->isIntegerType()       // int || unsigned int || char
            || type->isFloatingType()); // float || double || long ... || std::complex
}

clang::QualType getTemplatedType(clang::QualType type, std::string_view declarationName)
{
    auto optValueType = getFirstTemplatedType(type);
    if (optValueType.has_value())
        return optValueType.value();

    // Не всегда получается получить тип через getOptionalDeclaration и declarationName
    // Пример: QVector<std::optional<int>> (у std::optional не будет вообще никакхи полей)
    // Тем не менее, это может послужить как запасной метод получения типа находящегося в шаблоне
    if (!declarationName.empty())
    {
        optValueType = getOptionalDeclaration(type, declarationName);
        if (optValueType.has_value())
            return optValueType.value();
    }

    throw std::runtime_error(fmt::format("no decl {} with type '{}'", declarationName, type.getAsString()));
}

clang::QualType getIntInEnumeral(clang::QualType type)
{
    const auto intType = getOptionalIntTypeInEnumeral(type);
    if (intType.has_value())
        return intType.value();

    throw std::runtime_error(fmt::format("no Integer type in Enumeral with type '{}'", type.getAsString()));
}

clang::QualType getQualTypeFromDecl(const clang::Decl* decl)
{
    if (!decl)
        return clang::QualType();

    // Поле структуры или класса
    if (const auto* fieldDecl = llvm::dyn_cast<clang::FieldDecl>(decl))
    {
        return fieldDecl->getType();
    }
    // Глобальная или локальная переменная
    else if (const auto* varDecl = llvm::dyn_cast<clang::VarDecl>(decl))
    {
        return varDecl->getType();
    }
    // Параметр функции
    else if (const auto* parmVarDecl = llvm::dyn_cast<clang::ParmVarDecl>(decl))
    {
        return parmVarDecl->getType();
    }
    // Поле структуры или класса
    else if (const auto* typedefDecl = llvm::dyn_cast<clang::TypedefNameDecl>(decl))
    {
        return typedefDecl->getUnderlyingType();
    }
    // Структура или класс
    else if (const auto* recordDecl = llvm::dyn_cast<clang::RecordDecl>(decl))
    {
        if (recordDecl->isCompleteDefinition())
        {
            return decl->getASTContext().getRecordType(recordDecl);
        }
    }

    throw std::runtime_error("failed to get QualType from Decl in helper::getQualTypeFromDecl");
}

const clang::CXXRecordDecl* getCxxRecordDecl(const clang::RecordDecl* rd)
{
    const clang::CXXRecordDecl* cppRecordDecl = llvm::dyn_cast<clang::CXXRecordDecl>(rd);
    if (!cppRecordDecl)
    {
        std::string errMsg = fmt::format("Failed to get CPP class from C from {}", rd->getNameAsString());
        throw std::runtime_error(errMsg);
    }

    return cppRecordDecl;
}

clang::RecordDecl* getRecordDeclFromQualType(const clang::QualType& type)
{
    const auto fieldTypePtr = type.getTypePtr();
    if (!fieldTypePtr)
        return nullptr;

    const auto recordType = fieldTypePtr->getAs<clang::RecordType>();
    if (!recordType)
        return nullptr;

    return recordType->getDecl();
}

const clang::CXXRecordDecl* getCXXRecordDeclFromQualType(const clang::QualType& type)
{
    const auto rDecl = getRecordDeclFromQualType(type);
    if (!rDecl)
        return nullptr;

    return helper::getCxxRecordDecl(rDecl);
}

ASTHelper::ASTHelper(clang::ASTContext* astContext) :
    astContext(astContext) {}

std::optional<clang::QualType> getFirstTemplatedType(clang::QualType type)
{
    if (const auto* templType = type->getAs<clang::TemplateSpecializationType>())
    {
        const auto templatesArg = templType->template_arguments();
        if (!templatesArg.empty())
        {
            return templatesArg[0].getAsType();
        }
    }
    return std::nullopt;
}

std::string ASTHelper::typeName(const clang::QualType& type)
{
    auto temlatedType = getFirstTemplatedType(type);
    if (temlatedType.has_value())
    {
        // Получаем только тип шаблонного контейнера (без его типа в шаблоне)
        const auto* templDecl = type->getAs<clang::TemplateSpecializationType>()->getTemplateName().getAsTemplateDecl(); // После getAs проверка не нужна, поскольку она прошла в getFirstTemplatedType(type)
        if (templDecl)
            return fmt::format("{}<{}>", templDecl->getQualifiedNameAsString(), typeName(temlatedType.value()));

        throw std::runtime_error(fmt::format("no TemplateDecl with type '{}'", type.getAsString()));
    }

    // Получаем тип нешаблонного класса
    if (const auto* recordType = type->getAs<clang::RecordType>())
    {
        if (const clang::RecordDecl* recordDecl = recordType->getDecl())
        {
            const std::string nsName = utils::collectNamespacesFromDeclToString(recordDecl->getDeclContext());
            const std::string recorDeclName = recordDecl->getNameAsString();

            return utils::prefixNamespace(nsName, recorDeclName);
        }
    }

    // Получаем тип Енума
    if (const auto enumType = type->getAs<clang::EnumType>())
    {
        if (const clang::EnumDecl* enumDecl = enumType->getDecl())
        {
            const std::string nsName = utils::collectNamespacesFromDeclToString(enumDecl->getDeclContext());
            std::string enumDeclName = enumDecl->getNameAsString();

            if (enumDeclName.empty()) // в случае typedef enum {} typeName
                enumDeclName = type.getAsString(getPrintingPolicy());

            // Пытаемся узнать, находится ли enum в другом классе
            const clang::DeclContext* declContext = enumDecl->getDeclContext();
            if (const clang::CXXRecordDecl* recordDecl = llvm::dyn_cast<clang::CXXRecordDecl>(declContext))
            {
                std::string fullClassType = typeName(recordDecl->getTypeForDecl()->getCanonicalTypeInternal());

                return utils::prefixNamespace(fullClassType, enumDeclName);
            }

            return utils::prefixNamespace(nsName, enumDeclName);
        }
    }

    return type.getAsString(getPrintingPolicy());
}

std::string ASTHelper::templateName(const clang::QualType& type)
{
    auto temlatedType = getFirstTemplatedType(type);
    if (!temlatedType.has_value())
        return {};
    // Получаем только тип шаблонного контейнера (без его типа в шаблоне)
    const auto* templDecl = type->getAs<clang::TemplateSpecializationType>()->getTemplateName().getAsTemplateDecl(); // После getAs проверка не нужна, поскольку она прошла в getFirstTemplatedType(type)
    if (!templDecl)
        return {};

    return templDecl->getQualifiedNameAsString();
}

std::string ASTHelper::firstTemplateTypeName(const clang::QualType& type)
{
    auto temlatedType = getFirstTemplatedType(type);
    if (temlatedType.has_value())
        return typeName(temlatedType.value());
    return {};
}

bool ASTHelper::relatedToJsonArray(clang::QualType type)
{
    return (!isString(type)
            && helper::isIterable(type));
}

bool ASTHelper::isTemplatedContainer(clang::QualType type)
{
    return (relatedToJsonArray(type)
            && type->getAs<clang::TemplateSpecializationType>());
}

bool ASTHelper::isArray(clang::QualType type)
{
    return (relatedToJsonArray(type)
            && !isTemplatedContainer(type));
}

bool ASTHelper::relatedToJsonValue(clang::QualType type)
{
    return (isString(type)
            || helper::isNumber(type)
            || type->isBooleanType()
            || type->isEnumeralType()
            || isStdOptional(type));
}

bool ASTHelper::isStruct(clang::QualType type)
{
    // В некоторых случаях, мы получаем указатели на тип. К примеру, QByteArray вернет char *,
    //   который не будет биться ни с одним текущим типом (потому что это указатель).
    //   Текущая конструкция, позволит рекурсивно получить тип, который уже можно будет обработать ранее написанными методами.
    // Теоретически, это можно пропихнуть перед каждым QualType, но на текущий момент в этом нет необходимости.
    while (type->isPointerType())
        type = type->getPointeeType();

    return (!relatedToJsonValue(type) && !relatedToJsonArray(type));
}

bool ASTHelper::isNumberToString(clang::QualType type)
{
    if (!(type->isIntegerType() || type->isEnumeralType()))
        return false;

    const auto intWidth = astContext->getIntWidth(type);

    // qint64 (long long) || quint64 (unsigned long long)
    if (intWidth == 64)
        return true;

    // quint32 (unsigned int)
    if (type->isUnsignedIntegerType() && intWidth == 32)
        return true;

    return false;
}

bool ASTHelper::isQString(clang::QualType type)
{
    return typeName(type) == "QString";
}

bool ASTHelper::isStdString(clang::QualType type)
{
    return typeName(type) == "std::basic_string<char>";
}

bool ASTHelper::isString(clang::QualType type)
{
    return isQString(type) || isStdString(type);
}

std::string ASTHelper::getRawComment(const clang::Decl* rDecl)
{
    const clang::RawComment* Comment = astContext->getRawCommentForDeclNoCache(rDecl);
    if (Comment)
        return Comment->getFormattedText(rDecl->getASTContext().getSourceManager(), astContext->getDiagnostics());
    return {};
}

std::string ASTHelper::displayContent(const clang::Decl* decl)
{
    const auto declComments = rgen::parseDeclComments(*astContext, *decl);

    if (auto val = declComments.at_optional(COMMAND_DISPLAY))
        return val.value().second;
    else if (auto val = declComments.at_optional(COMMAND_BRIEF))
        return val.value().second;
    else if (declComments.size() == 0)
        return getRawComment(decl);

    return {};
}

bool ASTHelper::hasIgnoreAttribute(const clang::Decl* decl)
{
    const auto declComments = rgen::parseDeclComments(*astContext, *decl);

    if (auto val = declComments.at_optional(COMMAND_IGNORE))
        return true;

    return false;
}

std::string ASTHelper::getMacrosNameClassVisibility(const clang::CXXRecordDecl* rDecl)
{
    const clang::SourceManager& SM = astContext->getSourceManager();

    const clang::SourceLocation classLoc = rDecl->getBeginLoc();
    const clang::SourceLocation classKeywordEndLoc = clang::Lexer::getLocForEndOfToken(classLoc, 0, SM, clang::LangOptions());
    const clang::SourceLocation classNameBeginLoc = rDecl->getLocation();

    const clang::SourceRange range(classKeywordEndLoc, classNameBeginLoc);

    const llvm::StringRef macrosName = clang::Lexer::getSourceText(clang::CharSourceRange::getCharRange(range), SM, clang::LangOptions());

    return macrosName.trim().str();
}

clang::PrintingPolicy ASTHelper::getPrintingPolicy()
{
    auto p = astContext->getPrintingPolicy();
    p.SuppressTagKeyword = true; // Не печатать ключевые слова, а только типы
    return p;
}

clang::RecordDecl* getRecordDeclFromFieldDecl(const clang::FieldDecl* fd)
{
    if (!fd)
        return nullptr;

    const auto rt = fd->getType()->getAs<clang::RecordType>();
    if (!rt)
        return nullptr;

    const auto rd = rt->getDecl();
    if (!rd)
        return nullptr;

    return rd;
}

} // namespace helper
} // namespace rgen

#pragma once

#include <clang/AST/ASTContext.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceManager.h>

#include <inja/inja.hpp>

#include "string_util.h"

namespace rgen
{

namespace utils
{

namespace
{

/*!
 * \brief Получить полный путь фейкового генеративного файла поданного на вход
 */
std::filesystem::path getFakeCxxFile(const clang::SourceManager& SM)
{
    return { SM.getFileEntryForID(SM.getMainFileID())->tryGetRealPathName().str() };
}

} // namespace

/*!
 * \brief Помогатор с хедерами при посещении узлов AST
 */
class AstVisitorHeaderHelper
{
public:
    AstVisitorHeaderHelper(clang::Decl* decl);
    fs::path headerPath;
    /// Остановить разбор для этой декларации
    /// если имя фейкового cxx отличается от имени хедера
    bool breakVisit = false;

private:
    /*!
     * \brief Прочитать первую строку фейкового генеративного файла и получить информацию о хедере
     * \details Данное действие требуется для того, чтобы получить информацию о том, где реально расположен хедер.
     *          Эта информацию вносится при генерации фейкового cpp файла.
     * \return Путь к файлу
     */
    std::filesystem::path getHeaderPath(clang::Decl* decl);

    /*!
     * \brief Сравнение на равенство имени разбираемого файла(decl), и содержимого инклюда в фейковой генерации
     */
    bool compareIncludeFakeAndDeclFiles(clang::Decl* decl, const fs::path& includeFakeCpp);
};

struct NameInfo
{
    std::vector<std::string> parts;
    std::string namespace_;
    std::string name;
    std::string id;
};

NameInfo getNameInfo(const clang::NamedDecl* decl);

/*!
 * \brief Функция для вывода в консоль информации о настроечном содержимом json файла класса inja
 */
void injaJsonDump(const inja::json& jsn);

/*!
 * \brief Получение всех namespace класса в одну строку через :: разделитель
 */
std::string collectNamespacesFromDeclToString(const clang::DeclContext* declContext);

/*!
 * \brief Получение всех namespace класса в одну строку через :: разделитель
 */
std::vector<std::string> qualifiedNameParts(const clang::DeclContext* declContext);

/*!
 * \brief Объединить namespace c переданной строкой
 */
std::string prefixNamespace(const std::string_view ns, const std::string& name);

/*!
 * \brief Вычислить хеш файла
 */
std::string getHashFromString(const std::string& str);

/*!
 * \brief По полному пути к файлу, вычислить хеш и вернуть строку вида filename_hash
 */
std::string getFilenameWithHash(const std::filesystem::path fullpath);

/*!
 * \brief Получить полное расширение файла по указанное директории
 * \param file - директория до файла
 * \return Полное расширение файла
 */
std::string getFullExtension(const std::filesystem::path file);

/*!
 * \brief Обернуть содержимое файла в header guard
 * \param file - директория до файла
 * \param data - содержимое файла
 * \return Содержимое файла, обернутое в header guard
 */
std::string wrapIntoHeaderGuard(const std::filesystem::path file, const std::string& data);

/*!
 * \brief Найти и вернуть полный путь к папке, в которой находится хедер
 * \param buildDir -  Сборочная директория библиотеки
 * \param libDirectory - Директория, полученная из имени файла
 * \param hashFile - хеш-индикатор для определения уникальности
 */
fs::path findBuildDirInclude(const std::string_view buildDir, const std::string_view libDirectory, const std::string_view hashFile);

/*!
 * \brief Получить относительный до файла внутри библиотеки
 * \param libPath - путь до собранной библиотеки
 * \param headerSource - путь до оригинального хедера
 */
std::string getRelativeLibPath(fs::path libPath, fs::path headerSource);

} // namespace utils
} // namespace rgen

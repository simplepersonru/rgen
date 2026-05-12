#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace clang
{
class NamedDecl;
class CXXMethodDecl;
} // namespace clang

namespace rgen
{

std::string qualifiedName(const clang::NamedDecl& decl);

// void foo(int x, double y, std::string z) -> "int x, double y, std::string z"
std::string getSignature(const clang::CXXMethodDecl& methodDecl);

// void foo(int x, double y, std::string z) -> "x, y, z"
std::string joinArgs(const clang::CXXMethodDecl& methodDecl);

// void foo(int x, double y, std::string z) -> "int, double, std::string"
std::string joinTypes(const clang::CXXMethodDecl& methodDecl);

// "/a/b/c/foo.cpp" -> "foo.cpp"
std::string_view afterLastSlash(std::string_view s);

// "foo.cpp" -> "foo.extra.cpp"
std::string insertBeforeExt(std::string_view s, std::string_view extra);

// "foo.extra.cpp" -> "foo.extra"
std::string_view removeLastExt(std::string_view s);

// "foo.extra.h" -> "foo.extra.cpp"
std::string replaceLastExt(std::string_view s, std::string_view extra);

// "asd|qwe123|iiz", "|" -> {"asd", "qwe123", "iiz"}
std::vector<std::string> split(std::string string, std::string_view delim);

std::string join(const std::vector<std::string>& parts, std::string_view separator);

// " a  b c" -> ["a", "b", "c"]
std::vector<std::string_view> splitBySpace(std::string_view s);

// " a/b/c" -> ["a", "b", "c"]
std::vector<std::string_view> splitByDelim(std::string_view s, char delim);

// " a/b=c?d" -> ["a", "b", "c", "d"]
std::vector<std::string_view> splitByDelims(std::string_view s, std::string_view delims);

// "__string____some___" -> "_string_some_"
std::string removeSymbolDublicate(const std::string_view str, const char symbol);

// "__string____some___" -> "__string____some"
std::string removeAllEndSymbol(std::string str, const char symbol);

// ": < >" -> "_"
std::string replaceSymbols(std::string str, const std::vector<char> replaceSymbols, const char toSymbol);

// Удалить std, который находится в начале строки или же перед которым есть '<' или  ' ' или ','
std::string removeStd(std::string str);

// "ABC" -> "abc"
std::string toLower(std::string str);

// " asd " -> "asd"
std::string stripSpaces(const std::string& s);

std::string replace(const std::string& s, std::string_view before, std::string_view after);

/*!
 * \brief Проверка, содержится ли текст suffix в конце строки str
 */
bool endsWith(const std::string& str, const std::string& suffix);

namespace fPath
{

// Определение разделителя в зависимости от системы
#ifdef _WIN32
constexpr const char SEP = '\\';
#else
constexpr const char SEP = '/';
#endif

/*!
 * \brief Получить список файлов из директории рекурсивно
 * \param directory - директория, в которой следует получить списко файлов
 * \param pattern - имя файла, путь к которому будет возвращен как полный путь
 * \return Список найденных файлов
 */
std::vector<std::filesystem::path> listFiles(const std::filesystem::path& directory, const std::string_view pattern = "");

} // namespace fPath
} // namespace rgen

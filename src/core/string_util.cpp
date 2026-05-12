#include "string_util.h"

#include "logger.h"

#include <clang/AST/DeclCXX.h>

#include <regex>
#include <sstream>

namespace rgen
{

namespace
{

template <typename Func>
std::vector<std::string_view> Split(std::string_view s, Func IsDelim)
{
    std::vector<std::string_view> result;
    int first_non_delim = s.size();
    int last_non_delim = -1;
    for (int i = 0; i < s.size(); ++i)
    {
        if (IsDelim(s[i]))
        {
            if (first_non_delim != s.size())
            {
                result.push_back(s.substr(first_non_delim, last_non_delim - first_non_delim + 1));
            }
            first_non_delim = s.size();
        }
        else
        {
            // https://stackoverflow.com/a/22023122
            first_non_delim = (std::min)(first_non_delim, i);
            last_non_delim = i;
        }
    }
    if (first_non_delim != s.size())
    {
        result.push_back(s.substr(first_non_delim, last_non_delim - first_non_delim + 1));
    }
    return result;
}

template <typename Func>
std::string JoinParams(const clang::CXXMethodDecl& methodDecl, Func func)
{
    std::stringstream ss;
    for (const auto param : methodDecl.parameters())
    {
        if (ss.rdbuf()->in_avail())
        { // aka `if (!ss.str())`
            ss << ", ";
        }
        ss << func(*param);
    }
    return ss.str();
}

} // namespace

std::string removeSymbolDublicate(const std::string_view str, const char symbol)
{
    std::string result;
    bool lastCharWasSymbol = false;

    for (const char ch : str)
    {
        if (ch != symbol)
        {
            result += ch;
            lastCharWasSymbol = false;
        }
        else
        {
            if (!lastCharWasSymbol)
            {
                result += ch;
                lastCharWasSymbol = true;
            }
        }
    }
    return result;
}

std::string removeAllEndSymbol(std::string str, const char symbol)
{
    while (!str.empty() && str.back() == symbol)
        str.pop_back();

    return str;
}

std::string qualifiedName(const clang::NamedDecl& decl)
{
    std::string name;
    llvm::raw_string_ostream stream { name };
    decl.printQualifiedName(stream);
    return name;
}

std::string getSignature(const clang::CXXMethodDecl& methodDecl)
{
    return JoinParams(methodDecl, [](const auto& param) {
        return param.getType().getAsString() + " " + param.getNameAsString();
    });
}

std::string joinArgs(const clang::CXXMethodDecl& methodDecl)
{
    return JoinParams(methodDecl, [](const auto& param) {
        return param.getNameAsString();
    });
}

std::string joinTypes(const clang::CXXMethodDecl& methodDecl)
{
    return JoinParams(methodDecl, [](const auto& param) {
        return param.getType().getAsString();
    });
}

std::string_view afterLastSlash(std::string_view s)
{
    if (const auto pos = s.find_last_of(fPath::SEP); pos != std::string_view::npos)
    {
        s = s.substr(pos + 1);
    }
    return s;
}

std::string insertBeforeExt(std::string_view s, std::string_view extra)
{
    if (const auto lastPos = s.find_last_of('.'); lastPos != std::string_view::npos)
    {
        if (const auto prevLastPos = s.find_last_of('.', lastPos - 1); prevLastPos != std::string_view::npos)
        {
            return std::string { s.substr(0, prevLastPos) } + "." + std::string { extra } + std::string { s.substr(lastPos) };
        }
        return std::string { s.substr(0, lastPos) } + "." + std::string { extra } + std::string { s.substr(lastPos) };
    }
    return std::string { s };
}

std::string_view removeLastExt(std::string_view s)
{
    if (const auto pos = s.find_last_of('.'); pos != std::string_view::npos)
    {
        return s.substr(0, pos);
    }
    return s;
}

std::vector<std::string> split(std::string string, std::string_view delim)
{
    std::vector<std::string> result;

    if (delim.empty())
    {
        // Если разделитель пустой, возвращаем всю строку как один элемент
        if (!string.empty())
        {
            result.push_back(std::move(string));
        }
        return result;
    }

    size_t pos = 0;
    size_t found_pos;
    while ((found_pos = string.find(delim, pos)) != std::string::npos)
    {
        result.push_back(string.substr(pos, found_pos - pos));
        pos = found_pos + delim.length();
    }

    // Добавляем остаток строки после последнего разделителя
    result.push_back(string.substr(pos));

    return result;
}

std::vector<std::string_view> splitBySpace(std::string_view s)
{
    return Split(s, [](char c) { return std::isspace(c); });
}

std::vector<std::string_view> splitByDelim(std::string_view s, char delim)
{
    return Split(s, [delim](char c) { return c == delim; });
}

std::vector<std::string_view> splitByDelims(std::string_view s, std::string_view delims)
{
    return Split(s, [&delims](char c) { return delims.find(c) != std::string_view::npos; });
}

std::string replaceLastExt(std::string_view s, std::string_view extra)
{
    std::string str { removeLastExt(s) };
    str.append(extra);
    return str;
}

bool endsWith(const std::string& str, const std::string& suffix)
{
    if (str.length() >= suffix.length())
        return (str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0);
    return false;
}

std::string removeStd(std::string type)
{
    // Совпадет, если начинается с начала строки, или же перед std будет '<' или 'пробел' или запятая
    std::regex pattern(R"((^|[<, ])std::)");

    // Удалить найденные совпадения
    return std::regex_replace(type, pattern, "$1");
}

std::string replaceSymbols(std::string str, const std::vector<char> replaceSymbols, const char toSymbol)
{
    std::replace_if(
    str.begin(), str.end(),
    [replaceSymbols](char c) { return std::find(replaceSymbols.begin(), replaceSymbols.end(), c) != replaceSymbols.end(); },
    toSymbol);

    return str;
}

std::string toLower(std::string str)
{
    for (char& ch : str)
    {
        // Проходим по каждому символу
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return str;
}

std::string stripSpaces(const std::string& s)
{
    std::string_view sv = s;
    if (!sv.empty() && std::isspace(sv.front()))
    {
        sv.remove_prefix(1);
    }
    if (!sv.empty() && std::isspace(sv.back()))
    {
        sv.remove_suffix(1);
    }
    return std::string { sv };
}

std::string join(const std::vector<std::string>& parts, std::string_view separator)
{
    // Если вектор пуст, возвращаем пустую строку
    if (parts.empty())
    {
        return {};
    }

    std::string result = parts[0]; // Начинаем с первого элемента

    // Добавляем остальные элементы с разделителем
    for (size_t i = 1; i < parts.size(); ++i)
    {
        result.append(separator);
        result.append(parts[i]);
    }

    return result;
}

std::string replace(const std::string& s, std::string_view before, std::string_view after)
{
    if (before.empty())
    {
        return s; // Ничего не заменяем, если before пуст
    }

    std::string result;
    result.reserve(s.size()); // Резервируем память для оптимизации

    size_t pos = 0;
    size_t found_pos = 0;

    while ((found_pos = s.find(before, pos)) != std::string::npos)
    {
        // Добавляем часть до найденного before
        result.append(s, pos, found_pos - pos);
        // Добавляем after вместо before
        result.append(after);
        // Перемещаем позицию за найденный before
        pos = found_pos + before.size();
    }

    // Добавляем оставшуюся часть строки
    result.append(s, pos, s.size() - pos);

    return result;
}

} // namespace rgen

namespace rgen::fPath
{

std::vector<std::filesystem::path> listFiles(const std::filesystem::path& directory, const std::string_view pattern)
{
    std::vector<fs::path> filePaths;

    if (!fs::exists(directory) && !fs::is_directory(directory))
    {
        rlog::error("An error occurred with the directory: {}", directory.string());
        return filePaths; // Возвращаем пустой вектор в случае ошибки
    }

    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(directory))
    {
        // Директории нас не интересуют
        if (entry.is_directory())
            continue;

        if (pattern.empty())
        {
            // Если pattern не задан, то получим список всех фалов
            filePaths.push_back(entry.path());
        }
        else
        {
            // Иначе ищем только те имена файлов, которые заданы
            if (entry.path().filename() == pattern)
                filePaths.push_back(entry);
        }
    }

    return filePaths;
}

} // namespace rgen::fPath

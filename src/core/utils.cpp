#include "utils.h"

#include "command_line_args.h"
#include "logger.h"

#include <clang/AST/Decl.h>

#include <fmt/core.h>

namespace rgen::utils
{
namespace
{
using namespace fmt::literals;
}

void injaJsonDump(const inja::json& jsn)
{
    rlog::debug("Render inja template with data:\n{}", jsn.dump(2));
}

std::string collectNamespacesFromDeclToString(const clang::DeclContext* declContext)
{
    std::vector<std::string> parts = qualifiedNameParts(declContext);

    std::string resString {};

    for (int i = 0; i < parts.size(); ++i)
    {
        if (!resString.empty())
            resString += "::";

        resString += parts[i];
    }

    return resString;
}

std::string prefixNamespace(const std::string_view ns, const std::string& name)
{
    if (!ns.empty())
        return fmt::format("{}::{}", ns, name);
    return name;
}

std::string getHashFromString(const std::string& str)
{
    uint32_t sum = 0;
    for (char c : str)
        sum += static_cast<uint32_t>(c);

    auto hash = sum * static_cast<uint32_t>(str.length());
    return std::to_string(hash);
}

std::string getFilenameWithHash(const std::filesystem::path fullpath)
{
    return fmt::format("{filename}_{hash_fullpath}",
                       "filename"_a = fullpath.filename().stem().string(),
                       "hash_fullpath"_a = utils::getHashFromString(fullpath.string()));
}

fs::path findBuildDirInclude(const std::string_view buildDir, const std::string_view libDirectory, const std::string_view hashFile)
{
    if (!fs::exists(buildDir))
        return {};

    fs::path libDir = fs::path(buildDir) / libDirectory;

    if (!fs::exists(libDir))
        return {};

    for (const auto& entry : fs::directory_iterator(libDir))
    {
        if (entry.path().filename().string() != hashFile)
            continue;
        return entry.path();
    }
    return {};
}

AstVisitorHeaderHelper::AstVisitorHeaderHelper(clang::Decl* decl)
{
    headerPath = getHeaderPath(decl);
    breakVisit = !compareIncludeFakeAndDeclFiles(decl, headerPath);
}

std::filesystem::path AstVisitorHeaderHelper::getHeaderPath(clang::Decl* decl)
{
    std::string s;
    std::ifstream file(getFakeCxxFile(decl->getASTContext().getSourceManager()));
    std::getline(file, s); // читаем только первую строку файла
    file.close();

    s.replace(0, 3, ""); // Удаляем комментарий (//) из строки
    return { s };
}

bool AstVisitorHeaderHelper::compareIncludeFakeAndDeclFiles(clang::Decl* decl, const std::filesystem::path& includeFakeCpp)
{
    const auto& SM = decl->getASTContext().getSourceManager();
    fs::path curDeclFilePath { SM.getFilename(decl->getLocation()).str() }; // Полный путь текущего разбираемого файла

    return curDeclFilePath == includeFakeCpp;
}

std::vector<std::string> qualifiedNameParts(const clang::DeclContext* declContext)
{
    std::vector<std::string> res;
    res.reserve(5); // предположение о максимальной вложенности по неймспейсам

    // Захват namespace ближайшего к имени
    if (declContext && declContext->isNamespace())
        res.push_back(clang::cast<clang::NamespaceDecl>(declContext)->getNameAsString());

    // Последующие namespace'ы
    while (true)
    {
        const auto* parentContext = declContext->getParent();
        if (parentContext == nullptr)
            break;

        auto* parentDecl = clang::Decl::castFromDeclContext(parentContext);
        if (parentDecl == nullptr)
            break;

        declContext = parentContext;
        const auto* namespaceDecl = llvm::dyn_cast<clang::NamespaceDecl>(parentDecl);
        if (namespaceDecl == nullptr)
            continue;
        res.push_back(namespaceDecl->getNameAsString());
    }
    std::reverse(res.begin(), res.end());
    return res;
}

NameInfo getNameInfo(const clang::NamedDecl* decl)
{
    NameInfo result;
    result.parts = qualifiedNameParts(decl->getDeclContext());
    result.namespace_ = collectNamespacesFromDeclToString(decl->getDeclContext());
    result.name = decl->getNameAsString();
    result.id = utils::prefixNamespace(result.namespace_, result.name);
    return result;
}

std::string getFullExtension(const std::filesystem::path file)
{
    std::string ext;
    std::string filename = file.filename().string();
    size_t dotPos = filename.find('.');
    if (dotPos != std::string::npos) {
        ext = filename.substr(dotPos + 1);
    }
    return ext;
}

std::string wrapIntoHeaderGuard(const std::filesystem::path file, const std::string& data)
{
    auto guard = fmt::format("HEADER_GUARD_{}", std::to_string(llvm::hash_value(file.string())));
    return fmt::format(
        "#ifndef {guard}\n"
        "#define {guard}\n\n"
        "{data}\n\n"
        "#endif // {guard}",
        "guard"_a = guard,
        "data"_a = data);
}

std::string getRelativeLibPath(fs::path libPath, fs::path headerSource)
{
    std::string relLibDir; ///< Empty by default

    // Получаем название библиотеки
    auto libFolderName = libPath.stem().string() + '/';
    // Ищем соответствующую папку в исходниках
    size_t pos = headerSource.string().rfind(libFolderName);
    // Если нашли, то получаем относительный путь до файла
    if (pos != std::string::npos) {
        relLibDir = headerSource.string().substr(pos + libFolderName.length());
    }

    // Проверяем, что название библиотеки в директории встречается только один раз.
    size_t checkPos = headerSource.string().find(libFolderName);
    if(pos != checkPos)
        rlog::warn("The library name appears multiple times in the directory. The library path may be incorrect.");

    return fs::path(relLibDir).parent_path().string();
}

} // namespace rgen::utils

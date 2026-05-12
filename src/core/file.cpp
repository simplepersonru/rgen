#include "file.h"
#include "common_constants.h"
#include "logger.h"
#include "utils.h"

#include <llvm/Support/raw_ostream.h>

#include <fstream>
#include <map>
#include <string>

#include <fmt/core.h>

namespace rgen
{

namespace
{

using namespace fmt::literals;

class FilePrinter : public IFilePrinter
{
public:
    explicit FilePrinter(std::ofstream&& stream) :
        stream_ { std::move(stream) }
    {}

private:
    void Print(std::string_view s) override
    {
        stream_ << s;
    }

private:
    std::ofstream stream_;
};

} // namespace

std::filesystem::path Content::getGenerationFile() const
{
    return m_outDir / fmt::format("{filename}.{extension}",
                                  "filename"_a = m_filename,
                                  "extension"_a = m_extension);
}

Content& ContentManager::getContentRef(const std::string_view extension)
{
    // Создаем объект и возвращаем на него ссылку
    m_contents.emplace_back(Content(m_outDir,
                                    m_filepath.filename().stem().string(),
                                    extension));
    return m_contents.back();
}

std::vector<fs::path> ContentManager::generate()
{
    std::map<fs::path /*file suffix*/, std::string /*data for file*/> collector;

    for (const auto& content : m_contents)
    {
        // Для каждого файла с расширением getExtension собираем содержимое
        collector[content.getGenerationFile()] += content.getGeneration();
    }

    // Для хедеров создаём header guard
    for (auto& [path, data] : collector)
    {
        if (utils::getFullExtension(path) == FILE_EXTENSION_RGEN_HXX)
            data = utils::wrapIntoHeaderGuard(path, data);
    }

    std::vector<fs::path> genPaths;
    genPaths.reserve(collector.size());
    for (const auto& contentData : collector)
    {
        genPaths.push_back(contentData.first);
        FilePrinter printer((std::ofstream(contentData.first)));

        printer << contentData.second;
    }

    return genPaths;
}

std::unique_ptr<ContentManager> BuildContentManager(const std::filesystem::path& outDir, const std::filesystem::path& filepath)
{
    return std::make_unique<ContentManager>(outDir, filepath);
}

std::string readFileToString(const std::string& filePath, bool isPrintError)
{
    if (!fs::exists(filePath))
    {
        if (isPrintError)
            rlog::error("File does not exist: {}", filePath);
        return {};
    }
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open())
    {
        if (isPrintError)
            rlog::error("Failed to open file: {}", filePath);
        return {};
    }
    return std::string((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
}

void printInjaContent(Content &contentRef, const inja::json &injaJson, const std::string &templ)
{
    try
    {
        contentRef << inja::render(templ, injaJson);
    }
    catch (inja::InjaError& err)
    {
        auto lines = split(templ, "\n");
        if (err.location.line >= lines.size())
            throw;
        std::string problemLine = lines[err.location.line - 1];
        rlog::error("Render error with file {}:\n{}. Line:\n{}",
                    contentRef.getGenerationFile().string(),
                    err.message,
                    problemLine);
    }
}

} // namespace rgen

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/ArgumentsAdjusters.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>

#include <core/command_line_args.h>
#include <core/common_constants.h>
#include <core/file.h>
#include <core/logger.h>
#include <core/ppcallbacks.h>
#include <core/registry.h>
#include <core/string_util.h>
#include <core/utils.h>
#include <core/visitor.h>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>

#include <nlohmann/json.hpp>

using namespace rgen;

static std::vector<std::string> customCommands;

namespace
{

class RedGenASTConsumer : public clang::ASTConsumer
{
public:
    RedGenASTConsumer(ContentManager& contentManager, clang::CompilerInstance& compiler, std::string inFile) :
        ContentManager_ { contentManager }, InFile_ { inFile }
    {
        clang::Preprocessor& preprocessor = compiler.getPreprocessor();
        preprocessor.addPPCallbacks(std::make_unique<PreprocessorCallbacks>(preprocessor));
        PreprocessorCallbacks_ = dynamic_cast<PreprocessorCallbacks*>(preprocessor.getPPCallbacks());
    }

    void HandleTranslationUnit(clang::ASTContext& astContext) override
    {
        auto extractedData = rgen::Visitor::extractAll(astContext);

        Context ctx {
            /* .ContentManager = */ ContentManager_,
            /* .AstContext = */ astContext,
            /* .extractedData = */ extractedData,
            /* .ppcallbacks = */ PreprocessorCallbacks_
        };

        for (const auto& module : ModuleRegistry::GetModules())
        {
            rlog::debug("Begin module processing {}", module.Name);
            try
            {
                module.Func(ctx);
            }
            catch (std::exception& ex)
            {
                rlog::error("{}", ex.what());
            }
        }
        rlog::debug("Module processing completed");
    }

private:
    ContentManager& ContentManager_;
    PreprocessorCallbacks* PreprocessorCallbacks_;
    std::string InFile_;
};

class RedGenFrontendAction : public clang::ASTFrontendAction
{
public:
    RedGenFrontendAction(ContentManager& contentManager) :
        contentManager_(contentManager)
    {}

    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance& compiler, llvm::StringRef inFile) override
    {
        return std::make_unique<RedGenASTConsumer>(contentManager_, compiler, std::string { inFile });
    }

private:
    ContentManager& contentManager_;
};

class RedGenFrontendActionFactory : public clang::tooling::FrontendActionFactory
{
public:
    RedGenFrontendActionFactory(ContentManager& contentManager) :
        contentManager_(contentManager)
    {}

    std::unique_ptr<clang::FrontendAction> create() override
    {
        return std::make_unique<RedGenFrontendAction>(contentManager_);
    }

private:
    ContentManager& contentManager_;
};

std::optional<std::string> BuildCommandsArgs()
{
    std::vector<std::string_view> allCommands;
    for (const auto& module : ModuleRegistry::GetModules())
    {
        allCommands.insert(allCommands.end(), module.Info.Commands.begin(), module.Info.Commands.end());
    }

    for (const auto& cmd : customCommands)
    {
        allCommands.push_back(cmd);
    }

    if (allCommands.empty())
    {
        return std::nullopt;
    }

    // clang понимает только те команды, которые подаются с этим ключом
    std::stringstream ss;
    ss << "-fcomment-block-commands=";
    for (const auto command : allCommands)
    {
        ss << command << ",";
    }
    return ss.str();
}

clang::tooling::CommandLineArguments RedGenArgumentsAdjuster(
const clang::tooling::CommandLineArguments& args, llvm::StringRef /*filename*/)
{
    auto result = args;
    auto itrEndOptSeparator = std::find(result.begin(), result.end(), "--"); // Ищем разделитель конца опций, чтобы добавить свои опции

    if (itrEndOptSeparator == result.end())
        // Не смогли найти разделитель. Попробуем добавить в конец.
        rlog::debug("Couldn't find the separator. Let's try adding to the end.");

    auto newPos = result.insert(itrEndOptSeparator, "-fparse-all-comments");

    if (auto arg = BuildCommandsArgs())
    {
        rlog::debug("Add custom commands:\n\n{}\n", *arg);
        result.insert(newPos, std::move(*arg));
    }
    return result;
}

/*!
 * \brief Хранение информации о пути файла
 * \details На вход поступает информация о хедере, с которым будет производиться работа.
 *              Так же хранит в себе информацию о пути фейкового cpp файла и о полученном в ходе генерации
 *          Поскольку clang работает только с файлами исходного кода, поэтому мы не может так просто скормить ему хедер.
 *          Для этого приходит на помощь фейковый cpp файл, который ссылается на нужный хедер.
 *          Так же этот fakeCPP нужно указывать в compile_commands.json, чтобы clang смог правильно распознать,
 *              какие includePath нужно использовать для обработки этого файла.
 *          В частности: без compile_commands.json и правильного указания пути к fakeCPP не получится использовать библиотеки Qt,
 *              покольку clang просто не будет знать что это такое
 */
struct FileInfo
{
    FileInfo(const std::filesystem::path& path, const std::filesystem::path& outputDir) :
        inputFilePath(path)
    {
        if (outputDir.empty())
            outDir = inputFilePath.parent_path();
        else
            outDir = outputDir;

        workDir = outDir / inputFilePath.stem();

        setOutFakeFilePath(inputFilePath);
        genFakeCpp();
    }

private:
    /*!
     * \brief Генерация фейкового исходного файла
     */
    void genFakeCpp() const
    {
        std::ofstream out { outFakeFilePath };
        const std::string incComm = fmt::format("// {}\n", inputFilePath.string());
        const std::string inc = fmt::format("#include \"{}\"", inputFilePath.string());
        if (out.is_open())
            out << incComm << inc << std::endl;
        else
            rlog::error("Unable to write to {}", outFakeFilePath.string());
        out.close();
    }

    void setOutFakeFilePath(const std::filesystem::path& path)
    {
        fs::create_directories(workDir); // Создаем предварительные папки, в случае необходимости
        outFakeFilePath = workDir / path.filename();
        outFakeFilePath += ".cxx";
    }

public:
    std::filesystem::path inputFilePath;   // полный путь к хедеру, который поступает на вход
    std::filesystem::path outDir;          // Папка в которую нужно поместить генерируемый файл
    std::filesystem::path outFakeFilePath; // Путь к фейковому файлу для дальнейшей генерации
    std::filesystem::path workDir;         // для временных файлов
};

} // namespace

int workMode_onlyArtifactsName()
{
    for (const fs::path inputFilepath : cLine::Arg_fileForGen)
        for (const auto& path : ModuleRegistry::getArtifactsFilenames(inputFilepath, std::string(cLine::Arg_outputDir)))
            llvm::outs() << path << " ";
    return rlog::isFinishedWithError();
}

int workMode_getFileModuleExtensions()
{
    std::string contentStream;
    if (cLine::Arg_modulesInfoType == "json")
    {
        contentStream = ModuleRegistry::getJsonModuleExtensions();
    }
    else if (cLine::Arg_modulesInfoType == "js")
    {
        contentStream = fmt::format(
        "function getModuleInfo() {{ return {}; }}\n"
        "var moduleNames = {{ {} }};",
        ModuleRegistry::getJsonModuleExtensions(),
        ModuleRegistry::getModuleNames());
    }
    else
    {
        rlog::error("This type is not supported for 'module_info_type'. More information 'redkit-gen --help'");
    }

    const fs::path dir { std::string(cLine::Arg_getAllModulesInfo) };
    const std::string filename = fmt::format("ModuleExtensions.{}", cLine::Arg_modulesInfoType);

    std::ofstream stream(dir / filename);
    if (stream.is_open())
    {
        stream << contentStream;
    }
    else
    {
        rlog::error("Failed to write module extension information");
    }

    stream.close();

    return rlog::isFinishedWithError();
}

int main(int argc, const char** argv)
{
    llvm::cl::ParseCommandLineOptions(argc, argv);

    if (!cLine::Arg_custom_commands.empty())
    {
        auto cfg = nlohmann::json::parse(cLine::Arg_custom_commands);

        if (!cfg.is_array())
        {
            rlog::error("customModules должен быть массивом");
            return 1;
        }

        for (const auto& item : cfg)
        {
            if (item.contains("command"))
            {
                customCommands.push_back(item["command"]);
            }
        }
    }

    if (!cLine::Arg_getAllModulesInfo.empty())
        return workMode_getFileModuleExtensions();

    if (cLine::Arg_onlyArtifacts)
        return workMode_onlyArtifactsName();

    // TODO Сделать так же, как и с input (разделить на несколько тегов, а не через запятую)
    // Выбираем нужные модули для обработки
    ModuleRegistry::FilterModules(cLine::Arg_modules);

    // Обычный режим работы
    if (cLine::Arg_dbDir.empty())
    {
        rlog::error("No folder specified before compile_commands.json");
        // FIXME GMock. Вернуть после переезда модуля на рельсы qbs (когда-нибудь). Так же уточнить, может этот блок и вовсе не понадобится с параметром Required.
    }

    std::string errorMsg;
    std::unique_ptr<clang::tooling::CompilationDatabase> db = nullptr;
    if (!cLine::Arg_dbDir.empty())
        db = clang::tooling::CompilationDatabase::autoDetectFromDirectory(cLine::Arg_dbDir.data(), errorMsg);

    if (!db)
    {
        rlog::error("Compilation database NOT loaded. Error: \n{}", errorMsg);
        return rlog::isFinishedWithError();
    }
    rlog::debug("Compilation database loaded for {} files", db->getAllFiles().size());

    // Создание списка-требования для генерации файлов
    std::vector<FileInfo> pathsForNamesGen;
    for (fs::path inputFile : cLine::Arg_fileForGen)
    {
        if (inputFile.extension() == ".h" || inputFile.extension() == ".cpp")
        {
            const auto& fileInfo = pathsForNamesGen.emplace_back(FileInfo { inputFile, cLine::Arg_outputDir.data() });

            const std::string hash = utils::getHashFromString(fileInfo.inputFilePath.string());
            std::ofstream hashFile(fileInfo.workDir / hash);
            hashFile.close();
        }
    }

    if (pathsForNamesGen.empty())
    {
        rlog::error("No files to generate.");
    }

    // Запуск разбора и обработки файлов
    for (FileInfo& file : pathsForNamesGen)
    {
        rlog::openFile(file.workDir / fmt::format("{}.{}", file.inputFilePath.stem().string(), FILE_EXTENSION_LOG));

        auto contentManager = BuildContentManager(file.outDir, file.inputFilePath);

        // Логируем аргументы запуска
        rlog::debug("Program launch arguments:\n\n{}\n", fmt::join(std::vector<std::string> { argv, argv + argc }, "\n"));

        auto action = std::make_unique<RedGenFrontendActionFactory>(*contentManager);

        clang::tooling::ClangTool tool(*db.get(), { file.outFakeFilePath.string() });

        tool.appendArgumentsAdjuster(RedGenArgumentsAdjuster);
        if (tool.run(action.get()) != 0)
        {
            rlog::error("Some problems with the file \"{}\"", file.outFakeFilePath.string());
        }

        std::vector<fs::path> genPaths = contentManager->generate();

        if (cLine::Arg_DebugPrintArtifactsWithHeader)
            for (const auto& filePath : genPaths)
                // Копируем получившийся артефакт рядом с хедером
                std::filesystem::copy_file(filePath, file.inputFilePath.parent_path() / filePath.filename(), fs::copy_options::overwrite_existing);
    }

    for (const auto& file : pathsForNamesGen)
    {
        for (const auto& path : ModuleRegistry::getArtifactsFilenames(file.inputFilePath, file.outDir.string()))
        {
            // Создание пустых файлов, которые не были сгенерированы модулями (требование QBS)
            if (!fs::is_regular_file(path))
            {
                std::ofstream emptyFile(path);
                emptyFile.close();
            }
        }
    }

    return rlog::isFinishedWithError();
}

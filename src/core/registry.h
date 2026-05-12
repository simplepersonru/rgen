#pragma once

#include <filesystem>
#include <functional>
#include <unordered_set>
#include <vector>

#include "extracteddata.h"
#include "file.h"

namespace clang
{
class ASTContext;
} // namespace clang

/*!
 * Принцип работы:
 *  Модульность проекта осуществляется путем регистрации каждого модуля в статический объект MODULES.
 *  Для каждого модуля при запуске вызывается метод AddModule и, после чего,
 *      в основном теле программы (main.cpp) вызывается метод GetModules, который и сообщает о том,
 *      какие модули сейчас в работе.
 *  С помощью метода FilterModules можно включить отдельные модули. Без вызова - активируются все доступные.
 */

namespace rgen
{

class PreprocessorCallbacks;

struct Context
{
    ContentManager& contentManager;     // Менеджер для управления выводом сгенерированных файлов
    clang::ASTContext& astContext;      // Контекст AST для доступу к информации о типах
    ExtractedData& extractedData;       // Предварительно извлеченные данные из AST, подготовленные для обработки
    PreprocessorCallbacks* ppcallbacks; // Данные с этапа препроцессинга
};

struct FileExtension
{
    std::string_view Extension; // Расширение артефакта
    std::string_view FileTag;   // Его тег для QBS

    bool operator==(const FileExtension& other) const
    {
        return Extension == other.Extension && FileTag == other.FileTag;
    }
};

struct ModuleInfo
{
    std::vector<std::string_view> Commands;    // Перечисление комманд требующихся для парсинга этого модуля
    std::vector<FileExtension> FileExtensions; // Расширения артефактов, получившихся в результате работы модуля
    std::vector<std::string_view> Includes;    // Список инклюдов, необходимый для работы модуля
    std::vector<std::string_view> Templates;   // Имя экспортируемого шаблона
    bool IsDefault = true;                     // Использовать модуль при отсутствии --additional_modules
};

/*!
 * \brief Информация об одном модуле
 */
struct Module
{
    std::string_view Name;              // Имя модуля
    std::function<void(Context&)> Func; // Команда начала работы модуля
    ModuleInfo Info;                    // Дополнительная информация предоставляемая модулем
};

/*!
 * \brief регистрация модуля
 */
class ModuleRegistry
{
public:
    /*!
     * \brief Добавить модуль в глобальную переменную
     */
    static void AddModule(Module module);
    /*!
     * \brief Получить список модулей содержащихся глобальной переменной
     */
    static const std::vector<Module>& GetModules();
    /*!
     * \brief Очистить глобальную переменную
     */
    static void ClearModules();

    /*!
     * \brief Определить модули, необходимые для обработки файла генератором
     * Модули по умолчанию добавляются всегда
     * Для добавления модулей, не являющихся модулями по умолчанию, необходимо перечислить их
     * в аргументе additionalModulesString через запятую
     * \param additionalModules - строка дополнительных модулей, которые необходимо обработать,
     * перечисленных через запятую
     */
    static void FilterModules(const std::string& additionalModulesString);

    /*!
     * \brief Получить имена выходящих файлов для подключенных модулей
     * \param filename - Путь к хедеру для получения имени файла в нужном формате
     * \param outputDir - путь к папке, куда следует поместить сгенерированные файлы
     */
    static std::unordered_set<std::string> getArtifactsFilenames(const std::filesystem::path& headerPath, const std::filesystem::path& outputDir = {});

    /*!
     * \brief Получить информацию по активным модуям и их расширениям в формате json
     */
    static std::string getJsonModuleExtensions();

    /*!
     * \brief Получить информацию по названиям активных модулей в виде ключ-значений
     */
    static std::string getModuleNames();
};

struct ModuleRegistrator
{
    ModuleRegistrator(Module&& module)
    {
        ModuleRegistry::AddModule(std::move(module));
    }
};

} // namespace rgen

// Макрос склеивания данных
// MACRO_CONCAT(foo, 123) -> foo123
#define MACRO_CONCAT(x, y) x##y

// Макрос регистрации модуля
// REGISTER_MODULE(Some_Name, { "SomeCommand" }, nullprt, { "cxx" }); ->
// static ::rgen::ModuleRegistrator ModuleRegistratorSome_Name(Module { "Some_Name", { "SomeCommand" }, nullprt, { "cxx" }})
#define REGISTER_MODULE(Name, Func, Info) \
    static ::rgen::ModuleRegistrator      \
    MACRO_CONCAT(ModuleRegistrator, Name)(Module { #Name, Func, Info })

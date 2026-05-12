#pragma once

#include <filesystem>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace clang
{
class Decl;
}

namespace rlog
{

/// Стандартный шаблон форматирования для сообщений об ошибках
inline std::string problemPattern = "%^%v%$";

/*!
 * \struct PatternGuard
 * \brief RAII-обертка для временного изменения паттерна (формата) логирования
 * \details При создании устанавливает новый шаблон, при разрушении - восстанавливает предыдущий
*/
struct PatternGuard
{
    /*!
     * \brief Конструктор, устанавливающий новый шаблон
     * \param newPattern Новый шаблон форматирования
    */
    explicit PatternGuard(const std::string& newPattern);
    /*!
     * \brief Деструктор, восстанавливающий исходный шаблон
    */
    ~PatternGuard();
};

/*!
 * \brief Генерирует форматированную строку с информацией об ошибке
 * \param decl Указатель на AST-узел
 * \param levelName Название уровня сообщения ("error", "warning" и т.д.)
 * \param msg Текст сообщения
 * \return Отформатированная строка с:
 *      Местоположением в исходном коде
 *      Строкой кода с ошибкой
 *      Указателем на проблемное место
 *
 *    file:line:col: level: message
 *     line | code
 *          | ^
*/
std::string generateProblemLine(const clang::Decl* const decl,
                                const std::string& levelName,
                                const std::string& msg);

/*!
 * \brief Логирует отладочное сообщение
 * \tparam Args Типы аргументов для форматирования
 * \param fmt Строка формата (в стиле fmtlib)
 * \param args Аргументы для подстановки в строку формата
 */
template <typename... Args>
inline void debug(fmt::format_string<Args...> fmt, Args&&... args)
{
    spdlog::debug(fmt, std::forward<Args>(args)...);
}

/*!
 * \brief Логирует сообщение об ошибке
 * \tparam Args Типы аргументов для форматирования
 * \param fmt Строка формата (в стиле fmtlib)
 * \param args Аргументы для подстановки в строку формата
 */
template <typename... Args>
inline void error(fmt::format_string<Args...> fmt, Args&&... args)
{
    spdlog::error(fmt, std::forward<Args>(args)...);
}

/*!
 * \brief Логирует предупреждение
 * \tparam Args Типы аргументов для форматирования
 * \param fmt Строка формата (в стиле fmtlib)
 * \param args Аргументы для подстановки в строку формата
 */
template <typename... Args>
inline void warn(fmt::format_string<Args...> fmt, Args&&... args)
{
    spdlog::warn(fmt, std::forward<Args>(args)...);
}

/*!
 * \brief Логирует сообщение об ошибке для указанного AST-узла
 * \tparam Args Типы аргументов для форматирования
 * \param decl Указатель на AST-узел
 * \param fmt Строка формата (в стиле fmtlib)
 * \param args Аргументы для подстановки в строку формата
*/
template <typename... Args>
inline void error(const clang::Decl* const decl, fmt::format_string<Args...> fmt, Args&&... args)
{
    PatternGuard guard(problemPattern);
    const std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
    spdlog::error(generateProblemLine(decl, "error", msg));
}

/*!
 * \brief Логирует предупреждение для указанного AST-узла
 * \tparam Args Типы аргументов для форматирования
 * \param decl Указатель на AST-узел
 * \param fmt Строка формата (в стиле fmtlib)
 * \param args Аргументы для подстановки в строку формата
*/
template <typename... Args>
inline void warn(const clang::Decl* const decl, fmt::format_string<Args...> fmt, Args&&... args)
{
    PatternGuard guard(problemPattern);
    const std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
    spdlog::warn(generateProblemLine(decl, "warning", msg));
}

/*!
 * \brief Открывает файл для записи логов
 * \param filepath Путь к файлу (строка или std::filesystem::path)
 * \throws spdlog::spdlog_ex Если не удалось открыть файл
 */
void openFile(const std::filesystem::path& filepath);
/*!
 * \brief Закрывает файл логов. После закрытия файла логи будут выводиться только в консоль
 * \note Вызывается автоматически в деструкторе логгера и при открытии нового файла
 */
void closeFile();

/*!
 * \brief Проверяет наличие ошибок
 * \return true если были сообщения уровня err
 */
bool isFinishedWithError();

} // namespace rlog

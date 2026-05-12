#include "logger.h"

#include "command_line_args.h"

#include <memory>

#include <spdlog/sinks/ansicolor_sink.h>

#include <clang/AST/ASTContext.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Lexer.h>

namespace rlog
{
namespace
{
using namespace fmt::literals;
}

/*!
 * \var inline std::string logPattern
 * \brief Стандартный шаблон форматирования для логов
 * \details Использует следующий формат:
 * \code
 * [YYYY.MM.DD HH:MM:SS.MS] [LEVEL] message
 * \endcode
 * Уровень показывается в консоли некоторым цветом в зависимости от уровня логирования.
*/
inline std::string logPattern = "[%Y.%m.%d %T.%f] %^[%l]%$ %v";

/*!
 * \brief Фильтрующий sink для сообщений с учетом verbose-режима
 *
 * \tparam WrappedSink Тип оборачиваемого sink'а (должен наследоваться от spdlog::sinks::sink)
 *
 * \details Пропускает сообщения только если:
 * 1. Уровень сообщения попадает в заданный диапазон [min, max]
 * 2. Установлен флаг --verbose при запуске программы
 */
template <typename WrappedSink>
class verbose_level_range_sink_st final : public spdlog::sinks::sink
{
public:
    /// Тип для уровней логирования spdlog
    using level_t = spdlog::level::level_enum;

    /*!
     * \brief Конструктор sink'а
     *
     * \param inner Оборачиваемый sink
     * \param minLvl Минимальный уровень логирования (включительно)
     * \param maxLvl Максимальный уровень логирования (включительно)
     */
    verbose_level_range_sink_st(std::shared_ptr<WrappedSink> inner, level_t minLvl, level_t maxLvl) :
        m_inner(std::move(inner)), m_min(minLvl), m_max(maxLvl) {}

    verbose_level_range_sink_st(const verbose_level_range_sink_st&) = delete;
    verbose_level_range_sink_st& operator=(const verbose_level_range_sink_st&) = delete;

    /// spdlog::sinks::sink interface
protected:
    void log(const spdlog::details::log_msg& msg) override
    {
        if (msg.level >= m_min && msg.level <= m_max && cLine::Arg_verbose)
            m_inner->log(msg);
    }
    void flush() override
    {
        m_inner->flush();
    }
    void set_pattern(const std::string& pattern) override
    {
        m_inner->set_pattern(pattern);
    }
    void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override
    {
        m_inner->set_formatter(std::move(sink_formatter));
    }

private:
    std::shared_ptr<WrappedSink> m_inner; ///< Оборачиваемый sink
    level_t m_min;                        ///< Минимальный уровень логирования
    level_t m_max;                        ///< Максимальный уровень логирования
};

/*!
 * \class LoggerHandler
 * \brief Обработчик логгера с поддержкой нескольких sink'ов и отслеживанием ошибок. Однопоточная реализация.
 *
 * \details Класс реализует:
 * - Вывод в консоль с цветным форматированием
 * - Запись в файл
 * - Отслеживание ошибок (уровень err)
 *
 * \note Инициализируется в момент запуска программы. В силу того, что
 * глобальный логгер spdlog имеет ленивую инициализацию, предусмотрена
 * следующая последовательность инициализации: сначала LoggerHandler,
 * в котором создаётся кастомный логгер, который встаёт на место глобального
 * логгера. Когда кастомный логгер встаёт на место глобального логгера,
 * глобальный логгер инициализируется.
 */
class LoggerHandler
{
private:
    /*!
     * \class error_tracking_sink
     * \brief Внутренний sink для отслеживания ошибок
     * \tparam Mutex Тип мьютекса для потокобезопасности
     */
    template <typename Mutex>
    class error_tracking_sink : public spdlog::sinks::base_sink<Mutex>
    {
    public:
        /*!
         * \brief Проверяет наличие ошибок
         * \return true если что-то логировалось на уровне error
         */
        bool getError() const { return m_errorHandler; }

    protected:
        /*!
         * \brief Обрабатывает сообщение лога
         * \param msg Сообщение для обработки
         * \internal
         */
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            if (msg.level == spdlog::level::err)
            {
                m_errorHandler = true;
            }
        }
        /*!
         * \brief Сбрасывает буфер (реализация интерфейса). Не требуется для этого sink-а (по идее?)
         * \internal
         */
        void flush_() override {}

    private:
        bool m_errorHandler = false; ///< Флаг наличия ошибок
    };

public:
    /*!
     * \brief Открывает файловый sink для записи логов
     * \param filepath Путь к файлу лога
     * \note Автоматически закрывает предыдущий файловый sink
     * \throws spdlog::spdlog_ex при ошибках открытия файла
     */
    void openFileOutput(const std::filesystem::path& filepath);

    /*!
     * \brief Закрывает файловый sink
     * \details Логи будут продолжать выводиться в консоль
     */
    void closeFileOutput();

    /*!
     * \brief Возвращает единственный экземпляр логгера
     * \return Ссылка на LoggerHandler
     */
    static LoggerHandler& getInstance();

    /*!
     * \brief Проверяет завершение с ошибкой
     * \return true если были сообщения уровня err
     */
    bool isFinishedWithError();

private:
    LoggerHandler();
    ~LoggerHandler();
    /*!
     * \brief Инициализирует консольный sink для стандартного потока. Логируются только level::debug
     * \return shared_ptr на инициализированный sink
     * \details Настраивает:
     * - Цветовое форматирование
     * - Шаблон вывода
     */
    std::shared_ptr<verbose_level_range_sink_st<spdlog::sinks::stdout_color_sink_st>> initStdOutSink();

    /*!
     * \brief Инициализирует консольный sink для потока ошибок. Логируются от level::warn и выше
     * \return shared_ptr на инициализированный sink
     * \details Настраивает:
     * - Цветовое форматирование
     * - Шаблон вывода
     */
    std::shared_ptr<spdlog::sinks::stderr_color_sink_st> initStdErrSink();

    /*!
     * \brief Инициализирует sink для отслеживания ошибок
     * \return shared_ptr на инициализированный sink
     */
    std::shared_ptr<error_tracking_sink<spdlog::details::null_mutex>> initErrorTrackingSink();

    /*!
     * \brief Инициализирует файловый sink
     * \param filepath Путь к файлу лога
     * \return shared_ptr на инициализированный sink
     * \throws spdlog::spdlog_ex при ошибках открытия файла
     */
    std::shared_ptr<spdlog::sinks::basic_file_sink_st> initFileSink(const std::filesystem::path& filepath);

private:
    static LoggerHandler instance;                                                        ///< Эксземпляр singleton
    std::shared_ptr<spdlog::sinks::dist_sink_st> m_sinkList;                              ///< Основной распределенный sink
    std::shared_ptr<spdlog::sinks::basic_file_sink_st> m_fileSink;                        ///< Sink для файлового вывода
    std::shared_ptr<error_tracking_sink<spdlog::details::null_mutex>> m_errorTrackerSink; ///< Sink для отслеживания ошибок
};

// Инициализация LoggerHandler-а для настройки глобального логера при запуске программы
LoggerHandler LoggerHandler::instance;

LoggerHandler::LoggerHandler() :
    m_sinkList(new spdlog::sinks::dist_sink_st), m_errorTrackerSink(initErrorTrackingSink())
{
    m_sinkList->set_sinks({ m_errorTrackerSink, initStdErrSink(), initStdOutSink() });
    set_default_logger(std::make_shared<spdlog::logger>("multi_sink", m_sinkList));
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern(logPattern);
}

LoggerHandler::~LoggerHandler()
{
    closeFileOutput();
}

void LoggerHandler::openFileOutput(const std::filesystem::path& filepath)
{
    // Закрываем ранее открытый файл.
    closeFileOutput();
    try
    {
        m_fileSink = initFileSink(filepath);
        m_sinkList->add_sink(m_fileSink);
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        rlog::error("Failed to open log file in directory: {}", filepath.string());
        throw;
    }

    rlog::debug("===== FILE {} LOGGING START =====", std::filesystem::path(m_fileSink->filename()).filename().string());
}

void LoggerHandler::closeFileOutput()
{
    if (!m_fileSink)
        return;
    rlog::debug("===== FILE {} LOGGING STOP =====", std::filesystem::path(m_fileSink->filename()).filename().string());
    m_sinkList->remove_sink(m_fileSink);
    m_fileSink.reset();
}

LoggerHandler& LoggerHandler::getInstance()
{
    return instance;
}

bool LoggerHandler::isFinishedWithError()
{
    return m_errorTrackerSink->getError();
}

std::shared_ptr<verbose_level_range_sink_st<spdlog::sinks::stdout_color_sink_st>> LoggerHandler::initStdOutSink()
{
    auto coutSinkRaw = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    coutSinkRaw->set_level(spdlog::level::debug);
    coutSinkRaw->set_color_mode(spdlog::color_mode::always);
    auto stdOutSink = std::make_shared<verbose_level_range_sink_st<spdlog::sinks::stdout_color_sink_st>>(coutSinkRaw, spdlog::level::debug, spdlog::level::debug);
    return stdOutSink;
}

std::shared_ptr<spdlog::sinks::stderr_color_sink_st> LoggerHandler::initStdErrSink()
{
    auto stdErrSink = std::make_shared<spdlog::sinks::stderr_color_sink_st>();
    stdErrSink->set_level(spdlog::level::warn);
    stdErrSink->set_color_mode(spdlog::color_mode::always);
    return stdErrSink;
}

std::shared_ptr<LoggerHandler::error_tracking_sink<spdlog::details::null_mutex>> LoggerHandler::initErrorTrackingSink()
{
    auto errorSink = std::make_shared<error_tracking_sink<spdlog::details::null_mutex>>();
    return errorSink;
}

std::shared_ptr<spdlog::sinks::basic_file_sink_st> LoggerHandler::initFileSink(const std::filesystem::path& filepath)
{
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_st>(filepath.string(), true);
    fileSink->set_level(spdlog::level::debug);
    fileSink->set_pattern(logPattern);
    return fileSink;
}

void openFile(const std::filesystem::path& filepath)
{
    LoggerHandler::getInstance().openFileOutput(filepath);
}

void closeFile()
{
    LoggerHandler::getInstance().closeFileOutput();
}

bool isFinishedWithError()
{
    return LoggerHandler::getInstance().isFinishedWithError();
}

PatternGuard::PatternGuard(const std::string& newPattern)
{
    spdlog::set_pattern(newPattern);
}

PatternGuard::~PatternGuard()
{
    spdlog::set_pattern(logPattern);
}

std::string generateProblemLine(const clang::Decl* const decl, const std::string& levelName, const std::string& msg)
{
    const auto& Ctx = decl->getASTContext();                                // Контекст AST (содержит информацию о типах, символах и т.п.)
    const auto& SM = Ctx.getSourceManager();                                // SourceManager управляет исходниками
    const clang::PresumedLoc ploc = SM.getPresumedLoc(decl->getLocation()); // Местоположение символа в исходниках

    const std::string filename = ploc.getFilename();
    const unsigned line_num = ploc.getLine();
    const unsigned column_num = ploc.getColumn();

    // Получаем начало и конец строки из исходника. Считаем, что её размер меньше 1000 сиволов
    const clang::SourceLocation lineStart = SM.translateLineCol(ploc.getFileID(), line_num, 1);
    const clang::SourceLocation lineEnd = clang::Lexer::getLocForEndOfToken(
    SM.translateLineCol(ploc.getFileID(), line_num, 1000),
    0, SM, Ctx.getLangOpts());

    // Извлекаем текст строки между lineStart и lineEnd, преобразуем к std::string
    const auto line_str = clang::Lexer::getSourceText(
                          clang::CharSourceRange::getCharRange(lineStart, lineEnd),
                          SM, Ctx.getLangOpts())
                          .str();

    // Динамически определяем ширину для номера строки. Минимум 5 символов отступа
    const int line_num_width = std::max(static_cast<int>(log10(line_num)) + 1, 4);

    // Вычисляем позицию каретки
    const size_t caret_pos = column_num - 1;

    return fmt::format(
    "{filename}:{line_num}:{column_num}: {levelName}: {msg}\n"
    " {line_num:>{line_num_width}} |{line_str}\n"
    " {space:>{line_num_width}} |{space:>{caret_pos}}^",
    "filename"_a = filename,
    "line_num"_a = line_num,
    "column_num"_a = column_num,
    "levelName"_a = levelName,
    "msg"_a = msg,
    "line_num_width"_a = line_num_width,
    "line_str"_a = line_str,
    "caret_pos"_a = caret_pos,
    "space"_a = "");
}

} // namespace rlog

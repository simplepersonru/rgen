#pragma once

#include <deque>
#include <string_view>
#include <vector>

#include <inja/inja.hpp>

#include "string_util.h"

namespace rgen
{

/*!
 * \brief Читает текстовое содержимое файла в строку
 * \param filePath - путь к файлу
 * \param isPrintError - Выводить сообщение об ошибке
 * \return возвращает строку содержимого
 */
std::string readFileToString(const std::string& filePath, bool isPrintError = true);

/*!
 * \brief Вывод полученного содержимого в процессе генерации в файл
 */
class IFilePrinter
{
public:
    virtual ~IFilePrinter() = default;

private:
    virtual void Print(std::string_view s) = 0;

public:
    friend IFilePrinter& operator<<(IFilePrinter& printer, std::string_view s)
    {
        printer.Print(s);
        return printer;
    }
};

/*!
 * \brief Хранение генерации и некоторой информации от отработавшего модуля для одного файла
 */
class Content
{
public:
    Content(fs::path dir, fs::path filename, std::string_view extension) :
        m_outDir(dir), m_filename(filename.string()), m_extension(extension) {}

    /*!
     * \brief Получить путь к файлу, куда будет совершена генерация
     */
    fs::path getGenerationFile() const;

    /*!
     * \brief Получить результат генерации модуля
     */
    std::string_view getGeneration() const { return m_dataGeneration; }

    /*!
     * \brief Только дописываем содержимое генерации, без перезаписи
     */
    Content& operator<<(const std::string_view s)
    {
        m_dataGeneration += s;
        return *this;
    }

private:
    std::string m_dataGeneration; // Результат работы модуля

    std::string m_extension;        // Расширение файла
    std::string m_filename;         // Имя файла хедера
    std::filesystem::path m_outDir; // Папка для вывода генерации
};

/*!
 * \brief Ответственный за агрегирование сгенерированных данных
 */
class ContentManager
{
public:
    ContentManager(std::filesystem::path outDir, std::filesystem::path filepath) :
        m_outDir(outDir), m_filepath(filepath)
    {
        if (!m_outDir.empty())
            fs::create_directories(m_outDir);
    }
    /*!
     * \brief Создать новый экземляр генерации и вернуть на него ссылку для заполнения
     */
    Content& getContentRef(const std::string_view extension);

    /*!
     * \brief Вывести по файлам содержимое m_contents
     */
    std::vector<std::filesystem::path> generate();

private:
    std::deque<Content> m_contents;   // результаты работы модулей
    std::filesystem::path m_outDir;   // Папка для вывода генерации
    std::filesystem::path m_filepath; // Путь к хедеру
};

std::unique_ptr<ContentManager> BuildContentManager(const std::filesystem::path& outDir, const std::filesystem::path& filepath);

void printInjaContent(Content& contentRef, const inja::json& injaJson, const std::string& templ);

} // namespace rgen

#pragma once

#include <core/command_line_args.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace rgen
{

/*!
 * \brief Загружает содержимое шаблона для генерации кода
 *
 * \details Функция выбирает путь к шаблону в следующем порядке:
 *          1. Если передан аргумент --template_dir: [template_dir]
 *          2. Если аргумент --template_dir НЕ передан (Fallback): [localTemplateDirPath]
 *
 * \param templateName Имя файла шаблона
 * \param localTemplateDirPath Fallback путь (используется только при отсутствии --template_dir)
 *
 * \return Содержимое файла шаблона
 */
inline std::string loadTemplate(const std::string& templateName, const std::string& localTemplateDirPath)
{
    std::string templateDir = cLine::Arg_templateDir;

    std::filesystem::path dirPath = templateDir.empty() ? std::filesystem::path(localTemplateDirPath)
                                                        : std::filesystem::path(templateDir);


    std::ifstream file(dirPath / templateName);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace rgen

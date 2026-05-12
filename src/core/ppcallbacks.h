#pragma once

#include <clang/Lex/PPCallbacks.h>

#include <string>
#include <vector>

namespace clang
{
class MacroArgs;
}

namespace rgen
{

/*!
 * \brief Макрос, вызванный в файле
 */
struct Macros
{
    std::string name;              /// Название макроса
    std::vector<std::string> args; /// Аргументы макроса
};

/*!
 * \brief Обработчик команд препроцессора
 */
class PreprocessorCallbacks : public clang::PPCallbacks
{
public:
    PreprocessorCallbacks(clang::Preprocessor& preprocessor);

    /*!
     * \brief Получить все макросы, вызванные в файле
     * \return Вектор из макросов
     */
    std::vector<Macros> getMacroses() const;

    /*!
     * \brief Обработчик вызова макросов в файле
     */
    void MacroExpands(const clang::Token& Tok, const clang::MacroDefinition&,
                      clang::SourceRange, const clang::MacroArgs* Args) override;

private:
    /*!
     * \brief Парсер аргументов макроса
     * \param Args - clang-класс аргументов макроса
     * \param idx - индекс аргумента макроса
     * \return Вектор строк, обозначающий аргументы макроса
     */
    std::vector<std::string> getArg(const clang::MacroArgs* Args, unsigned idx);

private:
    clang::Preprocessor& m_preprocessor;
    std::vector<Macros> m_macroses;
};

} // namespace rgen

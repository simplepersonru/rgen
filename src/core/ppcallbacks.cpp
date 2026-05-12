#include "ppcallbacks.h"

#include <clang/Lex/MacroArgs.h>
#include <clang/Tooling/Tooling.h>

namespace rgen
{

namespace

{

// Если у нас identifier или friend, то это признак сложного аргумента,
bool isComplexArg(const clang::Token& token)
{
    return token.is(clang::tok::identifier)
            || token.is(clang::tok::raw_identifier)
            || token.is(clang::tok::kw_friend);
}

}

PreprocessorCallbacks::PreprocessorCallbacks(clang::Preprocessor& preprocessor) :
    m_preprocessor(preprocessor) {}

std::vector<Macros> PreprocessorCallbacks::getMacroses() const
{
    return m_macroses;
}

void PreprocessorCallbacks::MacroExpands(const clang::Token& Tok, const clang::MacroDefinition&, clang::SourceRange, const clang::MacroArgs* Args)
{
    std::string macroName = m_preprocessor.getSpelling(Tok);
    std::vector<std::string> arguments;

    if (Args)
    {
        unsigned int numArgs = Args->getNumMacroArguments();
        arguments.reserve(numArgs);
        for (unsigned int i = 0; i < numArgs; i++)
        {
            std::vector<std::string> argTokens = getArg(Args, i);
            for (const auto& token : argTokens)
            {
                arguments.push_back(token);
            }
        }
    }

    m_macroses.emplace_back(macroName, std::move(arguments));
}

std::vector<std::string> PreprocessorCallbacks::getArg(const clang::MacroArgs* Args, unsigned int idx)
{
    const clang::Token* tokens = Args->getUnexpArgument(idx);
    std::vector<std::string> result;

    // Временная строка для накопления сложных идентификаторов
    std::string current;

    // Если аргумент пустой, то всё-равно добавляем пустую строку в результаты
    if(Args->getArgLength(tokens) == 0)
    {
        result.push_back("");
    }

    for (unsigned i = 0; i < Args->getArgLength(tokens); i++)
    {
        std::string tokenStr = m_preprocessor.getSpelling(tokens[i]);

        // Если у нас friend или идентификатор, при это предыдущий был таким же, то
        // между ними должен быть пробел.
        if(i != 0 && isComplexArg(tokens[i]) && isComplexArg(tokens[i - 1]) )
        {
            current += " " + tokenStr;
        }
        else if(tokens[i].is(clang::tok::comma))
        {
            result.push_back(current);
            current.clear();
        }
        else
        {
            current += tokenStr;
        }
    }

    // Не забываем последний накопленный токен
    if (!current.empty())
    {
        result.push_back(current);
    }

    return result;
}

} // namespace rgen

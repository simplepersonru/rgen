#include "comment.h"

#include "core/logger.h"
#include "string_util.h"

#include <clang/AST/ASTContext.h>
#include <clang/AST/Comment.h>

#include <sstream>

namespace rgen
{

namespace
{

std::string parseParagraph(const clang::comments::ParagraphComment& paragraph)
{
    std::stringstream ss;
    bool isFirstInsertion = true;
    for (const auto* child = paragraph.child_begin(); child != paragraph.child_end(); ++child)
    {
        if (const auto* textComment = clang::dyn_cast_or_null<clang::comments::TextComment>(*child))
        {
            auto text = std::string { textComment->getText() };
            // Убираем слева и справа от строки whitespaces
            for (auto prepText = stripSpaces(text); prepText != text;)
            {
                text = prepText;
                prepText = stripSpaces(text);
            }

            // Если кроме whitespaces ничего не было, то и добавлять нам нечего
            if (text.empty())
                continue;

            // Добавляем ручками перенос строки. На последней строке переноса быть не должно
            if (!isFirstInsertion)
                ss << '\n';
            else
                isFirstInsertion = false;

            ss << text;
        }
    }
    return ss.str();
}

} // namespace

DeclComments parseDeclComments(const clang::ASTContext& astContext, const clang::Decl& decl, bool useUncached)
{
    DeclComments result;

    const auto* comment = useUncached ? astContext.getLocalCommentForDeclUncached(&decl)
                                      : astContext.getCommentForDecl(&decl, /*PP=*/nullptr);
    if (!comment)
        return result;

    for (const auto* block : comment->getBlocks())
    {
        const auto* commentPtr = clang::dyn_cast_or_null<clang::comments::BlockCommandComment>(block);
        if (!commentPtr)
            continue;

        const auto& comment = *commentPtr;
        auto key = std::string { comment.getCommandName(astContext.getCommentCommandTraits()) };
        std::string text;
        if (const auto* paragraph = comment.getParagraph())
        {
            text = stripSpaces(parseParagraph(*paragraph));
        }
        result[std::move(key)] = std::move(text);
    }

    return result;
}

DeclComments::JsonResult declJson(const clang::Decl& decl, const std::string& key)
{
    auto comments = parseDeclComments(decl.getASTContext(), decl);
    auto jsonResult = comments.at_json(key);
    if (jsonResult.code == DeclComments::InvalidHjson)
    {
        rlog::error(&decl, "invalid hjson at '{}' comment:\n{}\nSee https://hjson.github.io/", key, prepareJsonToParse(comments.at(key)));
    }
    else if (jsonResult.code == DeclComments::EmptyStringValue)
    {
        rlog::error(&decl, "empty string hjson comment at '{}'. see https://hjson.github.io/", key);
    }
    return jsonResult;
}

std::string prepareJsonToParse(std::string inputHjson)
{
    stripSpaces(inputHjson);
    inputHjson.insert(1, "\n");
    inputHjson.insert(inputHjson.size() - 1, "\n");
    return inputHjson;
}

} // namespace rgen

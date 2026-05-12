#ifndef VISITORBASE_H
#define VISITORBASE_H

#include <clang/AST/RecursiveASTVisitor.h>
#include <core/comment.h>
#include <core/extracteddata.h>
#include <core/utils.h>
#include <optional>

namespace rgen
{

class Visitor : public clang::RecursiveASTVisitor<Visitor>
{
public:
    explicit Visitor(clang::ASTContext& astContext) :
        m_astContext { astContext } {}

    std::optional<ExtractedData> getExtractedData()
    {
        if (m_extractedData.declList.empty() && m_extractedData.commandsMap.empty())
        {
            return std::nullopt;
        }
        return m_extractedData;
    }

    bool VisitEnumDecl(clang::EnumDecl* decl)
    {
        processDecl(decl);
        return true;
    }

    bool VisitCXXRecordDecl(clang::CXXRecordDecl* decl)
    {
        processDecl(decl);
        return true;
    }

    bool VisitTypeAliasDecl(clang::TypeAliasDecl* decl)
    {
        processDecl(decl);
        return true;
    }

    bool VisitFunctionDecl(clang::FunctionDecl* decl)
    {
        processDecl(decl);
        return true;
    }

    bool VisitVarDecl(clang::VarDecl* decl)
    {
        processDecl(decl);
        return true;
    }

    ExtractedData getResult() const
    {
        return m_extractedData;
    }

    static ExtractedData extractAll(clang::ASTContext& astContext)
    {
        Visitor extractor(astContext);
        extractor.TraverseDecl(astContext.getTranslationUnitDecl());
        return extractor.getResult();
    }

protected:
    // Обработка декларации
    void processDecl(clang::Decl* decl)
    {
        const utils::AstVisitorHeaderHelper helper(decl);
        if (helper.breakVisit)
        {
            return;
        }

        const auto comments = parseDeclComments(m_astContext, *decl);

        if (comments.empty())
        {
            return;
        }

        if (m_extractedData.fullHeaderPath.empty())
        {
            m_extractedData.fullHeaderPath = helper.headerPath;
        }

        m_extractedData.declList.push_back(decl);

        for (const auto& [key, value] : comments)
        {
            m_extractedData.commandsMap[key].push_back(decl);
        }
    }

    clang::ASTContext& m_astContext;
    ExtractedData m_extractedData;
};

} // namespace rgen

#endif // VISITORBASE_H

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/Tooling.h>
#include <gtest/gtest.h>

#include <core/logger.h>
#include <core/ppcallbacks.h>

#include <fmt/ranges.h>

namespace
{
static std::vector<rgen::Macros> g_macroses;
}

class PreprocessorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        g_macroses.clear();
    }
};

class TestPreprocessorCallbacks : public rgen::PreprocessorCallbacks
{
public:
    TestPreprocessorCallbacks(clang::Preprocessor& preprocessor) :
        rgen::PreprocessorCallbacks(preprocessor) {}

    void MacroExpands(const clang::Token& Tok, const clang::MacroDefinition& MacroDefinition,
                      clang::SourceRange SourceRange, const clang::MacroArgs* Args) override
    {
        rgen::PreprocessorCallbacks::MacroExpands(Tok, MacroDefinition, SourceRange, Args);
        g_macroses = getMacroses();
    }
};

class TestFrontendAction : public clang::ASTFrontendAction
{
public:
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance& compiler,
    llvm::StringRef file) override
    {
        clang::Preprocessor& preprocessor = compiler.getPreprocessor();
        preprocessor.addPPCallbacks(std::make_unique<TestPreprocessorCallbacks>(preprocessor));
        return std::make_unique<clang::ASTConsumer>();
    }
};

bool RunCodeAnalyze(const std::string& code)
{
    auto action = std::make_unique<TestFrontendAction>();

    return clang::tooling::runToolOnCodeWithArgs(
    std::move(action),
    code,
    { "-std=c++17" });
}

TEST_F(PreprocessorTest, SimpleTest)
{
    std::string code = R"(
#define MY_MACROS(a)
MY_MACROS(1)
)";
    bool analyzeCode = RunCodeAnalyze(code);
    ASSERT_TRUE(analyzeCode);

    ASSERT_EQ(g_macroses.size(), 1);

    EXPECT_EQ(g_macroses.at(0).name, "MY_MACROS");

    ASSERT_EQ(g_macroses.at(0).args.size(), 1);
    EXPECT_EQ(g_macroses.at(0).args.at(0), "1");
}

TEST_F(PreprocessorTest, SpacingArgs)
{
    std::string code = R"(
#define MY_MACROS(a, b)
MY_MACROS(  1  ,  2  )
)";
    bool analyzeCode = RunCodeAnalyze(code);
    ASSERT_TRUE(analyzeCode);

    ASSERT_EQ(g_macroses.size(), 1);

    ASSERT_EQ(g_macroses.at(0).args.size(), 2);
    EXPECT_EQ(g_macroses.at(0).args.at(0), "1");
    EXPECT_EQ(g_macroses.at(0).args.at(1), "2");
}

TEST_F(PreprocessorTest, NamespaceLikeArgument)
{
    std::string code = R"(
#define MY_MACROS(a)
MY_MACROS(Namespace::Class)
)";
    bool analyzeCode = RunCodeAnalyze(code);
    ASSERT_TRUE(analyzeCode);

    ASSERT_EQ(g_macroses.size(), 1);

    ASSERT_EQ(g_macroses.at(0).args.size(), 1);
    EXPECT_EQ(g_macroses.at(0).args.at(0), "Namespace::Class");
}

TEST_F(PreprocessorTest, VariadicMacrosSimple)
{
    std::string code = R"(
#define MY_MACROS(...)
MY_MACROS(a, b, c)
)";
    bool analyzeCode = RunCodeAnalyze(code);
    ASSERT_TRUE(analyzeCode);

    ASSERT_EQ(g_macroses.size(), 1);

    ASSERT_EQ(g_macroses.at(0).args.size(), 3);
    EXPECT_EQ(g_macroses.at(0).args.at(0), "a");
    EXPECT_EQ(g_macroses.at(0).args.at(1), "b");
    EXPECT_EQ(g_macroses.at(0).args.at(2), "c");
}

TEST_F(PreprocessorTest, VariadicMacrosSpacingWithArg)
{
    std::string code = R"(
#define MY_MACROS(type, ...)
MY_MACROS(  a  ,  b  ,  c  )
)";
    bool analyzeCode = RunCodeAnalyze(code);
    ASSERT_TRUE(analyzeCode);

    ASSERT_EQ(g_macroses.size(), 1);

    ASSERT_EQ(g_macroses.at(0).args.size(), 3);
    EXPECT_EQ(g_macroses.at(0).args.at(0), "a");
    EXPECT_EQ(g_macroses.at(0).args.at(1), "b");
    EXPECT_EQ(g_macroses.at(0).args.at(2), "c");
}

TEST_F(PreprocessorTest, NextLineArgs)
{
    std::string code = R"(
#define MY_MACROS(a, b) \
a \
b;

MY_MACROS(
int,
var
)
)";
    bool analyzeCode = RunCodeAnalyze(code);
    ASSERT_TRUE(analyzeCode);

    ASSERT_EQ(g_macroses.size(), 1);

    ASSERT_EQ(g_macroses.at(0).args.size(), 2);
    EXPECT_EQ(g_macroses.at(0).args.at(0), "int");
    EXPECT_EQ(g_macroses.at(0).args.at(1), "var");
}

TEST_F(PreprocessorTest, TabArgs)
{
    std::string code = R"(
#define MY_MACROS(a, b) a b;

MY_MACROS(  int ,    var )
)";
    bool analyzeCode = RunCodeAnalyze(code);
    ASSERT_TRUE(analyzeCode);

    ASSERT_EQ(g_macroses.size(), 1);

    ASSERT_EQ(g_macroses.at(0).args.size(), 2);
    EXPECT_EQ(g_macroses.at(0).args.at(0), "int");
    EXPECT_EQ(g_macroses.at(0).args.at(1), "var");
}

TEST_F(PreprocessorTest, EmptyArgs)
{
    std::string code = R"(
#define MY_MACROS(a, b)

MY_MACROS(   ,  var )
)";
    bool analyzeCode = RunCodeAnalyze(code);
    ASSERT_TRUE(analyzeCode);

    ASSERT_EQ(g_macroses.size(), 1);

    ASSERT_EQ(g_macroses.at(0).args.size(), 2);
    EXPECT_EQ(g_macroses.at(0).args.at(0), "");
    EXPECT_EQ(g_macroses.at(0).args.at(1), "var");

}

TEST_F(PreprocessorTest, ComplexArgs)
{
    std::string code = R"(
#define MY_MACROS(a, b)
#define RGEN_DECL_EXPORT

MY_MACROS(friend RGEN_DECL_EXPORT,  RGEN_DECL_EXPORT friend )
)";
    bool analyzeCode = RunCodeAnalyze(code);
    ASSERT_TRUE(analyzeCode);

    ASSERT_EQ(g_macroses.size(), 1);

    ASSERT_EQ(g_macroses.at(0).args.size(), 2);
    EXPECT_EQ(g_macroses.at(0).args.at(0), "friend RGEN_DECL_EXPORT");
    EXPECT_EQ(g_macroses.at(0).args.at(1), "RGEN_DECL_EXPORT friend");

}

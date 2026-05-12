#include <core/comment.h>
#include <gtest/gtest.h>

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/ADT/StringRef.h>

#include <vector>

using namespace rgen;

namespace
{

const std::vector<std::string> ARGS = {
    "-fparse-all-comments",
    "-fcomment-block-commands=serializable,stringvalue,hjsoncommand",
};

// Parse comment from any VarDecl
DeclComments ParseCommentDataFromVarDecl(const clang::ASTContext& ctx)
{
    struct Visitor : clang::RecursiveASTVisitor<Visitor>
    {
        Visitor(const clang::ASTContext& ctx) :
            m_astContext { ctx } {}

        bool VisitVarDecl(clang::VarDecl* decl)
        {
            declComments = parseDeclComments(m_astContext, *decl);
            return true;
        }

        const clang::ASTContext& m_astContext;
        DeclComments declComments;
    } visitor { ctx };
    visitor.TraverseTranslationUnitDecl(ctx.getTranslationUnitDecl());

    return std::move(visitor.declComments);
}

// Тестируем различные формат ввода комментария.
// Подразумеваются следующие поля и данные:
// "x": 4
// "y": 6
// "z": "hello, boy!"
// "a": [1, 2, 3]
void testStandardHjsonParsing(std::string_view code, const std::string& fieldName)
{
    llvm::StringRef codeRef { code.data(), code.size() };

    auto unit = clang::tooling::buildASTFromCodeWithArgs(codeRef, ARGS);
    auto declComments = ParseCommentDataFromVarDecl(unit->getASTContext());

    // check missing commands
    ASSERT_EQ(declComments.at_optional("x"), std::nullopt);
    ASSERT_EQ(declComments.at_optional("y"), std::nullopt);
    ASSERT_EQ(declComments.at_optional("z"), std::nullopt);
    ASSERT_EQ(declComments.at_optional("a"), std::nullopt);

    auto hjsoncommandComment = declComments.at_json(fieldName);
    ASSERT_EQ(hjsoncommandComment.code, DeclComments::JsonExitCode::Success);
    {
        auto j = hjsoncommandComment.json;
        EXPECT_TRUE(j.contains("x"));
        EXPECT_TRUE(j.contains("y"));
        EXPECT_TRUE(j.contains("z"));
        EXPECT_TRUE(j.contains("a"));

        ASSERT_EQ(j.at("x"), 4);
        ASSERT_EQ(j.at("y"), 6);
        ASSERT_EQ(j.at("z"), "hello, boy!");

        const std::vector<int> vectorCompare = {1, 2, 3};
        ASSERT_EQ(j.at("a").get<std::vector<int>>(), vectorCompare);
    }
}

void testHJsonError(std::string_view code, const std::string& fieldName, DeclComments::JsonExitCode exitCodeExpecting)
{
    llvm::StringRef codeRef { code.data(), code.size() };

    auto unit = clang::tooling::buildASTFromCodeWithArgs(codeRef, ARGS);
    auto declComments = ParseCommentDataFromVarDecl(unit->getASTContext());

    auto hjsoncommandComment = declComments.at_json(fieldName);

    ASSERT_EQ(hjsoncommandComment.json, nlohmann::json{});
    ASSERT_EQ(hjsoncommandComment.code, exitCodeExpecting);
}

} // namespace

TEST(CommentTest, Simple)
{
    constexpr std::string_view code = R"(
    /// @brief just `foo` value, nothing special
    /// \serializable
    /// @stringvalue FOO Foo foo
    int foo = 12345;
)";

    llvm::StringRef codeRef { code.data(), code.size() };

    auto unit = clang::tooling::buildASTFromCodeWithArgs(codeRef, ARGS);
    auto declComments = ParseCommentDataFromVarDecl(unit->getASTContext());

    // check size
    ASSERT_EQ(declComments.size(), 3);

    // check missing commands
    ASSERT_EQ(declComments.at_optional("author"), std::nullopt);
    ASSERT_EQ(declComments.at_optional("wrongname"), std::nullopt);

    // check existing commands
    auto briefComment = declComments.at_optional("brief");
    ASSERT_EQ(briefComment.has_value(), true);
    {
        const auto& [attribute, text] = briefComment.value();
        ASSERT_STREQ(attribute.c_str(), "brief");
        ASSERT_STREQ(text.c_str(), "just `foo` value, nothing special");
    }

    auto serializableComment = declComments.at_optional("serializable");
    ASSERT_EQ(serializableComment.has_value(), true);
    {
        const auto& [attribute, text] = serializableComment.value();
        ASSERT_STREQ(attribute.c_str(), "serializable");
        ASSERT_STREQ(text.c_str(), "");
    }

    auto stringvalueComment = declComments.at_optional("stringvalue");
    ASSERT_EQ(stringvalueComment.has_value(), true);
    {
        const auto& [attribute, text] = stringvalueComment.value();
        ASSERT_STREQ(attribute.c_str(), "stringvalue");
        ASSERT_STREQ(text.c_str(), "FOO Foo foo");
    }
}

TEST(HJsonCommentTest, EmptyStringError)
{
    constexpr std::string_view code = R"(
    /*!
     * \hjsoncommand
     */
    int foo = 12345;
)";

    testHJsonError(code, "hjsoncommand", DeclComments::JsonExitCode::EmptyStringValue);
}

TEST(HJsonCommentTest, WrongKeyError)
{
    constexpr std::string_view code = R"(
    /*!
     * \serializable
     */
    int foo = 12345;
)";

    testHJsonError(code, "hjsoncommand", DeclComments::JsonExitCode::KeyNotFound);
}

TEST(HJsonCommentTest, HJsonFormatError)
{
    constexpr std::string_view code = R"(
    /*!
     * \hjsoncommand {
            // Специально опускаем "]" в конце, должна быть ошибка
            list: [
                1
                2
                3
        }
     */
    int foo = 12345;
)";

    testHJsonError(code, "hjsoncommand", DeclComments::JsonExitCode::InvalidHjson);
}

TEST(HJsonCommentTest, CompactFormatStandardJson)
{
    constexpr std::string_view code = R"(
    /*!
     * \hjsoncommand {"x":4,"y":6,"z":"hello, boy!","a":[1,2,3]}
     * \serializable
     */
    int foo = 12345;
)";
    testStandardHjsonParsing(code, "hjsoncommand");
}

TEST(HJsonCommentTest, CompactFormatHJson)
{
    constexpr std::string_view code = R"(
    /*!
     * \hjsoncommand {x:4,y:6,z:"hello, boy!",a:[1,2,3]}
     * \serializable
     */
    int foo = 12345;
)";
    testStandardHjsonParsing(code, "hjsoncommand");
}

TEST(HJsonCommentTest, PrettyFormatStandardJson)
{
    constexpr std::string_view code = R"(
    /*!
     * \hjsoncommand {
                        "x": 4,
                        "y": 6,
                        "z": "hello, boy!",
                        "a": [1, 2, 3]
                     }
     * \serializable
     */
    int foo = 12345;
)";
    testStandardHjsonParsing(code, "hjsoncommand");
}

TEST(HJsonCommentTest, PrettyFormatHJson)
{
    constexpr std::string_view code = R"(
    /*!
     * \hjsoncommand {
                        x: 4
                        y: 6
                        // This value for someone
                        z: hello, boy!
                        a: [1, 2, 3]
                     }
     * \serializable
     */
    int foo = 12345;
)";
    testStandardHjsonParsing(code, "hjsoncommand");
}

// Пустые строки при записи значения параметра комментария недопустимы
TEST(HJsonCommentTest, EmptyLineInCommentHJsonError)
{
    constexpr std::string_view code = R"(
    /*!
     * \hjsoncommand {
                        x: 4
                        y: 6

                        z: hello, boy!
                        a: [1, 2, 3]
                     }
     * \serializable
     */
    int foo = 12345;
)";
    testHJsonError(code, "hjsoncommand", DeclComments::JsonExitCode::InvalidHjson);
}

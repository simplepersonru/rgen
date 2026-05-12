#include <core/string_util.h>
#include <gtest/gtest.h>

using namespace rgen::fPath;

TEST(StringUtilTest, AfterLastSlashSimple)
{
#ifdef _WIN32
    constexpr std::string_view s = "C:\\home\\me\\some\\folder\\inside\\cool_file.cpp";
#else
    constexpr std::string_view s = "/home/me/some/folder/inside/cool_file.cpp";
#endif

    ASSERT_STREQ(rgen::afterLastSlash(s).data(), "cool_file.cpp");
}

TEST(StringUtilTest, AfterLastSlashNoSlash)
{
    constexpr std::string_view s = "cool_file.cpp";
    ASSERT_STREQ(rgen::afterLastSlash(s).data(), "cool_file.cpp");
}

TEST(StringUtilTest, InsertBeforeExtSimple)
{
    constexpr std::string_view s = "foo.cpp";
    constexpr std::string_view extra = "extra";
    ASSERT_STREQ(rgen::insertBeforeExt(s, extra).data(), "foo.extra.cpp");
}

TEST(StringUtilTest, InsertBeforeExtSlashesAndDots)
{
    constexpr std::string_view s = "/a/b/c/foo.header.cpp";
    constexpr std::string_view extra = "long_word";
    ASSERT_STREQ(rgen::insertBeforeExt(s, extra).data(), "/a/b/c/foo.long_word.cpp");
}

TEST(StringUtilTest, RemoveLastExtSimple)
{
    constexpr std::string_view s = "foo.extra.cpp";
    ASSERT_STREQ(std::string { rgen::removeLastExt(s) }.c_str(), "foo.extra");
}

TEST(StringUtilTest, RemoveLastExtNoExt)
{
    constexpr std::string_view s = "very_long_word";
    ASSERT_STREQ(std::string { rgen::removeLastExt(s) }.c_str(), "very_long_word");
}

TEST(StringUtilTest, SplitBySpaceSimple)
{
    constexpr std::string_view s = " aaa  bbbb ccccc";
    const auto split = rgen::splitBySpace(s);
    ASSERT_EQ(split.size(), 3);
    ASSERT_STREQ(std::string { split[0] }.c_str(), "aaa");
    ASSERT_STREQ(std::string { split[1] }.c_str(), "bbbb");
    ASSERT_STREQ(std::string { split[2] }.c_str(), "ccccc");
}

TEST(StringUtilTest, SplitBySpaceSinglePart)
{
    constexpr std::string_view s = "aaa";
    const auto split = rgen::splitBySpace(s);
    ASSERT_EQ(split.size(), 1);
    ASSERT_STREQ(std::string { split[0] }.c_str(), "aaa");
}

TEST(StringUtilTest, splitByDelim)
{
    constexpr std::string_view s = "/employees/{id}/info/{type}";
    const auto split = rgen::splitByDelim(s, '/');
    ASSERT_EQ(split.size(), 4);
    ASSERT_STREQ(std::string { split[0] }.c_str(), "employees");
    ASSERT_STREQ(std::string { split[1] }.c_str(), "{id}");
    ASSERT_STREQ(std::string { split[2] }.c_str(), "info");
    ASSERT_STREQ(std::string { split[3] }.c_str(), "{type}");
}

TEST(StringUtilTest, SplitByDelims)
{
    constexpr std::string_view s = "/employees/find?lowerBound={lowerBound}&upperBound={upperBound}";
    const auto split = rgen::splitByDelims(s, "/?=&");
    ASSERT_EQ(split.size(), 6);
    ASSERT_STREQ(std::string { split[0] }.c_str(), "employees");
    ASSERT_STREQ(std::string { split[1] }.c_str(), "find");
    ASSERT_STREQ(std::string { split[2] }.c_str(), "lowerBound");
    ASSERT_STREQ(std::string { split[3] }.c_str(), "{lowerBound}");
    ASSERT_STREQ(std::string { split[4] }.c_str(), "upperBound");
    ASSERT_STREQ(std::string { split[5] }.c_str(), "{upperBound}");
}

TEST(StringUtilTest, replaceLastExt)
{
    constexpr std::string_view s = "some.cpp";
    constexpr std::string_view extra = ".h";
    ASSERT_STREQ(rgen::replaceLastExt(s, extra).data(), "some.h");
}

TEST(StringUtilTest, ReplaceLastExtExtra)
{
    constexpr std::string_view s = "some.extra.cpp";
    constexpr std::string_view extra = ".h";
    ASSERT_STREQ(rgen::replaceLastExt(s, extra).data(), "some.extra.h");
}

TEST(StringUtilTest, endsWith)
{
    const std::string str = "SomeText";
    const std::string trueSfx = "xt";
    ASSERT_EQ(rgen::endsWith(str, trueSfx), true);

    const std::string wrongSfx = "ixt";
    ASSERT_EQ(rgen::endsWith(str, wrongSfx), false);
}

TEST(StringUtilTest, StdRegEx)
{
    const std::string str0 = "empty";
    ASSERT_EQ(rgen::removeStd(str0), str0);

    const std::string str1 = "std::string";
    ASSERT_EQ(rgen::removeStd(str1), "string");

    const std::string str2 = "std::vector<std::string>";
    ASSERT_EQ(rgen::removeStd(str2), "vector<string>");

    const std::string str3 = "std::map<std::string, std::vector<std::string>>";
    ASSERT_EQ(rgen::removeStd(str3), "map<string, vector<string>>");

    const std::string str4 = "other::std::map<stdlone::SomeClass, std::vector<_std::other>>";
    ASSERT_EQ(rgen::removeStd(str4), "other::std::map<stdlone::SomeClass, vector<_std::other>>");
}

TEST(StringUtilTest, RemoveDublicate)
{
    const std::string str = "__string____some___";
    ASSERT_EQ(rgen::removeSymbolDublicate(str, '_'), "_string_some_");

    const std::string str0 = "stringSome";
    ASSERT_EQ(rgen::removeSymbolDublicate(str0, '_'), str0);
}

TEST(StringUtilTest, RemoveAllEndSymbol)
{
    const std::string str = "__string____some___";
    ASSERT_EQ(rgen::removeAllEndSymbol(str, '_'), "__string____some");

    const std::string str0 = "stringSome";
    ASSERT_EQ(rgen::removeAllEndSymbol(str0, '_'), str0);
}

TEST(StringUtilTest, underscore)
{
    const std::vector<char> needReplace = { ':', '<', '>' };
    const char toReplace = '_';

    const std::string strOne = "::one";
    ASSERT_EQ(rgen::replaceSymbols(strOne, needReplace, toReplace), "__one");

    const std::string strTwo = "_two:<>";
    ASSERT_EQ(rgen::replaceSymbols(strTwo, needReplace, toReplace), "_two___");

    const std::string strThree = "std::vector<std::list<std::string>>";
    ASSERT_EQ(rgen::replaceSymbols(strThree, needReplace, toReplace), "std__vector_std__list_std__string__");

    const std::string strNope = "someText-and simple";
    ASSERT_EQ(rgen::replaceSymbols(strNope, needReplace, toReplace), strNope);
}

TEST(StringUtilTest, transformTypeToStringforPython)
{
    auto transformTypeToStringForPython_fromSWIG = [](std::string typeString) {
        const char underscore = '_';

        const std::string removedStd = rgen::removeStd(typeString);
        const std::string underscoring = rgen::replaceSymbols(removedStd, { ':', '<', '>' }, underscore);
        const std::string removedEndUnderscore = rgen::removeAllEndSymbol(underscoring, underscore);
        return rgen::removeSymbolDublicate(removedEndUnderscore, underscore);
    };

    const std::string str1 = "other::std::map<stdlone::SomeClass, std::vector<_std::other>>";

    ASSERT_EQ(transformTypeToStringForPython_fromSWIG(str1), "other_std_map_stdlone_SomeClass, vector_std_other");
}

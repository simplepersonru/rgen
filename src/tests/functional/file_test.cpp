#include <core/common_constants.h>
#include <core/file.h>
#include <core/utils.h>

#include <fstream>

#include <fmt/core.h>
#include <gtest/gtest.h>

using namespace rgen;

/*!
 * \brief Получить из файла все содержимое, включая переносы строк
 */
static std::string Slurp(std::ifstream& in)
{
    std::ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}

// Проверка создания файлов в файловой системе
TEST(FileTest, SomeExtension)
{
    fs::path outputDir {};
    fs::path headerPath = "testHeader_1.h";

    {
        auto mgr = BuildContentManager(outputDir, headerPath);
        for (int i = 0; i < 10; ++i)
        {
            mgr->getContentRef("ex" + std::to_string(i)) << "conTest_" << std::to_string(i);
        }

        mgr->generate();
    }

    {
        for (int i = 0; i < 10; ++i)
        {
            std::ifstream stream { fmt::format("{}.ex{}", headerPath.stem().string(), i) };

            std::string expected = "conTest_" + std::to_string(i);
            ASSERT_STREQ(Slurp(stream).c_str(), expected.c_str());
        }
    }
}

// Проверка создания файлов с разными расширениями, но пересекающимися данными
TEST(FileTest, TwoExtension)
{
    fs::path outputDir {};
    fs::path headerPath = "testHeader_2.h";

    {
        auto mgr = BuildContentManager(outputDir, headerPath);
        for (int i = 0; i < 6; ++i)
        {
            auto& content = mgr->getContentRef(i % 2 ? "one" : "two");
            content << std::to_string(i) << "\n";
        }

        mgr->generate();
    }

    {
        std::ifstream stream { fmt::format("{}.one", headerPath.stem().string()) };
        std::string expected = "1\n3\n5\n";
        ASSERT_STREQ(Slurp(stream).c_str(), expected.c_str());
    }

    {
        std::ifstream stream { fmt::format("{}.two", headerPath.stem().string()) };
        std::string expected = "0\n2\n4\n";
        ASSERT_STREQ(Slurp(stream).c_str(), expected.c_str());
    }
}

// Проверка создания файлов с разными расширениями с указанием папки
TEST(FileTest, TwoExtensionWithDir)
{
    fs::path outputDir { "testDir/moreDir" };
    fs::path headerPath = "/home/rep/dir/testHeader_3.h";

    {
        auto mgr = BuildContentManager(outputDir, headerPath);
        for (int i = 0; i < 6; ++i)
        {
            auto& content = mgr->getContentRef(i % 2 ? "four" : "five");
            content << std::to_string(i) << "\n";
        }

        mgr->generate();
    }

    {
        std::ifstream stream { outputDir / fmt::format("{}.four", headerPath.stem().string()) };
        std::string expected = "1\n3\n5\n";
        ASSERT_STREQ(Slurp(stream).c_str(), expected.c_str());
    }

    {
        std::ifstream stream { outputDir / fmt::format("{}.five", headerPath.stem().string()) };
        std::string expected = "0\n2\n4\n";
        ASSERT_STREQ(Slurp(stream).c_str(), expected.c_str());
    }
}

TEST(FileTest, GeneralRelativeLibDir)
{
    std::string libDir = "/project/build/Debug_redkit_123/cim-shared.234/cim_shared";
    std::string headerSource = "/project/cim_shared/odb/somelib/someThere/file.h";
    std::string result = utils::getRelativeLibPath(libDir, headerSource);
    EXPECT_EQ(result, "odb/somelib/someThere");
}

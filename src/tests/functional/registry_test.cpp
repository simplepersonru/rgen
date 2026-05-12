#include <core/common_constants.h>
#include <core/registry.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include <fmt/core.h>
#include <gtest/gtest.h>
#include <thirdparty/include/nlohmann/json.hpp>

using namespace rgen;

namespace
{
/*!
 * \brief Заполнение модуля какими-то данными
 */
void fillData()
{
    // Перед заполненем, следует очистить от предыдущих данных
    ModuleRegistry::ClearModules();

    {
        ModuleInfo info1 {
            .Commands = { { "hello1" } },
            .FileExtensions = { { "rgen.cxx" } },
            .Includes = { "include 1",
                          "includeMore",
                          "include2More" }
        };
        ModuleRegistry::AddModule(Module { "dummy", nullptr, info1 });
    }

    {
        ModuleInfo info2 {
            .Commands = { { "hello2" } },
            .FileExtensions = { { "rgen.hxx" } },
            .IsDefault = false
        };
        ModuleRegistry::AddModule(Module { "otherModule", nullptr, info2 });
    }

    {
        ModuleInfo info3 {
            .Commands = { { "hello4" } },
            .FileExtensions = { { "rgen.cxx" }, { "rgen.foo" }, { "rgen.MoreFoo" } },
            .Includes = { "include" }
        };
        ModuleRegistry::AddModule(Module { "kek", nullptr, info3 });
    }

    {
        ModuleInfo info4 {
            .Commands = { { "hello3" } },
            .Includes = { "include" }
        };
        ModuleRegistry::AddModule(Module { "someMod", nullptr, info4 });
    }

    {
        ModuleInfo info5 {
            .Commands = { { "hello5" } },
            .FileExtensions = { { "rgen.in" } },
        };
        ModuleRegistry::AddModule(Module { "hloi", nullptr, info5 });
    }
}

} // namespace

TEST(RegistryTest, AddModule)
{
    for (int i = 0; i < 10; ++i)
    {
        ModuleRegistry::AddModule(Module {});
    }
    ASSERT_EQ(ModuleRegistry::GetModules().size(), 10);

    ModuleRegistry::ClearModules();
}

TEST(RegistryTest, RegistratorMacro)
{
    REGISTER_MODULE(dummy, nullptr, {});
    ASSERT_EQ(ModuleRegistry::GetModules().size(), 1);

    ModuleRegistry::ClearModules();
}

TEST(RegistryTest, FileExtensionsMacros)
{
    {
        ModuleInfo info1;
        info1.FileExtensions.push_back({ "cxx" });

        REGISTER_MODULE(dummy, nullptr, info1);
        ASSERT_EQ(ModuleRegistry::GetModules().size(), 1);
        ASSERT_EQ(ModuleRegistry::GetModules()[0].Info.FileExtensions, info1.FileExtensions);
    }

    {
        REGISTER_MODULE(otherModule, nullptr, {});
        ASSERT_EQ(ModuleRegistry::GetModules().size(), 2);
        ASSERT_EQ(ModuleRegistry::GetModules()[1].Info.FileExtensions.size(), 0);
    }

    {
        FileExtension ext { "hxx" };
        REGISTER_MODULE(kek, nullptr, { .FileExtensions = { ext } });
        ASSERT_EQ(ModuleRegistry::GetModules().size(), 3);
        ASSERT_EQ(ModuleRegistry::GetModules()[2].Info.FileExtensions[0].Extension, ext.Extension);
    }

    {
        REGISTER_MODULE(someMod, nullptr, { .FileExtensions = { { "d" } } });
        ASSERT_EQ(ModuleRegistry::GetModules().size(), 4);
        ASSERT_EQ(ModuleRegistry::GetModules()[3].Info.FileExtensions, std::vector<FileExtension> { { { "d" } } });
    }

    {
        std::vector<FileExtension> some { { "cxx" }, { "hxx" }, { "aaa" } };
        REGISTER_MODULE(hloi, nullptr, { .FileExtensions = some });
        ASSERT_EQ(ModuleRegistry::GetModules().size(), 5);
        ASSERT_EQ(ModuleRegistry::GetModules()[4].Info.FileExtensions, some);
    }

    ModuleRegistry::ClearModules();
    ASSERT_EQ(ModuleRegistry::GetModules().size(), 0);
}

TEST(RegistryTest, FileNames)
{
    /* Подготовка данных */
    fillData();
    ASSERT_EQ(ModuleRegistry::GetModules().size(), 5);

    std::string filename = "someFilename";
    const std::unordered_set<std::string> actualFilenames = ModuleRegistry::getArtifactsFilenames(filename);
    ASSERT_EQ(actualFilenames.size(), 5);

    const std::vector<std::string> expectedData {
        fmt::format("{}.rgen.cxx", filename),
        fmt::format("{}.rgen.hxx", filename),
        fmt::format("{}.rgen.foo", filename),
        fmt::format("{}.rgen.MoreFoo", filename),
        fmt::format("{}.rgen.in", filename),
    };

    /*  Тестирование */
    for (const auto& exp : expectedData)
        ASSERT_TRUE(actualFilenames.find(exp) != actualFilenames.end());

    std::string broken = fmt::format("/{}.rgen.cxx", filename);
    ASSERT_FALSE(actualFilenames.find(broken) != actualFilenames.end());

    /* Очистка */
    ModuleRegistry::ClearModules();
}

TEST(RegistryTest, FileNamesWitgOutputDir)
{
    /* Подготовка данных */
    fillData();
    ASSERT_EQ(ModuleRegistry::GetModules().size(), 5);

    std::string filename = "someFilename";
#ifdef _WIN32
    std::filesystem::path outputDir = R"(C:\reps\rep)";
#else
    std::filesystem::path outputDir = "/home/somedir/rep";
#endif

    const std::unordered_set<std::string> actualFilenames = ModuleRegistry::getArtifactsFilenames(filename, outputDir);
    ASSERT_EQ(actualFilenames.size(), 5);

    const std::vector<std::string> expectedData {
        (outputDir / fmt::format("{}.rgen.cxx", filename)).string(),
        (outputDir / fmt::format("{}.rgen.hxx", filename)).string(),
        (outputDir / fmt::format("{}.rgen.cxx", filename)).string(),
        (outputDir / fmt::format("{}.rgen.foo", filename)).string(),
        (outputDir / fmt::format("{}.rgen.MoreFoo", filename)).string(),
        (outputDir / fmt::format("{}.rgen.in", filename)).string(),
    };

    /*  Тестирование */
    for (const auto& exp : expectedData)
        ASSERT_TRUE(actualFilenames.find(exp) != actualFilenames.end());

    std::string broken = fmt::format("{}.rgen.in", filename);
    ASSERT_FALSE(actualFilenames.find(broken) != actualFilenames.end());

    /* Очистка */
    ModuleRegistry::ClearModules();
}

TEST(RegistryTest, JsonInfoModules)
{
    fillData();
    using json = nlohmann::json;

    auto jExpected = json::parse(ModuleRegistry::getJsonModuleExtensions());
    auto jActual = json::parse(R"JS(
[
    {
        "default": true,
        "extension": [
            {
                "fileTag": "",
                "name": ".rgen.cxx"
            }
        ],
        "include": [
            "include 1",
            "includeMore",
            "include2More"
        ],
        "name": "dummy"
    },
    {
        "default": false,
        "extension": [
            {
                "fileTag": "",
                "name": ".rgen.hxx"
            }
        ],
        "name": "otherModule"
    },
    {
        "default": true,
        "extension": [
            {
                "fileTag": "",
                "name": ".rgen.cxx"
            },
            {
                "fileTag": "",
                "name": ".rgen.foo"
            },
            {
                "fileTag": "",
                "name": ".rgen.MoreFoo"
            }
        ],
        "include": [
            "include"
        ],
        "name": "kek"
    },
    {
        "default": true,
        "extension": [
            {
                "fileTag": "",
                "name": ".rgen.in"
            }
        ],
        "name": "hloi"
    }
] )JS");

    ASSERT_EQ(jExpected, jActual);
}

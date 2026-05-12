#include "fmt/format.h"
#include <core/process.h>
#include <gtest/gtest.h>

namespace
{

struct StandaloneTestResult
{
    ProcessResult clean;
    ProcessResult run;
};

bool runTest(std::string testDir)
{
    const std::string testWorkDir = fmt::format("{}/{}", TESTS_WORK_DIR, testDir);
    const std::string build_dir = "build_qbs";
    StandaloneTestResult res;
    res.clean = runProcess(QBS_EXECUTABLE_PATH, { "clean", "--build-directory", build_dir }, testWorkDir);
    res.run = runProcess(QBS_EXECUTABLE_PATH, { "run", "modules.rgen.debug_print_artifacts_with_header:true", "--build-directory", build_dir }, testWorkDir);
    std::cerr << res.run.stdErr << std::endl;
    std::cout << res.run.stdOut << std::endl;
    return res.run.code == 0;
}

bool runTestCmake(std::string testDir)
{
    const std::string testWorkDir = fmt::format("{}/{}", TESTS_WORK_DIR, testDir);
    const std::string src_dir = ".";
    const std::string build_dir = "build_cmake";

    StandaloneTestResult res;

    res.clean = runProcess(CMAKE_EXECUTABLE_PATH, { "--build", build_dir, "--target", "clean" }, testWorkDir);

#ifdef _WIN32
    const std::string qtKit = "-DCMAKE_PREFIX_PATH=F:\\devtools\\qt-kit\\qt-5.15.15";
#else
    const std::string qtKit = "-DCMAKE_PREFIX_PATH=/kits/qt_5.15.13_linux_x64_gcc";
#endif

    res.run = runProcess(CMAKE_EXECUTABLE_PATH, { qtKit, "-S", src_dir, "-B", build_dir }, testWorkDir);

    std::cerr << res.run.stdErr << std::endl;
    std::cout << res.run.stdOut << std::endl;

    bool testPass = false;

    return (res.run.code == 0);
}

} // namespace

TEST(StandaloneTest, SwigInterface)
{
    const std::string testName = "swig_interface";
    EXPECT_TRUE(runTest(testName));
}

// щас не работает
// TEST(StandaloneTest, GMock)
// {
//     EXPECT_TRUE(runTest("test-project-5-google_mock"));
// }

#include "enum_gen.h"
#include "enum_notgen.h"
#include <gtest/gtest.h>
#include <rgen/enum.h>
using namespace rgen;
namespace tests
{

TEST(NotGenTest, SimpleUsage)
{
    EXPECT_EQ(rgen::Enum<Gen>::toString(Gen::Good), "Good");
    EXPECT_EQ(rgen::Enum<NOTGEN>::fromString("Bad"), NOTGEN::Bad);
}

} // namespace tests

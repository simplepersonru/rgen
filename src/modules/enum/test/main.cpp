#include "custom_names.h"
#include "misc_enum_places.h"

#include <gtest/gtest.h>

#include <rgen/enum.h>

#include <stdexcept>

using namespace rgen;

TEST(Enum, SimpleUsage)
{
    EXPECT_EQ(rgen::Enum<BookColor>::fromString("Red"), BookColor::Red);
    EXPECT_EQ(rgen::Enum<BookColor>::toString(BookColor::Red), "Red");
}

TEST(Enum, SetDefaultResult)
{
    // Достаём существующее значение
    EXPECT_EQ(rgen::Enum<BookColor>::fromString("Blue", BookColor::Red), BookColor::Blue);
    // Пытаемся достать несуществующее значение, должны получить дефолтное
    EXPECT_EQ(rgen::Enum<BookColor>::fromString("Pink", BookColor::Red), BookColor::Red);
}

TEST(Enum, GetFromStringNonexistedValue)
{
    // Pink отсутствует в BookColor. Должны получить исключение.
    EXPECT_THROW(rgen::Enum<BookColor>::fromString("Pink"), std::runtime_error);
}

TEST(Enum, DisplayValues)
{
    EXPECT_EQ(rgen::Enum<Number>::fromString("one"), Number::ONE);
    EXPECT_EQ(rgen::Enum<Number>::toString(Number::ONE), "one");
}

TEST(Enum, Using)
{
    EXPECT_EQ(rgen::Enum<NumberUsing>::fromString("Two"), NumberUsing::Two);
    EXPECT_EQ(rgen::Enum<BookColor>::toString(BookColor::Red), "Red");
}

TEST(Enum, GetValues)
{
    auto values = rgen::Enum<BookColor>::values();
    ASSERT_EQ(values.size(), 2);
    EXPECT_EQ(values[0], BookColor::Red);
    EXPECT_EQ(values[1], BookColor::Blue);
}

TEST(Enum, NestedStructDefault)
{
    EXPECT_EQ(rgen::Enum<Card::Form>::toString(Card::Form::Rect), "Rect");
}

TEST(Enum, NestedStructUsing)
{
    EXPECT_EQ(rgen::Enum<OwnerCardUsing>::toString(OwnerCardUsing::Me), "Me");
}

TEST(Enum, NestedNamespaceDefault)
{
    using namespace The::Longest::Namespace;

    EXPECT_EQ(rgen::Enum<Color>::toString(Color::Red), "Red");
}

TEST(Enum, NestedNamespaceUsing)
{
    using namespace The::Longest::Namespace;

    EXPECT_EQ(rgen::Enum<ElementUsing>::toString(ElementUsing::Water), "Water");
}

TEST(Enum, EnumInStructInNamespace)
{
    using namespace The::Longest::Namespace;

    EXPECT_EQ(rgen::Enum<Book::Format>::toString(Book::Format::A5), "A5");
}

TEST(Enum, EnumInStructInNamespaceUsing)
{
    using namespace The::Longest::Namespace;

    EXPECT_EQ(rgen::Enum<PriceUsing>::toString(PriceUsing::Expensive), "Expensive");
}

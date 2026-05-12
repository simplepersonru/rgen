/// @Enum
enum class BookColor
{
    Red,
    Blue
};

/// @Enum
enum Number
{
    ONE ///< one
};

enum NumberUsingTest
{
    Two
};

/// @Enum
using NumberUsing = NumberUsingTest;

/// @Enum
enum IgnoreEnumTest
{
    A,
    B ///< @Ignore
};

// Сериализация во вложенных структурах
struct Card
{
    /// @Enum
    enum Form
    {
        Rect
    };

    enum Owner
    {
        Me
    };
};

/// @Enum
using OwnerCardUsing = Card::Owner;

// Сериализация в неймспейсах
namespace The::Longest::Namespace
{

/// @Enum
enum class Color
{
    Red
};

enum Element
{
    Water
};

///@Enum
using ElementUsing = Element;

// Сериализация в структуре в namespace
struct Book
{
    /// @Enum
    enum Format
    {
        A5
    };

    enum Price
    {
        Expensive
    };
};

///@Enum
using PriceUsing = Book::Price;

} // namespace The::Longest::Namespace

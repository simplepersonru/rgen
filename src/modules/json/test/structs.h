#pragma once

#include <QHash>
#include <QMap>
#include <QString>
#include <QVector>

#include <deque>
#include <forward_list>
#include <list>
#include <vector>

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <cstdint>

#include "json_myown.h"
#include <rgen/operator_equals.h>

#include <externalLib/extLib.h>

/*!
 * \brief Структура для простейшего теста
 *
 * @Json
 * @OperatorEquals
 */
struct SimpleStruct
{
    DECLARE_EQUALS_OPERATOR(SimpleStruct)

    int value;
};

/// Значение по умолчанию для полей в JsonKeysUsage
constexpr int defaultKeyValueForJsonKeysTest = 5;

/// @Json
/// @OperatorEquals
struct JsonKeysUsage
{
    DECLARE_EQUALS_OPERATOR(JsonKeysUsage)

    /// @Json { ignore: true }
    int ignoredKey = defaultKeyValueForJsonKeysTest;

    /// @Json { key: myCustomKey }
    int originalKey = defaultKeyValueForJsonKeysTest;
};

/// @Json
struct JsonIgnore
{
    int ignoredKey = defaultKeyValueForJsonKeysTest; ///< @Ignore
    int someKey;
};

enum TestEnum
{
    A
};

/// @Json
struct JsonEnum
{
    TestEnum value;
};

/*!
 * \brief Создаем собственную структуры, в которую входит структура из внешней библиотеки,
 * для которой мы заранее прописали операторы стрелок
 *
 * @Json
 * @OperatorEquals
 */
struct Point
{
    DECLARE_EQUALS_OPERATOR(Point)

    ExtPoint coord;
};

/*!
 * \brief Для теста на using
 */
template <typename T>
struct MyCustomPoint
{
    T x;
    T y;
};

/// @Json
using MyDotPointInt = MyCustomPoint<int>;

// Необходимо для инициализации MyDotPointInt. Иначе получаем ошибку
// error: Template with alias "MyCustomPoint" is not initialized
template struct MyCustomPoint<int>;

template <typename T>
inline bool operator==(const MyCustomPoint<T>& lhs, const MyCustomPoint<T>& rhs)
{
    return lhs.x == rhs.x
    && lhs.y == rhs.y;
}

/*!
 * \brief Тестируем сериализацию для вложенной в namespace структуры
 */
namespace The
{
/// @Json
/// @OperatorEquals
struct Book
{
    DECLARE_EQUALS_OPERATOR(Book)

    int pages;
};

} // namespace The

/*!
 * \brief Структура для теста линейный контейнеров
 * @Json
 * @OperatorEquals
 */
struct LinearsStruct
{
    DECLARE_EQUALS_OPERATOR(LinearsStruct)

    std::vector<int> vec;
    std::deque<int> deq;
    std::list<int> list;
    std::forward_list<int> flist;
};

/*!
 * \brief Структура для теста ассоциативных контейнеров
 * @Json
 * @OperatorEquals
 */
struct AssociatedStruct
{
    DECLARE_EQUALS_OPERATOR(AssociatedStruct)

    std::map<std::string, Point> m;
    std::unordered_map<std::string, Point> unord_m;
    std::set<int> s;
    std::unordered_set<int> unord_s;
};

/*!
 * \brief Структура для теста некоторых std структур
 * @Json
 * @OperatorEquals
 */
struct StdStruct
{
    DECLARE_EQUALS_OPERATOR(StdStruct)

    std::pair<int, int> pair;
    std::optional<int> opt;
};

/*!
 * \brief Структура для теста Qt контейнеров
 * @Json
 * @OperatorEquals
 */
struct QStructs
{
    DECLARE_EQUALS_OPERATOR(QStructs)

    QVector<int> vec;
    QMap<int, int> map;
    QString name;
    QUuid uuid;
    QPair<int, int> pair;
    QStringList strList;
    QHash<int, int> hash;
    QByteArray byteArray;
};

/*!
 * \brief Структура для теста стандартных переменных
 * @Json
 * @OperatorEquals
 */
struct Variables
{
    DECLARE_EQUALS_OPERATOR(Variables)

    char c;
    double d;
    float f;
    bool b;
    int8_t i8;
    uint8_t ui8;
    int16_t i16;
    uint16_t ui16;
    int32_t i32;
    uint32_t ui32;
    int64_t i64;
    uint64_t ui64;
};

/*!
 * \brief Структура для теста стандартных переменных
 * @Json
 * @OperatorEquals
 */
struct Pointers
{
    DECLARE_EQUALS_OPERATOR(Pointers)

    std::unique_ptr<int> uPtr;
    std::shared_ptr<int> sPtr;
    QSharedPointer<int> qsPtr;
};

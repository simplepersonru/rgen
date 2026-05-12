#pragma once

#include <QHash>
#include <QMap>
#include <QString>
#include <QVector>
#include <QUuid>
#include <QSharedPointer>
#include <QPair>
#include <QByteArray>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>
#include <set>
#include <unordered_set>
#include <optional>
#include <memory>
#include <cstdint>
#include <rgen/operator_equals.h>

/// @OperatorEquals
struct TestClass
{
    DECLARE_EQUALS_OPERATOR(TestClass)

    int a;
};

/// @OperatorEquals
struct TestIgnore
{
    DECLARE_EQUALS_OPERATOR(TestIgnore)

    int a; ///< @Ignore
    int b;
};

/// @OperatorEquals
struct ComplexStruct
{
    DECLARE_EQUALS_OPERATOR(ComplexStruct)

    TestClass day;
    std::string name;
};

template<typename T>
struct StructTemplate
{
    T a;
};

/// @OperatorEquals
using StructUsing = StructTemplate<int>;
RGEN_Equals(, StructUsing)
// Необходимо для инициализации StructUsing. Иначе получаем ошибку
// error: Template with alias "StructTemplate" is not initialized
template struct StructTemplate<int>;

namespace The::Longest::Namespace
{

/// @OperatorEquals
struct ClassInNamespace
{
    DECLARE_EQUALS_OPERATOR(ClassInNamespace)

    int a;
};

template<typename T>
struct StructTemplateNamespace
{
    T a;
};

/// @OperatorEquals
using StructUsingNamespace = StructTemplateNamespace<int>;
RGEN_Equals(, StructUsingNamespace)
// Необходимо для инициализации StructUsingNamespace. Иначе получаем ошибку
// error: Template with alias "StructTemplateNamespace" is not initialized
template struct StructTemplateNamespace<int>;

} // namespace The::Longest::Namespace

/*!
 * \brief Структура для теста линейный контейнеров
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
 * @OperatorEquals
 */
struct AssociatedStruct
{
    DECLARE_EQUALS_OPERATOR(AssociatedStruct)

    std::set<int> s;
    std::unordered_set<int> unord_s;
};

/*!
 * \brief Структура для теста некоторых std структур
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
 * @OperatorEquals
 */
struct Pointers
{
    DECLARE_EQUALS_OPERATOR(Pointers)

    std::unique_ptr<int> uPtr;
    std::shared_ptr<int> sPtr;
    QSharedPointer<int> qsPtr;
};

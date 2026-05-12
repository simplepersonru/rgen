#pragma once

#include <rgen/private/qvariantmap_pre.h>

#include <deque>
#include <forward_list>
#include <list>
#include <vector>

#include <map>
#include <set>

#include <unordered_map>
#include <unordered_set>

#include <memory>
#include <optional>
#include <utility>

#include <QByteArray>
#include <QHash>
#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QUuid>
#include <QVariant>
#include <QVector>

template <typename T>
void fromTypeToVariant(QVariant& variant, const T& val)
requires std::is_integral_v<T> || std::is_floating_point_v<T>
{
    variant = QVariant::fromValue(val);
}

template <typename T>
void fromVariantToType(const QVariant& variant, T& val)
requires std::is_integral_v<T> || std::is_floating_point_v<T>
{
    val = variant.value<T>();
}

template <typename T>
void fromTypeToVariant(QVariant& variant, const T& var)
requires std::is_enum_v<T>
{
    variant = static_cast<qlonglong>(var);
}

template <typename T>
void fromVariantToType(const QVariant& variant, T& var)
requires std::is_enum_v<T>
{
    var = static_cast<T>(variant.toLongLong());
}

namespace rgen::_details_std
{

template <class C>
using size_type_t = decltype(std::declval<const C&>().size());

template <typename Container>
void containerToQVariant(QVariant& variant, const Container& val)
{
    QVariantList list;

    for (const auto& item : val)
    {
        QVariant element;
        fromTypeToVariant(element, item);
        list.append(std::move(element));
    }

    variant = list;
}

template <typename Container>
void seqContainerFromQVariant(const QVariant& variant, Container& val)
{
    QVariantList list = variant.toList();
    val.resize(list.size());
    for (size_type_t<Container> i = 0; i < val.size(); ++i)
    {
        fromVariantToType(list.at(i), val[i]);
    }
}

template <typename Container>
void setFromQVariant(const QVariant& variant, Container& val)
{
    using T = typename Container::value_type;
    for (const auto& item : variant.toList())
    {
        T element;
        fromVariantToType(item, element);
        val.emplace(std::move(element));
    }
}

template <typename Container>
void mapFromQVariant(const QVariant& variant, Container& val)
{
    using K = typename Container::value_type::first_type;
    using V = typename Container::value_type::second_type;
    for (const auto& item : variant.toList())
    {
        std::pair<std::remove_const_t<K>, V> temp;
        fromVariantToType(item, temp);
        val.emplace(std::move(temp));
    }
}

template <typename Pair>
void pairFromQVariant(const QVariant& variant, Pair& p)
{
    QVariantMap map = variant.toMap();
    fromVariantToType(map["key"], p.first);
    fromVariantToType(map["value"], p.second);
}

template <typename Pair>
void pairToQVariant(QVariant& variant, const Pair& p)
{
    QVariantMap map;
    fromTypeToVariant(map["key"], p.first);
    fromTypeToVariant(map["value"], p.second);
    variant = map;
}

template <typename Pointer>
void pointerToQVariant(QVariant& variant, const Pointer& ptr)
{
    if (ptr)
    {
        fromTypeToVariant(variant, *ptr);
    }
    else
    {
        variant = QVariant();
    }
}

template <typename List>
void listFromQVariant(const QVariant& variant, List& list)
{
    using T = typename List::value_type;
    for (const auto& item : variant.toList())
    {
        T element;
        fromVariantToType(item, element);
        list.push_front(std::move(element));
    }
    list.reverse();
}

} // namespace rgen::_details_std

namespace rgen::_details_qt
{

template <typename Container>
void mapToQVariant(QVariant& variant, const Container& map)
{
    auto list = QVariantList();
    for (auto it = map.keyValueBegin(); it != map.keyValueEnd(); ++it)
    {
        QVariant element;
        fromTypeToVariant(element, *it);
        list.push_back(std::move(element));
    }
    variant = list;
}

template <typename Container>
void mapFromQVariant(const QVariant& variant, Container& map)
{
    using key_type = typename Container::key_type;
    using mapped_type = typename Container::mapped_type;
    for (const auto& item : variant.toList())
    {
        QPair<key_type, mapped_type> temp;
        fromVariantToType(item, temp);
        map.insert(temp.first, temp.second);
    }
}

} // namespace rgen::_details_qt

template <typename Key, typename Value>
void fromTypeToVariant(QVariant& variant, const std::pair<Key, Value>& p)
{
    rgen::_details_std::pairToQVariant(variant, p);
}

template <typename Key, typename Value>
void fromVariantToType(const QVariant& variant, std::pair<Key, Value>& p)
{
    rgen::_details_std::pairFromQVariant(variant, p);
}

template <typename T>
void fromTypeToVariant(QVariant& variant, const std::optional<T>& opt)
{
    if (opt.has_value())
    {
        fromTypeToVariant(variant, opt.value());
    }
    else
    {
        variant = QVariant();
    }
}

template <typename T>
void fromVariantToType(const QVariant& variant, std::optional<T>& opt)
{
    if (variant.isNull())
    {
        opt = std::nullopt;
    }
    else
    {
        T temp;
        fromVariantToType(variant, temp);
        opt = std::move(temp);
    }
}

template <typename T>
void fromTypeToVariant(QVariant& variant, const std::shared_ptr<T>& ptr)
{
    rgen::_details_std::pointerToQVariant(variant, ptr);
}

template <typename T>
void fromVariantToType(const QVariant& variant, std::shared_ptr<T>& ptr)
{
    if (variant.isNull())
    {
        ptr.reset();
    }
    else
    {
        auto temp = std::make_shared<T>();
        fromVariantToType(variant, *temp);
        ptr = std::move(temp);
    }
}

template <typename T>
void fromTypeToVariant(QVariant& variant, const std::unique_ptr<T>& ptr)
{
    rgen::_details_std::pointerToQVariant(variant, ptr);
}

template <typename T>
void fromVariantToType(const QVariant& variant, std::unique_ptr<T>& ptr)
{
    if (variant.isNull())
    {
        ptr.reset();
    }
    else
    {
        auto temp = std::make_unique<T>();
        fromVariantToType(variant, *temp);
        ptr = std::move(temp);
    }
}

template <typename T>
void fromTypeToVariant(QVariant& variant, const std::vector<T>& val)
{
    rgen::_details_std::containerToQVariant(variant, val);
}

template <typename T>
void fromVariantToType(const QVariant& variant, std::vector<T>& val)
{
    rgen::_details_std::seqContainerFromQVariant(variant, val);
}

template <typename T>
void fromTypeToVariant(QVariant& variant, const std::deque<T>& val)
{
    rgen::_details_std::containerToQVariant(variant, val);
}

template <typename T>
void fromVariantToType(const QVariant& variant, std::deque<T>& val)
{
    rgen::_details_std::seqContainerFromQVariant(variant, val);
}

template <typename T>
void fromTypeToVariant(QVariant& variant, const std::list<T>& val)
{
    rgen::_details_std::containerToQVariant(variant, val);
}

template <typename T>
void fromVariantToType(const QVariant& variant, std::list<T>& val)
{
    rgen::_details_std::listFromQVariant(variant, val);
}

template <typename T>
void fromTypeToVariant(QVariant& variant, const std::forward_list<T>& val)
{
    rgen::_details_std::containerToQVariant(variant, val);
}

template <typename T>
void fromVariantToType(const QVariant& variant, std::forward_list<T>& val)
{
    rgen::_details_std::listFromQVariant(variant, val);
}

template <typename K, typename V>
void fromTypeToVariant(QVariant& variant, const std::map<K, V>& val)
{
    rgen::_details_std::containerToQVariant(variant, val);
}

template <typename K, typename V>
void fromVariantToType(const QVariant& variant, std::map<K, V>& val)
{
    rgen::_details_std::mapFromQVariant(variant, val);
}

template <typename K, typename V>
void fromTypeToVariant(QVariant& variant, const std::unordered_map<K, V>& val)
{
    rgen::_details_std::containerToQVariant(variant, val);
}

template <typename K, typename V>
void fromVariantToType(const QVariant& variant, std::unordered_map<K, V>& val)
{
    rgen::_details_std::mapFromQVariant(variant, val);
}

template <typename T>
void fromTypeToVariant(QVariant& variant, const std::set<T>& val)
{
    rgen::_details_std::containerToQVariant(variant, val);
}

template <typename T>
void fromVariantToType(const QVariant& variant, std::set<T>& val)
{
    rgen::_details_std::setFromQVariant(variant, val);
}

template <typename T>
void fromTypeToVariant(QVariant& variant, const std::unordered_set<T>& val)
{
    rgen::_details_std::containerToQVariant(variant, val);
}

template <typename T>
void fromVariantToType(const QVariant& variant, std::unordered_set<T>& val)
{
    rgen::_details_std::setFromQVariant(variant, val);
}

template <>
inline void fromTypeToVariant(QVariant& variant, const std::string& val)
{
    variant = QVariant(QString::fromStdString(val));
}

template <>
inline void fromVariantToType(const QVariant& variant, std::string& val)
{
    val = variant.toString().toStdString();
}

template <>
inline void fromTypeToVariant(QVariant& variant, const QString& s)
{
    variant = QVariant::fromValue(s);
}

template <>
inline void fromVariantToType(const QVariant& variant, QString& s)
{
    s = variant.toString();
}

template <>
inline void fromTypeToVariant(QVariant& variant, const QUuid& uuid)
{
    variant = QVariant::fromValue(uuid);
}

template <>
inline void fromVariantToType(const QVariant& variant, QUuid& uuid)
{
    uuid = variant.toUuid();
}

template <typename T>
void fromVariantToType(const QVariant& variant, QVector<T>& v)
{
    rgen::_details_std::seqContainerFromQVariant(variant, v);
}

template <typename T>
void fromTypeToVariant(QVariant& variant, const QVector<T>& v)
{
    rgen::_details_std::containerToQVariant(variant, v);
}

template <>
inline void fromVariantToType(const QVariant& variant, QStringList& v)
{
    v = variant.toStringList();
}

template <>
inline void fromTypeToVariant(QVariant& variant, const QStringList& val)
{
    variant = QVariant::fromValue(val);
}

template <typename Key, typename Value>
void fromTypeToVariant(QVariant& variant, const QPair<Key, Value>& p)
{
    rgen::_details_std::pairToQVariant(variant, p);
}

template <typename Key, typename Value>
void fromVariantToType(const QVariant& variant, QPair<Key, Value>& p)
{
    rgen::_details_std::pairFromQVariant(variant, p);
}

template <typename K, typename V>
void fromTypeToVariant(QVariant& variant, const QMap<K, V>& map)
{
    rgen::_details_qt::mapToQVariant(variant, map);
}

template <typename Key, typename Value>
void fromVariantToType(const QVariant& variant, QMap<Key, Value>& map)
{
    rgen::_details_qt::mapFromQVariant(variant, map);
}

template <typename K, typename V>
void fromTypeToVariant(QVariant& variant, const QHash<K, V>& map)
{
    rgen::_details_qt::mapToQVariant(variant, map);
}

template <typename Key, typename Value>
void fromVariantToType(const QVariant& variant, QHash<Key, Value>& map)
{
    rgen::_details_qt::mapFromQVariant(variant, map);
}

template <>
inline void fromTypeToVariant(QVariant& variant, const QByteArray& s)
{
    variant = QVariant::fromValue(s);
}

template <>
inline void fromVariantToType(const QVariant& variant, QByteArray& s)
{
    s = variant.toByteArray();
}

template <typename T>
void fromTypeToVariant(QVariant& variant, const QSharedPointer<T>& ptr)
{
    if (ptr.isNull())
    {
        variant = QVariant();
    }
    else
    {
        fromTypeToVariant(variant, *ptr);
    }
}

template <typename T>
void fromVariantToType(const QVariant& variant, QSharedPointer<T>& ptr)
{
    if (variant.isNull())
    {
        ptr.clear();
    }
    else
    {
        T value;
        fromVariantToType(variant, value);
        ptr = QSharedPointer<T>::create(std::move(value));
    }
}

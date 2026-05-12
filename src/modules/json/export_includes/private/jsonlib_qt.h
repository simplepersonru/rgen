#pragma once

#include <rgen/common/export_macro.h>
#include <rgen/private/json_pre.h>
#include <rgen/private/jsonlib_std.h>

#include <QByteArray>
#include <QHash>
#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QUuid>
#include <QVector>

namespace rgen::_details_qt
{

template <typename Container>
void mapToJson(rgen::json& json, const Container& map)
{
    json = rgen::json::array();

    for (auto it = map.keyValueBegin(); it != map.keyValueEnd(); ++it)
    {
        rgen::json element;
        fromTypeToJson(element, *it);
        json.emplace_back(std::move(element));
    }
}

template <typename Container>
void mapFromJson(const rgen::json& json, Container& map)
{
    using key_type = typename Container::key_type;
    using mapped_type = typename Container::mapped_type;
    for (const auto& item : json)
    {
        QPair<key_type, mapped_type> temp;
        fromJsonToType(item, temp);
        map.insert(temp.first, temp.second);
    }
}
} // namespace rgen::_details_qt

template <>
RGEN_DECL_EXPORT void fromTypeToJson(rgen::json& json, const QString& s);

template <>
RGEN_DECL_EXPORT void fromJsonToType(const rgen::json& json, QString& s);

template <>
RGEN_DECL_EXPORT void fromTypeToJson(rgen::json& json, const QUuid& uuid);

template <>
RGEN_DECL_EXPORT void fromJsonToType(const rgen::json& json, QUuid& uuid);

template <>
RGEN_DECL_EXPORT void fromJsonToType(const rgen::json& json, QStringList& v);

template <>
RGEN_DECL_EXPORT void fromTypeToJson(rgen::json& json, const QStringList& val);

template <>
RGEN_DECL_EXPORT void fromTypeToJson(rgen::json& json, const QByteArray& s);

template <>
RGEN_DECL_EXPORT void fromJsonToType(const rgen::json& json, QByteArray& s);

template <typename T>
void fromJsonToType(const rgen::json& json, QVector<T>& v)
{
    rgen::_details_std::seqContainerFromJson(json, v);
}
template <typename T>
void fromTypeToJson(rgen::json& json, const QVector<T>& v)
{
    rgen::_details_std::containerToJson(json, v);
}

template <typename Key, typename Value>
void fromTypeToJson(rgen::json& json, const QPair<Key, Value>& p)
{
    rgen::_details_std::pairToJson(json, p);
}

template <typename Key, typename Value>
void fromJsonToType(const rgen::json& json, QPair<Key, Value>& p)
{
    rgen::_details_std::pairFromJson(json, p);
}

template <typename K, typename V>
void fromTypeToJson(rgen::json& json, const QMap<K, V>& map)
{
    rgen::_details_qt::mapToJson(json, map);
}

template <typename Key, typename Value>
void fromJsonToType(const rgen::json& json, QMap<Key, Value>& map)
{
    rgen::_details_qt::mapFromJson(json, map);
}

template <typename K, typename V>
void fromTypeToJson(rgen::json& json, const QHash<K, V>& map)
{
    rgen::_details_qt::mapToJson(json, map);
}

template <typename Key, typename Value>
void fromJsonToType(const rgen::json& json, QHash<Key, Value>& map)
{
    rgen::_details_qt::mapFromJson(json, map);
}

template <typename T>
void fromTypeToJson(rgen::json& json, const QSharedPointer<T>& ptr)
{
    rgen::_details_std::pointerToJson(json, ptr);
}

template <typename T>
void fromJsonToType(const rgen::json& json, QSharedPointer<T>& ptr)
{
    if (json.is_null())
    {
        ptr.clear();
    }
    else
    {
        T value;
        fromJsonToType(json, value);
        ptr = QSharedPointer<T>::create(std::move(value));
    }
}

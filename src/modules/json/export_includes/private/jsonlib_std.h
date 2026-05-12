#pragma once

#include <rgen/private/json_pre.h>

#include <deque>
#include <forward_list>
#include <list>
#include <vector>

#include <map>
#include <set>

#include <unordered_map>
#include <unordered_set>

#include <cstdint>
#include <memory>
#include <utility>
#include <optional>

template <typename T>
void fromJsonToType(const rgen::json& json, T& val)
requires std::is_integral_v<T> || std::is_floating_point_v<T>
{
    val = json.get<T>();
}

template <typename T>
void fromTypeToJson(rgen::json& json, const T& val)
requires std::is_integral_v<T> || std::is_floating_point_v<T>
{
    json = val;
}

template <typename T>
void fromTypeToJson(rgen::json& json, const T& var)
requires std::is_enum_v<T>
{
    json = static_cast<int64_t>(var);
}

template <typename T>
void fromJsonToType(const rgen::json& json, T& var)
requires std::is_enum_v<T>
{
    var = static_cast<T>(json.get<int64_t>());
}

namespace rgen::_details_std
{

template <class C>
using size_type_t = decltype(std::declval<const C&>().size());

template <typename Container>
void containerToJson(rgen::json& json, const Container& val)
{
    json = rgen::json::array();

    for (const auto& item : val)
    {
        rgen::json element;
        fromTypeToJson(element, item);
        json.emplace_back(std::move(element));
    }
}

template <typename Container>
void seqContainerFromJson(const rgen::json& json, Container& val)
{
    val.resize(json.size());
    for (size_type_t<Container> i = 0; i < val.size(); ++i)
    {
        fromJsonToType(json.at(i), val[i]);
    }
}

template <typename Container>
void setFromJson(const rgen::json& json, Container& val)
{
    using T = typename Container::value_type;
    for (const auto& item : json)
    {
        T element;
        fromJsonToType(item, element);
        val.emplace(std::move(element));
    }
}

template <typename Container>
void mapFromJson(const rgen::json& json, Container& val)
{
    using K = typename Container::value_type::first_type;
    using V = typename Container::value_type::second_type;
    for (const auto& item : json)
    {
        std::pair<std::remove_const_t<K>, V> temp;
        fromJsonToType(item, temp);
        val.emplace(std::move(temp));
    }
}

template <typename Pair>
void pairFromJson(const rgen::json& json, Pair& p)
{
    fromJsonToType(json.at("key"), p.first);
    fromJsonToType(json.at("value"), p.second);
}

template <typename Pair>
void pairToJson(rgen::json& json, const Pair& p)
{
    fromTypeToJson(json["key"], p.first);
    fromTypeToJson(json["value"], p.second);
}

template <typename Pointer>
void pointerToJson(rgen::json& json, const Pointer& ptr)
{
    if (ptr)
    {
        fromTypeToJson(json, *ptr);
    }
    else
    {
        json = nullptr;
    }
}

template <typename List>
void listFromJson(const rgen::json& json, List& list)
{
    using T = typename List::value_type;
    for (const auto& item : json)
    {
        T element;
        fromJsonToType(item, element);
        list.push_front(std::move(element));
    }
    list.reverse();
}

} // namespace rgen::_details_std

template <typename Key, typename Value>
void fromTypeToJson(rgen::json& json, const std::pair<Key, Value>& p)
{
    rgen::_details_std::pairToJson(json, p);
}

template <typename Key, typename Value>
void fromJsonToType(const rgen::json& json, std::pair<Key, Value>& p)
{
    rgen::_details_std::pairFromJson(json, p);
}

template <typename T>
void fromTypeToJson(rgen::json& json, const std::optional<T>& opt)
{
    if (opt)
    {
        fromTypeToJson(json, opt.value());
    }
    else
    {
        json = nullptr;
    }
}

template <typename T>
void fromJsonToType(const rgen::json& json, std::optional<T>& opt)
{
    if (json.is_null())
    {
        opt = std::nullopt;
    }
    else
    {
        T temp;
        fromJsonToType(json, temp);
        opt = std::move(temp);
    }
}

template <typename T>
void fromTypeToJson(rgen::json& json, const std::shared_ptr<T>& ptr)
{
    rgen::_details_std::pointerToJson(json, ptr);
}

template <typename T>
void fromJsonToType(const rgen::json& json, std::shared_ptr<T>& ptr)
{
    if (json.is_null())
    {
        ptr.reset();
    }
    else
    {
        auto temp = std::make_shared<T>();
        fromJsonToType(json, *temp);
        ptr = std::move(temp);
    }
}

template <typename T>
void fromTypeToJson(rgen::json& json, const std::unique_ptr<T>& ptr)
{
    rgen::_details_std::pointerToJson(json, ptr);
}

template <typename T>
void fromJsonToType(const rgen::json& json, std::unique_ptr<T>& ptr)
{
    if (json.is_null())
    {
        ptr.reset();
    }
    else
    {
        auto temp = std::make_unique<T>();
        fromJsonToType(json, *temp);
        ptr = std::move(temp);
    }
}

template <typename T>
void fromTypeToJson(rgen::json& json, const std::vector<T>& val)
{
    rgen::_details_std::containerToJson(json, val);
}

template <typename T>
void fromJsonToType(const rgen::json& json, std::vector<T>& val)
{
    rgen::_details_std::seqContainerFromJson(json, val);
}

template <typename T>
void fromTypeToJson(rgen::json& json, const std::deque<T>& val)
{
    rgen::_details_std::containerToJson(json, val);
}

template <typename T>
void fromJsonToType(const rgen::json& json, std::deque<T>& val)
{
    rgen::_details_std::seqContainerFromJson(json, val);
}

template <typename T>
void fromTypeToJson(rgen::json& json, const std::list<T>& val)
{
    rgen::_details_std::containerToJson(json, val);
}

template <typename T>
void fromJsonToType(const rgen::json& json, std::list<T>& val)
{
    rgen::_details_std::listFromJson(json, val);
}

template <typename T>
void fromTypeToJson(rgen::json& json, const std::forward_list<T>& val)
{
    rgen::_details_std::containerToJson(json, val);
}

template <typename T>
void fromJsonToType(const rgen::json& json, std::forward_list<T>& val)
{
    rgen::_details_std::listFromJson(json, val);
}

template <typename K, typename V>
void fromTypeToJson(rgen::json& json, const std::map<K, V>& val)
{
    rgen::_details_std::containerToJson(json, val);
}

template <typename K, typename V>
void fromJsonToType(const rgen::json& json, std::map<K, V>& val)
{
    rgen::_details_std::mapFromJson(json, val);
}

template <typename K, typename V>
void fromTypeToJson(rgen::json& json, const std::unordered_map<K, V>& val)
{
    rgen::_details_std::containerToJson(json, val);
}

template <typename K, typename V>
void fromJsonToType(const rgen::json& json, std::unordered_map<K, V>& val)
{
    rgen::_details_std::mapFromJson(json, val);
}

template <typename T>
void fromTypeToJson(rgen::json& json, const std::set<T>& val)
{
    rgen::_details_std::containerToJson(json, val);
}

template <typename T>
void fromJsonToType(const rgen::json& json, std::set<T>& val)
{
    rgen::_details_std::setFromJson(json, val);
}

template <typename T>
void fromTypeToJson(rgen::json& json, const std::unordered_set<T>& val)
{
    rgen::_details_std::containerToJson(json, val);
}

template <typename T>
void fromJsonToType(const rgen::json& json, std::unordered_set<T>& val)
{
    rgen::_details_std::setFromJson(json, val);
}

template <>
inline void fromTypeToJson(rgen::json& json, const std::string& val)
{
    json = val;
}

template <>
inline void fromJsonToType(const rgen::json& json, std::string& val)
{
    val = json.get<std::string>();
}

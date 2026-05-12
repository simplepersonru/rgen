#pragma once

#include <rgen/common/json.hpp>


namespace rgen
{

using json = nlohmann::json;

template <typename T>
struct JsonKeys
{};

}

template <typename T>
void fromJsonToType(const rgen::json& json, T& val);

template <typename T>
void fromTypeToJson(rgen::json& json, const T& val);

// Объявляем собственные операторы стрелок для библиотечной структуры,
// которую мы не можем пометить тегом генерации.
#pragma once

#include <rgen/private/json_pre.h>

#include <externalLib/extLib.h>

template <>
inline void fromTypeToJson(rgen::json& json, const ExtPoint& val)
{
    fromTypeToJson(json["x"], val.x);
    fromTypeToJson(json["y"], val.y);
}

template <>
inline void fromJsonToType(const rgen::json& json, ExtPoint& val)
{
    fromJsonToType(json["x"], val.x);
    fromJsonToType(json["y"], val.y);
}

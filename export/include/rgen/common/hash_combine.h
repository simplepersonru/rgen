#pragma once

#include <QHash>

#include <cstddef>
#include <functional>
#include <utility>

namespace rgen
{

template <typename T, typename... Args>
void qhash_combine(uint& seed, const T& val, Args... args)
{
    seed ^= qHash(val, seed) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    if constexpr (sizeof...(args) > 0)
    {
        qhash_combine(seed, std::forward<Args>(args)...);
    }
}

template <typename T, typename... Args>
void hash_combine(std::size_t& seed, const T& val, Args... args)
{
    seed ^= std::hash<T> {}(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    if constexpr (sizeof...(args) > 0)
    {
        hash_combine(seed, std::forward<Args>(args)...);
    }
}

} // namespace rgen

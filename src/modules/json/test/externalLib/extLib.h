#pragma once

#include <optional>
#include <tuple>

#include "extLib_global.h"

struct EXTERNAL_EXPORT ExtPoint
{
    double x = 0;
    std::optional<double> y = 0;

    bool operator<(const ExtPoint& other) const
    {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }
};

EXTERNAL_EXPORT inline bool operator==(const ExtPoint& lhs, const ExtPoint& rhs)
{
    return std::tie(lhs.x, lhs.y) == std::tie(rhs.x, rhs.y);
}

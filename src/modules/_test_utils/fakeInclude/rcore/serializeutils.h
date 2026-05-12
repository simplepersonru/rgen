#pragma once

#include <QDataStream>
#include <optional>

template <typename T>
QDataStream& operator<<(QDataStream& stream, const std::optional<T>& data)
{
    const bool isNull = !data;
    stream << isNull;
    if (isNull)
        return stream;
    return stream << data.value();
}

template <typename T>
QDataStream& operator>>(QDataStream& stream, std::optional<T>& data)
{
    static_assert(std::is_default_constructible_v<T> && std::is_move_constructible_v<T>);
    bool isNull = false;
    stream >> isNull;
    if (isNull)
        data = std::nullopt;
    else
    {
        T value;
        stream >> value;
        data = std::move(value);
    }
    return stream;
}

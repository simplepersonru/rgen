#pragma once

#include <QString>

/*!
 * @JsonSchema
 * @Json
 */
struct TestStructJS
{
    /// @Display Имя пользователя
    QString username;
    /// Возраст
    int age;
    bool isActive;///< Активен
};

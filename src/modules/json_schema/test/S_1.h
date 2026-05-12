#pragma once

#include <QString>
#include <rgen/json_qt.h>

#include "S_1_1.h"
#include "S_1_2.h"

/*!
 * @JsonSchema
 * @Json
 */
struct Hierarchy_lvl1
{
    Hierarchy_lvl1_lvl1 hierarchy_lvl1_lvl1; // Простой комментарий
    Hierarchy_lvl1_lvl2 hierarchy_lvl1_lvl2; ///< Тег без Display
};

namespace The::Longest::Namespace
{

/*!
 * @JsonSchema
 * @Json
 */
struct StructNamespaceSchema
{
    int value;
};

} // namespace The::Longest::Namespace

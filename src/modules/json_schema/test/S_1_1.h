#pragma once

#include <QByteArray>
#include <QString>
#include <QVector>

#include <rgen/json_qt.h>

#include <optional>
#include <qobject.h>

/// @Json
struct SelfStruct_l4
{
    /// @Display Описание для переменной quint__field_SelfStruct_l4
    quint8 quint8__field_SelfStruct_l4;
};

/// @Json
struct SelfStruct_l3
{
    SelfStruct_l4 SelfStruct_l4__field_SelfStruct_l3;
    QVector<SelfStruct_l3> qVector_SelfStruct_l3__field_SelfStruct_l3;
};

/// @Json
struct Hierarchy_lvl1_lvl1
{
    int int__field_Hierarchy_lvl1_lvl1;       // integer
    bool bool__field_Hierarchy_lvl1_lvl1;     // boolean
    double double__field_Hierarchy_lvl1_lvl1; // number

    QVector<int> qVector_int__field_Hierarchy_lvl1_lvl1;            // array to int
    QVector<QVector<int>> qVec_qVec_int__field_Hierarchy_lvl1_lvl1; // array to array to int

    QString qString__field_Hierarchy_lvl1_lvl1;       // string
    QByteArray qByteArray__field_Hierarchy_lvl1_lvl1; // array

    std::optional<SelfStruct_l3> opt_selfStruct__field_Hierarchy_lvl1_lvl1; // object + null
};

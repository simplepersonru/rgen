#pragma once

#include <QDataStream>
#include <QString>
#include <rgen/derive.h>

/// @Derive
struct Simple
{
    RGEN_DERIVE(friend, Simple, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_QVariantMap);

    int id;
    QString name;
};

/// @Derive
struct Base
{
    RGEN_DERIVE(friend, Base, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_QVariantMap);

    int baseField;
};

/// @Derive
struct Derived : public Base
{
    RGEN_DERIVE(friend, Derived, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_QVariantMap);

    QString derivedField;
};

/// @Derive
struct Another
{
    RGEN_DERIVE(friend, Another, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_QVariantMap);

    double value;
};

/// @Derive
struct MultiDerived : public Base,
                      public Another
{
    RGEN_DERIVE(friend, MultiDerived, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_QVariantMap);

    bool flag;
};

/// @Derive
struct OverMultiDerived : public MultiDerived,
                          public Simple
{
    RGEN_DERIVE(friend, OverMultiDerived, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_QVariantMap);

    bool flag1;
};

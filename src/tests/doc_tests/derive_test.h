#pragma once

#include <QString>
#include <rgen/derive.h>

/// @Derive
struct TestStruct
{
    RGEN_DERIVE(friend, TestStruct, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_QHasher, RGEN_QVariantMap)

    int id; ///< @Ignore
    QString name;

    /// @Json { ignore: true }
    int ignoredKey = 5;

    /// @Json { key: myCustomKey }
    int originalKey = 5;
};

/// @Derive
struct TestStructHasher
{
    RGEN_DERIVE(friend, TestStructHasher, RGEN_Hasher, RGEN_Equals)

    int id; ///< @Ignore
    std::string name;
};

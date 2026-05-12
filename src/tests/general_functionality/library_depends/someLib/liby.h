#pragma once

#include "liby_global.h"

#include <rgen/derive.h>
#include <rgen/hasher.h>
#include <rgen/operator_equals.h>
#include <rgen/operator_less.h>
#include <rgen/operator_qstream.h>
#include <rgen/qhasher.h>

#include <QDataStream>

#include <cstdlib>

/*!
 * @Enum
 */
enum class EnumGeneration
{
    Here
};

/*!
 * @Json
 * @JsonSchema
 * @QMetaTypesRegistrator
 * @OperatorEquals
 * @OperatorLess
 * @OperatorQStream
 * @Hasher
 * @QHasher
 */
struct LIBY_GLOBAL StructGeneration
{
    int a;
};

RGEN_DERIVE(LIBY_GLOBAL, StructGeneration, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap)

template <typename T>
struct TestTemplate
{
    T a;
};

/// @Derive
/// @JsonSchema
/// @QMetaTypesRegistrator
using TestUsing = TestTemplate<int>;
RGEN_DERIVE(LIBY_GLOBAL, TestUsing, RGEN_Equals, RGEN_Less, RGEN_QStream, RGEN_Json, RGEN_Hasher, RGEN_QHasher, RGEN_QVariantMap)
// Необходимо для инициализации TestUsing. Иначе получаем ошибку
// error: Template with alias "TestTemplate" is not initialized
template struct LIBY_GLOBAL TestTemplate<int>;

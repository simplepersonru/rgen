#pragma once

#include <rgen/hasher.h>
#include <rgen/json_qt.h>
#include <rgen/operator_equals.h>
#include <rgen/operator_less.h>
#include <rgen/operator_qstream.h>
#include <rgen/qhasher.h>
#include <rgen/qvariantmap_serializer.h>

/*!
 * \warning Не для вызова, а для поддержки переходов по символу в Doxygen,
 * см документацию ниже
 */
void Derive();

#define INTERNAL_DERIVE_1(Prefix, Type, a1) a1(Prefix, Type)
#define INTERNAL_DERIVE_2(Prefix, Type, a1, a2) a1(Prefix, Type) INTERNAL_DERIVE_1(Prefix, Type, a2)
#define INTERNAL_DERIVE_3(Prefix, Type, a1, a2, a3) a1(Prefix, Type) INTERNAL_DERIVE_2(Prefix, Type, a2, a3)
#define INTERNAL_DERIVE_4(Prefix, Type, a1, a2, a3, a4) a1(Prefix, Type) INTERNAL_DERIVE_3(Prefix, Type, a2, a3, a4)
#define INTERNAL_DERIVE_5(Prefix, Type, a1, a2, a3, a4, a5) a1(Prefix, Type) INTERNAL_DERIVE_4(Prefix, Type, a2, a3, a4, a5)
#define INTERNAL_DERIVE_6(Prefix, Type, a1, a2, a3, a4, a5, a6) a1(Prefix, Type) INTERNAL_DERIVE_5(Prefix, Type, a2, a3, a4, a5, a6)
#define INTERNAL_DERIVE_7(Prefix, Type, a1, a2, a3, a4, a5, a6, a7) a1(Prefix, Type) INTERNAL_DERIVE_6(Prefix, Type, a2, a3, a4, a5, a6, a7)
#define INTERNAL_DERIVE_8(Prefix, Type, a1, a2, a3, a4, a5, a6, a7, a8) a1(Prefix, Type) INTERNAL_DERIVE_7(Prefix, Type, a2, a3, a4, a5, a6, a7, a8)
#define INTERNAL_DERIVE_9(Prefix, Type, a1, a2, a3, a4, a5, a6, a7, a8, a9) a1(Prefix, Type) INTERNAL_DERIVE_8(Prefix, Type, a2, a3, a4, a5, a6, a7, a8, a9)

// Макрос для выбора нужного количества аргументов
#define INTERNAL_DERIVE_SELECT(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
#define RGEN_DERIVE(Prefix, Type, ...)                                                           \
    INTERNAL_DERIVE_SELECT(__VA_ARGS__,                                             \
                           INTERNAL_DERIVE_9, INTERNAL_DERIVE_8, INTERNAL_DERIVE_7, \
                           INTERNAL_DERIVE_6, INTERNAL_DERIVE_5, INTERNAL_DERIVE_4, \
                           INTERNAL_DERIVE_3, INTERNAL_DERIVE_2, INTERNAL_DERIVE_1) \
    (Prefix, Type, __VA_ARGS__)

#pragma once

/// Библиотека для подключения в файлы, в которых будет происходить генерация QVariantMapSerializer
///
/// Создается пользователем. Первым обязательно должен быть rgen/private/qvariantmap_pre.h,
/// а последним rgen/private/qvariantmap_post.h. Между ними включается пользовательский набор
/// определений операторов.
///
/// В пользовательском наборе операторов первым следует включать стандартный набор операторов
/// rgen/private/qvariantmap_lib.h. После включаются собственные.

//clang-format off
#include <rgen/private/qvariantmap_pre.h>

#include <rgen/private/qvariantmap_lib.h>

#include "private/ownLib.h"

#include <rgen/private/qvariantmap_post.h>

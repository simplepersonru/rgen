#pragma once

/// Библиотека для подключения в файлы, в которых будет происходить генерация Json
/// Создается пользователем. Первым обязательно должен быть rgen/private/json_pre.h,
/// а последним rgen/private/json_post.h. Между ними включается пользовательский набор
/// определений операторов.
///
/// В пользовательском наборе операторов первым следует включать стандартный набор операторов
/// rgen/private/jsonlib_std.h. При необходимости использования Qt, строго
/// после jsonlib_std.h добавить rgen/private/jsonlib_qt.h. После включаются собственные.

//clang-format off
#include <rgen/private/json_pre.h>

#include <rgen/private/jsonlib_std.h>
#include <rgen/private/jsonlib_qt.h>
#include "ownLib.h"

// Обязательно последний!
#include <rgen/private/json_post.h>

#pragma once

#ifdef __linux__
#   define RGEN_DECL_EXPORT     __attribute__((visibility("default")))
#   define RGEN_DECL_IMPORT     __attribute__((visibility("default")))
#elif defined(_WIN64) || defined(_WIN32)
#   define RGEN_DECL_EXPORT     __declspec(dllexport)
#   define RGEN_DECL_IMPORT     __declspec(dllimport)
#else
#   define RGEN_DECL_EXPORT
#   define RGEN_DECL_IMPORT
#endif

#pragma once

#include <iostream>

// Assertions are automatically turned off on release build.
#ifndef NDEBUG
#include <cassert>
#define EASSERT(expr) assert(expr)
#define EASSERT_MSG(expr, msg)                                                 \
    if (!(expr)) {                                                             \
        std::cerr << "Assertion failed: " << msg << std::endl;                 \
        assert(expr);                                                          \
    }
#else // #ifndef NDEBUG
#define EASSERT(expr) void(0);
#define EASSERT_MSG(expr, msg) void(0);
#endif // #ifndef NDEBUG

// Warning is meant to report improper behaviors of the program,
// which may not hinder the program from running.
// #define EUCLID_NO_WARNING before including this header to disable the
// warning message
#ifndef EUCLID_NO_WARNING
#define EWARNING(msg)                                                          \
    std::cerr << "Euclid Warning: " << msg << std::endl                        \
              << "(In file " << __FILE__ << ", line " << __LINE__              \
              << ", function " << __func__ << ")" << std::endl;
#else // #ifndef EUCLID_NO_WARNING
#define EWARNING(msg) void(0);
#endif // #ifndef EUCLID_NO_WARNING

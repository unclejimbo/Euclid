#ifndef NDEBUG
#include <cassert>
#include <iostream>
#endif

#define EASSERT(expr) assert(expr)

#ifndef NDEBUG
#define EASSERT_MSG(expr, msg) \
	if (!(expr)) { std::cerr << "Assertion Message: " << msg << std::endl; assert(expr); }
#else
#define EASSERT_MSG(expr, msg) void(0);
#endif

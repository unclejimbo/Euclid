/** Assert.
 *
 *  @defgroup PkgAssert Assert
 *  @ingroup PkgUtil
 */
#ifndef NDEBUG
#include <cassert>
#include <iostream>

#define EASSERT(expr) assert(expr)

#define EASSERT_MSG(expr, msg) \
	if (!(expr)) { std::cerr << "Assertion Message: " << msg << std::endl; assert(expr); }

#else

#define EASSERT(expr) void(0);

#define EASSERT_MSG(expr, msg) void(0);

#endif

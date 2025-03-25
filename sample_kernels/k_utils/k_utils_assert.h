#ifndef K_UTILS_ASSERT_H_
#define K_UTILS_ASSERT_H_

#ifdef NDEBUG
#undef NDEBUG
#include <assert.h>
#define NDEBUG
#else
#include <assert.h>
#endif

#define ASSERT(X) assert(X)

#endif // K_UTILS_ASSERT_H_
//==============================================================
// Copyright (C) Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

#ifndef PHPROF_UTILS_PHPROF_ASSERT_H_
#define PHPROF_UTILS_PHPROF_ASSERT_H_

#ifdef NDEBUG
#undef NDEBUG
#include <assert.h>
#define NDEBUG
#else
#include <assert.h>
#endif

#define PHPROF_ASSERT(X) assert(X)

#endif // PHPROF_UTILS_PHPROF_ASSERT_H_
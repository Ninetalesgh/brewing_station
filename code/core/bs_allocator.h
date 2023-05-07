#pragma once

#include <platform/bs_platform.h>

#include "allocator/bs_thread_safe_linear_allocator.h"
#include "allocator/bs_buddy_allocator.h"

#define ALLOCATE(size) bs::allocate(bsp::platform->default.mainThreadAllocator, size)
#define FREE(allocation) bs::free(bsp::platform->default.mainThreadAllocator, allocation)
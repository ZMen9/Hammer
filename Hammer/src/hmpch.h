#pragma once 
#include "Hammer/Core/PlatformDetection.h"

#ifdef HM_PLATFORM_WINDOWS
#ifndef NOMINMAX
// See
// github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#define NOMINMAX
#endif
#endif  // HM_PLATFORM_WINDOWS

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <array>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include "Hammer/Core/Base.h"
#include "Hammer/Core/Log.h"
#include "Hammer/Debug/Instrumentor.h"

#ifdef HM_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

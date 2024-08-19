#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <deque>
#include <atomic>
#include <thread>
#include <mutex>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <numeric>
#include <random>
#include <tuple>

#include "RE_math.h"
#include "RE_debug.h"
#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

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

#include "REdebug.h"

#include "REFixpoint.h"
#include "REmath.h"
// #include "REThreadPool.h"
#include "RETexture.h"

#include "MSWindow.h"
#pragma once

#ifdef MESH_EDITOR_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <optional>

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>

#include <format>

// namespace
namespace fs = std::filesystem;

// glm
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtx/quaternion.hpp>

// Common Macros
#include "Core/Macro.h"

// Log
#include "Core/Logger/Logger.h"

// Input
#include "Core/Input/Input.h"

// OpenGL
#include <glad/glad.h>

// Math
#include "Core/Math/Math.h"

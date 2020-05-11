#ifndef COMMON_H
#define COMMON_H

// ----------------- GLFW3 + VULKAN ----------------------------
// #define GLFW_DLL
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>


// ----------------- GML ----------------------------
#define GLM_FORCE_RADIANS
// GLM use the OpenGL depth range of -1.0 to 1.0 by default.
// We want range from 0.0 to 1.0 for Vulkan.
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// include hash function for glm objects
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>


// ----------------- STD C++17 ----------------------------
#include <chrono>

#include <iostream> // io
#include <stdexcept> // exception
#include <functional> // lambda fnc
#include <cstdlib>  // EXIT_FAILURE EXIT_SUCCESS
#include <cstring> // strcomp
#include <vector>
#include <array>
#include <sstream>
#include <map> // multimap
#include <optional>
#include <set>
#include <cstdint> // Necessary for UINT32_MAX
#include <fstream>
#include <unordered_map>

// ----------------- APPLICATION PARAMETERS ----------------------------
static const int APP_WIDTH = 1280;
static const int APP_HEIGHT = 800;

#define mdebug(msg) { std::cout << "MiniVulkan debug : " << msg << std::endl; }
static const int MAX_FRAMES_IN_FLIGHT = 2;

const std::string SHADER_VERT_PATH = "shaders/vert.spv";
const std::string SHADER_FRAG_PATH = "shaders/frag.spv";


#ifdef NDEBUG
 const bool enableValidationLayers = false;
#else
 const bool enableValidationLayers = true;
#endif

#endif // COMMON_H

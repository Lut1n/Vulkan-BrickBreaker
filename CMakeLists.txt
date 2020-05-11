cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(VkBrickBreaker_Proj)

## Must be changed if does not match with the system
## (todo : Add Module finders to the project)
set(GLFW_PATH "C:/glfw-3.3/win32")
set(GLM_PATH "C:/glm")
message(STATUS "Using GLFW_PATH: ${GLFW_PATH} (change it if needed)")
message(STATUS "Using GLM_PATH: ${GLM_PATH} (change it if needed)")


set(Exe_Name "VkBrickBreaker")
set(VULKAN_SDK $ENV{VULKAN_SDK})
set(VULKAN_LIB_LIST "vulkan-1")

set(GAME_PATH "${PROJECT_SOURCE_DIR}/game")
set(APPLICATION_PATH "${PROJECT_SOURCE_DIR}/app")
set(VKINTERFACE_PATH "${PROJECT_SOURCE_DIR}/vkinterface")
set(THIRD_PARTY_PATH "${PROJECT_SOURCE_DIR}/thirdparty")


### --------------- THIRD PARTY -----------------------------
### STB (sean tool box)
include_directories(AFTER "${THIRD_PARTY_PATH}/stb/include")
### Tiny OBJ loader
include_directories(AFTER "${THIRD_PARTY_PATH}/tinyobjloader/include")
### GLM
include_directories(AFTER "${GLM_PATH}")


### --------------------- GLFW -------------------------------
include_directories(AFTER "${GLFW_PATH}/include")
link_directories(AFTER "${GLFW_PATH}/lib-mingw")
# find_package(glfw3 3.3 REQUIRED)


###---------------------- VULKAN -----------------------------
# vulkan-1 library for build Vulkan application.
message(STATUS "Using Vulkan system path: ${VULKAN_SDK}")
include_directories(AFTER ${VULKAN_SDK}/Include)
# Link directory for vulkan-1
link_directories(AFTER ${VULKAN_SDK}/Bin;${VULKAN_SDK}/Lib32;)


###---------------------- APPLICATION FILES --------------------------
set(SRCS
    ${PROJECT_SOURCE_DIR}/main.cpp

    ${APPLICATION_PATH}/application.cpp
    ${APPLICATION_PATH}/drawcommandbuffer.cpp
    ${APPLICATION_PATH}/gcore.cpp
    ${APPLICATION_PATH}/scene.cpp
    ${APPLICATION_PATH}/drawables.cpp
    ${APPLICATION_PATH}/shaderpipeline.cpp
    ${APPLICATION_PATH}/swapchainresources.cpp
    ${APPLICATION_PATH}/assetsloader.cpp

    ${GAME_PATH}/background.cpp
    ${GAME_PATH}/ball.cpp
    ${GAME_PATH}/brick.cpp
    ${GAME_PATH}/paddle.cpp
    ${GAME_PATH}/camera.cpp
    ${GAME_PATH}/platform.cpp
    ${GAME_PATH}/collisions.cpp

    ${VKINTERFACE_PATH}/buffer.cpp
    ${VKINTERFACE_PATH}/commandbuffer.cpp
    ${VKINTERFACE_PATH}/debugmessenger.cpp
    ${VKINTERFACE_PATH}/descriptorset.cpp
    ${VKINTERFACE_PATH}/extensions.cpp
    ${VKINTERFACE_PATH}/framebuffer.cpp
    ${VKINTERFACE_PATH}/image.cpp
    ${VKINTERFACE_PATH}/imageview.cpp
    ${VKINTERFACE_PATH}/instance.cpp
    ${VKINTERFACE_PATH}/logicaldevice.cpp
    ${VKINTERFACE_PATH}/physicaldevice.cpp
    ${VKINTERFACE_PATH}/pipeline.cpp
    ${VKINTERFACE_PATH}/pipelinelayout.cpp
    ${VKINTERFACE_PATH}/queue.cpp
    ${VKINTERFACE_PATH}/renderpass.cpp
    ${VKINTERFACE_PATH}/shader.cpp
    ${VKINTERFACE_PATH}/surface.cpp
    ${VKINTERFACE_PATH}/swapchain.cpp
    ${VKINTERFACE_PATH}/synchronization.cpp
    ${VKINTERFACE_PATH}/texture.cpp
    ${VKINTERFACE_PATH}/uniformbuffer.cpp
    ${VKINTERFACE_PATH}/validationlayers.cpp
    ${VKINTERFACE_PATH}/vertexbuffer.cpp
    ${VKINTERFACE_PATH}/vkiobject.cpp
    )
set(HEADERS
    ${PROJECT_SOURCE_DIR}/common.h

    ${APPLICATION_PATH}/application.h
    ${APPLICATION_PATH}/drawcommandbuffer.h
    ${APPLICATION_PATH}/gcore.h
    ${APPLICATION_PATH}/scene.h
    ${APPLICATION_PATH}/drawables.h
    ${APPLICATION_PATH}/shaderpipeline.h
    ${APPLICATION_PATH}/swapchainresources.h
    ${APPLICATION_PATH}/assetsloader.h

    ${GAME_PATH}/background.h
    ${GAME_PATH}/ball.h
    ${GAME_PATH}/brick.h
    ${GAME_PATH}/paddle.h
    ${GAME_PATH}/camera.h
    ${GAME_PATH}/platform.h
    ${GAME_PATH}/collisions.h

    ${VKINTERFACE_PATH}/buffer.h
    ${VKINTERFACE_PATH}/commandbuffer.h
    ${VKINTERFACE_PATH}/debugmessenger.h
    ${VKINTERFACE_PATH}/descriptorset.h
    ${VKINTERFACE_PATH}/extensions.h
    ${VKINTERFACE_PATH}/framebuffer.h
    ${VKINTERFACE_PATH}/image.h
    ${VKINTERFACE_PATH}/imageview.h
    ${VKINTERFACE_PATH}/instance.h
    ${VKINTERFACE_PATH}/logicaldevice.h
    ${VKINTERFACE_PATH}/physicaldevice.h
    ${VKINTERFACE_PATH}/pipeline.h
    ${VKINTERFACE_PATH}/pipelinelayout.h
    ${VKINTERFACE_PATH}/queue.h
    ${VKINTERFACE_PATH}/renderpass.h
    ${VKINTERFACE_PATH}/shader.h
    ${VKINTERFACE_PATH}/surface.h
    ${VKINTERFACE_PATH}/swapchain.h
    ${VKINTERFACE_PATH}/synchronization.h
    ${VKINTERFACE_PATH}/texture.h
    ${VKINTERFACE_PATH}/uniformbuffer.h
    ${VKINTERFACE_PATH}/validationlayers.h
    ${VKINTERFACE_PATH}/vertexbuffer.h
    ${VKINTERFACE_PATH}/vkiobject.h
    )

include_directories(AFTER ${PROJECT_SOURCE_DIR})
add_executable(${Exe_Name} ${SRCS} ${HEADERS})


# target_link_libraries(${Exe_Name} glfw3dll)
target_link_libraries(${Exe_Name} glfw3)

# Link the debug and release libraries to the project
target_link_libraries( ${Exe_Name} ${VULKAN_LIB_LIST} )







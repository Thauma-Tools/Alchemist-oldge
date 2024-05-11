cmake_minimum_required(VERSION 3.20)
project(
  Alchemist
  LANGUAGES C CXX
  VERSION 0.0.1
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG "v1.14.1"
)
FetchContent_Declare(
  sdl
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
  GIT_TAG "prerelease-3.1.2"
)
FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
  FetchContent_Populate(spdlog)

  set(SPDLOG_BUILD_PIC ON)

  add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif()
FetchContent_GetProperties(sdl)
if(NOT sdl_POPULATED)
  FetchContent_Populate(sdl)

  set(SDL_STATIC OFF)
  set(SDL_TEST_LIBRARY OFF)

  add_subdirectory(${sdl_SOURCE_DIR} ${sdl_BINARY_DIR})
endif()

# FetchContent_GetProperties(glfw)
# if(NOT glfw_POPULATED)
#   FetchContent_Populate(glfw)
#
#   set(GLFW_INSTALL OFF)
#   set(GLFW_BUILD_DOCS OFF)
#   set(GLFW_BUILD_TESTS OFF)
#   set(GLFW_BUILD_EXAMPLES OFF)
#
#   add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
# endif()
find_package(Vulkan REQUIRED)

if(UNIX AND (NOT APPLE))
  set(
    ALCHEMIST_DEFINITIONS
      ALCHEMIST_PLATFORM_LINUX
  )
endif()

if(WIN32)
  set(
    ALCHEMIST_DEFINITIONS
      ALCHEMIST_PLATFORM_WINDOWS
  ) 
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  list(
    APPEND
    ALCHEMIST_DEFINITIONS
      ALCHEMIST_DEBUG
  )
endif()

add_library(Alchemist SHARED)
set_target_properties(
  Alchemist
  PROPERTIES
    DEBUG_POSTFIX "-d"
    CXX_STANDARD 20
    CXX_VISIBILITY_PRESET hidden
)
target_sources(
  Alchemist
  PRIVATE
    src/Alchemist.cpp
    src/Alchemist_logger.cpp
    src/Alchemist_application.cpp
    src/Alchemist_renderer.cpp

    src/Vulkan/Alchemist_vulkan.cpp
    src/Vulkan/Alchemist_device.cpp
    src/Vulkan/Alchemist_swapchain.cpp
    src/Vulkan/Alchemist_pipeline.cpp
    src/Vulkan/Alchemist_renderpass.cpp
    src/Vulkan/Alchemist_shader.cpp
)
target_compile_definitions(
  Alchemist
  PRIVATE
    ALCHEMIST_EXPORT_BUILD
)
target_compile_definitions(
  Alchemist
  PUBLIC
    ${ALCHEMIST_DEFINITIONS}
)
target_link_libraries(
  Alchemist
  PUBLIC
    spdlog
    SDL3::SDL3
)
target_link_libraries(
  Alchemist
  PRIVATE
    ${Vulkan_LIBRARIES}
)
target_include_directories(
  Alchemist
  PUBLIC
    include
)
target_include_directories(
  Alchemist
  PRIVATE
    ${Vulkan_INCLUDE_DIRS}
)

add_subdirectory(sandbox)


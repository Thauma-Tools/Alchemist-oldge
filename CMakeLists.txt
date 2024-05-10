cmake_minimum_required(VERSION 3.20)
project(
  Alchemist
  LANGUAGES C CXX
  VERSION 0.0.1
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG "v1.14.1"
)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY "https://github.com/glfw/glfw.git"
  GIT_TAG "3.4"
)
FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
  FetchContent_Populate(spdlog)

  set(SPDLOG_BUILD_PIC ON)

  add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif()
FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
  FetchContent_Populate(glfw)

  set(GLFW_INSTALL OFF)
  set(GLFW_BUILD_DOCS OFF)
  set(GLFW_BUILD_TESTS OFF)
  set(GLFW_BUILD_EXAMPLES OFF)

  add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()
find_package(Vulkan REQUIRED)

if(UNIX AND (NOT APPLE))
  set(
    ALCHEMIST_DEFINITIONS
      ALCHEMIST_PLATFORM_LINUX
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

    src/Vulkan/Alchemist_vulkan.cpp
    src/Vulkan/Alchemist_device.cpp
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
)
target_link_libraries(
  Alchemist
  PRIVATE
    glfw
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
    ${VUlkan_INCLUDE_DIRS}
)

add_subdirectory(sandbox)


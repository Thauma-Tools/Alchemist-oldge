cmake_minimum_required(VERSION 3.20)
project(
  Alchemist-Sandbox
  LANGUAGES C CXX
  VERSION 0.0.1
)

add_executable(AlchemistSandbox WIN32)
target_sources(
  AlchemistSandbox
  PRIVATE
    Main.cpp
)
target_link_libraries(
  AlchemistSandbox
  PRIVATE
    Alchemist
)

if(CMAKE_SOURCE_DIR STREQUAL Alchemist_SOURCE_DIR)
  add_custom_target(
    run
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND AlchemistSandbox
  )
endif()


cmake_minimum_required(VERSION 3.20)

if(NOT ESP_PLATFORM)
  project(tlvcpp LANGUAGES CXX)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

file(GLOB_RECURSE SOURCES "src/*.h" "src/*.cpp")

if(ESP_PLATFORM)
  idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "include" PRIV_INCLUDE_DIRS "src")
else()
  add_library(${PROJECT_NAME} ${SOURCES})

  target_include_directories(${PROJECT_NAME} PUBLIC include PRIVATE src)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Werror -Wextra -Wpedantic -Wconversion)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4 /WX)
endif()

include(test)

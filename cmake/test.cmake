string(TOUPPER ${PROJECT_NAME} PROJECT)
set(BUILD_CHECK_VAR BUILD_${PROJECT}_TESTS)

option(${BUILD_CHECK_VAR} "build \"${PROJECT_NAME}\"'s tests." OFF)

if(${${BUILD_CHECK_VAR}})
  get_target_property(TARGET_TYPE ${PROJECT_NAME} TYPE)

  if(${TARGET_TYPE} STREQUAL "EXECUTABLE")
    return()
  endif()

  set(INSTALL_GTEST OFF CACHE BOOL "Disable gtest's intall")

  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG origin/main
  )
  FetchContent_MakeAvailable(googletest)

  include(CTest)
  include(GoogleTest)

  set(TEST_MAIN_TARGET_NAME ${PROJECT_NAME}_test_main)
  add_library(${TEST_MAIN_TARGET_NAME} OBJECT tests/main.cpp)
  target_link_libraries(${TEST_MAIN_TARGET_NAME} PRIVATE gtest)

  file(GLOB PROJECT_TESTS LIST_DIRECTORIES false tests/*.cpp)
  list(REMOVE_ITEM PROJECT_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp)

  foreach(SOURCE ${PROJECT_TESTS})
    cmake_path(GET SOURCE STEM TEST_FILE_NAME)
    set(TEST_TARGET_NAME ${PROJECT_NAME}_test_${TEST_FILE_NAME})
    add_executable(${TEST_TARGET_NAME} ${SOURCE} $<TARGET_OBJECTS:${TEST_MAIN_TARGET_NAME}>)
    target_link_libraries(${TEST_TARGET_NAME} PRIVATE ${PROJECT_NAME} gtest gmock)
    gtest_discover_tests(${TEST_TARGET_NAME})
  endforeach()

endif()
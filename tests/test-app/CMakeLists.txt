project(track-hat-driver-test-app
    LANGUAGES CXX
    VERSION ${LIBRARY_VERSION})

# Set C++ 14 Standard
set(CMAKE_CXX_STANDARD 14)

include(${CMAKE_SOURCE_DIR}/src/CMakeSources.txt)
include_directories(${TRACK_HAT_DRIVER_INCLUDES})

# Set headers
include_directories(${CMAKE_SOURCE_DIR}/src)

# Set sources
set(SOURCES
  track_hat_test_app.cpp)

#message(FATAL_ERROR ${SOURCES})
# Shared library target
add_executable(
  track-hat-test
  ${SOURCES})

## Link shared library
target_link_libraries(
  track-hat-test
  PUBLIC track-hat)

# Copy track-hat.dll
add_custom_command(
  TARGET track-hat-test PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:track-hat>
  $<TARGET_FILE_DIR:track-hat-test>)

install(
  TARGETS track-hat-test
  RUNTIME
  DESTINATION bin)

project(track-hat-driver-test-app
    LANGUAGES CXX
    VERSION ${LIBRARY_VERSION})

# Set C++ 14 Standard
set(CMAKE_CXX_STANDARD 14)

# Set headers
include_directories(${CMAKE_SOURCE_DIR}/src)

# Set sources
set(SOURCES
  track_hat_test_app.cpp)

# Shared library target
add_executable(
  track-hat-test
  ${SOURCES})

# Link shared library
target_link_libraries(
  track-hat-test
  PUBLIC track-hat)

# Copy track-hat.dll
add_custom_command(
  TARGET track-hat-test POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:track-hat>
  ${CMAKE_CURRENT_BINARY_DIR}
)

install(
  TARGETS track-hat-test
  RUNTIME
  DESTINATION bin)

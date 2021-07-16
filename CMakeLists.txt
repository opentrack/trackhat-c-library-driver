cmake_minimum_required(VERSION 3.10)

# Set TrackHat library version
set(LIBRARY_VERSION 0.1.1)

project(track-hat-driver
    VERSION ${LIBRARY_VERSION})

# Be verbose about what is being done
set(CMAKE_VERBOSE_MAKEFILE ON)

# Add library sources
add_subdirectory(src)

# Add tests
add_subdirectory(tests)

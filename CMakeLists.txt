cmake_minimum_required(VERSION 3.20)

# Set TrackHat library version
set(LIBRARY_VERSION 0.1.0)

project(track-hat-driver
    VERSION ${LIBRARY_VERSION})

# Be verbose about what is being done
set(CMAKE_VERBOSE_MAKEFILE ON)

# Add library sources
add_subdirectory(src)

# Add tests
add_subdirectory(tests)

# Installer
if (MSVC)
    include (InstallRequiredSystemLibraries)
    set(CPACK_PACKAGE_NAME "TrackHat driver")
    set(CPACK_GENERATOR NSIS)
endif (MSVC)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "TrackHat driver - installer")
set(CPACK_PACKAGE_VENDOR "WizzDev")
set(CPACK_PACKAGE_VERSION ${LIBRARY_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_OUTPUT_FILE_PREFIX "installer")
include(CPack)

# Set C++ 14 Standard
set(CMAKE_CXX_STANDARD 14)

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeSources.txt)

# Library headers
include_directories(${TRACK_HAT_DRIVER_INCLUDES})

# Shared library target
add_library(
  track-hat
  SHARED
  ${TRACK_HAT_DRIVER_SOURCES})

# Headers installation
install(
  FILES ${TRACK_HAT_DRIVER_INCLUDES_INSTALL}
  DESTINATION include)

# Library installation
install(
  TARGETS track-hat
  ARCHIVE RUNTIME
  DESTINATION bin)

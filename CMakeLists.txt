# Set minimum required version of CMake (3.20.1 properly supports Apple Silicon)
cmake_minimum_required(VERSION 3.20.1)

# Set project name and language
project(
  pathmaster
  LANGUAGES CXX
)

# Add the library as an interface library, require C++17
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

# Add install targets
install(TARGETS ${PROJECT_NAME} DESTINATION include)
install(DIRECTORY include/ DESTINATION include)

# Add package target
set(package_files include/ CMakeLists.txt LICENSE)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip
    COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip --format=zip -- ${package_files}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${package_files})
add_custom_target(${PROJECT_NAME}_package DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip)

# If we're in the root, add the test executable
if(PROJECT_IS_TOP_LEVEL)  # CMake v3.21 or later
  # Set standard without compiler specific extensions, export compile commands
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  # Require out-of-source builds
  if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a separate build directory and run CMake from there.")
  endif()

  # Set default build type to Release
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to \"Release\" as none was specified.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  endif()

  # Add executable target
  add_executable(pathmaster_test tests/pathmaster_test.cpp)

  # Enable compile warnings (if not on Windows)
  # This will also check the library, not just the test executable
  if(NOT WIN32)
    target_compile_options(pathmaster_test PRIVATE
      -Wall                 # Enable most warning flags
      -Wcast-align          # Warn for potential performance problems when casting a pointer to a type with stricter alignment requirements
      -Wconversion          # Warn on type conversions that may lose data
      -Wdouble-promotion    # Warn if a float is implicitly promoted to double
      -Werror               # Treat warnings as errors (stop compilation if any warning is present)
      -Wextra               # Enable extra warning flags that are not enabled by "-Wall"
      -Wformat=2            # Warn on security issues around functions that format output (like printf), this includes all "-Wformat" warnings and more
      -Wnon-virtual-dtor    # Warn when a class with virtual functions has a non-virtual destructor, which can lead to undefined behavior
      -Wnull-dereference    # Warn if a null dereference is detected
      -Wold-style-cast      # Warn for usage of C-style casts
      -Woverloaded-virtual  # Warn when a derived class function declaration may be an error due to hiding a virtual function from the base class
      -Wpedantic            # Warn on features that are not part of the ISO C++ standard
      -Wshadow              # Warn when a local variable shadows another local variable, parameter, global variable, or function
      -Wsign-conversion     # Warn on sign conversions (e.g., when a negative integer is converted to an unsigned integer)
      -Wunused              # Warn on anything being unused
    )
  endif()

  # Link the dependencies to the target
  target_link_libraries(pathmaster_test PRIVATE ${PROJECT_NAME})

  # Print the build type
  message(STATUS "Build type: ${CMAKE_BUILD_TYPE}.")
endif()

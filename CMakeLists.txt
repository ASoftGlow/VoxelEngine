cmake_minimum_required(VERSION 3.10)
project(VoxelEngine)

# Specify the directories containing the header files
include_directories(${CMAKE_SOURCE_DIR}/Lib/glm)

set(CMAKE_BUILD_TYPE Debug)


# Specify the directories containing the .so library files
#link_directories(${CMAKE_SOURCE_DIR}/Lib)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/Lib/glfw)
add_subdirectory(${CMAKE_SOURCE_DIR}/Lib/glm)

# Define the executable
add_executable(VoxelEngine
            main.cpp 
)

set_property(TARGET VoxelEngine PROPERTY CXX_STANDARD 17)
set_property(TARGET VoxelEngine PROPERTY CXX_STANDARD_REQUIRED ON)

add_library(vulkan STATIC IMPORTED)
set_target_properties(vulkan PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/Lib/vulkan.lib")

# Link the executable to the local GLFW and Vulkan libraries
target_link_libraries(VoxelEngine glfw vulkan)
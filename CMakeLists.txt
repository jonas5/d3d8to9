cmake_minimum_required(VERSION 3.15)
project(d3d8to9 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the library target
add_library(d3d8to9 SHARED
  source/d3d8.hpp
  source/d3d8to9.cpp
  source/d3d8to9.hpp
  source/d3d8to9_base.cpp
  source/d3d8to9_device.cpp
  source/d3d8to9_index_buffer.cpp
  source/d3d8to9_surface.cpp
  source/d3d8to9_swap_chain.cpp
  source/d3d8to9_texture.cpp
  source/d3d8to9_vertex_buffer.cpp
  source/d3d8to9_volume.cpp
  source/d3d8types.cpp
  source/d3d8types.hpp
  source/d3dx9.hpp
  source/interface_query.cpp
  source/interface_query.hpp
  res/d3d8to9.rc
)

# Include directories
target_include_directories(d3d8to9 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Include
)

# Compiler definitions
target_compile_definitions(d3d8to9 PRIVATE
    __uuidof=__mingw_uuidof
)

# Link system DirectX libraries
target_link_libraries(d3d8to9 PRIVATE
    d3d9
    d3dx9
)

# Set output name
set_target_properties(d3d8to9 PROPERTIES OUTPUT_NAME "d3d8")

# Optional: install target
install(TARGETS d3d8to9 DESTINATION bin)

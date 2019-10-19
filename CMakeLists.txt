cmake_minimum_required ( VERSION 3.1 )

set ( CMAKE_BUILD_TYPE Release )

project ( craft CXX )
file ( GLOB craft_source src/*.cpp )
add_executable ( craft ${craft_source} )

set_target_properties ( craft PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
    target_compile_options ( craft PRIVATE -Wall -Wextra )
elseif ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    target_compile_options ( craft PRIVATE /W4 )
endif ()

set ( GLAD_API "gl=4.2" CACHE STRING "" FORCE )
set ( GLAD_REPRODUCIBLE ON CACHE BOOL "" FORCE )
add_subdirectory ( third_party/glad )
target_link_libraries ( craft glad )
target_include_directories ( craft PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/third_party/glad/include )
add_dependencies ( craft glad )

set ( GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE )
set ( GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE )
set ( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )
add_subdirectory ( third_party/glfw )
target_link_libraries ( craft glfw )
target_include_directories ( craft PRIVATE third_party/glfw/include )
add_dependencies ( craft glfw )

target_include_directories ( craft PRIVATE third_party/glm )

target_include_directories ( craft PRIVATE third_party/stb )

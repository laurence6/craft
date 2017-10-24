cmake_minimum_required ( VERSION 3.1 )

project ( craft LANGUAGES CXX )

FILE ( GLOB craft_header src/*.hpp )
FILE ( GLOB craft_source src/*.cpp )
add_executable ( craft ${craft_header} ${craft_source} )

set ( CMAKE_BUILD_TYPE RelWithDebInfo )

set ( CMAKE_CXX_STANDARD 14 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )

if ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
    target_compile_options ( craft PRIVATE -Wall -Wextra )
elseif ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    target_compile_options ( craft PRIVATE /W4 )
endif ()

find_package ( OpenGL REQUIRED )
target_include_directories ( craft PRIVATE ${OPENGL_INCLUDE_DIR} )
target_link_libraries ( craft ${OPENGL_gl_LIBRARY} )

find_package ( GLEW REQUIRED )
target_include_directories ( craft PRIVATE ${GLEW_INCLUDE_DIRS} )
target_link_libraries ( craft ${GLEW_LIBRARIES} )

set ( GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE )
set ( GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE )
set ( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )
add_subdirectory ( third_party/glfw )
target_include_directories ( craft PRIVATE third_party/glfw/include )
target_link_libraries ( craft glfw )

target_include_directories ( craft PRIVATE third_party/glm )

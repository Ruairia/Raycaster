cmake_minimum_required(VERSION 3.10)
project(EPQ)

set(CMAKE_CXX_STANDARD 17)

# Use pkg-config to find raylib
find_package(PkgConfig REQUIRED)
if (PkgConfig_FOUND)
    pkg_check_modules(RAYLIB REQUIRED raylib)
endif()

if (NOT RAYLIB_LIBRARIES) #Then probably running on windows
    set(RAYLIB_DIR "C:/raylib")
endif()
include_directories(${RAYLIB_INCLUDE_DIRS})
link_directories(${RAYLIB_LIBRARY_DIRS})

add_executable(raycaster
        main.cpp
        Vector2D.h
        Player.cpp
        Player.h
        Map.h
        ray.h
        raycaster_shader.glsl
)
add_executable(raylib_test
        raylib_test.cpp
        raylib_test.h)
add_executable(ShaderTest shader_test.cpp)

# Link required libraries
target_link_libraries(raycaster ${RAYLIB_LIBRARIES})
target_link_libraries(raylib_test ${RAYLIB_LIBRARIES})
target_link_libraries(ShaderTest ${RAYLIB_LIBRARIES})

configure_file(shader_test.glsl shader_test.glsl COPYONLY)
configure_file(raycaster_shader.glsl raycaster_shader.glsl COPYONLY)